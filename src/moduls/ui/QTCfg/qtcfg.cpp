
//OpenSCADA system module UI.QTCfg file: qtcfg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>	      

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
#include <QListWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QDateTime>
#include <QLineEdit>
#include <QErrorMessage>
#include <QDateTimeEdit>
#include <QScrollArea>
#include <QCloseEvent>
#include <QFileDialog>

#include "xpm/up.xpm"
#include "xpm/previous.xpm"
#include "xpm/next.xpm"
#include "xpm/identity.xpm"
#include "xpm/close.xpm"
#include "xpm/exit.xpm"
#include "xpm/help.xpm"
#include "xpm/contexthelp.xpm"
#include "xpm/start.xpm"
#include "xpm/stop.xpm"
#include "xpm/reload.xpm"
#include "xpm/ok.xpm"
#include "xpm/button_ok.xpm"
#include "xpm/button_cancel.xpm"
#include "xpm/oscada_cfg.xpm"

#include <tmess.h>
#include <tsys.h>
#include <resalloc.h>
#include <tsecurity.h>

#include "selfwidg.h"
#include "tuimod.h"
#include "qtcfg.h"

#define CH_REFR_TM 100

using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( string open_user ) : que_sz(20), block_tabs(false), pg_info("info"), root(&pg_info), tbl_init(false)
{   
    setAttribute(Qt::WA_DeleteOnClose,true);
    QImage ico_t;
    mod->regWin( this );

    setWindowTitle(mod->I18N("QT Configurator of OpenSCADA"));
    setWindowIcon(QIcon(oscada_cfg_xpm));
    
    //Centrall widget
    setCentralWidget( new QWidget(this) );
    QGridLayout *QTCfgLayout = new QGridLayout(centralWidget());
    
    //Splitter
    QSplitter *splitter = new QSplitter( centralWidget() );
    splitter->setOrientation( Qt::Horizontal );	
    
    //Create Navigator tree
    CtrTree = new QTreeWidget( splitter );
    CtrTree->setContextMenuPolicy(Qt::CustomContextMenu);
    //splitter->setSizeConstraint(QSplitter::KeepSize);
    QStringList headerLabels;
    headerLabels << mod->I18N("Name") << mod->I18N("Type") << mod->I18N("Path");
    CtrTree->setHeaderLabels(headerLabels);
    CtrTree->header()->setStretchLastSection(false);
    CtrTree->header()->resizeSection(0,200);
    //CtrTree->header()->setSectionHidden(1,true);
    //CtrTree->header()->setSectionHidden(2,true);
    //CtrTree->header()->hide();
    //CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->setMinimumSize( QSize( 150, 0 ) );
    CtrTree->setMaximumSize( QSize( 400, 32767 ) );
    //connect( CtrTree, SIGNAL( currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ), this, SLOT( selectItem(QTreeWidgetItem*) ) );
    connect( CtrTree, SIGNAL( itemSelectionChanged() ), this, SLOT( selectItem() ) );
    connect( CtrTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( viewChild(QTreeWidgetItem*) ) );
    //connect( CtrTree, SIGNAL( itemEntered(QTreeWidgetItem*,int) ), this, SLOT( onItem(QTreeWidgetItem*) ) );
    connect( CtrTree, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( ctrTreePopup() ) );
		
    //Right frame add
    QFrame *gFrame = new QFrame( splitter );    
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sp.setHorizontalStretch(3);
    gFrame->setSizePolicy( sp );
    gFrame->setFrameShape( QFrame::WinPanel );
    gFrame->setFrameShadow( QFrame::Raised );	        

    QGridLayout *gFrameLayout = new QGridLayout(gFrame);    
    
    titleIco = new QLabel( gFrame );
    titleIco->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
    gFrameLayout->addWidget( titleIco, 0, 0 );
    
    titleLab = new QLabel( gFrame );
    titleLab->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 1 );    
    
    if(!ico_t.load(TUIS::icoPath("identity").c_str())) ico_t = QImage(identity_xpm);
    if(!SYS->security().at().usrPresent(open_user))	open_user = "user";
    w_user = new QPushButton(QPixmap::fromImage(ico_t), open_user.c_str(), gFrame );
    w_user->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum) );
    w_user->setWhatsThis(mod->I18N("This button change the OpenSCADA system user."));	    
    w_user->setToolTip(mod->I18N("Change user."));
    connect(w_user, SIGNAL(clicked()), this, SLOT(userSel()));

    QPalette palette;
    palette.setColor(w_user->backgroundRole(),(open_user=="root")?QColor(255,0,0):QColor(0,255,0));
    w_user->setPalette(palette);
    gFrameLayout->addWidget( w_user, 0, 2 );
    
    tabs = new QTabWidget( gFrame );
    connect( tabs, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabSelect(QWidget*) ) );
    gFrameLayout->addWidget( tabs, 1, 0, 1, 3 );
    
    QList<int> splSz;
    splSz.push_back(200);
    splSz.push_back(600);
    splitter->setSizes(splSz);
    QTCfgLayout->addWidget( splitter, 0, 0 );    
    
    //Status bar
    statusBar()->showMessage(mod->I18N("Ready"), 2000 );
    resize( 800, 600 );
    
    //Create actions
    //Close
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t = QImage(close_xpm);
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(mod->I18N("Close configurator window"));
    actClose->setWhatsThis(mod->I18N("Close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //Quit
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t = QImage(exit_xpm);
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(mod->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(mod->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //Up button
    if(!ico_t.load(TUIS::icoPath("up").c_str())) ico_t = QImage(up_xpm);
    actUp = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Up"),this);
    actUp->setShortcut(Qt::ALT+Qt::Key_Up);
    actUp->setToolTip(mod->I18N("Up page"));
    actUp->setWhatsThis(mod->I18N("Go to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(pageUp()));    
    //Previos page
    if(!ico_t.load(TUIS::icoPath("previous").c_str())) ico_t = QImage(previous_xpm);
    actPrev = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Previos"),this);
    actPrev->setShortcut(Qt::ALT+Qt::Key_Left);
    actPrev->setToolTip(mod->I18N("Previos page"));
    actPrev->setWhatsThis(mod->I18N("Go to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(pagePrev()));    
    //Previos page
    if(!ico_t.load(TUIS::icoPath("next").c_str())) ico_t = QImage(next_xpm);
    actNext = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Next"),this);
    actNext->setShortcut(Qt::ALT+Qt::Key_Right);
    actNext->setToolTip(mod->I18N("Next page"));
    actNext->setWhatsThis(mod->I18N("Go to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(pageNext()));    
    //Update
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t = QImage(reload_xpm);
    QAction *actUpdate = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Refresh"),this);
    actUpdate->setShortcut(Qt::Key_F5);
    actUpdate->setToolTip(mod->I18N("Refresh current page"));
    actUpdate->setWhatsThis(mod->I18N("Button for refreshing a content of the current page."));
    connect(actUpdate, SIGNAL(activated()), this, SLOT(pageRefresh()));        
    //Start of "Auto update"
    if(!ico_t.load(TUIS::icoPath("start").c_str())) ico_t = QImage(start_xpm);
    actStartUpd = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Start"),this);
    actStartUpd->setShortcut(Qt::CTRL+Qt::Key_B);
    actStartUpd->setToolTip(mod->I18N("Start cycled refresh"));
    actStartUpd->setWhatsThis(mod->I18N("Button for start of cycled refresh content of the current page."));
    connect(actStartUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStart()));        
    //Stop of "Auto update"
    if(!ico_t.load(TUIS::icoPath("stop").c_str())) ico_t = QImage(stop_xpm);
    actStopUpd = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&Stop"),this);
    actStopUpd->setShortcut(Qt::CTRL+Qt::Key_E);
    actStopUpd->setToolTip(mod->I18N("Stop cycled refresh"));
    actStopUpd->setWhatsThis(mod->I18N("Button for stop of cycled refresh content of the current page."));
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStop()));        
    //About "System info"
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t = QImage(help_xpm);
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),mod->I18N("&About"),this);
    actAbout->setShortcut(Qt::Key_F1);
    actAbout->setToolTip(mod->I18N("Programm and OpenSCADA information"));
    actAbout->setWhatsThis(mod->I18N("Display programm and OpenSCADA information"));
    connect(actAbout, SIGNAL(activated()), this, SLOT(about()));
    //About Qt
    QAction *actQtAbout = new QAction(mod->I18N("About &Qt"),this);
    actQtAbout->setToolTip(mod->I18N("Qt information"));
    actQtAbout->setWhatsThis(mod->I18N("Using QT information"));
    connect(actQtAbout, SIGNAL(activated()), this, SLOT(aboutQt()));
    //What is
    if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t = QImage(contexthelp_xpm);    
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),mod->I18N("What's &This"),this);
    actWhatIs->setToolTip(mod->I18N("Question about GUI elements"));
    actWhatIs->setWhatsThis(mod->I18N("Get respond about user interface elements"));
    connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));
    
    //Create menu "file"
    QMenu *mn_file = menuBar()->addMenu(mod->I18N("&File"));
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    //Create menu "view"    
    QMenu *mn_view = menuBar()->addMenu(mod->I18N("&View"));    
    mn_view->addAction(actUpdate);
    mn_view->addAction(actStartUpd);
    mn_view->addAction(actStopUpd);
    //Create menu "go"    
    QMenu *mn_go = menuBar()->addMenu(mod->I18N("&Go"));    
    mn_go->addAction(actUp);
    mn_go->addAction(actPrev);
    mn_go->addAction(actNext);
    //Create menu "help"
    QMenu *help = menuBar()->addMenu(mod->I18N("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addSeparator();
    help->addAction(actWhatIs);
    
    //Tool bar
    QToolBar *toolBar = new QToolBar(mod->I18N("OpenSCADA toolbar"),this);
    addToolBar(toolBar);
    toolBar->addAction(actUp);
    toolBar->addAction(actPrev);
    toolBar->addAction(actNext);
    toolBar->addSeparator();
    toolBar->addAction(actUpdate);
    toolBar->addAction(actStartUpd);
    toolBar->addAction(actStopUpd);
    
    //Display resource
    hd_res = ResAlloc::resCreate();
    
    //Create auto update timer
    autoUpdTimer = new QTimer( this );
    connect( autoUpdTimer, SIGNAL(timeout()), SLOT(pageRefresh()) );
    
    //Display root page and init external pages
    initHosts();
    try{ pageDisplay(mod->startPath()); } 
    catch(TError err) { postMess(err.cat,err.mess,4); }    
}

ConfApp::~ConfApp()
{
    ResAlloc::resDelete(hd_res);
    mod->unregWin( this );
}

void ConfApp::quitSt()
{
    SYS->stop();
}

void ConfApp::pageUp()
{
    int i_l = string::npos;
    while(true)
    {
	i_l = sel_path.rfind("/",i_l);
	if( i_l == string::npos || i_l == 0 ) return;
	if(sel_path.substr(i_l).size()>1)	break;
	i_l--;
    }
    //Prev and next
    if( sel_path.size() )	prev.insert(prev.begin(),sel_path);
    if( prev.size() >= que_sz )	prev.pop_back();
    next.clear();
    
    try{ pageDisplay( sel_path.substr(0,i_l) );	} 
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::pagePrev()
{
    if( !prev.size() ) return;
    next.insert(next.begin(),sel_path);
    string path = prev[0];
    prev.erase(prev.begin());
    
    try{ pageDisplay( path ); } catch(TError err) { postMess(err.cat,err.mess,4); }   
}

void ConfApp::pageNext()
{
    if( !next.size() ) return;
    prev.insert(prev.begin(),sel_path);
    string path = next[0];
    next.erase(next.begin()); 
    
    try{ pageDisplay( path ); } catch(TError err) { postMess(err.cat,err.mess,4); }       
}

void ConfApp::userSel()
{
    vector<string> u_list;
	
    DlgUser *d_usr = new DlgUser( );
    SYS->security().at().usrList(u_list);
    d_usr->user(u_list);
    int rez = d_usr->exec();
    string dl_user   = d_usr->user().toAscii().data();
    string dl_passwd = d_usr->password().toAscii().data();
    delete d_usr;    
    
    if( rez && dl_user != w_user->text().toAscii().data() )
    {
	try
	{
	    if( SYS->security().at().usrAt(dl_user).at().auth(dl_passwd) ) 
	    {
		w_user->setText( dl_user.c_str() );
		QPalette palette;
		if( dl_user == "root" ) palette.setColor(w_user->backgroundRole(),QColor(255,0,0));
		else 			palette.setColor(w_user->backgroundRole(),QColor(0,255,0));
		w_user->setPalette(palette);
		pg_info.attr("path",""); //sel_path="";	//Clear last path for correct user change
		pageDisplay( mod->startPath() ); // getItemPath( CtrTree->firstChild() ) );
		initHosts();
	    }
	    else postMess(mod->nodePath(),mod->I18N("Auth wrong!!!"),2);
	} catch(TError err) { postMess(err.cat,err.mess,4); }       
    }    
}

void ConfApp::pageRefresh()
{
    try{ pageDisplay(sel_path); }
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::pageCyclRefrStart()
{
    actStartUpd->setEnabled(false);
    actStopUpd->setEnabled(true);

    autoUpdTimer->setSingleShot(false);
    autoUpdTimer->start(1000);
}

void ConfApp::pageCyclRefrStop()
{
    actStopUpd->setEnabled(false);
    actStartUpd->setEnabled(true);
        
    autoUpdTimer->stop();
}

void ConfApp::about()
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),mod->I18N(
        "%s v%s.\n"
	"Autor: %s\n"
	"License: %s\n"),
        mod->modInfo("Name").c_str(), mod->modInfo("Version").c_str(), mod->modInfo("Author").c_str(), mod->modInfo("License").c_str() );
    
    QMessageBox::about(this,windowTitle(),buf);
}

void ConfApp::aboutQt()
{
    QMessageBox::aboutQt( this, mod->modInfo("Name").c_str() );
}

void ConfApp::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void ConfApp::selectItem( )
{    
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if(sel_ls.size() != 1)	return;
    
    try
    { 
	if(sel_path != sel_ls.at(0)->text(2).toAscii().data())
	{
    	    //Prev and next
	    if( sel_path.size() )	prev.insert(prev.begin(),sel_path);
	    if( prev.size() >= que_sz )	prev.pop_back();
	    next.clear();

	    pageDisplay( sel_ls.at(0)->text(2).toAscii().data() );
	}
	
	if(!sel_ls.at(0)->parent() && !CtrTree->isItemExpanded(sel_ls.at(0)))
	    viewChild(sel_ls.at(0));
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget, bool refr )
{
    QVBoxLayout *layout;
    
    //For abbend no named widgets
    QHBoxLayout *l_hbox = NULL;    
    int		i_area = 0;	//Areas counter
    int 	l_pos = 0;

    //View title name
    
    if( a_path == "/" ) 
    {
	//Set node icon
	if( node.childGet("id","ico",true) )
        {
	    XMLNode dt_req("get");
	    dt_req.attr("path",sel_path+"/"+TSYS::strEncode(a_path+"ico",TSYS::PathEl))->attr("user",w_user->text().toAscii().data());
    	    if( cntrIfCmd(dt_req) ) postMess(dt_req.attr("mcat"),dt_req.text(),4);
	    else
	    {
    		string simg = TSYS::strDecode(dt_req.text(),TSYS::base64);
		QImage img;
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    titleIco->setPixmap(QPixmap::fromImage(img.scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		else titleIco->clear();	
	    }
        }else titleIco->clear();
	
	//Set title
	titleLab->setText((string("<p align='center'><i><b>")+TSYS::strEncode(node.attr("dscr"),TSYS::Html)+"</b></i></p>").c_str());

	//Delete tabs of deleted areas
	for( int i_tbs = 0; i_tbs < tabs->count(); i_tbs++ )
	{
	    unsigned i_cf;
	    for( i_cf = 0; i_cf < node.childSize(); i_cf++)
		if( node.childGet(i_cf)->name() == "area" &&
			tabs->tabText(i_tbs) == node.childGet(i_cf)->attr("dscr").c_str() )
		    break;
	    if( i_cf >= node.childSize() )
	    {
		block_tabs = true;
		if( i_tbs == tabs->currentIndex() )
		    node.childGet(i_tbs)->attr("qview","0");
		QWidget *tab = tabs->widget(i_tbs);
        	tabs->removeTab(i_tbs);
		delete tab;
		i_tbs--;
		block_tabs = false;
	    }
	}
	//Add new tabs
	for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++)
	{
	    XMLNode &t_s = *node.childGet(i_cf);
	    if( t_s.name() != "area" )	continue;
	    
	    unsigned i_tbs;
    	    for( i_tbs = 0; i_tbs < tabs->count(); i_tbs++ )
		if( tabs->tabText(i_tbs) == t_s.attr("dscr").c_str() )
		    break;
	    if( i_tbs >= tabs->count() )
	    {
		QScrollArea *scrl = new QScrollArea();
                tabs->insertTab(i_area, scrl, t_s.attr("dscr").c_str() );
		t_s.attr("qview","0");
	    }

	    //Find and prepare curent tabs
	    if( tabs->currentIndex() == i_area )
            {
		bool refresh = atoi(t_s.attr("qview").c_str());
		QScrollArea *scrl = (QScrollArea *)tabs->widget(i_area);
		QWidget *wdg = scrl->widget();
		if( !refresh )
		{
		    if( wdg != NULL ) delete wdg;
		
		    wdg = new QFrame(scrl);		
		    QVBoxLayout *wdg_lay = new QVBoxLayout(wdg);
		    wdg_lay->setSpacing(3);
		    wdg_lay->setAlignment( Qt::AlignTop );
		
		    selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refresh);
    		    wdg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
		    scrl->setWidget(wdg);
		    scrl->setWidgetResizable(true);
		    
		    wdg->show();
                    //tabs->showPage(tabs->currentWidget());				    
				
		    //Mark last drawed tabs       	    
		    t_s.attr("qview","1");		
		}
		else selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refresh);		
            } 
	    //else t_s.attr("qview","0");	//Mark no view tabs
	    i_area++;
	}
	return;
    }
    else
    {
	//Read node tree and create widgets
	for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++)
	{	    
	    XMLNode &t_s = *node.childGet(i_cf);
    
    	    //Check access to node
    	    bool wr = false;	
	    if( atoi(t_s.attr("acs").c_str())&SEQ_WR ) wr = true;
	
	    //View areas
	    if( t_s.name() == "area" )
	    {
		QWidget *wdg;
		if( !refr )
		{
		    wdg = new QGroupBox( t_s.attr("dscr").c_str(), widget );
		    QVBoxLayout *w_lay = new QVBoxLayout(wdg);
		    //((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical );
       		    w_lay->setSpacing( 3 );
		    w_lay->setMargin( 3 );
		    //w_lay->setAlignment( Qt::AlignTop );

		    widget->layout()->addWidget(wdg);
		}else wdg = widget;
		selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refr);
	    }	
	    //View list elements
	    else if( t_s.name() == "list" )
	    {
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);
		
		QLabel *lab;
		QListWidget *lstbox;
		
		if( !refr )
		{
		    lstbox = new QListWidget(widget);
		    lstbox->setStatusTip((sel_path+"/"+br_path).c_str());
		    lstbox->setObjectName(br_path.c_str());
		    lstbox->setMinimumSize( QSize( 150, 110 ) );
		    lstbox->setContextMenuPolicy(Qt::CustomContextMenu);
    		    connect( lstbox, SIGNAL( customContextMenuRequested(const QPoint&) ),
    		    	this, SLOT( listBoxPopup() ) );
    		    //Go branche signal connect
    		    if( t_s.attr("tp") == "br" )
    			connect( lstbox, SIGNAL( itemDoubleClicked(QListWidgetItem*) ),
    			    this, SLOT( listBoxGo(QListWidgetItem*) ) );
    			
    		    lstbox->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );
	    
    		    QVBoxLayout *vbox = new QVBoxLayout;
		    lab = new QLabel(widget);
    		    vbox->addWidget(lab);
    		    QHBoxLayout *hbox = new QHBoxLayout;
    		    hbox->addWidget( lstbox );
    		    hbox->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    		    vbox->addLayout(hbox);
    		    widget->layout()->addItem(vbox);
		    
		    t_s.attr("addr_lab",addr2str(lab));
		    t_s.attr("addr_el",addr2str(lstbox));
		}
		else
		{
		    lab    = (QLabel *)str2addr(t_s.attr("addr_lab"));
		    lstbox = (QListWidget *)str2addr(t_s.attr("addr_el"));
		    lstbox->clear();
		}
		//Fill list
		lab->setText((t_s.attr("dscr")+":").c_str());
		XMLNode dt_req("get");
		dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text().toAscii().data());
		if(cntrIfCmd(dt_req)) postMess(dt_req.attr("mcat"),dt_req.text(),4);
		else
		    for( unsigned i_el = 0; i_el < dt_req.childSize(); i_el++ )
                	if( dt_req.childGet(i_el)->name() == "el")
                    	    lstbox->addItem(dt_req.childGet(i_el)->text().c_str());
    	    }
	    //View table elements
	    else if( t_s.name() == "table" )
	    {
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);
		
		//QLabel *lab;
		QTableWidget *tbl;
		
		if( !refr )
		{		  
		    tbl = new QTableWidget(widget);		    
		    tbl->setItemDelegate(new ComboBoxDelegate);
		    tbl->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		    tbl->setStatusTip((sel_path+"/"+br_path).c_str());
		    tbl->setObjectName(br_path.c_str());
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
		    sp.setVerticalStretch(1);
		    tbl->setSizePolicy( sp );
		    tbl->setContextMenuPolicy(Qt::CustomContextMenu);
    		    connect( tbl, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( tablePopup(const QPoint&) ) );
    		    connect( tbl, SIGNAL( cellChanged(int,int) ), this, SLOT( tableSet(int,int) ) );
		    tbl->setMinimumSize( QSize( 100, 100 ) );
		    tbl->setMaximumSize( QSize( 32767, 300 ) );
		    
		    widget->layout()->addWidget( new QLabel((t_s.attr("dscr")+":").c_str(),widget) );
		    widget->layout()->addWidget( tbl );
		    
		    //t_s.attr("addr_lab",addr2str(lab));
		    t_s.attr("addr_tbl",addr2str(tbl));
		}
		else
		{
		    //lab = (QLabel *)str2addr(t_s.attr("addr_lab"));
		    tbl = (QTableWidget *)str2addr(t_s.attr("addr_tbl"));
		}		    
		//Fill tasetNumRowsble
                XMLNode dt_req("get");
		dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text().toAscii().data());
	        if(cntrIfCmd(dt_req)) postMess(dt_req.attr("mcat"),dt_req.text(),4);		
		else
		{
		    //Copy values to info tree
		    for( int i_col = 0; i_col < dt_req.childSize(); i_col++ )
		    {
			XMLNode *t_lsel = dt_req.childGet(i_col);
			XMLNode *t_linf = t_s.childGet("id",t_lsel->attr("id"),true);
			if(!t_linf) continue;
			t_linf->childClean();
			for( int i_rw = 0; i_rw < t_lsel->childSize(); i_rw++ )
			    *t_linf->childAdd() = *t_lsel->childGet(i_rw);
		    }	
		
		    //Calc rows and columns
		    int n_col = t_s.childSize();
		    int n_row = (n_col)?t_s.childGet(0)->childSize():0;
		    
		    if( tbl->columnCount() != n_col )	tbl->setColumnCount(n_col);
		    if( tbl->rowCount() != n_row )	tbl->setRowCount(n_row);
		    
		    for( unsigned i_lst = 0; i_lst < t_s.childSize(); i_lst++ )	    
		    {
			XMLNode *t_linf = t_s.childGet(i_lst);
			if(!t_linf) continue;
			bool c_wr = wr && (atoi(t_linf->attr("acs").c_str())&SEQ_WR);
		    
			QTableWidgetItem *thd_it = tbl->horizontalHeaderItem(i_lst);
			if( !thd_it ) 
			{
			    thd_it = new QTableWidgetItem("");
			    tbl->setHorizontalHeaderItem(i_lst,thd_it);
			}
			thd_it->setData(Qt::DisplayRole,t_linf->attr("dscr").c_str());
			
			//Set elements
			tbl_init = true;
			for( int i_el = 0; i_el < t_linf->childSize(); i_el++ )
			{
			    thd_it = tbl->item(i_el,i_lst);
			    if( !thd_it )
	                    {
	                        thd_it = new QTableWidgetItem("");
	                        tbl->setItem(i_el,i_lst,thd_it);
                            }
			
			    //Set element
			    if( t_linf->attr("tp") == "bool" )
				thd_it->setData(Qt::DisplayRole,(bool)atoi(t_linf->childGet(i_el)->text().c_str()));
			    else if( t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed" )
			    {
				QStringList elms;
				int sel_n = -1;
				bool u_ind = atoi(t_linf->attr("idm").c_str());
				
				XMLNode x_lst("get");
				x_lst.attr("path",sel_path+"/"+TSYS::strEncode( t_linf->attr("select"),TSYS::PathEl))->attr("user",w_user->text().toAscii().data());
				if(cntrIfCmd(x_lst)) postMess(x_lst.attr("mcat"),x_lst.text(),4);
				else
				    for( int i_ls = 0; i_ls < x_lst.childSize(); i_ls++ )
				    {
					elms+=x_lst.childGet(i_ls)->text().c_str();
					if( (u_ind && x_lst.childGet(i_ls)->attr("id") == t_linf->childGet(i_el)->text()) ||
						(!u_ind && x_lst.childGet(i_ls)->text() == t_linf->childGet(i_el)->text()) )
					    sel_n = i_ls;
				    }
				if( sel_n < 0 )
				{
				    elms.insert(elms.begin(),"");
				    sel_n = 0;
				}
			    
				thd_it->setData(Qt::DisplayRole,elms.at(sel_n));
				thd_it->setData(Qt::UserRole,elms);
			    }
			    else if( t_linf->attr("tp") == "time" )
			    {
				time_t tm_t = atoi(t_linf->childGet(i_el)->text().c_str());
				char *c_tm = ctime( &tm_t );
				for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
				    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
				thd_it->setData(Qt::DisplayRole,c_tm);
			    }
			    else thd_it->setData(Qt::DisplayRole,t_linf->childGet(i_el)->text().c_str());
			    //Set access
			    
			    if( !c_wr )	thd_it->setFlags(Qt::ItemIsEnabled);
			    else thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
			}
    		    }
		    if(!refr) 
		    {
			tbl->resizeColumnsToContents();
			//Resize too long columns
			int max_col_sz = vmax(700/tbl->columnCount(),50);
			for( int i_c = 0; i_c < tbl->columnCount(); i_c++ )
			    tbl->setColumnWidth(i_c,vmin(max_col_sz,tbl->columnWidth(i_c)));
		    }
		    
		    tbl_init = false;
		}
	    }
	    //View images
	    else if( t_s.name() == "img" )
	    {
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);
		
		QLabel *lab;
		ImgView *img;
		
		if( !refr )
		{
		    img = new ImgView(widget,0,atoi(t_s.attr("h_sz").c_str()),atoi(t_s.attr("v_sz").c_str()));
		    img->setObjectName(br_path.c_str());
		    img->setStatusTip((sel_path+"/"+br_path).c_str());
    		    img->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
		    img->setMinimumSize(200,200);
		    img->setContextMenuPolicy(Qt::CustomContextMenu);
    		    connect( img, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( imgPopup(const QPoint&) ) );
	    
		    int vsz = atoi(t_s.attr("v_sz").c_str());
    		    QBoxLayout *box = new QBoxLayout((vsz&&vsz<70)?QBoxLayout::LeftToRight:QBoxLayout::TopToBottom);
		    lab = new QLabel(widget);
    		    box->addWidget(lab);
    		    box->addWidget(img);
		    if(box->direction()==QBoxLayout::LeftToRight) 
		    {
			lab->setAlignment( Qt::AlignTop );
			box->setSpacing(6);
			box->addItem(new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
		    }
    		    widget->layout()->addItem(box);
		    
		    t_s.attr("addr_lab",addr2str(lab));
		    t_s.attr("addr_el",addr2str(img));
		}
		else
		{
		    lab = (QLabel *)str2addr(t_s.attr("addr_lab"));
		    img = (ImgView *)str2addr(t_s.attr("addr_el"));
		}
		//Set image
		lab->setText((t_s.attr("dscr")+":").c_str());
		
		XMLNode dt_req("get");
                dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text().toAscii().data());
                if(cntrIfCmd(dt_req)) postMess(dt_req.attr("mcat"),dt_req.text(),4);
		else img->setImage(TSYS::strDecode(dt_req.text(),TSYS::base64));
	    }	    	
	    //View standart fields
	    else if( t_s.name() == "fld" )
		basicFields( t_s, a_path, widget, wr, &l_hbox, l_pos, refr );
	    else if( t_s.name() == "comm" )
	    {
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

		QPushButton *button;
		QGroupBox *comm_pan = NULL;
		QHBoxLayout *c_hbox = NULL;		
		int c_pos = 0;

		if( !refr )
		{		
		    if( !t_s.childSize() )
		    {
			button = new QPushButton(widget);
			button->setObjectName(br_path.c_str());
			button->setStatusTip((sel_path+"/"+br_path).c_str());
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
			button->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
			widget->layout()->addWidget( button );
		    }
		    else
		    {	

			comm_pan = new QGroupBox( t_s.attr("dscr").c_str(), widget );
			QVBoxLayout *comm_lay = new QVBoxLayout(comm_pan);
		        //comm_pan->setColumnLayout(0, Qt::Vertical );
	       	    	//comm_lay->setMargin( 6 );
			comm_lay->setSpacing( 3 );
			comm_lay->setAlignment( Qt::AlignTop );
		    
			button = new QPushButton(comm_pan);
			button->setObjectName(br_path.c_str());
			button->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );		
		
			comm_lay->addWidget(button);
			widget->layout()->addWidget(comm_pan);
		    }
		    
		    t_s.attr("addr_butt",addr2str(button));
		}
		else 
		    button = (QPushButton *)str2addr(t_s.attr("addr_butt"));
		
		//Update or create parameters
		for( unsigned i_cf = 0; i_cf < t_s.childSize(); i_cf++)
		{
		    XMLNode &t_scm = *t_s.childGet(i_cf);
		    if( t_scm.name() == "fld" )
			basicFields( t_scm, a_path+t_s.attr("id")+'/', comm_pan, true, &c_hbox, c_pos, refr, true);
		}
		    
		//Fill command
		button->setText(t_s.attr("dscr").c_str());
	    }	
	}
    }
}

void ConfApp::basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool refr, bool comm )
{    
    string br_path = TSYS::strEncode( string((comm)?"b":"")+a_path+t_s.attr("id"),TSYS::PathEl);

    XMLNode data_req("get");
    if( !comm )
    {
        data_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text().toAscii().data());
        if(cntrIfCmd(data_req))
	{ 
	    postMess(data_req.attr("mcat"),data_req.text(),4);
	    data_req.text("");
	}
    }

    //View select fields
    if( (t_s.attr("dest") == "select" || t_s.attr("dest") == "sel_ed") )
    {	
	QLabel *lab     = NULL;
	QLabel *lab_r	= NULL;
        QComboBox *comb	= NULL;
	
	if( !refr )
	{   
	    if(wr)
	    { 	
		comb = new QComboBox(widget);
		comb->setObjectName(br_path.c_str());
		comb->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		comb->setStatusTip((sel_path+"/"+br_path).c_str());
		if( t_s.attr("dest") == "sel_ed" )
		    comb->setEditable(true);
		comb->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed) );	    
	    
		connect( comb, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
	    }
	    else
	    {
		lab_r = new QLabel( widget );
                lab_r->setStatusTip((sel_path+"/"+br_path).c_str());
	    }
	    
	    if(t_s.attr("dscr").size()) 
	    {
	        *l_hbox = new QHBoxLayout; l_pos = 0;
	        (*l_hbox)->setSpacing(6);
		lab = new QLabel(widget);
		(*l_hbox)->insertWidget( l_pos++, lab );
		if(comb)	(*l_hbox)->insertWidget( l_pos++, comb );
		if(lab_r) 	(*l_hbox)->insertWidget( l_pos++, lab_r );
		(*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem( *l_hbox );		
	    }
	    else
	    {
		if(comb)
		{
		    if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, comb );	
		    else delete comb;
		}
		if(lab_r)
		{
		    if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, lab_r );
                    else delete lab_r;
		}
	    }
	    
	    t_s.attr("addr_lab",addr2str(lab));
	    t_s.attr("addr_comb",addr2str(comb));
	    t_s.attr("addr_lab_r",addr2str(lab_r));
	}
	else
	{
	    lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
	    lab_r = (QLabel *)str2addr(t_s.attr("addr_lab_r"));
    	    comb = (QComboBox *)str2addr(t_s.attr("addr_comb"));
	    if(comb)	comb->clear();
	}	    
	//Fill combo
	if( lab ) lab->setText((t_s.attr("dscr")+":").c_str());	
	
	XMLNode x_lst("get");
	x_lst.attr("path",sel_path+"/"+TSYS::strEncode( t_s.attr("select"),TSYS::PathEl))->attr("user",w_user->text().toAscii().data());	
	if(cntrIfCmd(x_lst)) postMess(x_lst.attr("mcat"),x_lst.text(),4);
	else
	{
    	    bool sel_ok = false;
    	    unsigned i_el,c_el;
    	    for( i_el = 0, c_el = 0; i_el < x_lst.childSize(); i_el++ )
    		if( x_lst.childGet(i_el)->name() == "el")
    		{
        	    if(comb)	comb->insertItem(c_el++, x_lst.childGet(i_el)->text().c_str() );
	    	    bool ind_ok = x_lst.childGet(i_el)->attr("id").size();	//Index present
        	    if( (ind_ok && x_lst.childGet(i_el)->attr("id") == data_req.text()) || 
			    (!ind_ok && x_lst.childGet(i_el)->text() == data_req.text()) )
            	    {
                	sel_ok = true;
                	if(comb) 	comb->setCurrentIndex( c_el-1 );
			if(lab_r)	lab_r->setText((string("<b>")+x_lst.childGet(i_el)->text()+"</b>").c_str());
            	    }
        	}
    	    //Insert empty field if none selected
    	    if( !sel_ok )
    	    {
		if(comb)
		{
        	    comb->insertItem(c_el,"");
    		    comb->setCurrentIndex(c_el);
		}
		if(lab_r)	lab_r->setText("");
	    }
	}
    }	    
    else
    {		    
	//View boolean fields
	if( t_s.attr("tp") == "bool" )
	{
            QLabel *lab	    	= NULL;
	    QLabel *lab_r   	= NULL;
            QCheckBox *chBox 	= NULL;
	    
	    if( !refr )
	    {	    
		if(wr)
		{		
		    chBox  = new QCheckBox(widget);
		    chBox->setObjectName(br_path.c_str());
		    chBox->setStatusTip((sel_path+"/"+br_path).c_str());
		    connect( chBox, SIGNAL( stateChanged(int) ), this, SLOT( checkBoxStChange(int) ) );
		    //if(!wr)	chBox->setDisabled(true);
		}
		else
        	{
		    lab_r = new QLabel( widget );
		    lab_r->setStatusTip((sel_path+"/"+br_path).c_str());
		}

		if(t_s.attr("dscr").size()) 
		{
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    lab = new QLabel(widget);		    
    		    (*l_hbox)->insertWidget( l_pos++, lab );
    		    if(chBox)	(*l_hbox)->insertWidget( l_pos++, chBox );
		    if(lab_r)	(*l_hbox)->insertWidget( l_pos++, lab_r );
    		    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    		    widget->layout()->addItem( *l_hbox );
		}
		else
		{
		    if(chBox)
		    {
			if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, chBox );
			else delete chBox;
		    }
		    if(lab_r)
                    {
                        if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, lab_r );
                	else delete lab_r;
		    }
		}	
		
		t_s.attr("addr_lab",addr2str(lab));
		t_s.attr("addr_chb",addr2str(chBox));
		t_s.attr("addr_lab_r",addr2str(lab_r));
	    }
	    else		
	    { 
		lab = (QLabel *)str2addr(t_s.attr("addr_lab"));
		chBox = (QCheckBox *)str2addr(t_s.attr("addr_chb"));
		lab_r = (QLabel *)str2addr(t_s.attr("addr_lab_r"));
	    }
	    
	    //Fill CheckBox
	    if( lab )	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( chBox ) chBox->setChecked(atoi(data_req.text().c_str()));
	    if( lab_r )	lab_r->setText((string("<b>")+(atoi(data_req.text().c_str())?mod->I18N("On"):mod->I18N("Off"))+string("</b>")).c_str());
	}
	//View edit fields
	else if( t_s.attr("tp") == "str" && (t_s.attr("rows").size() || t_s.attr("cols").size()) )
	{
            QLabel *lab;
            TextEdit *edit;
	    
	    if( !refr )
	    {	
		lab = new QLabel(t_s.attr("dscr").c_str(),widget);
		widget->layout()->addWidget(lab);
		    
		edit = new TextEdit(widget,br_path.c_str());
		edit->setStatusTip((sel_path+"/"+br_path).c_str());
		if(atoi(t_s.attr("rows").c_str()) < 10)
		{
		    edit->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed) );
		    edit->edit()->setFixedHeight(2*edit->edit()->currentFont().pointSize()*atoi(t_s.attr("rows").c_str()));
		}
		else
		{
		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Expanding);
		    sp.setVerticalStretch(2);
		    edit->setSizePolicy( sp );
		    edit->edit()->setMinimumHeight(2*edit->edit()->currentFont().pointSize()*atoi(t_s.attr("rows").c_str()));		    
		}
		edit->edit()->setLineWrapMode(QTextEdit::NoWrap);
		widget->layout()->addWidget( edit );
		
		if( !wr )	edit->edit()->setReadOnly( true );    
		else
		{
		    connect( edit, SIGNAL( textChanged(const QString&) ), this, SLOT( editChange(const QString&) ) );
		    connect( edit, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		    connect( edit, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		}		    
	
		t_s.attr("addr_lab",addr2str(lab));
		t_s.attr("addr_edit",addr2str(edit));
	    }
	    else
	    {
		lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
		edit = (TextEdit *)str2addr(t_s.attr("addr_edit"));
	    }
	    //Fill Edit
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    edit->setText(data_req.text().c_str());
	}
        //View Data-Time fields
	else if( t_s.attr("tp") == "time" )
        {
            QLabel *lab   	 = NULL;
	    DateTimeEdit *val_w;
	    	    
	    struct tm *tm_tm;
	    time_t tm_t;
	    if( data_req.text().size() ) tm_t = atoi(data_req.text().c_str());
	    else tm_t = time(NULL);
	    tm_tm = localtime(&tm_t);

	    if( !refr )
	    {	    
		val_w = new DateTimeEdit( widget, comm );
		val_w->setObjectName(br_path.c_str());
		val_w->setStatusTip((sel_path+"/"+br_path).c_str());
	    	connect( val_w, SIGNAL( valueChanged(const QDateTime &) ), this, SLOT( dataTimeChange(const QDateTime&) ) );
		connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		if(!wr) val_w->setDisabled(true);

		//Check use label
		if(t_s.attr("dscr").size()) 
		{
		    lab = new QLabel(widget);
		    
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    (*l_hbox)->insertWidget( l_pos++, lab);
		    (*l_hbox)->insertWidget( l_pos++, val_w );
		    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		    widget->layout()->addItem(*l_hbox);
		}
		else
		{
		    if( *l_hbox )	(*l_hbox)->insertWidget( l_pos++, val_w );
		    else 	delete val_w;
		}
		
		t_s.attr("addr_lab",addr2str(lab));		
		t_s.attr("addr_dtw",addr2str(val_w));		
	    }
	    else
	    {
		lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
		val_w = (DateTimeEdit *)str2addr(t_s.attr("addr_dtw"));		
	    }	    
            //Fill data
 	    if( lab ) 	lab->setText((t_s.attr("dscr")+":").c_str());
	    val_w->setDateTime( QDateTime( QDate(tm_tm->tm_year+1900,tm_tm->tm_mon+1,tm_tm->tm_mday),
		    		QTime(tm_tm->tm_hour,tm_tm->tm_min,tm_tm->tm_sec)) );
	}
	//View other string and numberic fields	
	else
	{
            QLabel *lab   	= NULL;
	    QLabel *val_r 	= NULL;
	    LineEdit *val_w	= NULL;	    
	    if( !refr )
	    {	    
		//View info
		if( !wr )
		{
		    val_r = new QLabel( widget );
		    val_r->setStatusTip((sel_path+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy( sp );
		    val_r->setAlignment( Qt::AlignVCenter );
		    val_r->setWordWrap(true);
		}
		//View edit
		else
		{
		    val_w = new LineEdit( widget, comm );
		    val_w->setObjectName( br_path.c_str() );
		    val_w->setStatusTip((sel_path+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
		    sp.setHorizontalStretch(1);
		    val_w->setSizePolicy( sp );
		    connect( val_w, SIGNAL( textChanged(const QString&) ), this, SLOT( editChange(const QString&) ) );
		    connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		    connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		    
		    // addon parameters			
		    int val_n = atoi(t_s.attr("len").c_str());
		    if( val_n > 0 )	val_w->edit()->setMaxLength( val_n );
		
		    string tp = t_s.attr("tp");
		    if( tp == "dec" || tp == "hex" || tp == "oct" || tp == "real" )
		    {
			val_w->setFixedWidth( 5*15+30 );
			if( tp == "dec" )	val_w->edit()->setValidator( new QIntValidator(val_w->edit()) );
			else if( tp == "hex" ) 	val_w->edit()->setValidator( new QIntValidator(val_w->edit()) );
			else if( tp == "oct" )	val_w->edit()->setValidator( new QIntValidator(val_w->edit()) );
			else if( tp == "real" )	val_w->edit()->setValidator( new QDoubleValidator(val_w->edit()) );
		    }
		    else val_w->setMinimumWidth( 7*15+30 );
    		}
    		//Check use label
    		if(t_s.attr("dscr").size()) 
    		{
    		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
    		    lab = new QLabel(widget);
		    (*l_hbox)->insertWidget( l_pos++, lab );
		    if( val_w )	(*l_hbox)->insertWidget( l_pos++, val_w );
		    if( val_r )
		    { 
			(*l_hbox)->insertWidget( l_pos++, val_r );
			lab->setAlignment( Qt::AlignTop );
		    }
			
		    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		    widget->layout()->addItem(*l_hbox);
		}
		else
		{
		    if( *l_hbox ) 
		    {
			if( val_w ) 	(*l_hbox)->insertWidget( l_pos++, val_w );
			if( val_r ) 	(*l_hbox)->insertWidget( l_pos++, val_r );
		    }
		    else 
		    {
			if( val_w ) delete val_w;
			if( val_r ) delete val_r;
		    }
		}
		
		t_s.attr("addr_lab",addr2str(lab));		
		t_s.attr("addr_lew",addr2str(val_w));		
		t_s.attr("addr_ler",addr2str(val_r));
	    }
	    else
	    {
		lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
		val_r = (QLabel *)str2addr(t_s.attr("addr_ler"));
		val_w = (LineEdit *)str2addr(t_s.attr("addr_lew"));
	    }	    
            //Fill line
 	    if( lab ) 	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( val_r ) val_r->setText((string("<b>")+TSYS::strEncode(data_req.text(),TSYS::Html)+"</b>").c_str());
	    if( val_w )	val_w->setText(data_req.text().c_str());
	}
    }
}

void ConfApp::viewChild( QTreeWidgetItem * i )
{    
    try
    { 
	//Delete ViewItem childs
	while(i->childCount())	delete i->takeChild(0);
	viewChildRecArea(i,2);
	CtrTree->resizeColumnToContents(0);	
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::pageDisplay( const string &path )
{
    ResAlloc res(hd_res,true);    
    
    //Chek Up
    if( path.rfind("/") == string::npos || path.rfind("/") == 0 ) 
	actUp->setEnabled(false);
    else
	actUp->setEnabled(true);
    //Check Prev and Next
    if( prev.size() )	actPrev->setEnabled(true);	    
    else		actPrev->setEnabled(false);
    if( next.size() )	actNext->setEnabled(true);	    
    else		actNext->setEnabled(false);
    
    //Delete all tabs for new node
    if( path != pg_info.attr("path") ) //sel_path )
    {
	//Stop refresh
	pageCyclRefrStop();
    
    	block_tabs = true;
        while(tabs->widget(0))
        {
            QWidget *tab = tabs->widget(0);
            tabs->removeTab(0);
            delete tab;
        }	
    	block_tabs = false;
	sel_path = path;
	
	pg_info.childClean();
	pg_info.attr("path",sel_path)->attr("user",w_user->text().toAscii().data());
	if(cntrIfCmd(pg_info)) { postMess(pg_info.attr("mcat"),pg_info.text(),4); return; }
	root = pg_info.childGet(0);
    }
    else
    {    
	//Check the new node structure and the old node
	XMLNode n_node("info");
	n_node.attr("path",sel_path)->attr("user",w_user->text().toAscii().data());
	if(cntrIfCmd(n_node)) { postMess(n_node.attr("mcat"),n_node.text(),4); return; }
	upStruct(*root,*n_node.childGet(0));
    }	
    selectChildRecArea(*root,"/");
}

bool ConfApp::upStruct(XMLNode &w_nd, const XMLNode &n_nd)
{    
    bool str_ch = false;
    
    //Check access    
    if( w_nd.attr("acs") != n_nd.attr("acs") ) str_ch = true;
    
    //Scan deleted nodes    
    for( int i_w = 0; i_w < w_nd.childSize(); i_w++)
    {
	int i_n;
    	for( i_n = 0; i_n < n_nd.childSize(); i_n++)
	    if( w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id") )
		break;
	if( i_n >= n_nd.childSize() )
	{
	    w_nd.childDel(i_w--);
	    if( w_nd.name() != "table" && w_nd.name() != "list" ) str_ch = true;
	}
    }
    //Scan for new nodes and check present nodes
    for( int i_n = 0; i_n < n_nd.childSize(); i_n++)
    {	
	int i_w;
    	for( i_w = 0; i_w < w_nd.childSize(); i_w++)
	    if( w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id") )
		break;
	if( i_w >= w_nd.childSize() )	
	{
	    //Add node
	    *w_nd.childIns(i_n) = *n_nd.childGet(i_n);
	    str_ch = true;
	    i_w = i_n;
	}
	else 
	{
	    //Check present node
	    if( upStruct(*w_nd.childGet(i_w),*n_nd.childGet(i_n)) )	
		str_ch = true;
	    if( str_ch && w_nd.name() == "oscada_cntr" )
	    {
		w_nd.childGet(i_w)->attr_("qview","0");
		str_ch = false;
		continue;
	    }
	}
	//Check of the description present
	if( (bool)w_nd.childGet(i_w)->attr("dscr").size() ^ (bool)n_nd.childGet(i_n)->attr("dscr").size() )
	    str_ch = true;
	    
	//Check base fields destination change
	if( w_nd.childGet(i_w)->name() == "fld" && w_nd.childGet(i_w)->attr("dest") != n_nd.childGet(i_n)->attr("dest") )
	{
	    w_nd.childGet(i_w)->attr("dest","");
	    str_ch = true;
	}

	//Sync node parameters (text and atributes)
	w_nd.childGet(i_w)->text(n_nd.childGet(i_n)->text());
	vector<string> ls;
	n_nd.childGet(i_n)->attrList(ls);
	for( int i_a = 0; i_a < ls.size(); i_a++ )
	    w_nd.childGet(i_w)->attr(ls[i_a],n_nd.childGet(i_n)->attr(ls[i_a]));
    }
    
    return str_ch;
}

void ConfApp::onItem( QTreeWidgetItem * i )
{
    statusBar()->showMessage(i->text(2).toAscii().data(),10000);
}

void ConfApp::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    try
    {
	QAction *actRemHostUp = new QAction(mod->I18N("Update remote hosts list"),this);
	popup.addAction(actRemHostUp);
        QAction *rez = popup.exec(QCursor::pos());
	if( rez == actRemHostUp )	initHosts();
	popup.clear();
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::tabSelect(  QWidget * wdg )
{
    if( !block_tabs )
    {
	try{ pageDisplay( sel_path ); }
	catch(TError err) { postMess(err.cat,err.mess,4); }	    
    }
}

void ConfApp::viewChildRecArea( QTreeWidgetItem *i, int level )
{
    //Get icon
    string path = i->text(2).toAscii().data();
    if(path[0]=='*')	path = i->parent()->text(2).toAscii().data();
    if( i->text(2)[0] != '*' && i->icon(0).isNull() )
    {
	XMLNode dt_req("get");
	dt_req.attr("path",path+"/%2fico")->attr("user",w_user->text().toAscii().data());
        if( !cntrIfCmd(dt_req) )
	//else
	{    
	    string simg = TSYS::strDecode(dt_req.text(),TSYS::base64);
	    QImage img;
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
    		i->setIcon(0,QPixmap::fromImage(img));//.smoothScale(16,16)));
	}
    }
    
    //Get branches groups
    XMLNode br_req("info");
    br_req.attr("path",path+"/%2fbr")->attr("user",w_user->text().toAscii().data());
    
    if( cntrIfCmd(br_req) ) return;
    XMLNode *brs = br_req.childGet(0,true);
    if( !brs )	return;
    
    //Proccess branches groups
    for( int ibr = 0; ibr < brs->childSize(); ibr++ )
    {
	XMLNode *br = brs->childGet(ibr);
	QTreeWidgetItem *it = i;
	if( it->text(2)[0] == '*' )
	{
	    if( br->attr("id") != it->text(2).remove(0,1).toAscii().data() ) continue;
	}
	else if( brs->childSize() > 1 )
	{
	    it = new QTreeWidgetItem(i);
	    it->setText(0,(br->attr("dscr")+":").c_str());
	    it->setText(1,br->attr("dscr").c_str());
	    it->setText(2,(string("*")+br->attr("id")).c_str());
	    it->setFlags(Qt::ItemIsEnabled);
	    QFont fnt = it->font(0);
	    fnt.setItalic(true);
	    it->setFont(0,fnt);
    	}
	
	//Get branches
	XMLNode req("get");
	req.attr("path",path+"/"+TSYS::strEncode(br->attr("list"),TSYS::PathEl))->attr("user",w_user->text().toAscii().data());
	if( cntrIfCmd(req) ) { postMess(req.attr("mcat"),req.text(),4);	continue; }
	
	for( int i_lel = 0; i_lel < req.childSize(); i_lel++)
	{
	    XMLNode *br_el = req.childGet(i_lel);
	    //Check index-list
	    string br_path = br->attr("id");
	    if( br_el->attr("id").size() )	br_path.append(br_el->attr("id"));
	    else				br_path.append(br_el->text());
	    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
		    
	    QTreeWidgetItem *ch_it = new QTreeWidgetItem(it);
	    ch_it->setText(0,br_el->text().c_str());
	    ch_it->setText(1,br->attr("dscr").c_str());
	    ch_it->setText(2,(path+"/"+br_path).c_str());
			    			    
	    //Next node for next level
	    if( level-1 > 0 )	viewChildRecArea(ch_it,level-1);
	}
    }    
}

int ConfApp::cntrIfCmd( XMLNode &node )
{
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false);
    if( !station.size() ) station = sel_path = SYS->id();
    else node.attr("path",path.substr(station.size()+1));
    
    //- Check local station request -    
    if( station == SYS->id() )
    {
        SYS->cntrCmd(&node);
	node.attr("path",path);
	return atoi(node.attr("rez").c_str());
    }
    
    //- Check remote station request -
    ExtHost host = mod->extHostGet(w_user->text().toAscii().data(),station);
    if(!host.id.size() || !SYS->transport().at().modPresent(host.transp))
	throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error!"),station.c_str());
    //-- Check transport --
    string tr_nm = "RemStat"+station;
    if(!SYS->transport().at().at(host.transp).at().outPresent(tr_nm))
    {
	SYS->transport().at().at(host.transp).at().outAdd(tr_nm);
	SYS->transport().at().at(host.transp).at().outAt(tr_nm).at().addr(host.addr);
    }
    AutoHD<TTransportOut> tr = SYS->transport().at().at(host.transp).at().outAt(tr_nm);
    if(!tr.at().startStat())	tr.at().start();
    //-- Request to remote station --
    char buf[1000], buf1[256];
    string req, resp;
    int rez, resp_len;
    
    try
    {
        while(true)
	{
	    //--- Session open ---
	    if(host.ses_id < 0)
	    {
		req = "SES_OPEN "+host.user+" "+host.pass+"\n";
		resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),20);
		buf[resp_len] = 0;
		buf1[0] = 0;
		sscanf(buf,"REZ %d %255s\n",&rez,buf1);
		if(rez == 1)	throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> auth error: %s!"),station.c_str(),buf1);
    		else if(rez > 0)throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error: %s!"),station.c_str(),buf1);		
		host.ses_id = atoi(buf1);
		mod->extHostSet(host);
	    }
	    //--- Request ---
	    req = "REQ "+TSYS::int2str(host.ses_id)+"\n"+node.save();
	    buf[0] = 0;
	    resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),20);
	    resp.assign(buf,resp_len);
	    //Get head
	    buf1[0] = 0;
	    if(sscanf(resp.c_str(),"REZ %d %255s\n",&rez,buf1)<=0)
		throw TError(mod->nodePath().c_str(),mod->I18N("Station respond <%s> error!"),station.c_str());
	    if(rez == 1)	{ host.ses_id = -1; continue; }
	    if(rez > 0)	throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error: %d:%s!"),station.c_str(),rez,buf1);	    
	    int head_end = resp.find("\n",0);
	    if(head_end == string::npos)
		throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error: Respond broken!"),station.c_str());	    
	    int resp_size = atoi(buf1);	
	    //Wait tail
	    while(resp.size() < resp_size+head_end+sizeof('\n'))
	    {
		resp_len = tr.at().messIO(NULL,0,buf,sizeof(buf),20);
		resp.append(buf,resp_len);
	    }

	    node.load(resp.substr(head_end));
    	    node.attr("path",path);
	    if(!host.link_ok)
	    {
		host.link_ok = true; 
		mod->extHostSet(host);
	    }
	    return atoi(node.attr("rez").c_str());
	}
    }
    catch(TError err)
    {
	if(host.link_ok)
	{ 
	    host.link_ok = false; 
	    mod->extHostSet(host); 
	    tr.at().stop();
	}
	throw;
    }
}                          

void ConfApp::initHosts()
{   
    bool it_present = false;
    QTreeWidgetItem *nit = NULL;
    //- Remove no present hosts -
    for(int i_top = 0; i_top < CtrTree->topLevelItemCount(); i_top++)
    {
	nit = CtrTree->topLevelItem(i_top);
	if(SYS->id() == TSYS::pathLev(nit->text(2).toAscii().data(),0))
    	{
	    it_present = true;
    	    continue;
	}
	ExtHost host = mod->extHostGet(w_user->text().toAscii().data(),TSYS::pathLev(nit->text(2).toAscii().data(),0));
	if(host.id.size())	continue;
	delete CtrTree->takeTopLevelItem(i_top);
	i_top--;
    }	    
    
    //- Add new hosts -
    //-- Init local host --    
    if(!it_present)
    {
	nit = new QTreeWidgetItem(CtrTree);
	nit->setText(0,SYS->name().c_str());
	nit->setText(1,mod->I18N("Local station"));
	nit->setText(2,(string("/")+SYS->id()).c_str());
	//nit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    }	
    
    //-- Init remote hosts --
    vector<string> list;
    mod->extHostList(w_user->text().toAscii().data(),list);
    for(int i_h = 0; i_h < list.size(); i_h++)
    {
	ExtHost host = mod->extHostGet(w_user->text().toAscii().data(),list[i_h]);
	//--- Find alreadi present items ---
	it_present = false;
	for(int i_top = 0; i_top < CtrTree->topLevelItemCount(); i_top++)
	{
	    if(host.id != TSYS::pathLev(CtrTree->topLevelItem(i_top)->text(2).toAscii().data(),0))
		continue;
	    it_present = true;
	    break;
	}
	if(!it_present)
	{
	    nit = new QTreeWidgetItem(CtrTree);
	    nit->setText(0,host.name.c_str());
	    nit->setText(1,mod->I18N("Remote station"));
	    nit->setText(2,(string("/")+host.id).c_str());
	    //nit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	}
    }
}

void ConfApp::postMess( const string &cat, const string &mess, int type )
{
    //Put system message.
    Mess->put(cat.c_str(),(type==4)?TMess::Crit:(type==3)?TMess::Error:(type==2)?TMess::Warning:TMess::Info,"%s",mess.c_str());
    //QT message
    if( type == 1 )	QMessageBox::information( this,windowTitle(),mess.c_str());
    else if( type == 2 )QMessageBox::warning( this,windowTitle(),mess.c_str());
    else if( type == 3 )QMessageBox::critical( this,windowTitle(),mess.c_str());
    else if( type == 4 )QErrorMessage::qtHandler()->showMessage(mess.c_str());
}


//-------------------------------------------------------------
//-------------- Self widget's slots --------------------------
//-------------------------------------------------------------
void ConfApp::checkBoxStChange( int stat )
{
    XMLNode *n_el;
    QCheckBox *box = (QCheckBox *)sender();
    
    if(stat==Qt::PartiallyChecked) return;
    try
    { 
	string path = box->objectName().toAscii().data();
	string val = (stat==Qt::Checked)?"1":"0";
	//Check block element
	if(path[0] == 'b')
	{
	    SYS->ctrId(root,TSYS::strDecode(path.substr(1),TSYS::PathEl) )->text(val);
	    return;
	}
	else
	{
	    XMLNode req("get");
	    req.attr("path",sel_path+"/"+path)->attr("user",w_user->text().toAscii().data());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	    
	    
    	    if( req.text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Set <%s> to <%s>!"), w_user->text().toAscii().data(), 
		    (sel_path+"/"+path).c_str(), val.c_str() );    	    
	    
	    req.name("set"); req.text(val);
	    if(cntrIfCmd(req))	postMess(req.attr("mcat"),req.text(),4);
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::buttonClicked( )
{	
    QPushButton *button = (QPushButton *)sender();
    
    XMLNode req();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(button->objectName().toAscii().data(),TSYS::PathEl) );
	
	//Check link
	if( n_el->attr("tp") == "lnk")
	{
	    XMLNode req("get");
	    req.attr("path",sel_path+"/"+button->objectName().toAscii().data())->attr("user",w_user->text().toAscii().data());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }
	    
	    string url = "/"+TSYS::pathLev(sel_path,0)+req.text();
	    
    	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Go to link <%s>!"), w_user->text().toAscii().data(),url.c_str());
	    
	    //Prev and next
    	    if( sel_path.size() )       prev.insert(prev.begin(),sel_path);
	    if( prev.size() >= que_sz ) prev.pop_back();
	    next.clear();			
	    
	    pageDisplay( url );
	}
	else
	{
	    XMLNode req("set");
            req.attr("path",sel_path+"/"+button->objectName().toAscii().data())->attr("user",w_user->text().toAscii().data());
	    //Copy parameters
	    for( int i_ch = 0; i_ch < n_el->childSize(); i_ch++ )
		*(req.childAdd()) = *(n_el->childGet(i_ch));
	
 	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Press <%s>!"), w_user->text().toAscii().data(), 
		(sel_path+"/"+button->objectName().toAscii().data()).c_str() );
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::combBoxActivate( const QString& ival )
{
    bool block = false;
    string val = ival.toAscii().data();
    XMLNode *n_el;    
    QComboBox *comb = (QComboBox *)sender();    
    
    try
    {    
	string path = comb->objectName().toAscii().data();
	if(path[0] == 'b')
	{ 
	    block = true;
	    path = path.substr(1);
	}	    
	
	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );
	
        //Get list for index list check!
	if( n_el->attr("dest") == "select" )
	{
    	    bool find_ok = false;
    	    XMLNode x_lst("get");
	    x_lst.attr("path",sel_path+"/"+TSYS::strEncode( n_el->attr("select"),TSYS::PathEl))->attr("user",w_user->text().toAscii().data());
	    if(cntrIfCmd(x_lst)) { postMess(x_lst.attr("mcat"),x_lst.text(),4); return; }
	    
    	    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
    	    if( x_lst.childGet(i_el)->name() == "el" && x_lst.childGet(i_el)->text() == val )
    	    {
        	if( x_lst.childGet(i_el)->attr("id").size() )
        	    val = x_lst.childGet(i_el)->attr("id");
        	find_ok = true;
    	    }
    	    if( !find_ok ) { postMess(mod->nodePath().c_str(),mod->I18N("Value no valid: ")+val); return; }
	}
	
	//Check block element. Command box!
	if( block ) { n_el->text(val); return; }
	else
	{
	    XMLNode req("get");
	    req.attr("path",sel_path+"/"+path)->attr("user",w_user->text().toAscii().data());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }
	
    	    if( req.text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Change <%s> from <%s> to <%s>!"), 
		    w_user->text().toAscii().data(), (sel_path+"/"+path).c_str(), req.text().c_str(), val.c_str() );
    	    
	    req.name("set"); req.text(val);
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	    
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::listBoxPopup( )
{
    QMenu popup;
    QListWidget *lbox = (QListWidget *)sender();
    QListWidgetItem *item = lbox->currentItem();
    string el_path = sel_path+"/"+lbox->objectName().toAscii().data();
    XMLNode *n_el;

    QAction *last_it, *actBr, *actAdd, *actIns, *actEd, *actDel, *actMoveUp, *actMoveDown;
    last_it=actBr=actAdd=actIns=actEd=actDel=actMoveUp=actMoveDown=NULL;

    try
    {    
	n_el = SYS->ctrId(root,TSYS::strDecode(lbox->objectName().toAscii().data(),TSYS::PathEl) );
        if( n_el->attr("tp") == "br" && item != NULL )
        { 
	    actBr = last_it = new QAction(mod->I18N("Go"),this);
	    popup.addAction(actBr);
	    popup.addSeparator();
	}
	if( (atoi(n_el->attr("acs").c_str())&SEQ_WR) && n_el->attr("s_com").size() )
	{
	    if( n_el->attr("s_com").find("add") != string::npos )
	    {
	        actAdd = last_it = new QAction(mod->I18N("Add"),this); 
		popup.addAction(actAdd);
	    }
    	    if( n_el->attr("s_com").find("ins") != string::npos && item != NULL )
	    {		
    		actIns = last_it = new QAction(mod->I18N("Insert"),this); 
		popup.addAction(actIns);
	    }
    	    if( n_el->attr("s_com").find("edit") != string::npos && item != NULL )
	    {
    		actEd = last_it = new QAction(mod->I18N("Edit"),this); 
		popup.addAction(actEd);
	    }
    	    if( n_el->attr("s_com").find("del") != string::npos && item != NULL )
    	    { 
    		popup.addSeparator();
    		actDel = last_it = new QAction(mod->I18N("Delete"),this);
		popup.addAction(actDel);
    	    }
    	    if( n_el->attr("s_com").find("move") != string::npos && item != NULL )
    	    { 
    		popup.addSeparator();
		actMoveUp = last_it = new QAction(mod->I18N("Up"),this); 
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(mod->I18N("Down"),this);
		popup.addAction(actMoveDown);
    	    }
	}
	    
	if(last_it) 
	{
	    bool ok;
	    string p_text, p_id;
	    string text, id;	    
	    bool ind_m = atoi(n_el->attr("idm").c_str());
	    int  c_id  = lbox->currentRow();
	    
	    if( item != NULL )
	    {	    
		//Get select id
		XMLNode x_lst("get");
		x_lst.attr("path",el_path)->attr("user",w_user->text().toAscii().data());
		if(cntrIfCmd(x_lst)) { postMess(x_lst.attr("x_lst"),x_lst.text(),4); return; }
		
        	p_text = item->text().toAscii().data();
        	if( ind_m )
            	    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
            		if( x_lst.childGet(i_el)->text() == item->text().toAscii().data() )
                	{
                    	    p_id = x_lst.childGet(i_el)->attr("id");
                    	    break;
                	}
	    }
	    
	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }
	    if( rez == actAdd || rez == actIns || rez == actEd )
	    {
		InputDlg *dlg = new InputDlg(ind_m);
		if( rez == actEd )
		{
		    dlg->id(p_id.c_str());
		    dlg->name(p_text.c_str());
		}
		int dlrez = dlg->exec();
		id = dlg->id().toAscii().data();
		text = dlg->name().toAscii().data();
		delete dlg;
		if( dlrez != QDialog::Accepted ) return; 
	    }

	    //Make command	    
	    XMLNode n_el1;
	    n_el1.attr("path",el_path)->attr("user",w_user->text().toAscii().data())->text(p_text);
	    if( rez == actBr )
	    {
		listBoxGo(item);
		return;
	    }
	    else if( rez == actAdd )
	    {
		n_el1.name("add");
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Add <%s> element <%s:%s>!"), 
	    		w_user->text().toAscii().data(), el_path.c_str(), id.c_str(), text.c_str() );
	    }
	    else if( rez == actIns )
	    {
		n_el1.name("ins");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Insert <%s> element <%s:%s> to %d!"), 
	    		w_user->text().toAscii().data(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if( rez == actEd )
	    {
		n_el1.name("edit");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Set <%s> element %d to <%s:%s>!"), 
	    		w_user->text().toAscii().data(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if( rez == actDel )
	    {
		n_el1.name("del");
		n_el1.attr("pos",TSYS::int2str(c_id));
		if( ind_m ) n_el1.attr("id",p_id);
		else n_el1.text(item->text().toAscii().data());
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Delete <%s> element <%s:%s>!"), 
	    		w_user->text().toAscii().data(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if( rez == actMoveUp || rez == actMoveDown )
	    {
		int c_new = c_id-1;
		if( rez == actMoveDown )	c_new = c_id+1;		
		n_el1.name("move");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("to",TSYS::int2str(c_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Move <%s> from %d to %d!"), 
	    		w_user->text().toAscii().data(), el_path.c_str(), c_id, c_new);
	    }
	    if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }
	    
	    autoUpdTimer->setSingleShot(true);
	    autoUpdTimer->start(CH_REFR_TM);      //Redraw
	    
	    popup.clear();
	}
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	
	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(CH_REFR_TM);	//Redraw
    }
}

void ConfApp::tablePopup( const QPoint &pos )
{
    QMenu popup;
    QTableWidget *tbl = (QTableWidget *)sender();
    string el_path = sel_path+"/"+tbl->objectName().toAscii().data();
    
    QAction *last_it, *actAdd, *actIns, *actDel, *actMoveUp, *actMoveDown;
    last_it=actAdd=actIns=actDel=actMoveUp=actMoveDown=NULL;
    
    int row = tbl->currentRow();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(tbl->objectName().toAscii().data(),TSYS::PathEl) );
	
	if( (atoi(n_el->attr("acs").c_str())&SEQ_WR) && n_el->attr("s_com").size() )
	{
	    if( n_el->attr("s_com").find("add") != string::npos )
	    {
	        actAdd = last_it = new QAction(mod->I18N("Add record"),this);
		popup.addAction(actAdd);
	    }
    	    if( n_el->attr("s_com").find("ins") != string::npos && row != -1 )
	    {
		actIns = last_it = new QAction(mod->I18N("Insert record"),this);
		popup.addAction(actIns);
	    }
    	    if( n_el->attr("s_com").find("del") != string::npos && row != -1 )
	    {
		actDel = last_it = new QAction(mod->I18N("Delete record"),this);
		popup.addAction(actDel);
	    }
    	    if( n_el->attr("s_com").find("move") != string::npos && row != -1 )
	    {
    		popup.addSeparator();
		actMoveUp = last_it = new QAction(mod->I18N("Move Up"),this);
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(mod->I18N("Move Down"),this);
		popup.addAction(actMoveDown);
	    }
	}
	if(last_it) 
	{
	    bool ok;
	    QString text;
	    
	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	{ popup.clear(); return; }
	    
	    XMLNode n_el1;
	    n_el1.attr("path",el_path)->attr("user",w_user->text().toAscii().data());
	    if( rez == actAdd )
	    {
    		n_el1.name("add");
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Add <%s> record."),
			w_user->text().toAscii().data(), el_path.c_str() );
	    }
	    else if( rez == actIns )
	    {
		n_el1.name("ins");
	    	n_el1.attr("row",TSYS::int2str(row));
    		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Insert <%s> record %d."), 
			w_user->text().toAscii().data(), el_path.c_str(), row );
	    }
	    else if( rez == actDel )
	    {
		n_el1.name("del");
		string row_addr;
		if( !n_el->attr("key").size() )
		{
		    row_addr = TSYS::int2str(row);
		    n_el1.attr("row",row_addr);
		}
		else 
		{
		    //Get Key columns
            	    string key;
		    int i_key = 0;
		    while((key = TSYS::strSepParse(n_el->attr("key"),i_key++,',')).size())
		        for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
		            if( n_el->childGet(i_el)->attr("id") == key )
		            { 
				n_el1.attr("key_"+key,n_el->childGet(i_el)->childGet(row)->text()); 
				row_addr=row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
				break; 
			    }
		}    
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Delete <%s> record <%s>."), 
			w_user->text().toAscii().data(), el_path.c_str(), row_addr.c_str() );
	    }
	    else if( rez == actMoveUp || rez == actMoveDown )
	    {
		int r_new = row-1;
                if( rez == actMoveDown )  r_new = row+1;
		n_el1.name("move");
		n_el1.attr("row",TSYS::int2str(row))->attr("to",TSYS::int2str(r_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Move <%s> record from %d to %d."), 
			w_user->text().toAscii().data(), el_path.c_str(), row, r_new );
	    }
	    if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }
	    
	    autoUpdTimer->setSingleShot(true);
	    autoUpdTimer->start(CH_REFR_TM);
	    
	    popup.clear();
	}	
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	
	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(CH_REFR_TM);	//Redraw
    }
}

void ConfApp::imgPopup( const QPoint &pos )
{
    QMenu popup;
    ImgView *img = (ImgView *)sender();
    string el_path = sel_path+"/"+img->objectName().toAscii().data();
    
    QAction *last_it, *save_img, *load_img;
    last_it=save_img=load_img=NULL;
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(img->objectName().toAscii().data(),TSYS::PathEl) );
	if(!img->image().isNull())
	{
	    save_img = last_it = new QAction(mod->I18N("Save image"),this);
            popup.addAction(save_img);
	}
	if(atoi(n_el->attr("acs").c_str())&SEQ_WR)
	{
	    load_img = last_it = new QAction(mod->I18N("Load image"),this);
	    popup.addAction(load_img);
	}
	
	if(last_it)
        {
            QAction *rez = popup.exec(QCursor::pos());
            if(!rez)	return;
	    if( rez == save_img )
            {
		QString fileName = QFileDialog::getSaveFileName(this,mod->I18N("Save picture"),
			"img.png", mod->I18N("Images (*.png *.xpm *.jpg)"));
		if(!img->image().save(fileName)) 
		    throw TError(mod->nodePath().c_str(),mod->I18N("Save to file %s is error\n"),fileName.toAscii().data());
	    }
	    else if( rez == load_img )
	    {
		//- Get path to image file -
		QString fileName = QFileDialog::getOpenFileName(this,mod->I18N("Load picture"),"",mod->I18N("Images (*.png *.jpg)"));
		int len;
		char buf[STR_BUF_LEN];
		string rez;
		
		//- Load image file -
		int hd = open(fileName.toAscii().data(),O_RDONLY);
		if( hd < 0 )	throw TError(mod->nodePath().c_str(),mod->I18N("Open file %s is error\n"),fileName.toAscii().data());
		{
		    while( len = read(hd,buf,sizeof(buf)) )
		        rez.append(buf,len);
		    ::close(hd);
		}
		
		//- Set image to widget -
		if(!img->setImage(rez))	throw TError(mod->nodePath().c_str(),mod->I18N("Image file %s is error\n"),fileName.toAscii().data());
		
		//- Send image to system -
		XMLNode n_el1("set");
		n_el1.attr("path",el_path)->attr("user",w_user->text().toAscii().data())->text(TSYS::strEncode(rez,TSYS::base64));
	        Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Upload picture <%s> to: %s."),
	            w_user->text().toAscii().data(), fileName.toAscii().data(), el_path.c_str());
        	if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }
	    }
	}	
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	
	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(CH_REFR_TM);	//Redraw
    }
}

void ConfApp::tableSet( int row, int col )
{
    string value;
    if( tbl_init || row < 0 || col < 0 ) return;
    
    try
    {
	QTableWidget *tbl = (QTableWidget *)sender();
	string el_path = sel_path+"/"+tbl->objectName().toAscii().data();

	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(tbl->objectName().toAscii().data(),TSYS::PathEl) );	

	QVariant val = tbl->item(row,col)->data(Qt::EditRole);
	if( n_el->childGet(col)->attr("tp") == "bool" ) value = val.toBool()?"1":"0";
	else if( n_el->childGet(col)->attr("dest") == "select" )
	{
	    value = val.toString().toAscii().data();
	    bool find_ok = false;
	    XMLNode x_lst("get");
	    x_lst.attr("path",sel_path+"/"+TSYS::strEncode(n_el->childGet(col)->attr("select"),TSYS::PathEl))->attr("user",w_user->text().toAscii().data());
	    if(cntrIfCmd(x_lst)) { postMess(x_lst.attr("mcat"),x_lst.text(),4); return; }
	    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
		if( x_lst.childGet(i_el)->text() == value )
		{
		    if( atoi(n_el->childGet(col)->attr("idm").c_str()) )
			value = x_lst.childGet(i_el)->attr("id");
		    find_ok = true;
    		}
	    if( !find_ok ) throw TError(mod->nodePath().c_str(),mod->I18N("Value <%s> no valid!"),value.c_str());	
	}
	else value = val.toString().toAscii().data();
	
	//*** Prepare request ***
	XMLNode n_el1("set");
	n_el1.attr("path",el_path)->attr("user",w_user->text().toAscii().data())->text(value);
    	//Get current column id
     	for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
	    if( tbl->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString() == n_el->childGet(i_el)->attr("dscr").c_str() )
	    { n_el1.attr("col",n_el->childGet(i_el)->attr("id")); break; }
	//Get row position
	string row_addr;
	if( !n_el->attr("key").size() )
	{
	    row_addr=TSYS::int2str(row);
	    n_el1.attr("row",row_addr);
	}
	else
	{
	    //Get Key columns
	    string key;
	    int i_key = 0;	    
	    while((key = TSYS::strSepParse(n_el->attr("key"),i_key++,',')).size())
		for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
		    if( n_el->childGet(i_el)->attr("id") == key )
		    { 
			n_el1.attr("key_"+key,n_el->childGet(i_el)->childGet(row)->text());
			row_addr=row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
			break;
		    }
	}
    
	//Put request
	Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Set <%s> cell (<%s>:%s) to: %s."), 
	    w_user->text().toAscii().data(), el_path.c_str(), row_addr.c_str(), n_el1.attr("col").c_str(), value.c_str());
	if(cntrIfCmd(n_el1))	throw TError(n_el1.attr("mcat").c_str(),n_el1.text().c_str());    
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
    
    autoUpdTimer->setSingleShot(true);    
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::listBoxGo( QListWidgetItem* item )
{
    string path;
    
    if(!item)	return;
    QListWidget *lbox = item->listWidget();
    try
    { 	
        //Prev and next
	if( sel_path.size() )   	prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();	

	XMLNode &t_c = *TCntrNode::ctrId(root,TSYS::strDecode(lbox->objectName().toAscii().data(),TSYS::PathEl) );
	
	XMLNode req("get");
        req.attr("path",sel_path+"/"+lbox->objectName().toAscii().data())->attr("user",w_user->text().toAscii().data());
	if(cntrIfCmd(req)) throw TError(mod->nodePath().c_str(),"%s",req.text().c_str(),4);
	
	string br_pref = t_c.attr("br_pref");
	//Find selected index
        bool sel_ok = false;
        for( int i_el = 0; i_el < req.childSize(); i_el++ )
    	    if( req.childGet(i_el)->name() == "el" && req.childGet(i_el)->text() == item->text().toAscii().data() )
    	    {
		if( req.childGet(i_el)->attr("id").size() )
                    path = sel_path+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->attr("id"),TSYS::PathEl);
		else
                    path = sel_path+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->text(),TSYS::PathEl);
		sel_ok = true;
	    }
	if( !sel_ok ) throw TError(mod->nodePath().c_str(),mod->I18N("Select element <%s> no present!"),item->text().toAscii().data());
	
        //Prev and next
        if( sel_path.size() )       prev.insert(prev.begin(),sel_path);
        if( prev.size() >= que_sz ) prev.pop_back();
        next.clear();
	    
	sel_path = path;
	
	autoUpdTimer->setSingleShot(true);
	autoUpdTimer->start(CH_REFR_TM);
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::dataTimeChange( const QDateTime & qtm )
{
    XMLNode *n_el;    
    QDateTimeEdit *datat = (QDateTimeEdit *)sender();
    
    try
    {    
	//time_t c_tm = time(NULL);
	string path = datat->objectName().toAscii().data();

	struct tm tm_tm;
	//tm_tm = *localtime(&c_tm);
	//tm_tm.tm_isdst = 1;
	tm_tm.tm_year = qtm.date().year()-1900;
	tm_tm.tm_mon = qtm.date().month()-1;
	tm_tm.tm_mday = qtm.date().day();
	tm_tm.tm_hour = qtm.time().hour();
	tm_tm.tm_min = qtm.time().minute();
	tm_tm.tm_sec = qtm.time().second();
	tm_tm.tm_isdst = -1;
	string val = TSYS::int2str(mktime(&tm_tm));
	
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );    
    	n_el->text(val);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::editChange( const QString& txt )
{
    QWidget *wed = (QWidget *)sender();
    
    try
    {   
	string path = wed->objectName().toAscii().data();
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) )->text(txt.toAscii().data());
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::applyButton( )
{
    QWidget *bwidg = (QWidget *)sender();
    
    string path = bwidg->objectName().toAscii().data();

    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );    
	Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Change <%s> to: <%s>!"), 
		w_user->text().toAscii().data(), (sel_path+"/"+path).c_str(), n_el->text().c_str() );
	
	XMLNode n_el1("set");
	n_el1.attr("path",sel_path+"/"+path)->attr("user",w_user->text().toAscii().data())->text(n_el->text());
	if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }	
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::cancelButton( )
{
    //Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

//Address convert
string ConfApp::addr2str( void *addr )
{
    char buf[40];
    snprintf(buf,sizeof(buf),"%lX",addr);
    
    return buf;
}

void *ConfApp::str2addr( const string &str )
{
    return (void *)strtol(str.c_str(),NULL,16);
}

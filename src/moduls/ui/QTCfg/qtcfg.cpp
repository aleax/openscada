
//OpenSCADA system module UI.QTCfg file: qtcfg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <config.h>

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
#include <QListWidget>
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

#include <tmess.h>
#include <tsys.h>
#include <tsecurity.h>

#include "selfwidg.h"
#include "tuimod.h"
#include "qtcfg.h"

#define CH_REFR_TM 100

using namespace QTCFG;

//*************************************************
//* ConfApp                                       *
//*************************************************
ConfApp::ConfApp( string open_user ) :
    que_sz(20), pg_info("info"), root(&pg_info), tbl_init(false), copy_buf("0")
{
    //> Main window settings
    setAttribute(Qt::WA_DeleteOnClose,true);
    QImage ico_t;
    mod->regWin( this );

    setWindowTitle(_("QT Configurator of OpenSCADA"));
    setWindowIcon(mod->icon());

    //> Init centrall widget
    setCentralWidget( new QWidget(this) );
    QGridLayout *QTCfgLayout = new QGridLayout(centralWidget());
    QTCfgLayout->setMargin( 3 );
    //> Init splitter
    QSplitter *splitter = new QSplitter( centralWidget() );
    splitter->setOrientation( Qt::Horizontal );

    //> Create Navigator tree
    CtrTree = new QTreeWidget( splitter );
    CtrTree->setContextMenuPolicy(Qt::CustomContextMenu);
    CtrTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //splitter->setSizeConstraint(QSplitter::KeepSize);
    QStringList headerLabels;
    headerLabels << _("Name") << _("Type") << _("Path");
    CtrTree->setHeaderLabels(headerLabels);
    CtrTree->header()->setStretchLastSection(false);
    CtrTree->header()->resizeSection(0,200);
    //CtrTree->header()->setSectionHidden(1,true);
    //CtrTree->header()->setSectionHidden(2,true);
    //CtrTree->header()->hide();
    //CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->setMinimumSize( QSize( 150, 0 ) );
    CtrTree->setMaximumSize( QSize( 400, 32767 ) );
    CtrTree->setWhatsThis(_("The main navigation tree of the configurator."));
    //connect( CtrTree, SIGNAL( currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ), this, SLOT( selectItem(QTreeWidgetItem*) ) );
    connect( CtrTree, SIGNAL( itemSelectionChanged() ), this, SLOT( selectItem() ) );
    connect( CtrTree, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( viewChild(QTreeWidgetItem*) ) );
    //connect( CtrTree, SIGNAL( itemEntered(QTreeWidgetItem*,int) ), this, SLOT( onItem(QTreeWidgetItem*) ) );
    connect( CtrTree, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( ctrTreePopup() ) );

    //- Right frame add -
    QFrame *gFrame = new QFrame( splitter );
    gFrame->setWhatsThis(_("The main configurator frame."));
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
    //titleLab->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 1 );

    //- Create the tabulator -
    tabs = new QTabWidget( gFrame );
    tabs->setWhatsThis(_("Tabulated configuration pages."));
    connect( tabs, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabSelect(QWidget*) ) );
    gFrameLayout->addWidget( tabs, 1, 0, 1, 2 );

    QList<int> splSz;
    splSz.push_back(200);
    splSz.push_back(600);
    splitter->setSizes(splSz);
    QTCfgLayout->addWidget( splitter, 0, 0 );

    resize( 800, 600 );

    //- Create actions -
    //-- Close --
    if(!ico_t.load(TUIS::icoPath("close").c_str())) ico_t.load(":/images/close.png");
    QAction *actClose = new QAction(QPixmap::fromImage(ico_t),_("&Close"),this);
    actClose->setShortcut(Qt::CTRL+Qt::Key_W);
    actClose->setToolTip(_("Close configurator window"));
    actClose->setWhatsThis(_("The button for closing OpenSCADA configurator window"));
    actClose->setStatusTip(_("Press to close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //-- Quit --
    if(!ico_t.load(TUIS::icoPath("exit").c_str())) ico_t.load(":/images/exit.png");
    QAction *actQuit = new QAction(QPixmap::fromImage(ico_t),_("&Quit"),this);
    actQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    actQuit->setToolTip(_("Quit OpenSCADA"));
    actQuit->setWhatsThis(_("The button for full quit from OpenSCADA"));
    actQuit->setStatusTip(_("Press for full quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //-- Up button --
    if(!ico_t.load(TUIS::icoPath("up").c_str())) ico_t.load(":/images/up.png");
    actUp = new QAction(QPixmap::fromImage(ico_t),_("&Up"),this);
    actUp->setShortcut(Qt::ALT+Qt::Key_Up);
    actUp->setToolTip(_("Up page"));
    actUp->setWhatsThis(_("The button for going to level up"));
    actUp->setStatusTip(_("Press for going to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(pageUp()));
    //-- Previos page --
    if(!ico_t.load(TUIS::icoPath("previous").c_str())) ico_t.load(":/images/previous.png");
    actPrev = new QAction(QPixmap::fromImage(ico_t),_("&Previos"),this);
    actPrev->setShortcut(Qt::ALT+Qt::Key_Left);
    actPrev->setToolTip(_("Previos page"));
    actPrev->setWhatsThis(_("The button for going to previos page"));
    actPrev->setStatusTip(_("Press for going to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(pagePrev()));
    //-- Next page --
    if(!ico_t.load(TUIS::icoPath("next").c_str())) ico_t.load(":/images/next.png");
    actNext = new QAction(QPixmap::fromImage(ico_t),_("&Next"),this);
    actNext->setShortcut(Qt::ALT+Qt::Key_Right);
    actNext->setToolTip(_("Next page"));
    actNext->setWhatsThis(_("The button for going to next page"));
    actNext->setStatusTip(_("Press for going to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(pageNext()));
    //--- Load item from db ---
    if(!ico_t.load(TUIS::icoPath("load").c_str())) ico_t.load(":/images/load.png");
    actDBLoad = new QAction(QPixmap::fromImage(ico_t),_("Load from DB"),this);
    actDBLoad->setToolTip(_("Load item data from DB"));
    actDBLoad->setWhatsThis(_("The button for loading item data from DB"));
    actDBLoad->setStatusTip(_("Press for loading item data from DB."));
    actDBLoad->setEnabled(false);
    connect(actDBLoad, SIGNAL(activated()), this, SLOT(itDBLoad()));
    //--- Save item to db ---
    if(!ico_t.load(TUIS::icoPath("save").c_str())) ico_t.load(":/images/save.png");
    actDBSave = new QAction(QPixmap::fromImage(ico_t),_("Save to DB"),this);
    actDBSave->setToolTip(_("Save item data to DB"));
    actDBSave->setWhatsThis(_("The button for saving item data to DB"));
    actDBSave->setStatusTip(_("Press for saving item data to DB."));
    actDBSave->setShortcut(QKeySequence("Ctrl+S"));
    actDBSave->setEnabled(false);
    connect(actDBSave, SIGNAL(activated()), this, SLOT(itDBSave()));
    //-- Item add --
    if(!ico_t.load(TUIS::icoPath("it_add").c_str())) ico_t.load(":/images/it_add.png");
    actItAdd = new QAction(QPixmap::fromImage(ico_t),_("&Add"),this);
    actItAdd->setToolTip(_("Add item"));
    actItAdd->setWhatsThis(_("The button for adding new item"));
    actItAdd->setStatusTip(_("Press for adding new item."));
    actItAdd->setEnabled(false);
    connect(actItAdd, SIGNAL(activated()), this, SLOT(itAdd()));
    //-- Item delete --
    if(!ico_t.load(TUIS::icoPath("it_del").c_str())) ico_t.load(":/images/it_del.png");
    actItDel = new QAction(QPixmap::fromImage(ico_t),_("&Delete"),this);
    actItDel->setToolTip(_("Delete item"));
    actItDel->setWhatsThis(_("The button for deleting item"));
    actItDel->setStatusTip(_("Press for deleting item."));
    actItDel->setShortcut(QKeySequence("Ctrl+D"));
    actItDel->setEnabled(false);
    connect(actItDel, SIGNAL(activated()), this, SLOT(itDel()));
    //--- Cut item ---
    if(!ico_t.load(TUIS::icoPath("editcut").c_str())) ico_t.load(":/images/editcut.png");
    actItCut = new QAction(QPixmap::fromImage(ico_t),_("Item cut"),this);
    actItCut->setToolTip(_("Cutting of the item"));
    actItCut->setWhatsThis(_("The button for cutting of the item"));
    actItCut->setStatusTip(_("Press to cut the item."));
    actItCut->setShortcut(QKeySequence("Ctrl+X"));
    actItCut->setEnabled(false);
    connect(actItCut, SIGNAL(activated()), this, SLOT(itCut()));
    //--- Copy item ---
    if(!ico_t.load(TUIS::icoPath("editcopy").c_str())) ico_t.load(":/images/editcopy.png");
    actItCopy = new QAction(QPixmap::fromImage(ico_t),_("Item copy"),this);
    actItCopy->setToolTip(_("Copying of the item"));
    actItCopy->setWhatsThis(_("The button for copying of the item"));
    actItCopy->setStatusTip(_("Press to copy the item."));
    actItCopy->setShortcut(QKeySequence("Ctrl+C"));
    actItCopy->setEnabled(false);
    connect(actItCopy, SIGNAL(activated()), this, SLOT(itCopy()));
    //--- Paste item ---
    if(!ico_t.load(TUIS::icoPath("editpaste").c_str())) ico_t.load(":/images/editpaste.png");
    actItPaste = new QAction(QPixmap::fromImage(ico_t),_("Item paste"),this);
    actItPaste->setToolTip(_("Pasting of the item"));
    actItPaste->setWhatsThis(_("The button for pasting of the item"));
    actItPaste->setStatusTip(_("Press to paste the item."));
    actItPaste->setShortcut(QKeySequence("Ctrl+V"));
    actItPaste->setEnabled(false);
    connect(actItPaste, SIGNAL(activated()), this, SLOT(itPaste()));
    //-- Update --
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
    QAction *actUpdate = new QAction(QPixmap::fromImage(ico_t),_("&Refresh"),this);
    actUpdate->setShortcut(Qt::Key_F5);
    actUpdate->setToolTip(_("Refresh current page"));
    actUpdate->setWhatsThis(_("The button for refreshing of the content of the current page."));
    actUpdate->setStatusTip(_("Press for refreshing of the content of the current page."));
    connect(actUpdate, SIGNAL(activated()), this, SLOT(pageRefresh()));
    //-- Start of "Auto update" --
    if(!ico_t.load(TUIS::icoPath("start").c_str())) ico_t.load(":/images/start.png");
    actStartUpd = new QAction(QPixmap::fromImage(ico_t),_("&Start"),this);
    actStartUpd->setShortcut(Qt::CTRL+Qt::Key_B);
    actStartUpd->setToolTip(_("Start cycled refresh"));
    actStartUpd->setWhatsThis(_("The button for starting of the cycled refresh of the content of the current page."));
    actStartUpd->setStatusTip(_("Press to start the cycled refresh of the content of the current page."));
    connect(actStartUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStart()));
    //-- Stop of "Auto update" --
    if(!ico_t.load(TUIS::icoPath("stop").c_str())) ico_t.load(":/images/stop.png");
    actStopUpd = new QAction(QPixmap::fromImage(ico_t),_("&Stop"),this);
    actStopUpd->setShortcut(Qt::CTRL+Qt::Key_E);
    actStopUpd->setToolTip(_("Stop cycled refresh"));
    actStopUpd->setWhatsThis(_("The button for stopping of the cycled refresh of the content of the current page."));
    actStopUpd->setStatusTip(_("Press to stop the cycled refresh of the content of the current page."));
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStop()));
    //-- About "System info" --
    if(!ico_t.load(TUIS::icoPath("help").c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setShortcut(Qt::Key_F1);
    actAbout->setToolTip(_("Programm and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display programm and OpenSCADA information"));
    actAbout->setStatusTip(_("Press to display programm and OpenSCADA information"));
    connect(actAbout, SIGNAL(activated()), this, SLOT(about()));
    //-- About Qt --
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using QT information"));
    actQtAbout->setStatusTip(_("Press to get the using QT information."));
    connect(actQtAbout, SIGNAL(activated()), this, SLOT(aboutQt()));
    //-- What is --
    if(!ico_t.load(TUIS::icoPath("contexthelp").c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setToolTip(_("Question about GUI elements"));
    actWhatIs->setWhatsThis(_("The button for requesting the information about user interface elements"));
    actWhatIs->setStatusTip(_("Press for requesting about user interface elements"));
    connect(actWhatIs, SIGNAL(activated()), this, SLOT(enterWhatsThis()));

    //- Create menu -
    //-- Create menu "file" --
    QMenu *mn_file = menuBar()->addMenu(_("&File"));
    mn_file->addAction(actDBLoad);
    mn_file->addAction(actDBSave);
    mn_file->addSeparator( );
    mn_file->addAction(actClose);
    mn_file->addAction(actQuit);
    //-- Create menu "edit" --
    QMenu *mn_edit = menuBar()->addMenu(_("&Edit"));
    mn_edit->addAction(actItAdd);
    mn_edit->addAction(actItDel);
    mn_edit->addSeparator( );
    mn_edit->addAction(actItCopy);
    mn_edit->addAction(actItCut);
    mn_edit->addAction(actItPaste);
    //-- Create menu "view" --
    QMenu *mn_view = menuBar()->addMenu(_("&View"));
    mn_view->addAction(actUp);
    mn_view->addAction(actPrev);
    mn_view->addAction(actNext);
    mn_view->addSeparator( );
    mn_view->addAction(actUpdate);
    mn_view->addAction(actStartUpd);
    mn_view->addAction(actStopUpd);
    //-- Create menu "help" --
    QMenu *help = menuBar()->addMenu(_("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addSeparator( );
    help->addAction(actWhatIs);

    //- Create tool bars -
    //-- Main tool bar --
    QToolBar *toolBar = new QToolBar(_("OpenSCADA toolbar"),this);
    addToolBar(toolBar);
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

    //- Init status bar -
    w_user = new UserStBar(open_user.c_str(), this);
    w_user->setWhatsThis(_("This label displays curent user."));
    w_user->setToolTip(_("Field for display of the current user."));
    w_user->setStatusTip(_("Double click to change user."));
    statusBar()->insertPermanentWidget(0,w_user);
    connect(w_user, SIGNAL(userChanged()), this, SLOT(userSel()));

    mStModify = new QLabel(" ",this);
    mStModify->setWhatsThis(_("This label displays the local station modifying."));
    mStModify->setToolTip(_("Field for display of the local station modifying."));
    statusBar()->insertPermanentWidget(0,mStModify);

    statusBar()->showMessage(_("Ready"), 2000 );

    //- Other resources init -
    //-- Create auto update timer --
    autoUpdTimer = new QTimer( this );
    connect( autoUpdTimer, SIGNAL(timeout()), SLOT(pageRefresh()) );
    //-- Create end run timer --
    endRunTimer   = new QTimer( this );
    endRunTimer->setSingleShot(false);
    connect(endRunTimer, SIGNAL(timeout()), this, SLOT(endRunChk()));
    endRunTimer->start(STD_WAIT_DELAY);	

    //-- Display root page and init external pages --
    initHosts();
    try{ pageDisplay(mod->startPath()); }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

ConfApp::~ConfApp()
{
    endRunTimer->stop();
    autoUpdTimer->stop();

    mod->unregWin( this );
}

void ConfApp::quitSt()
{
    if( exitModifChk( ) ) SYS->stop();
}

bool ConfApp::exitModifChk( )
{
    //- Check for no saved local station -
    XMLNode req("modify");
    req.setAttr("path","/"+SYS->id()+"/%2fobj");
    if( !cntrIfCmd(req) && atoi(req.text().c_str()) )
    {
	bool saveExit = false;
	req.clear()->setName("get")->setAttr("path","/"+SYS->id()+"/%2fgen%2fsaveExit");
	if( !cntrIfCmd(req) )	saveExit |= atoi(req.text().c_str());
	req.setAttr("path","/"+SYS->id()+"/%2fgen%2fsavePeriod");
	if( !cntrIfCmd(req) )	saveExit |= atoi(req.text().c_str());
	if( !saveExit )
	{
	    int ret = QMessageBox::information(this,_("Changes save"),
		_("Some changing is made. Save changing to DB on exit?"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Yes);
	    switch( ret )
	    {
		case QMessageBox::Yes:
		    req.clear()->setName("save")->setAttr("path","/"+SYS->id()+"/%2fobj");
		    cntrIfCmd(req);
		    return true;
		case QMessageBox::No:
		    return true;
		case QMessageBox::Cancel:
		    return false;
	    }
	    /*InputDlg dlg(this,actDBSave->icon(),
		    _("Some nodes of local station is changed. Save changing to DB on exit?"),
		    _("Station save"),false,false);
	    if( dlg.exec() == QDialog::Accepted )
	    {
		req.clear()->setName("save")->setAttr("path","/"+SYS->id()+"/%2fobj");
		cntrIfCmd(req);
	    }*/
	}
    }
    return true;
}

void ConfApp::endRunChk( )
{
    if( mod->endRun() ) close();
}

void ConfApp::pageUp()
{
    long i_l = string::npos;
    while(true)
    {
	i_l = sel_path.rfind("/",i_l);
	if( i_l == string::npos || i_l == 0 ) return;
	if( (sel_path.size()-i_l) > 1 ) break;
	i_l--;
    }

    selectPage(sel_path.substr(0,i_l));
}

void ConfApp::pagePrev()
{
    if( !prev.size() ) return;
    next.insert(next.begin(),sel_path);
    string path = prev[0];
    prev.erase(prev.begin());

    try{ pageDisplay( path ); } catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::pageNext()
{
    if( !next.size() ) return;
    prev.insert(prev.begin(),sel_path);
    string path = next[0];
    next.erase(next.begin());

    try{ pageDisplay( path ); } catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); } 
}

void ConfApp::itDBLoad( )
{
    XMLNode req("load"); req.setAttr("path",sel_path+"/%2fobj");
    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    else pageRefresh();
}

void ConfApp::itDBSave( )
{
    XMLNode req("save"); req.setAttr("path",sel_path+"/%2fobj");
    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    else pageRefresh();
}

void ConfApp::itAdd( )
{
    if( sel_path.empty() || !root->childGet("id","br",true) )	return;
    XMLNode *branch = root->childGet("id","br");

    //> Load branches list
    vector<string> brs;
    for( int i_b = 0; i_b < branch->childSize(); i_b++ )
	if( atoi(branch->childGet(i_b)->attr("acs").c_str())&SEQ_WR )
	    brs.push_back( branch->childGet(i_b)->attr("idSz")+"\n"+branch->childGet(i_b)->attr("idm")+"\n"+
			   branch->childGet(i_b)->attr("id")+"\n"+branch->childGet(i_b)->attr("dscr"));
    if( !brs.size() )	{ mod->postMess(mod->nodePath().c_str(),_("No one editable container is present."),TUIMod::Info,this); return; }

    ReqIdNameDlg dlg(this,actItAdd->icon(), QString(_("Add item to node: '%1'.")).arg(sel_path.c_str()),_("Add node"));
    dlg.setTargets(brs);
    if( dlg.exec() != QDialog::Accepted )   return;

    //> Check for already present node
    XMLNode req("get");
    req.setAttr("path",sel_path+"/%2fbr%2f"+TSYS::strSepParse(dlg.target( ),2,'\n'));
    if( !cntrIfCmd(req) )
        for( int i_lel = 0; i_lel < req.childSize(); i_lel++ )
	    if( (req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->attr("id") == dlg.id().toAscii().data()) ||
	        (!req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->text() == dlg.id().toAscii().data()) )
	    {
		mod->postMess(mod->nodePath().c_str(),QString(_("Node '%1' is already present.")).arg(dlg.id()).toAscii().data(),TUIMod::Info,this);
		return;
	    }

    //> Send create request
    req.clear()->
	setName("add")->
	setAttr("path",sel_path+"/%2fbr%2f"+TSYS::strSepParse(dlg.target( ),2,'\n'));
    if( atoi(TSYS::strSepParse(dlg.target(),1,'\n').c_str()) )
	req.setAttr("id",dlg.id().toAscii().data())->setText(dlg.name().toAscii().data());
    else req.setText(dlg.id().toAscii().data());
    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
    else { treeUpdate(); pageRefresh(); }
}

void ConfApp::itDel( const string &iit )
{
    string rmits = iit, rmit;
    if( iit.empty() )
    {
	QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
	if( sel_ls.size() <= 1 ) rmits = sel_path;
	else for( int i_el = 0; i_el < sel_ls.size(); i_el++ )
	    rmits = rmits + sel_ls.at(i_el)->text(2).toAscii().data() + "\n";
    }
    if( rmits.empty() )	return;

    if( iit.empty() )
    {
	InputDlg dlg(this,actItDel->icon(),
		QString(_("Are you sure of deleting nodes: '%1'?")).arg(rmits.c_str()),_("Delete node"),0,0);
	if( dlg.exec() != QDialog::Accepted )   return;
    }

    for( int roff = 0; (rmit=TSYS::strSepParse(rmits,0,'\n',&roff)).size(); )
    {
	string t_el, sel_own, sel_el;
	int n_obj = 0;
	for( int off = 0; !(t_el=TSYS::pathLev(rmit,0,true,&off)).empty(); n_obj++ )
	{ if( n_obj ) sel_own += ("/"+sel_el); sel_el = t_el; }
	if( n_obj > 2 )
	{
	    XMLNode req("info");
	    req.setAttr("path",sel_own+"/%2fbr");
	    if( cntrIfCmd(req) || !req.childGet(0,true) ) return;

	    XMLNode *branch = req.childGet(0);
	    for( int i_b = 0; i_b < branch->childSize(); i_b++ )
	    {
		string b_id = branch->childGet(i_b)->attr("id");
		if( b_id == sel_el.substr(0,b_id.size()) && atoi(branch->childGet(i_b)->attr("acs").c_str())&SEQ_WR )
		{
		    bool idm = atoi(branch->childGet(i_b)->attr("idm").c_str());
		    req.clear()->setName("del")->setAttr("path",sel_own+"/%2fbr%2f"+b_id);
		    if( idm ) req.setAttr("id",sel_el.substr(b_id.size()));
		    else req.setText(sel_el.substr(b_id.size()));
		    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Info,this);
		    else treeUpdate();
		    break;
		}
	    }
	}
    }
}

void ConfApp::itCut( )
{
    copy_buf = "1"+sel_path;
    editToolUpdate();
}

void ConfApp::itCopy( )
{
    copy_buf = "0"+sel_path;
    editToolUpdate();
}

void ConfApp::itPaste( )
{
    int off;
    string s_el, s_elp, t_el, b_grp;

    //> Src elements calc
    int n_sel = 0;
    for( off = 0; !(t_el=TSYS::pathLev(copy_buf.substr(1),0,true,&off)).empty(); n_sel++ )
    { if( n_sel ) s_elp += ("/"+s_el); s_el = t_el; }

    if( TSYS::pathLev(copy_buf.substr(1),0) != TSYS::pathLev(sel_path,0) )
    { mod->postMess( mod->nodePath().c_str(), _("Copy is imposible."), TUIMod::Error, this ); return; }

    vector<string> brs;
    if( atoi(root->attr("acs").c_str())&SEQ_WR ) brs.push_back(string("-1\n0\n\n")+_("Selected"));

    XMLNode *branch = root->childGet("id","br",true);
    if( branch )
	for( int i_b = 0; i_b < branch->childSize(); i_b++ )
	    if( atoi(branch->childGet(i_b)->attr("acs").c_str())&SEQ_WR )
	    {
		string gbrId = branch->childGet(i_b)->attr("id");
		brs.push_back( branch->childGet(i_b)->attr("idSz")+"\n0\n"+gbrId+"\n"+branch->childGet(i_b)->attr("dscr"));
		if( s_el.substr(0,gbrId.size()) == gbrId ) { brs[brs.size()-1] = brs[brs.size()-1]+"\n1"; b_grp = gbrId; }
	    }

    //> Make request dialog
    ReqIdNameDlg dlg(this,actItAdd->icon(),"",_("Move or copy node"));
    dlg.setTargets(brs);
    dlg.setMess( QString((copy_buf[0] == '1') ? _("Move node '%1' to '%2'.\n") : _("Copy node '%1' to '%2'.\n")).
		arg(copy_buf.substr(1).c_str()).arg(sel_path.c_str()) );
    dlg.setId( s_el.substr(b_grp.size()).c_str() );
    if( dlg.exec() != QDialog::Accepted ) return;

    string stat_nm, src_nm, dst_nm;
    off = 1; stat_nm = TSYS::pathLev(copy_buf,0,true,&off); src_nm = copy_buf.substr(off);
    off = 0; stat_nm = TSYS::pathLev(sel_path,0,true,&off); dst_nm = sel_path.substr(off);

    if( atoi(TSYS::strSepParse(dlg.target( ),0,'\n').c_str()) >= 0 )
    {
	dst_nm = dst_nm + "/" +TSYS::strSepParse(dlg.target( ),2,'\n') + dlg.id().toAscii().data();
	//> Check for already present node
	XMLNode req("get");
	req.setAttr("path",sel_path+"/%2fbr%2f"+TSYS::strSepParse(dlg.target( ),2,'\n'));
	if( cntrIfCmd(req) ) { mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Error,this); return; }
	for( int i_lel = 0; i_lel < req.childSize(); i_lel++ )
	    if( (req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->attr("id") == dlg.id().toAscii().data()) ||
		(!req.childGet(i_lel)->attr("id").size() && req.childGet(i_lel)->text() == dlg.id().toAscii().data()) )
	    {
		InputDlg dlg1(this,actItPaste->icon(),QString(_("Node '%1' is already present. Continue?")).arg(dst_nm.c_str()).toAscii().data(),_("Move or copy node"),0,0);
		if( dlg1.exec() != QDialog::Accepted ) return;
		break;
	    }
    }

    //> Copy visual item
    XMLNode req("copy");
    req.setAttr("path","/"+stat_nm+"/%2fobj")->setAttr("src",src_nm)->setAttr("dst",dst_nm);
    if( cntrIfCmd(req) ) { mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TUIMod::Error,this); return; }

    //> Remove source widget
    if( copy_buf[0] == '1' ) { itDel(copy_buf.substr(1)); copy_buf = "0"; }

    treeUpdate( );
    pageRefresh( );
}

void ConfApp::editToolUpdate( )
{
    actItCut->setEnabled( (!sel_path.empty()&&root&&atoi(root->attr("acs").c_str())&SEQ_WR) ? true : false );
    actItCopy->setEnabled( !sel_path.empty() );
    actItPaste->setEnabled( false );

    //> Src and destination elements calc
    if( copy_buf.size() <= 1 || copy_buf.substr(1) == sel_path ||
	    TSYS::pathLev(copy_buf.substr(1),0) != TSYS::pathLev(sel_path,0) )
	return;
    string s_elp, s_el, t_el;
    for( int off = 0; !(t_el=TSYS::pathLev(copy_buf.substr(1),0,true,&off)).empty(); )
    { s_elp += ("/"+s_el); s_el = t_el; }

    XMLNode *branch = root->childGet("id","br",true);
    if( branch )
	for( int i_b = 0; i_b < branch->childSize(); i_b++ )
	    if( atoi(branch->childGet(i_b)->attr("acs").c_str())&SEQ_WR )
	    { actItPaste->setEnabled(true); break; }
    if( atoi(root->attr("acs").c_str())&SEQ_WR ) actItPaste->setEnabled(true);
}

void ConfApp::treeUpdate( )
{
    for( int i_t = 0; i_t < CtrTree->topLevelItemCount(); i_t++ )
	if( CtrTree->topLevelItem(i_t)->isExpanded() )
	    viewChildRecArea(CtrTree->topLevelItem(i_t),true);
}

void ConfApp::userSel()
{
    pg_info.setAttr("path","");

    try{ pageDisplay(mod->startPath()); }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    initHosts();
}

void ConfApp::pageRefresh( )
{
    try{ pageDisplay(sel_path); }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
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

void ConfApp::about( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"%s v%s.\n%s\nAutor: %s\nLicense: %s\n\n"
	"%s v%s.\n%s\nLicense: %s\nAuthor: %s\nWeb site: %s"),
	mod->modInfo("Name").c_str(), mod->modInfo("Version").c_str(), mod->modInfo("Descript").c_str(), mod->modInfo("Author").c_str(), mod->modInfo("License").c_str(),
	PACKAGE_NAME, VERSION, _(PACKAGE_DESCR), PACKAGE_LICENSE, _(PACKAGE_AUTHOR), PACKAGE_SITE );

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
    if( !SYS->stopSignal() && !exitModifChk( ) )
    {
	ce->ignore();
	return;
    }

    ce->accept();
}

void ConfApp::selectItem( )
{
    QList<QTreeWidgetItem *> sel_ls = CtrTree->selectedItems();
    if( sel_ls.size() == 1 && sel_path != sel_ls.at(0)->text(2).toAscii().data() )
	selectPage( sel_ls.at(0)->text(2).toAscii().data() );
}

void ConfApp::selectPage( const string &path )
{
    try
    {
	//> Prev and next
	if( sel_path.size() )	prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();
	next.clear();

	//> Display page
	pageDisplay( path );
    }
    catch(TError err)
    {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);
	if( err.cod == 10 ) initHosts( );
    }
}

void ConfApp::selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget )
{
    QVBoxLayout *layout;

    //> For append no named widgets
    QHBoxLayout *l_hbox = NULL;
    int		i_area = 0;	//Areas counter
    int		l_pos = 0;

    //> View title name
    if( a_path == "/" )
    {
	//>> Set node icon
	if( node.childGet("id","ico",true) )
	{
	    XMLNode req("get");
	    req.setAttr("path",sel_path+"/"+TSYS::strEncode(a_path+"ico",TSYS::PathEl));
	    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    else
	    {
		string simg = TSYS::strDecode(req.text(),TSYS::base64);
		QImage img;
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    titleIco->setPixmap(QPixmap::fromImage(img.scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		else titleIco->clear();
	    }
	}else titleIco->clear();

	//>> Set title
	titleLab->setText((string("<p align='center'><i><b>")+TSYS::strEncode(node.attr("dscr"),TSYS::Html)+"</b></i></p>").c_str());

	//>> Delete tabs of deleted areas
	bool actRemoved = false;
	for( int i_tbs = 0; i_tbs < tabs->count(); i_tbs++ )
	{
	    unsigned i_cf;
	    for( i_cf = 0; i_cf < node.childSize(); i_cf++ )
		if( node.childGet(i_cf)->name() == "area" &&
			tabs->tabText(i_tbs) == node.childGet(i_cf)->attr("dscr").c_str() )
		    break;
	    if( i_cf >= node.childSize() )
	    {
		if( tabs->currentIndex() == i_tbs ) actRemoved = true;
		tabs->blockSignals(true);
		tabs->widget(i_tbs)->deleteLater();
		tabs->removeTab(i_tbs);
		i_tbs--;
		tabs->blockSignals(false);
	    }
	}
	//>> Add new tabs
	for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++ )
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
		t_s.setAttr("qview","0");
	    }

	    //>>> Find and prepare current tab
	    if( actRemoved && i_area == 0 )
	    {
		tabs->blockSignals(true);
		tabs->setCurrentIndex(0);
		tabs->blockSignals(false);
	    }
	    if( tabs->currentIndex() == i_area )
	    {
		if( !atoi(t_s.attr("qview").c_str()) )
		{
		    QScrollArea *scrl = (QScrollArea*)tabs->widget(i_area);
		    QWidget *wdg = scrl->widget();

		    int v_scrl = (scrl->verticalScrollBar()) ? scrl->verticalScrollBar()->value() : 0;
		    if( wdg != NULL ) wdg->deleteLater(); // delete wdg;

		    wdg = new QFrame(scrl);
		    QVBoxLayout *wdg_lay = new QVBoxLayout(wdg);
		    wdg_lay->setSpacing(3);
		    wdg_lay->setAlignment( Qt::AlignTop );

		    selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg);
		    wdg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
		    scrl->setWidget(wdg);
		    scrl->setWidgetResizable(true);

		    if( scrl->verticalScrollBar() ) scrl->verticalScrollBar()->setValue(v_scrl);

		    //wdg->resize(wdg->size());
		    //tabs->showPage(tabs->currentWidget());

		    //>>>> Mark last drawed tabs
		    t_s.setAttr("qview","1");
		}
		else selectChildRecArea(t_s,a_path+t_s.attr("id")+'/');
	    }
	    //else t_s.attr("qview","0");	//Mark no view tabs
	    i_area++;
	}
	return;
    }
    //> Read node tree and create widgets
    else for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++ )
    {
	XMLNode &t_s = *node.childGet(i_cf);

	//>> Check access to node
	bool wr = atoi(t_s.attr("acs").c_str())&SEQ_WR;

	//>> View areas
	if( t_s.name() == "area" )
	{
	    QWidget *wdg = widget;
	    if( widget )
	    {
		wdg = new QGroupBox( t_s.attr("dscr").c_str(), widget );
		QVBoxLayout *w_lay = new QVBoxLayout(wdg);
		//((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical );
		w_lay->setSpacing( 3 );
		w_lay->setMargin( 3 );
		//w_lay->setAlignment( Qt::AlignTop );
		widget->layout()->addWidget(wdg);
	    }
	    selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg);
	}
	//>> View list elements
	else if( t_s.name() == "list" )
	{
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    QLabel *lab;
	    QListWidget *lstbox;

	    if( widget )
	    {
		lstbox = new QListWidget(widget);
		lstbox->setStatusTip((sel_path+"/"+br_path).c_str());
		lstbox->setObjectName(br_path.c_str());
		lstbox->setMinimumSize( QSize( 150, 110 ) );
		lstbox->setContextMenuPolicy(Qt::CustomContextMenu);
		connect( lstbox, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( listBoxPopup() ) );

		//>>> Go branche signal connect
		if( t_s.attr("tp") == "br" )
		    connect( lstbox, SIGNAL( itemDoubleClicked(QListWidgetItem*) ), this, SLOT( listBoxGo(QListWidgetItem*) ) );

		lstbox->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );

		QVBoxLayout *vbox = new QVBoxLayout;
		lab = new QLabel(widget);
		vbox->addWidget(lab);
		QHBoxLayout *hbox = new QHBoxLayout;
		hbox->addWidget( lstbox );
		hbox->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		vbox->addLayout(hbox);
		widget->layout()->addItem(vbox);

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_el",TSYS::addr2str(lstbox));
	    }
	    else
	    {
		lab    = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		lstbox = (QListWidget *)TSYS::str2addr(t_s.attr("addr_el"));
		lstbox->clear();
	    }
	    //>>> Fill list
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    lstbox->setToolTip(t_s.attr("help").c_str());
	    XMLNode req("get");
	    req.setAttr("path",sel_path+"/"+br_path);
	    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    else for( unsigned i_el = 0; i_el < req.childSize(); i_el++ )
		if( req.childGet(i_el)->name() == "el")
		    lstbox->addItem(req.childGet(i_el)->text().c_str());
	}
	//>> View table elements
	else if( t_s.name() == "table" )
	{
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    //QLabel *lab;
	    QTableWidget *tbl;

	    if( widget )
	    {
		tbl = new QTableWidget(widget);
		tbl->setItemDelegate(new TableDelegate);
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

		//t_s.attr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_tbl",TSYS::addr2str(tbl));
	    }
	    else
	    {
		//lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		tbl = (QTableWidget *)TSYS::str2addr(t_s.attr("addr_tbl"));
	    }
	    //>>> Fill table
	    tbl->setToolTip(t_s.attr("help").c_str());
	    XMLNode req("get"); req.setAttr("path",sel_path+"/"+br_path);
	    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    else
	    {
		//>>>> Copy values to info tree
		for( int i_col = 0; i_col < req.childSize(); i_col++ )
		{
		    XMLNode *t_lsel = req.childGet(i_col);
		    XMLNode *t_linf = t_s.childGet("id",t_lsel->attr("id"),true);
		    if( !t_linf ) continue;
		    t_linf->childClear();
		    for( int i_rw = 0; i_rw < t_lsel->childSize(); i_rw++ )
			*t_linf->childAdd() = *t_lsel->childGet(i_rw);
		}

		//>>>> Collumns adjusting flag
		bool adjCol = widget || !tbl->rowCount();

		//>>>> Calc rows and columns
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

		    //>>>> Set elements
		    tbl_init = true;
		    for( int i_el = 0; i_el < t_linf->childSize(); i_el++ )
		    {
			thd_it = tbl->item(i_el,i_lst);
			if( !thd_it )
			{
			    thd_it = new QTableWidgetItem("");
			    tbl->setItem(i_el,i_lst,thd_it);
			}

			//>>>> Set element
			if( t_linf->attr("tp") == "bool" ) thd_it->setData(Qt::DisplayRole,(bool)atoi(t_linf->childGet(i_el)->text().c_str()));
			else if( t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed" )
			{
			    QStringList elms;
			    int sel_n = -1;
			    bool u_ind = atoi(t_linf->attr("idm").c_str());

			    if( t_linf->attr("select").empty() )
			    {
				string s_nm;
				for( int ls_off = 0, id_off = 0, i_ls = 0; !(s_nm=TSYS::strSepParse(t_linf->attr("sel_list"),0,';',&ls_off)).empty(); i_ls++ )
				{
				    elms+=s_nm.c_str();
				    if( (u_ind && TSYS::strSepParse(t_linf->attr("sel_id"),0,';',&id_off) == t_linf->childGet(i_el)->text()) ||
					    (!u_ind && s_nm == t_linf->childGet(i_el)->text()) )
					sel_n = i_ls;
				}
			    }
			    else
			    {
				XMLNode x_lst("get");
				x_lst.setAttr("path",sel_path+"/"+TSYS::strEncode( t_linf->attr("select"),TSYS::PathEl));
				if(cntrIfCmd(x_lst)) mod->postMess(x_lst.attr("mcat"),x_lst.text(),TUIMod::Error,this);
				else
				    for( int i_ls = 0; i_ls < x_lst.childSize(); i_ls++ )
				    {
					elms+=x_lst.childGet(i_ls)->text().c_str();
					if( (u_ind && x_lst.childGet(i_ls)->attr("id") == t_linf->childGet(i_el)->text()) ||
						(!u_ind && x_lst.childGet(i_ls)->text() == t_linf->childGet(i_el)->text()) )
					    sel_n = i_ls;
				    }
			    }
			    if( sel_n < 0 )
			    {
				elms.insert(elms.begin(),t_linf->childGet(i_el)->text().c_str());
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

			//>>>> Set access
			if( !c_wr ) thd_it->setFlags(Qt::ItemIsEnabled);
			else thd_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
		    }
		}
		if(adjCol)
		{
		    tbl->resizeColumnsToContents();

		    //>>> Resize too long columns
		    int max_col_sz = vmax(700/tbl->columnCount(),50);
		    for( int i_c = 0; i_c < tbl->columnCount(); i_c++ )
			tbl->setColumnWidth(i_c,vmin(max_col_sz,tbl->columnWidth(i_c)));
		}
		tbl->setMinimumSize( QSize( 100, vmin(300,100+10*tbl->rowCount()) ) );

		tbl_init = false;
	    }
	}
	//>> View images
	else if( t_s.name() == "img" )
	{
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    QLabel *lab;
	    ImgView *img;

	    if( widget )
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
		if( box->direction() == QBoxLayout::LeftToRight )
		{
		    lab->setAlignment( Qt::AlignTop );
		    box->setSpacing(6);
		    box->addItem(new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ));
		}
		widget->layout()->addItem(box);

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_el",TSYS::addr2str(img));
	    }
	    else
	    {
		lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		img = (ImgView *)TSYS::str2addr(t_s.attr("addr_el"));
	    }

	    //>>> Set image
	    lab->setText((t_s.attr("dscr")+":").c_str());
	    img->setToolTip(t_s.attr("help").c_str());

	    XMLNode req("get");
	    req.setAttr("path",sel_path+"/"+br_path);
	    if(cntrIfCmd(req)) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    else img->setImage(TSYS::strDecode(req.text(),TSYS::base64));
	}
	//>> View standart fields
	else if( t_s.name() == "fld" ) basicFields( t_s, a_path, widget, wr, &l_hbox, l_pos );
	//>> View commands
	else if( t_s.name() == "comm" )
	{
	    string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);

	    QPushButton *button;
	    QGroupBox *comm_pan = NULL;
	    QHBoxLayout *c_hbox = NULL;
	    int c_pos = 0;

	    if( widget )
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

		t_s.setAttr("addr_butt",TSYS::addr2str(button));
	    }
	    else button = (QPushButton *)TSYS::str2addr(t_s.attr("addr_butt"));

	    //>>> Update or create parameters
	    for( unsigned i_cf = 0; i_cf < t_s.childSize(); i_cf++ )
	    {
		XMLNode &t_scm = *t_s.childGet(i_cf);
		if( t_scm.name() == "fld" ) basicFields( t_scm, a_path+t_s.attr("id")+'/', comm_pan, true, &c_hbox, c_pos, true );
	    }

	    //>>> Fill command
	    button->setText(t_s.attr("dscr").c_str());
	    button->setToolTip(t_s.attr("help").c_str());
	}
    }
}

void ConfApp::basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm )
{
    string br_path = TSYS::strEncode( string((comm)?"b":"")+a_path+t_s.attr("id"),TSYS::PathEl);

    XMLNode data_req("get");
    if( !comm )
    {
	data_req.setAttr("path",sel_path+"/"+br_path);
	if(cntrIfCmd(data_req))
	{
	    mod->postMess(data_req.attr("mcat"),data_req.text(),TUIMod::Error,this);
	    data_req.setText("");
	}
    }

    //> View select fields
    if( t_s.attr("dest") == "select" )
    {
	QLabel *lab = NULL, *val_r = NULL;
	QComboBox *val_w = NULL;

	if( widget )
	{
	    if( !wr )
	    {
		val_r = new QLabel( widget );
		val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		val_r->setStatusTip((sel_path+"/"+br_path).c_str());
	    }
	    else
	    {
		val_w = new QComboBox(widget);
		val_w->setMinimumSize(100,0);
		val_w->setObjectName(br_path.c_str());
		val_w->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		val_w->setStatusTip((sel_path+"/"+br_path).c_str());
		val_w->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed) );
		connect( val_w, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
	    }

	    if(t_s.attr("dscr").size())
	    {
	        *l_hbox = new QHBoxLayout; l_pos = 0;
	        (*l_hbox)->setSpacing(6);
		lab = new QLabel(widget);
		(*l_hbox)->insertWidget( l_pos++, lab );
		if(val_w)	(*l_hbox)->insertWidget( l_pos++, val_w );
		if(val_r)	(*l_hbox)->insertWidget( l_pos++, val_r );
		(*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem( *l_hbox );
	    }
	    else
	    {
		if(val_w) { if(*l_hbox) (*l_hbox)->insertWidget( l_pos++, val_w ); else { val_w->deleteLater(); val_w = NULL; } }
		if(val_r) { if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, val_r ); else { val_r->deleteLater(); val_r = NULL; } }
	    }

	    t_s.setAttr("addr_lab",TSYS::addr2str(lab));
	    t_s.setAttr("addr_val_w",TSYS::addr2str(val_w));
	    t_s.setAttr("addr_val_r",TSYS::addr2str(val_r));
	}
	else
	{
	    lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
	    val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_val_r"));
	    val_w = (QComboBox *)TSYS::str2addr(t_s.attr("addr_val_w"));
	}

	//>> Fill combo
	if( lab ) lab->setText((t_s.attr("dscr")+":").c_str());
	if( val_w || val_r )
	{
	    (val_w?(QWidget*)val_w:(QWidget*)val_r)->setToolTip(t_s.attr("help").c_str());
	    if( val_w ) val_w->clear();

	    bool sel_ok = false;
	    unsigned c_el = 0;
	    if( t_s.attr("select").empty() )
	    {
		string s_nm;
		bool ind_ok = t_s.attr("sel_id").size();	//Index present
		for( int ls_off = 0, id_off = 0; !(s_nm=TSYS::strSepParse(t_s.attr("sel_list"),0,';',&ls_off)).empty(); c_el++ )
		{
		    if( val_w )	val_w->insertItem( c_el, s_nm.c_str() );
		    if( (ind_ok && TSYS::strSepParse(t_s.attr("sel_id"),0,';',&id_off) == data_req.text()) ||
			(!ind_ok && s_nm == data_req.text()) )
		    {
			sel_ok = true;
			if( val_w )	val_w->setCurrentIndex( c_el );
			if( val_r )	val_r->setText((string("<b>")+s_nm+"</b>").c_str());
		    }
		}
	    }
	    else
	    {
		XMLNode x_lst("get");
		x_lst.setAttr("path",sel_path+"/"+TSYS::strEncode( t_s.attr("select"),TSYS::PathEl));
		if( !cntrIfCmd(x_lst) )
		    for( unsigned i_el = 0; i_el < x_lst.childSize(); i_el++ )
		    {
			if( x_lst.childGet(i_el)->name() != "el") continue;
			if( val_w )	val_w->insertItem(c_el++, x_lst.childGet(i_el)->text().c_str() );
			bool ind_ok = x_lst.childGet(i_el)->attr("id").size();	//Index present
			if( (ind_ok && x_lst.childGet(i_el)->attr("id") == data_req.text()) ||
			    (!ind_ok && x_lst.childGet(i_el)->text() == data_req.text()) )
			{
			    sel_ok = true;
			    if( val_w )	val_w->setCurrentIndex( c_el-1 );
			    if( val_r )	val_r->setText((string("<b>")+x_lst.childGet(i_el)->text()+"</b>").c_str());
			}
		    }
	    }
	    //>>> Insert empty field if none selected
	    if( !sel_ok )
	    {
		if(val_w)
		{
		    val_w->insertItem(c_el,data_req.text().c_str());
		    val_w->setCurrentIndex(c_el);
		}
		if(val_r)	val_r->setText((string("<b>")+data_req.text()+"</b>").c_str());
	    }
	}
    }
    else
    {
	//> View boolean fields
	if( t_s.attr("tp") == "bool" )
	{
	    QLabel *lab = NULL, *val_r = NULL;
	    QCheckBox *val_w	= NULL;

	    if( widget )
	    {
		//>> View info
		if( !wr )
		{
		    val_r = new QLabel( widget );
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    val_r->setStatusTip((sel_path+"/"+br_path).c_str());
		}
		//>> View edit
		else
		{
		    val_w = new QCheckBox(widget);
		    val_w->setObjectName(br_path.c_str());
		    val_w->setStatusTip((sel_path+"/"+br_path).c_str());
		    connect( val_w, SIGNAL( stateChanged(int) ), this, SLOT( checkBoxStChange(int) ) );
		    //if(!wr)	val_w->setDisabled(true);
		}
		//>> Check use label
		if( t_s.attr("dscr").size() )
		{
		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    lab = new QLabel(widget);
		    (*l_hbox)->insertWidget( l_pos++, lab );
		    if(val_w)	(*l_hbox)->insertWidget( l_pos++, val_w );
		    if(val_r)	(*l_hbox)->insertWidget( l_pos++, val_r );
		    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		    widget->layout()->addItem( *l_hbox );
		}
		else
		{
		    if( val_w ) { if(*l_hbox) (*l_hbox)->insertWidget( l_pos++, val_w ); else { val_w->deleteLater(); val_w = NULL; } }
		    if( val_r ) { if(*l_hbox) (*l_hbox)->insertWidget( l_pos++, val_r ); else { val_r->deleteLater(); val_r = NULL; } }
		}

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_val_w",TSYS::addr2str(val_w));
		t_s.setAttr("addr_val_r",TSYS::addr2str(val_r));
	    }
	    else
	    {
		lab = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_w = (QCheckBox *)TSYS::str2addr(t_s.attr("addr_val_w"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_val_r"));
	    }

	    //>> Fill CheckBox
	    if( lab )	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( val_w )	{ val_w->setToolTip(t_s.attr("help").c_str()); val_w->setChecked(atoi(data_req.text().c_str())); }
	    if( val_r )
	    {
		val_r->setToolTip(t_s.attr("help").c_str());
		val_r->setText((string("<b>")+(atoi(data_req.text().c_str())?_("On"):_("Off"))+string("</b>")).c_str());
	    }
	}
	//> View edit fields
	else if( t_s.attr("tp") == "str" && (t_s.attr("rows").size() || t_s.attr("cols").size()) )
	{
	    QLabel *lab;
	    TextEdit *edit;

	    if( widget )
	    {
		lab = new QLabel(t_s.attr("dscr").c_str(),widget);
		widget->layout()->addWidget(lab);

		edit = new TextEdit(widget,br_path.c_str());
		edit->setStatusTip((sel_path+"/"+br_path).c_str());
		if( atoi(t_s.attr("rows").c_str()) < 10 )
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
		if( atoi(t_s.attr("cols").c_str()) )
		{
		    edit->edit()->setLineWrapMode(QTextEdit::FixedColumnWidth);
		    edit->edit()->setLineWrapColumnOrWidth(atoi(t_s.attr("cols").c_str()));
		}else edit->edit()->setLineWrapMode(QTextEdit::NoWrap);
		widget->layout()->addWidget( edit );
		
		if( !wr )	edit->edit()->setReadOnly( true );
		else
		{
		    connect( edit, SIGNAL( textChanged(const QString&) ), this, SLOT( editChange(const QString&) ) );
		    connect( edit, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		    connect( edit, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		}

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_edit",TSYS::addr2str(edit));
	    }
	    else
	    {
		lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		edit = (TextEdit *)TSYS::str2addr(t_s.attr("addr_edit"));
	    }
	    //>> Fill Edit
	    if( lab )	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( edit && !edit->isChanged() )
	    {
		edit->setToolTip(t_s.attr("help").c_str());
		edit->setText(data_req.text().c_str());
	    }
	}
	//> View Data-Time fields
	else if( t_s.attr("tp") == "time" )
	{
	    QLabel	*lab = NULL, *val_r = NULL;
	    LineEdit	*val_w = NULL;

	    if( widget )
	    {
		if( !wr )
		{
		    val_r = new QLabel( widget );
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    val_r->setStatusTip((sel_path+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy( sp );
		    val_r->setAlignment( Qt::AlignVCenter );
		    val_r->setWordWrap(true);
		}
		else
		{
		    val_w = new LineEdit( widget, LineEdit::DateTime, comm );
		    val_w->setObjectName(br_path.c_str());
		    val_w->setStatusTip((sel_path+"/"+br_path).c_str());
		    val_w->setCfg("dd.MM.yyyy hh:mm:ss");
		    connect( val_w, SIGNAL( valChanged(const QString&) ), this, SLOT( editChange(const QString&) ) );
		    connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		    connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		}

		//>> Check use label
		if(t_s.attr("dscr").size())
		{
		    lab = new QLabel(widget);

		    *l_hbox = new QHBoxLayout; l_pos = 0;
		    (*l_hbox)->setSpacing(6);
		    (*l_hbox)->insertWidget( l_pos++, lab);
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
		    if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, (val_w?(QWidget*)val_w:(QWidget*)val_r) );
		    else { (val_w?(QWidget*)val_w:(QWidget*)val_r)->deleteLater(); val_w = NULL; val_r = NULL; }
		}
		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_dtw",TSYS::addr2str(val_w));
		t_s.setAttr("addr_dtr",TSYS::addr2str(val_r));
	    }
	    else
	    {
		lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_dtr"));
		val_w = (LineEdit *)TSYS::str2addr(t_s.attr("addr_dtw"));
	    }

	    //>> Fill data
	    if( lab )	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( val_r )
	    {
		time_t tm_t;
		if( data_req.text().size() ) tm_t = atoi(data_req.text().c_str());
		else tm_t = time(NULL);
		QDateTime dtm;
		dtm.setTime_t(tm_t);
		val_r->setToolTip(t_s.attr("help").c_str());
		val_r->setText( "<b>"+dtm.toString("dd.MM.yyyy hh:mm:ss")+"</b>" );
	    }
	    if( val_w && !val_w->isEdited() )
	    {
		val_w->setToolTip(t_s.attr("help").c_str());
		val_w->setValue(data_req.text().c_str());
	    }
	}
	//> View other string and numberic fields
	else
	{
	    QLabel *lab = NULL, *val_r	= NULL;
	    LineEdit *val_w	= NULL;
	    if( widget )
	    {
		//>> View info
		if( !wr )
		{
		    val_r = new QLabel( widget );
		    val_r->setTextInteractionFlags(Qt::TextSelectableByMouse);
		    val_r->setStatusTip((sel_path+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Preferred);
		    sp.setHorizontalStretch(1);
		    val_r->setSizePolicy( sp );
		    val_r->setAlignment( Qt::AlignVCenter );
		    val_r->setWordWrap(true);
		    //val_r->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		}
		//>> View edit
		else
		{
		    val_w = new LineEdit( widget, LineEdit::Text, comm );
		    val_w->setObjectName( br_path.c_str() );
		    val_w->setStatusTip((sel_path+"/"+br_path).c_str());
		    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Fixed);
		    sp.setHorizontalStretch(1);
		    val_w->setSizePolicy( sp );
		    connect( val_w, SIGNAL( valChanged(const QString&) ), this, SLOT( editChange(const QString&) ) );
		    connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		    connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );

		    //>>> addon parameters
		    string tp = t_s.attr("tp");
		    if( t_s.attr("dest") == "sel_ed" )
		    {
			val_w->setMinimumSize(100,0);
			val_w->setType( LineEdit::Combo );
		    }
		    else if( tp == "dec" )
		    {
			val_w->setFixedWidth( 5*15+30 );
			val_w->setType( LineEdit::Integer );
			QString	max = t_s.attr("max").empty() ? "9999999999" : t_s.attr("max").c_str();
			QString	min = t_s.attr("min").empty() ? "-9999999999" : t_s.attr("min").c_str();
			val_w->setCfg(min+":"+max+":1");
		    }
		    else if( tp == "hex" || tp == "oct" )	val_w->setFixedWidth( 5*15+30 );
		    else if( tp == "real" )
		    {
			val_w->setFixedWidth( 5*15+30 );
			val_w->setType(LineEdit::Text);
			QDoubleValidator *dv = new QDoubleValidator(val_w->workWdg());
			dv->setNotation(QDoubleValidator::ScientificNotation);
			((QLineEdit*)val_w->workWdg())->setValidator(dv);
			/*QString	max = t_s.attr("max").empty() ? "9999999999" : t_s.attr("max").c_str();
			QString	min = t_s.attr("min").empty() ? "-9999999999" : t_s.attr("min").c_str();
			val_w->setCfg(min+":"+max+":1:::4");*/
		    }
		    else
		    {
			val_w->setType(LineEdit::Text);
			val_w->setMinimumWidth( 7*15+30 );
			int flen = atoi(t_s.attr("len").c_str());
			if( flen ) ((QLineEdit*)val_w->workWdg())->setMaxLength(flen);
		    }
		}
		//>> Check use label
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
			if( val_w )	(*l_hbox)->insertWidget( l_pos++, val_w );
			if( val_r )	(*l_hbox)->insertWidget( l_pos++, val_r );
		    }
		    else
		    {
			if( val_w ) { val_w->deleteLater(); /*delete val_w;*/ val_w = NULL; }
			if( val_r ) { val_r->deleteLater(); /*delete val_r;*/ val_r = NULL; }
		    }
		}

		t_s.setAttr("addr_lab",TSYS::addr2str(lab));
		t_s.setAttr("addr_val_w",TSYS::addr2str(val_w));
		t_s.setAttr("addr_val_r",TSYS::addr2str(val_r));
	    }
	    else
	    {
		lab  = (QLabel *)TSYS::str2addr(t_s.attr("addr_lab"));
		val_r = (QLabel *)TSYS::str2addr(t_s.attr("addr_val_r"));
		val_w = (LineEdit *)TSYS::str2addr(t_s.attr("addr_val_w"));
	    }
	    //>> Fill line
	    string sval = data_req.text();
	    if( t_s.attr("tp") == "hex" )	sval = (QString("0x")+QString::number(atoi(data_req.text().c_str()),16)).toAscii().data();
	    else if( t_s.attr("tp") == "oct" )	sval = (QString("0")+QString::number(atoi(data_req.text().c_str()),8)).toAscii().data();

	    if( lab )	lab->setText((t_s.attr("dscr")+":").c_str());
	    if( val_r )
	    {
		val_r->setToolTip(t_s.attr("help").c_str());
		val_r->setText((string("<b>")+TSYS::strEncode(sval,TSYS::Html)+"</b>").c_str());
	    }
	    if( val_w && !val_w->isEdited() )
	    {
		val_w->setToolTip(t_s.attr("help").c_str());
		val_w->setValue(sval.c_str());

		//>> Fill combo
		if( t_s.attr("dest") == "sel_ed" )
		{
		    string cfg_vls;
		    if( t_s.attr("select").empty() )
		    {
			string s_nm;
			for( int ls_off = 0; !(s_nm=TSYS::strSepParse(t_s.attr("sel_list"),0,';',&ls_off)).empty(); )
			    cfg_vls += s_nm+"\n";
		    }
		    else
		    {
			XMLNode x_lst("get");
			x_lst.setAttr("path",sel_path+"/"+TSYS::strEncode( t_s.attr("select"),TSYS::PathEl));
			if( !cntrIfCmd(x_lst) )
			    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
				if( x_lst.childGet(i_el)->name() == "el")
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
    try
    {
	//> Delete ViewItem childs
	while( i->childCount() ) delete i->takeChild(0);
	viewChildRecArea(i);
	CtrTree->resizeColumnToContents(0);
    }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::pageDisplay( const string &path )
{
    //> Chek Up
    actUp->setEnabled( path.rfind("/") != string::npos && path.rfind("/") != 0 );

    //> Check Prev and Next
    actPrev->setEnabled( prev.size() );
    actNext->setEnabled( next.size() );

    if( path != pg_info.attr("path") )
    {
	//>> Stop refresh
	pageCyclRefrStop();

	sel_path = path;

	pg_info.clear()->setAttr("path",sel_path);
	if( cntrIfCmd(pg_info) ) { throw TError(atoi(pg_info.attr("rez").c_str()),pg_info.attr("mcat").c_str(),"%s",pg_info.text().c_str()); }
	root = pg_info.childGet(0);
    }
    else
    {
	//>> Check the new node structure and the old node
	XMLNode n_node("info");
	n_node.setAttr("path",sel_path);
	if( cntrIfCmd(n_node) ) { throw TError(atoi(n_node.attr("rez").c_str()),n_node.attr("mcat").c_str(),"%s",n_node.text().c_str()); }
	upStruct( *root, *n_node.childGet(0) );
    }

    tabs->blockSignals( true );
    selectChildRecArea( *root, "/" );
    tabs->blockSignals( false );

    //> The add and the delete access allow check
    actItAdd->setEnabled(false);
    if( root->childGet("id","br",true) )
    {
	XMLNode *branch = root->childGet("id","br");
	for( int i_b = 0; i_b < branch->childSize(); i_b++ )
	    if( atoi(branch->childGet(i_b)->attr("acs").c_str())&SEQ_WR )
	    { actItAdd->setEnabled(true); break; }
    }
    actItDel->setEnabled( root&&atoi(root->attr("acs").c_str())&SEQ_WR );

    //> Load and Save allow check
    actDBLoad->setEnabled(false); actDBSave->setEnabled(false);
    XMLNode req("modify");
    req.setAttr("path",sel_path+"/%2fobj");
    if( cntrIfCmd(req) ) mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
    else if( atoi(req.text().c_str()) )	{ actDBLoad->setEnabled(true); actDBSave->setEnabled(true); }

    //> Local station modifying check
    mStModify->setText(" ");
    req.setAttr("path","/"+SYS->id()+"/%2fobj");
    if( !cntrIfCmd(req) && atoi(req.text().c_str()) )	mStModify->setText("*");

    //> Edit tools update
    editToolUpdate( );
}

bool ConfApp::upStruct(XMLNode &w_nd, const XMLNode &n_nd)
{
    bool str_ch = false;

    //> Check access
    if( w_nd.attr("acs") != n_nd.attr("acs") ) str_ch = true;

    //> Scan deleted nodes
    for( int i_w = 0; i_w < w_nd.childSize(); i_w++ )
    {
	int i_n;
	for( i_n = 0; i_n < n_nd.childSize(); i_n++ )
	    if( w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id") )
		break;
	if( i_n >= n_nd.childSize() )
	{
	    w_nd.childDel(i_w--);
	    if( w_nd.name() != "table" && w_nd.name() != "list" ) str_ch = true;
	}
    }

    //> Scan for new nodes and check present nodes
    for( int i_n = 0; i_n < n_nd.childSize(); i_n++ )
    {
	int i_w;
	for( i_w = 0; i_w < w_nd.childSize(); i_w++)
	    if( w_nd.childGet(i_w)->name() == n_nd.childGet(i_n)->name() &&
		    w_nd.childGet(i_w)->attr("id") == n_nd.childGet(i_n)->attr("id") )
		break;
	if( i_w >= w_nd.childSize() )
	{
	    //>> Add node
	    *w_nd.childIns(i_n) = *n_nd.childGet(i_n);
	    str_ch = true;
	    i_w = i_n;
	}
	else
	{
	    //>> Check present node
	    if( upStruct(*w_nd.childGet(i_w),*n_nd.childGet(i_n)) ) str_ch = true;
	    if( str_ch && w_nd.name() == "oscada_cntr" )
	    {
		w_nd.childGet(i_w)->setAttr("qview","0");
		str_ch = false;
		continue;
	    }
	}

	//> Check of the description present
	if( (bool)w_nd.childGet(i_w)->attr("dscr").size() ^ (bool)n_nd.childGet(i_n)->attr("dscr").size() )
	    str_ch = true;

	//> Check base fields destination change
	if( w_nd.childGet(i_w)->name() == "fld" &&
	    (w_nd.childGet(i_w)->attr("dest") != n_nd.childGet(i_n)->attr("dest") ||
	     w_nd.childGet(i_w)->attr("tp") != n_nd.childGet(i_n)->attr("tp")) )
	{
	    w_nd.childGet(i_w)->setAttr("dest","");
	    w_nd.childGet(i_w)->setAttr("tp","");
	    str_ch = true;
	}

	//> Sync node parameters (text and atributes)
	w_nd.childGet(i_w)->setText(n_nd.childGet(i_n)->text());
	vector<string> ls;
	n_nd.childGet(i_n)->attrList(ls);
	for( int i_a = 0; i_a < ls.size(); i_a++ )
	    w_nd.childGet(i_w)->setAttr(ls[i_a],n_nd.childGet(i_n)->attr(ls[i_a]));
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
	if( lview && lview->currentItem() && lview->currentItem()->text(2)[0] != '*' )
	{
	    //> Load and Save actions
	    popup.addAction(actDBLoad);
	    popup.addAction(actDBSave);
	    popup.addSeparator();
	    //> Add and delete item action add
	    popup.addAction(actItAdd);
	    popup.addAction(actItDel);
	    popup.addSeparator();
	    popup.addAction(actItCut);
	    popup.addAction(actItCopy);
	    popup.addAction(actItPaste);
	    popup.addSeparator();
	}
	//> Main action add
	QImage ico_t;
	if( !ico_t.load(TUIS::icoPath("reload").c_str()) ) ico_t.load(":/images/reload.png");
	QAction *actRemHostUp = new QAction( QPixmap::fromImage(ico_t), _("Refresh items tree"), this );
	popup.addAction(actRemHostUp);
	QAction *rez = popup.exec(QCursor::pos());
	if( rez == actRemHostUp )
	{
	    initHosts( );
	    treeUpdate( );
	}
	popup.clear( );
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::tabSelect( QWidget *wdg )
{
    try
    {
	pageCyclRefrStop();
	pageDisplay( sel_path );
    }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::viewChildRecArea( QTreeWidgetItem *i, bool upTree )
{
#if OSC_DEBUG >= 3
    unsigned long long t_cnt = SYS->shrtCnt();
#endif

    QStringList grps = i->data(2,Qt::UserRole).toStringList();
    if( grps.empty() ) return;
    else if( grps.size() > 1 )
    {
	//> Add and update present
	for( int i_g = 0; i_g < grps.size(); i_g++ )
	{
	    bool grpChCnt = atoi(TSYS::strSepParse(grps[i_g].toAscii().data(),0,'\n').c_str());
	    string grpId = TSYS::strSepParse(grps[i_g].toAscii().data(),1,'\n');
	    string grpDscr = TSYS::strSepParse(grps[i_g].toAscii().data(),2,'\n');
	    QTreeWidgetItem *it = NULL;
	    //> Search present item
	    if( upTree )
		for( int i_it = 0; i_it < i->childCount(); i_it++ )
		    if( i->child(i_it)->text(2) == ("*"+grpId).c_str() )
		    { it = i->child(i_it); break; }
	    if( !it ) it = new QTreeWidgetItem(i);
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
	    //>> Next node for update
	    if( upTree && it->isExpanded() )	viewChildRecArea(it,upTree);
	}
	//> Delete no present
	if( upTree )
	    for( int i_it = 0; i_it < i->childCount(); i_it++ )
	    {
		int i_g;
		for( i_g = 0; i_g < grps.size(); i_g++ )
		    if( i->child(i_it)->text(2) == ("*"+TSYS::strSepParse(grps[i_g].toAscii().data(),1,'\n')).c_str() )
			break;
		if( i_g >= grps.size() ) { delete i->takeChild(i_it); i_it--; }
	    }
    }
    else
    {
	string path = i->text(2).toAscii().data();
	if(path[0]=='*') path = i->parent()->text(2).toAscii().data();
	string grpId = TSYS::strSepParse(grps[0].toAscii().data(),1,'\n');
	string grpDscr = TSYS::strSepParse(grps[0].toAscii().data(),2,'\n');
	XMLNode req("chlds");
	req.setAttr("path",path+"/%2fobj")->setAttr("grp",grpId);
	if( cntrIfCmd(req) )
	{
	    if( atoi(req.attr("rez").c_str()) == 10 ) initHosts( );
	    mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	    return;
	}
	//> Add and update present
	for( int i_e = 0; i_e < req.childSize(); i_e++ )
	{
	    XMLNode *chEl = req.childGet(i_e);
	    //>> Prepare branch patch
	    string br_path = grpId;
	    if( chEl->attr("id").size() ) br_path.append(chEl->attr("id")); else br_path.append(chEl->text());
	    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
	    //>> Find item
	    QTreeWidgetItem *it = NULL;
	    if( upTree )
		for( int i_it = 0; i_it < i->childCount(); i_it++ )
		    if( i->child(i_it)->text(2) == (path+"/"+br_path).c_str() )
		    { it = i->child(i_it); break; }
	    if( !it ) it = new QTreeWidgetItem(i);
	    it->setText(0,chEl->text().c_str());
	    it->setText(1,grpDscr.c_str());
	    it->setText(2,(path+"/"+br_path).c_str());
	    //>> Set icon
	    XMLNode *chIco = chEl->childGet("ico",0,true);
	    if( chIco )
	    {
		string simg = TSYS::strDecode(chIco->text(),TSYS::base64);
		QImage img;
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    it->setIcon(0,QPixmap::fromImage(img).scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	    }
	    //>> Set groups
	    QStringList it_grp;
	    for( int i_g = 0; i_g < chEl->childSize(); i_g++ )
		if( chEl->childGet(i_g)->name() == "grp" )
		    it_grp.push_back((chEl->childGet(i_g)->attr("chPresent")+"\n"+chEl->childGet(i_g)->attr("id")+"\n"+chEl->childGet(i_g)->attr("dscr")).c_str());
	    it->setData(2,Qt::UserRole,it_grp);
	    //>> Check for childs present
	    bool grpChCnt = it_grp.size() && (it_grp.size()>1 || atoi(TSYS::strSepParse(it_grp[0].toAscii().data(),0,'\n').c_str()));
	    it->setChildIndicatorPolicy(grpChCnt?QTreeWidgetItem::ShowIndicator:QTreeWidgetItem::DontShowIndicator);
	    //>> Next node for update
	    if( upTree && it->isExpanded() )	viewChildRecArea(it,upTree);
	}
	//> Delete no present
	if( upTree )
	    for( int i_it = 0; i_it < i->childCount(); i_it++ )
	    {
		int i_e;
		for( i_e = 0; i_e < req.childSize(); i_e++ )
		{
		    //>> Prepare branch patch
		    string br_path = grpId;
		    if( req.childGet(i_e)->attr("id").size() ) br_path.append(req.childGet(i_e)->attr("id")); else br_path.append(req.childGet(i_e)->text());
		    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
		    if( i->child(i_it)->text(2) == (path+"/"+br_path).c_str() )	break;
		}
		if( i_e >= req.childSize() ) { delete i->takeChild(i_it); i_it--; }
	    }

	if( !i->parent() && i->data(0,Qt::UserRole).toInt() == 10 ) initHosts( );
    }

#if OSC_DEBUG >= 3
    mess_debug("QTCfg DEBUG",_("Expand/update branch '%s' time %f ms."),i->text(2).toAscii().data(),1.0e3*((double)(SYS->shrtCnt()-t_cnt))/((double)SYS->sysClk()));
    t_cnt = SYS->shrtCnt();
#endif
}

int ConfApp::cntrIfCmd( XMLNode &node )
{
    int path_off = 0;
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false,&path_off);
    if( station.empty() ) station = sel_path = SYS->id();
    else node.setAttr("path",path.substr(path_off));

    try
    {
	//> Check local station request
	if( station == SYS->id() )
	{
	    node.setAttr("user",w_user->user().toAscii().data());
	    SYS->cntrCmd(&node);
	    node.setAttr("path",path);
	    return atoi(node.attr("rez").c_str());
	}

	//> Request to remote host
	TTransportS::ExtHost host = SYS->transport().at().extHostGet(w_user->user().toAscii().data(),station);
	AutoHD<TTransportOut> tr = SYS->transport().at().extHost(host,"TrCntr");
	if( !tr.at().startStat() ) tr.at().start();

	node.setAttr("rqDir","0")->setAttr("rqUser",host.user)->setAttr("rqPass",host.pass);
	tr.at().messProtIO(node,"SelfSystem");
	node.setAttr("path",path);
    }catch( TError err )
    { node.setAttr("mcat",err.cat)->setAttr("rez","10")->setText(err.mess); }
    return atoi(node.attr("rez").c_str());
}

void ConfApp::initHosts( )
{
    vector<string> stls;
    SYS->transport().at().extHostList(w_user->user().toAscii().data(),stls);
    stls.insert(stls.begin(),SYS->id());

    //> Remove no present hosts
    for( int i_top = 0; i_top < CtrTree->topLevelItemCount(); i_top++ )
    {
	int i_h;
	for( i_h = 0; i_h < stls.size(); i_h++ )
	    if( stls[i_h] == TSYS::pathLev(CtrTree->topLevelItem(i_top)->text(2).toAscii().data(),0) )
		break;
	if( i_h >= stls.size() ) { delete CtrTree->takeTopLevelItem(i_top); i_top--; }
    }

    //> Add/update hosts
    bool emptyTree = !CtrTree->topLevelItemCount();
    for( int i_st = 0; i_st < stls.size(); i_st++ )
    {
	int errCon = 0;

	QTreeWidgetItem *nit = NULL;
	if( !emptyTree )
	    for( int i_top = 0; i_top < CtrTree->topLevelItemCount(); i_top++ )
		if( stls[i_st] == TSYS::pathLev(CtrTree->topLevelItem(i_top)->text(2).toAscii().data(),0) )
		{ nit = CtrTree->topLevelItem(i_top); break; }
	if( !nit ) nit = new QTreeWidgetItem(CtrTree);
	if( stls[i_st] == SYS->id() )
	{
	    nit->setText(0,SYS->name().c_str());
	    nit->setText(1,_("Local station"));
	    nit->setText(2,("/"+SYS->id()).c_str());
	}
	else
	{
	    TTransportS::ExtHost host = SYS->transport().at().extHostGet(w_user->user().toAscii().data(),stls[i_st]);
	    nit->setText(0,host.name.c_str());
	    nit->setText(1,_("Remote station"));
	    nit->setText(2,("/"+host.id).c_str());
	}
	nit->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	//>>>> Check icon
	QImage img; string simg;
	XMLNode reqIco("get"); reqIco.setAttr("path","/"+stls[i_st]+"/%2fico");
	errCon = cntrIfCmd(reqIco);
	if( !errCon )
	{ simg = TSYS::strDecode(reqIco.text(),TSYS::base64);
	  if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
	    nit->setIcon(0,QPixmap::fromImage(img).scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	  else nit->setIcon(0,QPixmap());
	}
	//>>>> Process groups
	QStringList it_grp;
	XMLNode brReq("info"); brReq.setAttr("path","/"+SYS->id()+"/%2fbr");
	if( cntrIfCmd(brReq) == 10 ) errCon = 10;
	for( int i_br = 0; brReq.childSize() && i_br < brReq.childGet(0)->childSize(); i_br++ )
	    it_grp.push_back(("1\n"+brReq.childGet(0)->childGet(i_br)->attr("id")+"\n"+brReq.childGet(0)->childGet(i_br)->attr("dscr")).c_str());
	nit->setData(2,Qt::UserRole,it_grp);

	if( errCon == 10 )
	{
	    simg = TUIS::icoGet("disconnect");
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		nit->setIcon(0,QPixmap::fromImage(img).scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation));
	    nit->setExpanded(false);
	}
	nit->setData(0,Qt::UserRole,errCon);
    }
}

//*************************************************
//* ConfApp: Self widget's slots                  *
//*************************************************
void ConfApp::checkBoxStChange( int stat )
{
    XMLNode *n_el;
    QCheckBox *box = (QCheckBox *)sender();

    if( stat==Qt::PartiallyChecked ) return;
    try
    {
	string path = box->objectName().toAscii().data();
	string val = (stat==Qt::Checked)?"1":"0";

	//> Check block element
	if(path[0] == 'b')
	{
	    SYS->ctrId(root,TSYS::strDecode(path.substr(1),TSYS::PathEl) )->setText(val);
	    return;
	}
	else
	{
	    XMLNode req("get");
	    req.setAttr("path",sel_path+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }

	    if( req.text() == val ) return;
	    mess_info( mod->nodePath().c_str(), _("%s| Set <%s> to <%s>!"), w_user->user().toAscii().data(), (sel_path+"/"+path).c_str(), val.c_str() );

	    req.setName("set")->setText(val);
	    if(cntrIfCmd(req))	mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this);
	}
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //> Redraw
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

	//> Check link
	if( n_el->attr("tp") == "lnk")
	{
	    XMLNode req("get"); req.setAttr("path",sel_path+"/"+button->objectName().toAscii().data());
	    if( cntrIfCmd(req) ) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }
	    string url = "/"+TSYS::pathLev(sel_path,0)+req.text();
	    mess_info(mod->nodePath().c_str(),_("%s| Go to link <%s>!"), w_user->user().toAscii().data(),url.c_str());
	    selectPage( url );
	    return;
	}
	else
	{
	    XMLNode req("set"); req.setAttr("path",sel_path+"/"+button->objectName().toAscii().data());
	    //> Copy parameters
	    for( int i_ch = 0; i_ch < n_el->childSize(); i_ch++ )
		*(req.childAdd()) = *(n_el->childGet(i_ch));

	    mess_info(mod->nodePath().c_str(),_("%s| Press <%s>!"), w_user->user().toAscii().data(),
		(sel_path+"/"+button->objectName().toAscii().data()).c_str() );
	    if( cntrIfCmd(req) ) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }
	}
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //> Redraw
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
	if(path[0] == 'b') { block = true; path = path.substr(1); }

	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );

	//> Get list for index list check!
	if( n_el->attr("dest") == "select" )
	{
	    bool find_ok = false;
	    if( n_el->attr("select").empty() )
	    {
		bool ind_ok = n_el->attr("sel_id").size();
		string s_nm;
		for( int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(n_el->attr("sel_list"),0,';',&ls_off)).empty(); c_el++ )
		    if( s_nm == val )
		    {
			if( ind_ok )	val = TSYS::strSepParse(n_el->attr("sel_id"),c_el,';');
			find_ok = true;
		    }
	    }
	    else
	    {
		XMLNode x_lst("get");
		x_lst.setAttr("path",sel_path+"/"+TSYS::strEncode( n_el->attr("select"),TSYS::PathEl));
		if( cntrIfCmd(x_lst) ) { mod->postMess(x_lst.attr("mcat"),x_lst.text(),TUIMod::Error,this); return; }

		for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
		    if( x_lst.childGet(i_el)->name() == "el" && x_lst.childGet(i_el)->text() == val )
		    {
			if( x_lst.childGet(i_el)->attr("id").size() )
			    val = x_lst.childGet(i_el)->attr("id");
			find_ok = true;
		    }
	    }
	    if( !find_ok ) { mod->postMess(mod->nodePath().c_str(),_("Value no valid: ")+val,TUIMod::Info,this); return; }
	}

	//> Check block element. Command box!
	if( block ) { n_el->setText(val); return; }
	else
	{
	    XMLNode req("get"); req.setAttr("path",sel_path+"/"+path);
	    if(cntrIfCmd(req)) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }

	    if( req.text() == val ) return;
	    mess_info(mod->nodePath().c_str(),_("%s| Change <%s> from <%s> to <%s>!"),
		    w_user->user().toAscii().data(), (sel_path+"/"+path).c_str(), req.text().c_str(), val.c_str() );

	    req.setName("set")->setText(val);
	    if( cntrIfCmd(req) ) { mod->postMess(req.attr("mcat"),req.text(),TUIMod::Error,this); return; }
	}
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //- Redraw -
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
	    actBr = last_it = new QAction(_("Go"),this);
	    popup.addAction(actBr);
	    popup.addSeparator();
	}
	if( (atoi(n_el->attr("acs").c_str())&SEQ_WR) && n_el->attr("s_com").size() )
	{
	    if( n_el->attr("s_com").find("add") != string::npos )
	    {
	        actAdd = last_it = new QAction(_("Add"),this);
		popup.addAction(actAdd);
	    }
	    if( n_el->attr("s_com").find("ins") != string::npos && item != NULL )
	    {
		actIns = last_it = new QAction(_("Insert"),this);
		popup.addAction(actIns);
	    }
	    if( n_el->attr("s_com").find("edit") != string::npos && item != NULL )
	    {
		actEd = last_it = new QAction(_("Edit"),this);
		popup.addAction(actEd);
	    }
	    if( n_el->attr("s_com").find("del") != string::npos && item != NULL )
	    {
		popup.addSeparator();
		actDel = last_it = new QAction(_("Delete"),this);
		popup.addAction(actDel);
	    }
	    if( n_el->attr("s_com").find("move") != string::npos && item != NULL )
	    {
		popup.addSeparator();
		actMoveUp = last_it = new QAction(_("Up"),this);
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(_("Down"),this);
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
		//> Get select id
		XMLNode x_lst("get");
		x_lst.setAttr("path",el_path);
		if( cntrIfCmd(x_lst) )
		{
		    mod->postMess(x_lst.attr("x_lst"),x_lst.text(),TUIMod::Error,this);
		    return;
		}
		
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
	    if( !rez )	{ popup.clear(); return; }
	    if( rez == actAdd || rez == actIns || rez == actEd )
	    {
		ReqIdNameDlg dlg(this,this->windowIcon(),"",_("Item name set"));
		vector<string> ils;
		ils.push_back(n_el->attr("idSz")+"\n"+TSYS::int2str(ind_m));
		dlg.setTargets(ils);
		if( rez == actAdd )	dlg.setMess(_("Add new element."));
		else if( rez == actIns )dlg.setMess(_("Insert new element."));
		if( rez == actEd )
		{
		    dlg.setMess(_("Rename element."));
		    dlg.setId(p_id.c_str());
		    dlg.setName(p_text.c_str());
		}
		int dlrez = dlg.exec();
		id = dlg.id().toAscii().data();
		text = ind_m ? dlg.name().toAscii().data() : id;
		if( dlrez != QDialog::Accepted ) return;
	    }

	    //> Make command
	    XMLNode n_el1;
	    n_el1.setAttr("path",el_path);
	    if( rez == actBr )
	    {
		listBoxGo(item);
		return;
	    }
	    else if( rez == actAdd )
	    {
		n_el1.setName("add");
		if( ind_m ) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| Add <%s> element <%s:%s>!"),
			w_user->user().toAscii().data(), el_path.c_str(), id.c_str(), text.c_str() );
	    }
	    else if( rez == actIns )
	    {
		n_el1.setName("ins")->setAttr("pos",TSYS::int2str(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| Insert <%s> element <%s:%s> to %d!"),
			w_user->user().toAscii().data(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if( rez == actEd )
	    {
		n_el1.setName("edit")->setAttr("pos",TSYS::int2str(c_id))->setAttr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.setAttr("id",id);
		n_el1.setText(text);
		mess_info(mod->nodePath().c_str(),_("%s| Set <%s> element %d to <%s:%s>!"),
			w_user->user().toAscii().data(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if( rez == actDel )
	    {
		n_el1.setName("del")->setAttr("pos",TSYS::int2str(c_id));
		if( ind_m ) n_el1.setAttr("id",p_id);
		else n_el1.setText(item->text().toAscii().data());
		mess_info(mod->nodePath().c_str(),_("%s| Delete <%s> element <%s:%s>!"),
			w_user->user().toAscii().data(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if( rez == actMoveUp || rez == actMoveDown )
	    {
		int c_new = (rez==actMoveDown) ? c_id+1 : c_id-1;
		n_el1.setName("move")->setAttr("pos",TSYS::int2str(c_id))->setAttr("to",TSYS::int2str(c_new));
		mess_info(mod->nodePath().c_str(),_("%s| Move <%s> from %d to %d!"),
			w_user->user().toAscii().data(), el_path.c_str(), c_id, c_new);
	    }
	    if( cntrIfCmd(n_el1) )
	    {
		mod->postMess(n_el1.attr("mcat"),n_el1.text(),TUIMod::Error,this);
		return;
	    }

	    autoUpdTimer->setSingleShot(true);
	    autoUpdTimer->start(CH_REFR_TM);      //Redraw

	    if( n_el->attr("tp") == "br" && (rez == actAdd || rez == actIns || rez == actEd || rez == actDel) )
		treeUpdate();

	    popup.clear();
	}
    }catch(TError err)
    {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);

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
	        actAdd = last_it = new QAction(_("Add record"),this);
		popup.addAction(actAdd);
	    }
	    if( n_el->attr("s_com").find("ins") != string::npos && row != -1 )
	    {
		actIns = last_it = new QAction(_("Insert record"),this);
		popup.addAction(actIns);
	    }
	    if( n_el->attr("s_com").find("del") != string::npos && row != -1 )
	    {
		actDel = last_it = new QAction(_("Delete record"),this);
		popup.addAction(actDel);
	    }
	    if( n_el->attr("s_com").find("move") != string::npos && row != -1 )
	    {
		popup.addSeparator();
		actMoveUp = last_it = new QAction(_("Move Up"),this);
		popup.addAction(actMoveUp);
		actMoveDown = last_it = new QAction(_("Move Down"),this);
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
	    n_el1.setAttr("path",el_path);
	    if( rez == actAdd )
	    {
		n_el1.setName("add");
		mess_info(mod->nodePath().c_str(),_("%s| Add <%s> record."),
			w_user->user().toAscii().data(), el_path.c_str() );
	    }
	    else if( rez == actIns )
	    {
		n_el1.setName("ins");
		n_el1.setAttr("row",TSYS::int2str(row));
		mess_info(mod->nodePath().c_str(),_("%s| Insert <%s> record %d."),
			w_user->user().toAscii().data(), el_path.c_str(), row );
	    }
	    else if( rez == actDel )
	    {
		n_el1.setName("del");
		string row_addr;
		if( !n_el->attr("key").size() )
		{
		    row_addr = TSYS::int2str(row);
		    n_el1.setAttr("row",row_addr);
		}
		else
		{
		    //-- Get Key columns --
		    string key;
		    for( int i_off = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&i_off)).size(); )
		        for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
		            if( n_el->childGet(i_el)->attr("id") == key )
		            {
				n_el1.setAttr("key_"+key,n_el->childGet(i_el)->childGet(row)->text());
				row_addr=row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
				break;
			    }
		}
		mess_info(mod->nodePath().c_str(),_("%s| Delete <%s> record <%s>."),
			w_user->user().toAscii().data(), el_path.c_str(), row_addr.c_str() );
	    }
	    else if( rez == actMoveUp || rez == actMoveDown )
	    {
		int r_new = row-1;
		if( rez == actMoveDown )  r_new = row+1;
		n_el1.setName("move");
		n_el1.setAttr("row",TSYS::int2str(row))->setAttr("to",TSYS::int2str(r_new));
		mess_info(mod->nodePath().c_str(),_("%s| Move <%s> record from %d to %d."),
			w_user->user().toAscii().data(), el_path.c_str(), row, r_new );
	    }
	    if( cntrIfCmd(n_el1) )
	    {
		mod->postMess(n_el1.attr("mcat"),n_el1.text(),TUIMod::Error,this);
		return;
	    }

	    autoUpdTimer->setSingleShot(true);
	    autoUpdTimer->start(CH_REFR_TM);

	    popup.clear();
	}
    }catch(TError err)
    {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);

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
	    save_img = last_it = new QAction(_("Save image"),this);
	    popup.addAction(save_img);
	}
	if(atoi(n_el->attr("acs").c_str())&SEQ_WR)
	{
	    load_img = last_it = new QAction(_("Load image"),this);
	    popup.addAction(load_img);
	}

	if(last_it)
	{
	    QAction *rez = popup.exec(QCursor::pos());
	    if(!rez)	return;
	    if( rez == save_img )
	    {
		QString fileName = QFileDialog::getSaveFileName(this,_("Save picture"),
			"img.png", _("Images (*.png *.xpm *.jpg)"));
		if( !fileName.isEmpty() && !img->image().save(fileName) )
		    throw TError(mod->nodePath().c_str(),_("Save to file %s error\n"),fileName.toAscii().data());
	    }
	    else if( rez == load_img )
	    {
		//> Get path to image file
		QString fileName = QFileDialog::getOpenFileName(this,_("Load picture"),"",_("Images (*.png *.jpg)"));
		if( fileName.isNull( ) ) return;
		int len;
		char buf[STR_BUF_LEN];
		string rez;

		//> Load image file
		int hd = open(fileName.toAscii().data(),O_RDONLY);
		if( hd < 0 )	throw TError(mod->nodePath().c_str(),_("Open file %s error\n"),fileName.toAscii().data());
		{
		    while( len = read(hd,buf,sizeof(buf)) )
		        rez.append(buf,len);
		    ::close(hd);
		}

		//> Set image to widget
		if( !img->setImage(rez) )
		    throw TError(mod->nodePath().c_str(),_("Image file %s error\n"),fileName.toAscii().data());

		//> Send image to system
		XMLNode n_el1("set");
		n_el1.setAttr("path",el_path)->setText(TSYS::strEncode(rez,TSYS::base64));
		mess_info(mod->nodePath().c_str(),_("%s| Upload picture <%s> to: %s."),
		    w_user->user().toAscii().data(), fileName.toAscii().data(), el_path.c_str());
		if( cntrIfCmd(n_el1) )
		{
		    mod->postMess(n_el1.attr("mcat"),n_el1.text(),TUIMod::Error,this);
		    return;
		}
	    }
	}
    }catch(TError err)
    {
	mod->postMess(err.cat,err.mess,TUIMod::Error,this);

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
	    if( n_el->childGet(col)->attr("select").empty() )
	    {
		bool ind_ok = n_el->childGet(col)->attr("sel_id").size();
		string s_nm;
		for( int ls_off = 0, c_el = 0; !(s_nm=TSYS::strSepParse(n_el->childGet(col)->attr("sel_list"),0,';',&ls_off)).empty(); c_el++ )
		    if( s_nm == value )
		    {
			if( ind_ok )	value = TSYS::strSepParse(n_el->childGet(col)->attr("sel_id"),c_el,';');
			find_ok = true;
		    }
	    }
	    else
	    {
		XMLNode x_lst("get"); x_lst.setAttr("path",sel_path+"/"+TSYS::strEncode(n_el->childGet(col)->attr("select"),TSYS::PathEl));
		if( cntrIfCmd(x_lst) )
		{ mod->postMess(x_lst.attr("mcat"),x_lst.text(),TUIMod::Error,this); return; }

		for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
		    if( x_lst.childGet(i_el)->text() == value )
		    {
			if( atoi(n_el->childGet(col)->attr("idm").c_str()) )
			    value = x_lst.childGet(i_el)->attr("id");
			find_ok = true;
		    }
	    }
	    if( !find_ok ) throw TError(mod->nodePath().c_str(),_("Value <%s> is not valid!"),value.c_str());
	}
	else value = val.toString().toAscii().data();

	//> Prepare request
	XMLNode n_el1("set"); n_el1.setAttr("path",el_path)->setText(value);
	//>> Get current column id
	for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
	    if( tbl->horizontalHeaderItem(col)->data(Qt::DisplayRole).toString() == n_el->childGet(i_el)->attr("dscr").c_str() )
	    { n_el1.setAttr("col",n_el->childGet(i_el)->attr("id")); break; }
	//>> Get row position
	string row_addr;
	if( !n_el->attr("key").size() )
	{
	    row_addr=TSYS::int2str(row);
	    n_el1.setAttr("row",row_addr);
	}
	else
	{
	    //>> Get Key columns
	    string key;
	    for( int i_off = 0; (key=TSYS::strSepParse(n_el->attr("key"),0,',',&i_off)).size(); )
		for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
		    if( n_el->childGet(i_el)->attr("id") == key )
		    {
			n_el1.setAttr("key_"+key,n_el->childGet(i_el)->childGet(row)->text());
			row_addr=row_addr+"key_"+key+"="+n_el1.attr("key_"+key)+",";
			break;
		    }
	}

	//>> Put request
	mess_info(mod->nodePath().c_str(),_("%s| Set <%s> cell (<%s>:%s) to: %s."),
	    w_user->user().toAscii().data(), el_path.c_str(), row_addr.c_str(), n_el1.attr("col").c_str(), value.c_str());
	if(cntrIfCmd(n_el1))	throw TError(n_el1.attr("mcat").c_str(),n_el1.text().c_str());
    }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

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
	XMLNode &t_c = *TCntrNode::ctrId(root,TSYS::strDecode(lbox->objectName().toAscii().data(),TSYS::PathEl) );
	string br_pref = t_c.attr("br_pref");

	XMLNode req("get"); req.setAttr("path",sel_path+"/"+lbox->objectName().toAscii().data());
	if( cntrIfCmd(req) ) throw TError(mod->nodePath().c_str(),"%s",req.text().c_str(),4);

	//> Find selected index
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
	if( !sel_ok ) throw TError(mod->nodePath().c_str(),_("Selective element <%s> is not present!"),item->text().toAscii().data());

	selectPage(path);
    }
    catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::editChange( const QString& txt )
{
    QWidget *wed = (QWidget *)sender();

    try
    {
	string path = wed->objectName().toAscii().data();
	//- Check block element -
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) )->setText(txt.toAscii().data());
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }
}

void ConfApp::applyButton( )
{
    QWidget *bwidg = (QWidget *)sender();

    string path = bwidg->objectName().toAscii().data();

    try
    {
	XMLNode *el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl));
	string sval = el->text();
	if( el->attr("tp") == "hex" )		sval = TSYS::int2str(QString(sval.c_str()).toUInt(0,16));
	else if( el->attr("tp") == "oct" )	sval = TSYS::int2str(QString(sval.c_str()).toUInt(0,8));

	mess_info(mod->nodePath().c_str(),_("%s| Change <%s> to: <%s>!"),
		w_user->user().toAscii().data(), (sel_path+"/"+path).c_str(), sval.c_str() );

	XMLNode n_el("set");
	n_el.setAttr("path",sel_path+"/"+path)->setText(sval);
	if( cntrIfCmd(n_el) ) { mod->postMess(n_el.attr("mcat"),n_el.text(),TUIMod::Error,this); return; }
    }catch(TError err) { mod->postMess(err.cat,err.mess,TUIMod::Error,this); }

    //- Redraw -
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

void ConfApp::cancelButton( )
{
    //> Redraw
    autoUpdTimer->setSingleShot(true);
    autoUpdTimer->start(CH_REFR_TM);
}

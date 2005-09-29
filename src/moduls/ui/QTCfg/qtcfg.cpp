/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#include <qapplication.h>
#include <qheader.h>
#include <qcursor.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qtoolbar.h> 
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qgroupbox.h> 
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qlistbox.h>
#include <qerrormessage.h>
#include <qinputdialog.h>
#include <qvbox.h> 
#include <qimage.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qtable.h>
#include <qcolor.h>
#include <qvalidator.h>

#include "xpm/up.xpm"
#include "xpm/back.xpm"
#include "xpm/forward.xpm"
#include "xpm/window_new.xpm"
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

#include "tuimod.h"
#include "dlguser.h"
#include "qtcfg.h"


using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( ) : 
    QMainWindow( 0, "", WDestructiveClose ), que_sz(20), block_tabs(false)
{   
    mod->regWin( this );

    setCaption(mod->I18N("QT Configurator of OpenSCADA"));
    setIcon(QPixmap(QImage(oscada_cfg_xpm)));
    
    //Centrall widget
    setCentralWidget( new QWidget( this, "CentralWidget" ) );
    QGridLayout *QTCfgLayout = new QGridLayout( centralWidget(), 1, 1, 4, -1 );	    
    
    //Splitter
    QSplitter *splitter = new QSplitter( centralWidget() );
    splitter->setOrientation( QSplitter::Horizontal );	
    
    //Create Navigator tree
    CtrTree = new QListView( splitter );
    splitter->setResizeMode(CtrTree, QSplitter::KeepSize);
    CtrTree->setRootIsDecorated( true );
    CtrTree->addColumn(mod->I18N("Name"));
    CtrTree->addColumn("descr",0);    
    CtrTree->addColumn("path",0);
    CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->resize( QSize( 200, -1 ) );
    CtrTree->setMinimumSize( QSize( 150, 0 ) );
    CtrTree->setMaximumSize( QSize( 400, 32767 ) );
    CtrTree->setResizeMode( QListView::NoColumn );    
    //currentChanged(
    connect( CtrTree, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( pressed(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( expanded(QListViewItem*) ), this, SLOT( viewChild(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( onItem(QListViewItem*) ), this, SLOT( onItem(QListViewItem*) ) );
		
    QToolTip::remove( CtrTree );
    new ListViewToolTip( CtrTree );
    
    //Right frame add
    QFrame *gFrame = new QFrame( splitter );    
    gFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 3, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );		    
    gFrame->setFrameShape( QFrame::WinPanel );
    gFrame->setFrameShadow( QFrame::Raised );	        

    QGridLayout *gFrameLayout = new QGridLayout( gFrame, 1, 1, 7, -1 );    
    titleLab = new QLabel( gFrame );
    titleLab->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, 0, 0 ) );		    
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 0 );
    
    w_user = new QPushButton(QPixmap(QImage(identity_xpm)), "root", gFrame );		//!!!! Mybe not root!    
    w_user->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Maximum, 0, 0 ) );
    QToolTip::add( w_user, mod->I18N("Change user."));
    QWhatsThis::add( w_user, mod->I18N("This button change the OpenSCADA system user."));	
    connect(w_user, SIGNAL(clicked()), this, SLOT(userSel()));        
    w_user->setPaletteBackgroundColor(QColor(255,0,0));
    gFrameLayout->addWidget( w_user, 0, 1 );
    
    tabs = new QTabWidget( gFrame );
    connect( tabs, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabSelect(QWidget*) ) );
    gFrameLayout->addMultiCellWidget( tabs, 1, 1, 0, 1 );
    //gFrameLayout->addWidget( tabs, 1, 0 );
    
    QTCfgLayout->addWidget( splitter, 0, 0 );    
    
    QListViewItem *root_l_it = new QListViewItem(CtrTree,SYS->station(),mod->I18N("Local station"),SYS->station());
    CtrTree->insertItem( root_l_it );
    //viewChild( root_l_it );
    
    //Status bar
    statusBar()->message(mod->I18N("Ready"), 2000 );
    resize( 800, 600 );
    
    //Create actions
    //Close
    QAction *actClose = new QAction("",QIconSet(QImage(close_xpm)),mod->I18N("&Close"),CTRL+Key_W,this);
    actClose->setToolTip(mod->I18N("Close configurator window"));
    actClose->setWhatsThis(mod->I18N("Close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //Quit
    QAction *actQuit = new QAction("",QIconSet(QImage(exit_xpm)),mod->I18N("&Quit"),CTRL+Key_Q,this);
    actQuit->setToolTip(mod->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(mod->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //Up button
    actUp = new QAction(mod->I18N("Up"),QIconSet(QImage(up_xpm)),mod->I18N("&Up"),ALT+Key_Up,this);
    actUp->setToolTip(mod->I18N("Up page"));
    actUp->setWhatsThis(mod->I18N("Go to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(pageUp()));    
    //Previos page
    actPrev = new QAction(mod->I18N("Previos"),QPixmap(QImage(back_xpm)),mod->I18N("&Previos"),ALT+Key_Left,this);
    actPrev->setToolTip(mod->I18N("Previos page"));
    actPrev->setWhatsThis(mod->I18N("Go to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(pagePrev()));    
    //Previos page
    actNext = new QAction(mod->I18N("Next"),QPixmap(QImage(forward_xpm)),mod->I18N("&Next"),ALT+Key_Right,this);
    actNext->setToolTip(mod->I18N("Next page"));
    actNext->setWhatsThis(mod->I18N("Go to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(pageNext()));    
    //Update
    QAction *actUpdate = new QAction(mod->I18N("Refresh"),QPixmap(QImage(reload_xpm)),mod->I18N("&Refresh"),Key_F5,this);
    actUpdate->setToolTip(mod->I18N("Refresh current page"));
    actUpdate->setWhatsThis(mod->I18N("Button for refreshing a content of the current page."));
    connect(actUpdate, SIGNAL(activated()), this, SLOT(pageRefresh()));        
    //Start of "Auto update"
    actStartUpd = new QAction(mod->I18N("Start"),QPixmap(QImage(start_xpm)),mod->I18N("&Start"),CTRL+Key_B,this);
    actStartUpd->setToolTip(mod->I18N("Start cycled refresh"));
    actStartUpd->setWhatsThis(mod->I18N("Button for start of cycled refresh content of the current page."));
    connect(actStartUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStart()));        
    //Stop of "Auto update"
    actStopUpd = new QAction(mod->I18N("Stop"),QPixmap(QImage(stop_xpm)),mod->I18N("&Stop"),CTRL+Key_E,this);
    actStopUpd->setToolTip(mod->I18N("Stop cycled refresh"));
    actStopUpd->setWhatsThis(mod->I18N("Button for stop of cycled refresh content of the current page."));
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStop()));        
    
    //Create menu "file"
    QPopupMenu *mn_file = new QPopupMenu( this );
    menuBar()->insertItem(mod->I18N("&File"), mn_file );
    actClose->addTo(mn_file);
    actQuit->addTo(mn_file);
    //Create menu "view"
    QPopupMenu *mn_view = new QPopupMenu( this );
    menuBar()->insertItem(mod->I18N("&View"), mn_view );    
    actUpdate->addTo(mn_view);
    actStartUpd->addTo(mn_view);
    actStopUpd->addTo(mn_view);    
    //Create menu "go"
    QPopupMenu *mn_go = new QPopupMenu( this );
    menuBar()->insertItem(mod->I18N("&Go"), mn_go );    
    actUp->addTo(mn_go);
    actPrev->addTo(mn_go);
    actNext->addTo(mn_go);    
    //Create menu "help"
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem(mod->I18N("&Help"), help );
    help->insertItem(QPixmap(QImage(help_xpm)), mod->I18N("&About"), this, SLOT(about()), Key_F1 );
    help->insertItem(mod->I18N("About &Qt"), this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem(QPixmap(QImage(contexthelp_xpm)), mod->I18N("What's &This"), this, SLOT(whatsThis()), SHIFT+Key_F1 );
    
    //Tool bar
    QToolBar *toolBar = new QToolBar(mod->I18N("OpenSCADA toolbar"), this, DockTop );          
    actUp->addTo(toolBar);
    actPrev->addTo(toolBar);
    actNext->addTo(toolBar);
    toolBar->addSeparator();
    actUpdate->addTo(toolBar);
    actStartUpd->addTo(toolBar);
    actStopUpd->addTo(toolBar);
    
    //Display resource
    hd_res = ResAlloc::resCreate();
    
    //Create auto update timer
    autoUpdTimer = new QTimer( this );
    connect( autoUpdTimer, SIGNAL(timeout()), SLOT(pageRefresh()) );
    
    //Display root page
    viewChild( root_l_it );
    pageDisplay( getItemPath( root_l_it ) );
}

ConfApp::~ConfApp()
{
    mod->unregWin( this );

    ResAlloc::resDelete(hd_res);
}

void ConfApp::quitSt()
{
    SYS->stop();
}

void ConfApp::pageUp()
{
    int i_l = sel_path.rfind("/");
    if( i_l == string::npos || i_l == 0 ) return;
    //Prev and next
    if( sel_path.size() )	prev.insert(prev.begin(),sel_path);
    if( prev.size() >= que_sz )	prev.pop_back();
    next.clear();	
    
    try{ pageDisplay( sel_path.substr(0,i_l) );	} catch(TError err) { postMess(err.cat,err.mess,4); }
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
    mod->owner().owner().security().at().usrList(u_list);
    d_usr->user(u_list);
    int rez = d_usr->exec();
    string dl_user   = d_usr->user();
    string dl_passwd = d_usr->password();
    delete d_usr;    
    
    if( rez && dl_user != w_user->text() )
    {
	try
	{
	    if( mod->owner().owner().security().at().usrAt(dl_user).at().auth(dl_passwd) ) 
	    {
		w_user->setText( dl_user );
		if( dl_user == "root" )	w_user->setPaletteBackgroundColor(QColor(255,0,0));
		else 			w_user->setPaletteBackgroundColor(QColor(0,255,0));
		pageDisplay( getItemPath( CtrTree->firstChild() ) );
	    }
	    else postMess(mod->nodePath(),mod->I18N("Auth wrong!!!"),2);
	} catch(TError err) { postMess(err.cat,err.mess,4); }       
    }    
}

void ConfApp::pageRefresh()
{
    try{ pageDisplay(sel_path); }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::pageCyclRefrStart()
{
    actStartUpd->setEnabled(false);
    actStopUpd->setEnabled(true);

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
        "OpenSCADA Qt based system configurator v%s.\n"
	"Autor: %s\n"
	"License: %s\n"),
        mod->modInfo("Version").c_str(),mod->modInfo("Autors").c_str(),mod->modInfo("License").c_str());
    
    QMessageBox::about( this,caption(),buf);
}

void ConfApp::aboutQt()
{
    QMessageBox::aboutQt( this, mod->I18N("Qt Application."));
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void ConfApp::selectItem( QListViewItem * i )
{    
    if(!i || getItemPath(i) == sel_path ) return;
    
    try
    { 
        //Prev and next
	if( sel_path.size() )		prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();
	next.clear();	

	pageDisplay( getItemPath( i ) );	
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
	titleLab->setText(string("<p align='center'><i><b>")+TSYS::strCode(node.attr("dscr"),TSYS::Html)+"</b></i></p>");

	//Delete tabs of deleted areas
	for( int i_tbs = 0; i_tbs < tabs->count(); i_tbs++ )
	{
	    unsigned i_cf;
	    for( i_cf = 0; i_cf < node.childSize(); i_cf++)
		if( node.childGet(i_cf)->name() == "area" && 
			tabs->tabLabel(tabs->page(i_tbs)) == node.childGet(i_cf)->attr("dscr") )
		    break;
	    if( i_cf >= node.childSize() )
	    {
		block_tabs = true;
		if( i_tbs == tabs->currentPageIndex() )
		    node.childGet(i_tbs)->attr("qview","0");
		QWidget *tab = tabs->page(i_tbs);
        	tabs->removePage(tab);
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
		if( tabs->tabLabel(tabs->page(i_tbs)) == t_s.attr("dscr") )
		    break;
	    if( i_tbs >= tabs->count() )
	    {
		QScrollView *scrl = new QScrollView(tabs);
                tabs->insertTab( scrl, t_s.attr("dscr"), i_area );		
	    }

	    //Find and prepare curent tabs
	    if( tabs->currentPageIndex() == i_area )
            {
		bool refresh = atoi(t_s.attr("qview").c_str());
		
		QScrollView *scrl = (QScrollView *)tabs->page(i_area);
		QWidget *wdg = scrl->viewport()->childAt(0,0);
		
		if( !refresh )
		{
		    if( wdg != NULL ) delete wdg;
		
		    wdg = new QFrame( scrl );		
		    QVBoxLayout *wdg_lay = new QVBoxLayout( wdg, 6, 4);
		    wdg->layout()->setAlignment( Qt::AlignTop );
		
		    selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refresh);
    		    wdg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );		
		    scrl->addChild(wdg,0,0);
		    scrl->setResizePolicy( QScrollView::AutoOneFit );
		    
		    wdg->show();
                    tabs->showPage(tabs->currentPage());				    
				
		    //Mark last drawed tabs       	    
		    t_s.attr("qview","1");		
		}
		else selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refresh);
		
            } else t_s.attr("qview","0");	//Mark no view tabs
	    	    
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
    	    if( !chkAccess(t_s, w_user->text(), SEQ_RD) ) continue;
	    if( chkAccess(t_s, w_user->text(), SEQ_WR) ) wr = true;
	
	    //View areas
	    if( t_s.name() == "area" )
	    {
		QWidget *wdg;
		if( !refr )
		{
		    wdg = new QGroupBox( t_s.attr("dscr"), widget );
		    ((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical );
	       	    wdg->layout()->setMargin( 6 );
       		    wdg->layout()->setSpacing( 4 );
		    wdg->layout()->setAlignment( Qt::AlignTop );		     

		    widget->layout()->add(wdg);
		}else wdg = widget;
		selectChildRecArea(t_s,a_path+t_s.attr("id")+'/',wdg,refr);
	    }	
	    //View list elements
	    else if( t_s.name() == "list" )
	    {
		string br_path = TSYS::strCode(a_path+t_s.attr("id"),TSYS::Path);
		ctrCmd(sel_path+"/"+br_path, t_s, TCntrNode::Get);
		
		QLabel *lab;
		QListBox *lstbox;
		
		if( !refr )
		{
		    lstbox = new QListBox( widget, br_path.c_str() );
		    lstbox->setMinimumSize( QSize( 150, 110 ) );
    		    connect( lstbox, SIGNAL( rightButtonPressed(QListBoxItem*,const QPoint&) ),
    			this, SLOT( listBoxPopup( QListBoxItem* ) ) );
    		    //Go branche signal connect
    		    if( t_s.attr("tp") == "br" )
    			connect( lstbox, SIGNAL( doubleClicked(QListBoxItem*) ),
    			    this, SLOT( listBoxGo(QListBoxItem*) ) );
    			
    		    lstbox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum, 0, 0 ) );
	    
    		    QVBoxLayout *vbox = new QVBoxLayout( 0, 0, 6 );
		    lab = new QLabel(widget);
    		    vbox->addWidget(lab);
    		    QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );
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
		    lstbox = (QListBox *)str2addr(t_s.attr("addr_el"));
		    lstbox->clear();
		}
		//Fill list
		lab->setText(t_s.attr("dscr")+":");
		for( unsigned i_el = 0, c_el = 0; i_el < t_s.childSize(); i_el++ )
                    if( t_s.childGet(i_el)->name() == "el")
                        lstbox->insertItem( t_s.childGet(i_el)->text(), c_el++ );								    
    	    }
	    //View table elements
	    else if( t_s.name() == "table" )
	    {
		string br_path = TSYS::strCode(a_path+t_s.attr("id"),TSYS::Path);
		ctrCmd(sel_path+"/"+br_path, t_s, TCntrNode::Get);
		
		//QLabel *lab;
		QTable *tbl;
		
		if( !refr )
		{		    
		    tbl = new QTable( widget, br_path.c_str() );		    
		    tbl->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 0 ) );
    		    connect( tbl, SIGNAL( contextMenuRequested(int,int,const QPoint&) ), this, SLOT( tablePopup(int,int,const QPoint&) ) );
    		    connect( tbl, SIGNAL( valueChanged( int, int ) ), this, SLOT( tableSet(int,int) ) );
		    tbl->setMinimumSize( QSize( 100, 100 ) );
		    tbl->setMaximumSize( QSize( 32767, 300 ) );
		    if(!wr) tbl->setReadOnly(true);
		    tbl->setSelectionMode(QTable::NoSelection);
			
		    widget->layout()->add( new QLabel(t_s.attr("dscr")+":",widget) );
		    widget->layout()->add( tbl );		    
		    
		    //t_s.attr("addr_lab",addr2str(lab));
		    t_s.attr("addr_tbl",addr2str(tbl));
		}
		else
		{
		    //lab = (QLabel *)str2addr(t_s.attr("addr_lab"));
		    tbl = (QTable *)str2addr(t_s.attr("addr_tbl"));
		    tbl->setNumCols(0);
		    tbl->setNumRows(0);
		}		    
		//Fill table
		//Calc rows and columns
		int n_col = t_s.childSize();
		int n_row = (n_col)?t_s.childGet(0)->childSize():0;
		
		int c_col = tbl->currentColumn();
		int c_row = tbl->currentRow();

		if( tbl->numCols() != n_col )	tbl->setNumCols(n_col);
		if( tbl->numRows() != n_row )   tbl->setNumRows(n_row);		
		
		for( unsigned i_lst = 0; i_lst < t_s.childSize(); i_lst++ )	    
		{
		    XMLNode *t_lsel = t_s.childGet(i_lst);
		    tbl->horizontalHeader()->setLabel( i_lst, t_lsel->attr("dscr") );
		    //Set elements
		    for( int i_el = 0; i_el < t_lsel->childSize(); i_el++ )
		    {
			//Set element
			if( t_lsel->attr("tp") == "bool" )
			{			    			    
			    tbl->setItem(i_el,i_lst, new QCheckTableItem(tbl,""));
			    ((QCheckTableItem *)tbl->item(i_el,i_lst))->
				setChecked((t_lsel->childGet(i_el)->text() == "true")?true:false);
			    if( !wr ) ((QCheckTableItem *)tbl->item(i_el,i_lst))->setEnabled(false);
			}				    
			else if( t_lsel->attr("tp") == "str" && wr && t_lsel->attr("select").size() )				    
			{
			    QStringList elms;
			    XMLNode x_lst("list");
			    int sel_n = -1;
			    bool u_ind = atoi(t_lsel->attr("idm").c_str());
			    ctrCmd(sel_path+"/"+TSYS::strCode( t_lsel->attr("select"),TSYS::Path), x_lst, TCntrNode::Get);
			    for( int i_ls = 0; i_ls < x_lst.childSize(); i_ls++ )
			    {
				elms+=x_lst.childGet(i_ls)->text();
				if( (u_ind && x_lst.childGet(i_ls)->attr("id") == t_lsel->childGet(i_el)->text()) ||
					(!u_ind && x_lst.childGet(i_ls)->text() == t_lsel->childGet(i_el)->text()) )
				    sel_n = i_ls;
			    }
			    if( sel_n < 0 )
			    {
				elms.insert(elms.begin(),"");
				sel_n = 0;
			    }
			    
			    tbl->setItem(i_el,i_lst, new QComboTableItem(tbl,elms));
			    ((QComboTableItem *)tbl->item(i_el,i_lst))->setCurrentItem(sel_n);
			}
			else if( t_lsel->attr("tp") == "time" )
			{
			    time_t tm_t = strtol(t_lsel->childGet(i_el)->text().c_str(),NULL,16);
			    char *c_tm = ctime( &tm_t );
			    for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
				if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
			    tbl->setText(i_el,i_lst,c_tm);
			}
			else tbl->setText(i_el,i_lst,t_lsel->childGet(i_el)->text());
		    }
    		    tbl->adjustColumn(i_lst);
    		}		
		tbl->setCurrentCell( c_row, c_col );
	    }	
	    //View standart fields
	    else if( t_s.name() == "fld" )
		basicFields( t_s, a_path, widget, wr, &l_hbox, l_pos, refr );
	    else if( t_s.name() == "comm" )
	    {
		string br_path = TSYS::strCode(a_path+SYS->strCode(t_s.attr("id"),TSYS::PathEl),TSYS::Path);

		QPushButton *button;
		if( !refr )
		{		
		    if( !t_s.childSize() )
		    {
			button = new QPushButton( widget, br_path.c_str() );
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
			button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
			widget->layout()->add( button );
		    }
		    else
		    {	
			QHBoxLayout *c_hbox = NULL;    
			int 	c_pos = 0;

			QGroupBox *comm_pan = new QGroupBox( t_s.attr("dscr"), widget );
		        comm_pan->setColumnLayout(0, Qt::Vertical );
	       	    	comm_pan->layout()->setMargin( 6 );
			comm_pan->layout()->setSpacing( 4 );		
			comm_pan->layout()->setAlignment( Qt::AlignTop );
		    
			button = new QPushButton( comm_pan, br_path.c_str() );
			button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );		
		    
			for( unsigned i_cf = 0; i_cf < t_s.childSize(); i_cf++)
			{
			    XMLNode &t_scm = *t_s.childGet(i_cf);
			    if( t_scm.name() == "fld" )
				basicFields( t_scm, a_path+t_s.attr("id")+'/', comm_pan, true, &c_hbox, c_pos, refr, true);
			}
		
			comm_pan->layout()->add(button);				
			widget->layout()->add(comm_pan);
		    }
		    
		    t_s.attr("addr_butt",addr2str(button));
		}
		else button = (QPushButton *)str2addr(t_s.attr("addr_butt"));
		//Fill command
		button->setText(t_s.attr("dscr"));
	    }	
	}
    }
}

void ConfApp::basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool refr, bool comm )
{    
    string br_path = TSYS::strCode( string((comm)?"b":"")+a_path+TSYS::strCode(t_s.attr("id"),TSYS::PathEl),TSYS::Path);
    if( !comm ) ctrCmd(sel_path+"/"+br_path, t_s, TCntrNode::Get);

    //View select fields
    if( t_s.attr("dest") == "select" && wr )
    {	
	QLabel *lab     = NULL;
        QComboBox *comb = NULL;
	
	if( !refr )
	{    		
	    comb = new QComboBox( FALSE, widget, br_path.c_str() );
	    comb->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 0, 0 ) );	    
	    
	    connect( comb, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
	    if(t_s.attr("dscr").size()) 
	    {
		*l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		lab = new QLabel(widget);
		(*l_hbox)->insertWidget( l_pos++, lab );
		(*l_hbox)->insertWidget( l_pos++, comb );
		(*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem( *l_hbox );		
	    }
	    else
	    {
		if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, comb );
		else delete comb;
	    }
	    
	    t_s.attr("addr_lab",addr2str(lab));
	    t_s.attr("addr_comb",addr2str(comb));
	}
	else
	{
	    lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
    	    comb = (QComboBox *)str2addr(t_s.attr("addr_comb"));
	    comb->clear();
	}	    
	//Fill combo
	if( lab ) lab->setText(t_s.attr("dscr")+":");
	
	XMLNode x_lst("list");
        ctrCmd(sel_path+"/"+TSYS::strCode( t_s.attr("select"),TSYS::Path), x_lst, TCntrNode::Get);
	
        bool sel_ok = false;
        unsigned i_el,c_el;
        for( i_el = 0, c_el = 0; i_el < x_lst.childSize(); i_el++ )
    	    if( x_lst.childGet(i_el)->name() == "el")
    	    {
        	comb->insertItem( x_lst.childGet(i_el)->text(), c_el++ );
		bool ind_ok = x_lst.childGet(i_el)->attr("id").size();	//Index present
        	if( (ind_ok && x_lst.childGet(i_el)->attr("id") == t_s.text()) || 
			(!ind_ok && x_lst.childGet(i_el)->text() == t_s.text()) )
                {
                    sel_ok = true;
                    comb->setCurrentItem( c_el-1 );
                }
            }
        //Insert empty field if none selected
        if( !sel_ok )
        {
            comb->insertItem("",c_el);
    	    comb->setCurrentItem(c_el);
	}	
	//comb->setMinimumWidth(comb->listBox()->maxItemWidth());
	comb->setFont( comb->font() );
	comb->updateGeometry();
    }	    
    else
    {		    
	//View boolean fields
	if( t_s.attr("tp") == "bool" )
	{
            QLabel *lab;
            QCheckBox *chBox;
	    
	    if( !refr )
	    {	    
		chBox  = new QCheckBox(widget,br_path.c_str());
		connect( chBox, SIGNAL( stateChanged(int) ), this, SLOT( checkBoxStChange(int) ) );
		if(!wr)	chBox->setDisabled(true);

		if(t_s.attr("dscr").size()) 
		{
		    *l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		    lab = new QLabel(widget);		    
    		    (*l_hbox)->insertWidget( l_pos++, lab );
    		    (*l_hbox)->insertWidget( l_pos++, chBox );
    		    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    		    widget->layout()->addItem( *l_hbox );
		}
		else
		{
		    if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, chBox );
		    else delete chBox;
		}	
		
		//widget->layout()->add(chBox);		
		t_s.attr("addr_lab",addr2str(lab));
		t_s.attr("addr_chb",addr2str(chBox));
	    }
	    else		
	    { 
		lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
		chBox = (QCheckBox *)str2addr(t_s.attr("addr_chb"));
	    }
	    
	    //Fill CheckBox
	    //chBox->setText(t_s.attr("dscr"));
	    if( lab )	lab->setText(t_s.attr("dscr"));
	    if( t_s.text() == "true" )	chBox->setChecked(true);
    	    else			chBox->setChecked(false);
	}
	//View edit fields
	else if( t_s.attr("tp") == "str" && (t_s.attr("rows").size() || t_s.attr("cols").size()) )
	{
            QLabel *lab;
            QTextEdit *edit;
	    
	    if( !refr )
	    {	
		QVBoxLayout *layoutEdit = new QVBoxLayout( 0, 0, 6);
		lab = new QLabel(t_s.attr("dscr"),widget);
		layoutEdit->addWidget(lab);
		    
		edit = new QTextEdit(widget,br_path.c_str());
		//edit->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 0 ) );
		edit->setWordWrap(QTextEdit::NoWrap);
		edit->setMinimumSize(atoi(t_s.attr("cols").c_str())*8,atoi(t_s.attr("rows").c_str())*20);
		layoutEdit->addWidget( edit );
		
		if( !wr )       edit->setReadOnly( true );    
		else
		{
		    connect( edit, SIGNAL( textChanged() ), this, SLOT( editChange( ) ) );
		    
		    QHBoxLayout *bt_layout = new QHBoxLayout( 0, 0, 6);		    
		    
		    QPushButton *bt_ok = new QPushButton( QIconSet(QImage(button_ok_xpm)), mod->I18N("Apply"), widget, br_path.c_str() );
		    connect( edit, SIGNAL( modificationChanged(bool) ), bt_ok, SLOT( setShown(bool) ) );
		    bt_ok->setHidden(true);
		    connect( bt_ok, SIGNAL( clicked() ), this, SLOT( applyButton() ) );
		    
		    QPushButton *bt_cancel = new QPushButton( QIconSet(QImage(button_cancel_xpm)), mod->I18N("Cancel"), widget, br_path.c_str() );
		    connect( edit, SIGNAL( modificationChanged(bool) ), bt_cancel, SLOT( setShown(bool) ) );
		    bt_cancel->setHidden(true);
		    connect( bt_cancel, SIGNAL( clicked() ), this, SLOT( cancelButton() ) );
		    
		    bt_layout->addItem(new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ));
		    bt_layout->addWidget(bt_ok);
		    bt_layout->addWidget(bt_cancel);
		    layoutEdit->addItem(bt_layout);
		}		    
		widget->layout()->addItem(layoutEdit);
	
		t_s.attr("addr_lab",addr2str(lab));
		t_s.attr("addr_edit",addr2str(edit));
	    }
	    else
	    {
		lab  = (QLabel *)str2addr(t_s.attr("addr_lab"));
		edit = (QTextEdit *)str2addr(t_s.attr("addr_edit"));
	    }
	    //Fill Edit
	    lab->setText(t_s.attr("dscr")+":");
	    edit->setText(t_s.text());	    
	}
        //View Data-Time fields
	else if( t_s.attr("tp") == "time" )
        {
            QLabel *lab   	 = NULL;
	    DateTimeEdit *val_w;
	    	    
	    struct tm *tm_tm;
	    time_t tm_t;
	    if( t_s.text().size() ) tm_t = strtol(t_s.text().c_str(),NULL,16);
	    else
	    {
		tm_t = time(NULL);
		t_s.text(TSYS::int2str(tm_t,C_INT_HEX));
	    }
	    tm_tm = localtime(&tm_t);

	    if( !refr )
	    {	    
		val_w = new DateTimeEdit( widget, br_path.c_str(), comm );
	    	connect( val_w, SIGNAL( valueChanged(const QDateTime &) ), this, SLOT( dataTimeChange(const QDateTime&) ) );    		
		connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );

		//Check use label
		if(t_s.attr("dscr").size()) 
		{
		    lab = new QLabel(widget);
		
		    *l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;		
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
 	    if( lab ) 	lab->setText(t_s.attr("dscr")+":");
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
		    val_r->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 1, 0 ) );
		    val_r->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
		}
		//View edit
		else
		{
		    val_w = new LineEdit( widget, br_path.c_str(), comm );		    
		    val_w->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0 ) );
		    connect( val_w, SIGNAL( textChanged(const QString&) ), this, SLOT( editLineChange(const QString&) ) );
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
    		    *l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
    		    lab = new QLabel(widget);
		    (*l_hbox)->insertWidget( l_pos++, lab );
		    if( val_w )	(*l_hbox)->insertWidget( l_pos++, val_w );
		    if( val_r )
		    { 
			(*l_hbox)->insertWidget( l_pos++, val_r );
			lab->setAlignment( int( QLabel::AlignTop ) );
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
 	    if( lab ) 	lab->setText(t_s.attr("dscr")+":");
	    if( val_r ) val_r->setText(string("<b>")+TSYS::strCode(t_s.text(),TSYS::Html)+"</b>");
	    if( val_w )	val_w->setText(t_s.text());
	}
    }
}

void ConfApp::viewChild( QListViewItem * i )    
{    
    try
    { 
	//Delete ViewItem childs
	while(i->firstChild())
	{
	    QListViewItem *it = i->firstChild();
	    i->takeItem(it);
	    delete it;
	}
	
	XMLNode node;
	string path = getItemPath( i );    
	ctrCmd(path,node,TCntrNode::Info);
	viewChildRecArea(path,node,"/",i,2);  
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
    if( path != sel_path )
    {
    	block_tabs = true;
        while(tabs->page(0))
        {
            QWidget *tab = tabs->page(0);
            tabs->removePage(tab);
            delete tab;
        }	
    	block_tabs = false;
	sel_path = path;
	
	ctrCmd(sel_path,root,TCntrNode::Info);	
    }
    else
    {    
	//Check the new node structure and the old node
	XMLNode n_node;
	ctrCmd(sel_path,n_node,TCntrNode::Info);
	upStruct(root,n_node);
    }	
    
    selectChildRecArea(root,"/");
}

bool ConfApp::upStruct(XMLNode &w_nd, const XMLNode &n_nd)
{    
    bool str_ch = false;
    
    //Check access    
    if( (chkAccess(w_nd, w_user->text(), SEQ_RD) != chkAccess(n_nd, w_user->text(), SEQ_RD)) ||
	    (chkAccess(w_nd, w_user->text(), SEQ_WR) != chkAccess(n_nd, w_user->text(), SEQ_WR)) )
	str_ch = true;    
    
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
		w_nd.childGet(i_w)->attr_("qview","0");
	}
	//Check of the description present
	if( (w_nd.childGet(i_w)->attr("dscr").size() && !n_nd.childGet(i_n)->attr("dscr").size()) ||
		(!w_nd.childGet(i_w)->attr("dscr").size() && n_nd.childGet(i_n)->attr("dscr").size()) )
	    str_ch = true;

	//Sync node parameters (text and atributes)
	w_nd.childGet(i_w)->text(n_nd.childGet(i_n)->text());
	vector<string> ls;
	n_nd.childGet(i_n)->attrList(ls);
	for( int i_a = 0; i_a < ls.size(); i_a++ )
	    w_nd.childGet(i_w)->attr(ls[i_a],n_nd.childGet(i_n)->attr(ls[i_a]));
    }
    
    return str_ch;
}

void ConfApp::onItem( QListViewItem * i )
{
    statusBar()->message(getItemPath( i ), 10000 );
}

void ConfApp::tabSelect(  QWidget * wdg )
{
    if( !block_tabs )
    {
	try{ pageDisplay( sel_path ); }
	catch(TError err) { postMess(err.cat,err.mess,4); }	    
    }
}

int ConfApp::viewChildRecArea( const string &path, const XMLNode &node, const string &a_path, QListViewItem * i, int level, int grp )
{           
    //Calc number group into node
    if(a_path == "/" && level >= 0 ) 
    	grp = viewChildRecArea( path, node, a_path, i, -1, 0 );

    //Curent node 
    for( unsigned i_cf = 0; i_cf < node.childSize(); i_cf++)
    {    
	XMLNode &t_s = *node.childGet(i_cf);
	if( t_s.name() == "area" && chkAccess(t_s, w_user->text(), SEQ_RD) )
	{
    	    string area_path = a_path+t_s.attr("id")+'/';
    	    for( unsigned i_el = 0; i_el < t_s.childSize(); i_el++)
	    {
		XMLNode &t_c = *t_s.childGet(i_el);
		if( !chkAccess(t_c, w_user->text(), SEQ_RD) ) continue;
		if( t_c.name() == "list" && t_c.attr("tp") == "br" )
    		{		 
		    //Calc Child groups
                    if( level < 0 ) { grp++; continue; }
		    
		    QListViewItem *it = i;
		    string br_path = TSYS::strCode( area_path+t_c.attr("id"),TSYS::Path);
		    
		    if( it->text(2)[0] == '*' )
		    {
		        if( br_path != it->text(2).remove(0,1) ) continue;
		    }
		    else if( grp > 1 )
		    {
			it = new QListViewItem(i,t_c.attr("dscr"),t_c.attr("dscr"),string("*")+br_path);
			i->insertItem(it);
    		    }
		    
		    ctrCmd(path+"/"+br_path, t_c, TCntrNode::Get);
		    //Process group
		    for( unsigned i_lel = 0; i_lel < t_c.childSize(); i_lel++)
		    {
			XMLNode &t_cl = *t_c.childGet(i_lel);
			if( t_cl.name() == "el" )
			{   
			    //Get branch prefix
			    string br_pref;
			    if( t_c.attr("br_pref").size() )	br_pref = t_c.attr("br_pref");
			    else 				br_pref = area_path+t_c.attr("id")+'/';
			    //Check attach mode
			    if( t_c.attr("mode") == "att" )	br_pref.insert(0,"d");
			    else				br_pref.insert(0,"s");
			    //Check index-list
			    if( t_cl.attr("id").size() )	br_pref.append(TSYS::strCode(t_cl.attr("id"),TSYS::PathEl));
			    else				br_pref.append(TSYS::strCode(t_cl.text(),TSYS::PathEl));
			    br_path = TSYS::strCode(br_pref,TSYS::Path);
			    
			    QListViewItem *ch_it = new QListViewItem(it,t_cl.text(),t_c.attr("dscr"),br_path);
    			    it->insertItem(ch_it);
			    			    
			    //Next node for next level
			    if( level-1 > 0 ) 
			    {				
				XMLNode n_lev;
				ctrCmd(path+"/"+br_path,n_lev,TCntrNode::Info);
				viewChildRecArea(path+"/"+br_path,n_lev,"/",ch_it,level-1);
			    }
			}
		    }
		}
	    }
	    if( level < 0 ) grp+=viewChildRecArea( path, t_s, area_path, i, level );
	    else viewChildRecArea( path, t_s, area_path, i, level, grp );
	}
    }    
    
    return(grp);
}

string ConfApp::getItemPath( QListViewItem * i )
{
    QListViewItem *it_next = i;
    string p_path = "";    
    do
    {
	if(it_next->text(2)[0] != '*' )
	    p_path.insert(0,string("/")+it_next->text(2).ascii());
	it_next = it_next->parent();
    }
    while( it_next != NULL );
    
    return(p_path);
}

void ConfApp::ctrCmd( const string &path, XMLNode &node, TCntrNode::Command cmd )
{
    if( TSYS::pathLev(path,0,false) == SYS->station() )
        SYS->cntrCmd(path.substr(TSYS::pathLev(path,0,false).size()+1),&node,cmd);
    else
        throw TError(mod->nodePath().c_str(),"Station error!");
}                          

void ConfApp::postMess( const string &cat, const string &mess, int type )
{
    //Put system message.
    Mess->put(cat.c_str(),(type==4)?TMess::Crit:(type==3)?TMess::Error:(type==2)?TMess::Warning:TMess::Info,mess.c_str());
    //QT message
    if( type == 1 )	QMessageBox::information( this,caption(), mess);
    else if( type == 2 )QMessageBox::warning( this,caption(), mess);
    else if( type == 3 )QMessageBox::critical( this,caption(), mess);
    else if( type == 4 )QErrorMessage::qtHandler()->message(mess);
}

bool ConfApp::chkAccess( const XMLNode &fld, string user, char mode )
{
    string s_acc = fld.attr("acs");
    string s_usr = fld.attr("own");
    string s_grp = fld.attr("grp");
    if( !s_acc.size() ) s_acc = "0777";
    int accs = strtol(s_acc.c_str(),NULL,8);
    if( !s_usr.size() ) s_usr = "0";        //root
    int usr = atoi(s_usr.c_str());
    if( !s_grp.size() ) s_grp = "0";        //root
    int grp = atoi(s_grp.c_str());

    return( mod->owner().owner().security().at().access( user, mode, usr, grp, accs) );
}


//-------------------------------------------------------------
//-------------- Self widget's slots --------------------------
//-------------------------------------------------------------
void ConfApp::checkBoxStChange( int stat )
{
    XMLNode *n_el;
    QCheckBox *box = (QCheckBox *)sender();
    
    if(stat==QButton::NoChange) return;
    try
    { 
	string path = box->name();
	string val = (stat==QButton::On)?"true":"false";
	//Check block element
	if(path[0] == 'b')
	{
	    n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path.substr(1),TSYS::Path) );    
	    n_el->text(val);
	    return;
	}
	else
	{	
	    n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) );    
    	    ctrCmd(sel_path+"/"+path, *n_el,TCntrNode::Get);

    	    if( n_el->text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Set <%s> to <%s>!", w_user->text().ascii(), 
		    (sel_path+"/"+path).c_str(), val.c_str() );
    	    n_el->text(val);
    	    ctrCmd(sel_path+"/"+path, *n_el,TCntrNode::Set);
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::buttonClicked( )
{	
    QButton *button = (QButton *)sender();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(button->name(),TSYS::Path) );
	
	//Check link
	if( n_el->attr("tp") == "lnk")
	{
	    ctrCmd(sel_path+"/"+button->name(), *n_el, TCntrNode::Get);
	    string url = TSYS::pathLev(sel_path,0)+"/"+n_el->text();
	    
    	    Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Go to link <%s>!", w_user->text().ascii(),url.c_str());
	    
	    //Prev and next
    	    if( sel_path.size() )       prev.insert(prev.begin(),sel_path);
	    if( prev.size() >= que_sz ) prev.pop_back();
	    next.clear();			
	    
	    pageDisplay( url );
	}
	else
	{
 	    Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Press <%s>!", w_user->text().ascii(), 
		(sel_path+"/"+button->name()).c_str() );
	    ctrCmd(sel_path+"/"+button->name(), *n_el, TCntrNode::Set);
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::combBoxActivate( const QString& ival )
{
    bool block = false;
    string val = ival;
    XMLNode *n_el;    
    QComboBox *comb = (QComboBox *)sender();    
    
    try
    {    
	string path = comb->name();
	if(path[0] == 'b')
	{ 
	    block = true;
	    path = path.substr(1);
	}	    
	
	n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) );
	
        //Get list for index list check!
        bool find_ok = false;
        XMLNode x_lst("list");
        ctrCmd(sel_path+"/"+TSYS::strCode( n_el->attr("select"),TSYS::Path), x_lst, TCntrNode::Get);
        for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
        if( x_lst.childGet(i_el)->name() == "el" && x_lst.childGet(i_el)->text() == val )
        {
            if( x_lst.childGet(i_el)->attr("id").size() )
        	val = x_lst.childGet(i_el)->attr("id");
            find_ok = true;
        }
        if( !find_ok ) throw TError(mod->nodePath().c_str(),"Value <%s> no valid!",val.c_str());																								
	
	//Check block element. Command box!
	if( block ) { n_el->text(val); return; }
	else
	{
	    ctrCmd(sel_path+"/"+path, *n_el, TCntrNode::Get);
	
    	    if( n_el->text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Change <%s> from <%s> to <%s>!", 
		    w_user->text().ascii(), (sel_path+"/"+path).c_str(), n_el->text().c_str(), val.c_str() );
    	    n_el->text(val);
	    
    	    ctrCmd(sel_path+"/"+path, *n_el, TCntrNode::Set);	
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::listBoxPopup(QListBoxItem* item)
{
    QPopupMenu popup;
    QListBox *lbox = (QListBox *)sender();
    string el_path = sel_path+"/"+lbox->name();
    XMLNode *n_el;

    try
    {    
	n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(lbox->name(),TSYS::Path) );
        int last_it = -1;
        if( n_el->attr("tp") == "br" && item != NULL )
        { 
    	    last_it = popup.insertItem(mod->I18N("Go"),0); 
	    popup.insertSeparator();
	}
	if( chkAccess(*n_el, w_user->text(), SEQ_WR) && n_el->attr("s_com").size() )
	{
	    if( n_el->attr("s_com").find("add") != string::npos )
	        last_it = popup.insertItem(mod->I18N("Add"),1); 
    	    if( n_el->attr("s_com").find("ins") != string::npos && item != NULL )
    		last_it = popup.insertItem(mod->I18N("Insert"),2); 
    	    if( n_el->attr("s_com").find("edit") != string::npos && item != NULL )
    		last_it = popup.insertItem(mod->I18N("Edit"),3); 
    	    if( n_el->attr("s_com").find("del") != string::npos && item != NULL )
    	    { 
    		popup.insertSeparator();
    		last_it = popup.insertItem(mod->I18N("Delete"),4); 
    	    }
    	    if( n_el->attr("s_com").find("move") != string::npos && item != NULL )
    	    { 
    		popup.insertSeparator();
    		last_it = popup.insertItem(mod->I18N("Up"),5); 
    		last_it = popup.insertItem(mod->I18N("Down"),6); 
    	    }
	}
	    
	if(last_it >= 0) 
	{
	    bool ok;
	    string p_text, p_id;
	    string text, id;	    
	    bool ind_m = atoi(n_el->attr("idm").c_str());
	    int  c_id  = lbox->currentItem();
	    
	    if( item != NULL )
	    {	    
		//Get select id
        	p_text = item->text().ascii();
        	if( ind_m )
            	    for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
            		if( n_el->childGet(i_el)->text() == item->text() )
                	{
                    	    p_id = n_el->childGet(i_el)->attr("id");
                    	    break;
                	}
	    }
	    
	    int rez = popup.exec(QCursor::pos());
	    if( rez == 1 || rez == 2 || rez == 3 )
	    {
		InputDlg *dlg = new InputDlg(ind_m);
		if( rez==3 )
		{
		    dlg->id(p_id);
		    dlg->name(p_text);
		}
		int rez = dlg->exec();
		id = dlg->id().ascii();
		text = dlg->name().ascii();
		delete dlg;
		if( rez != QDialog::Accepted )	return; 
	    }

	    //Make command	    
	    XMLNode n_el1;
	    if( rez == 0 )
	    {
		listBoxGo( item );
		return;
	    }
	    else if( rez == 1 )
	    {
		n_el1.name("add");
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Add <%s> element <%s:%s>!", 
	    		w_user->text().ascii(), el_path.c_str(), id.c_str(), text.c_str() );
	    }
	    else if( rez == 2 )
	    {
		n_el1.name("ins");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Insert <%s> element <%s:%s> to %d!", 
	    		w_user->text().ascii(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if( rez == 3 )
	    {
		n_el1.name("edit");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Set <%s> element %d to <%s:%s>!", 
	    		w_user->text().ascii(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if( rez == 4 )
	    {
		n_el1.name("del");
		n_el1.attr("pos",TSYS::int2str(c_id));
		if( ind_m ) n_el1.attr("id",p_id);
		else n_el1.text(item->text());
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Delete <%s> element <%s:%s>!", 
	    		w_user->text().ascii(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if( rez == 5 || rez == 6 )
	    {
		int c_new = c_id-1;
		if( rez == 6 )	c_new = c_id+1;		
		n_el1.name("move");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("to",TSYS::int2str(c_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Move <%s> from %d to %d!", 
	    		w_user->text().ascii(), el_path.c_str(), c_id, c_new);
	    }
	    if( rez >= 0 ) 
	    {
		ctrCmd(el_path, n_el1, TCntrNode::Set);
		autoUpdTimer->start(100,true);      //Redraw
	    }	    	    
	}
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	autoUpdTimer->start(100,true);	//Redraw
    }
}

void ConfApp::tablePopup(int row, int col, const QPoint &pos )
{
    QPopupMenu popup;
    QTable *tbl = (QTable *)sender();
    string el_path = sel_path+"/"+tbl->name();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(tbl->name(),TSYS::Path) );
	
	int last_it = -1;
	if( chkAccess(*n_el, w_user->text(), SEQ_WR) && n_el->attr("s_com").size() )
	{
	    if( n_el->attr("s_com").find("add") != string::npos )
		last_it = popup.insertItem(mod->I18N("Add record"),1); 
    	    if( n_el->attr("s_com").find("ins") != string::npos && row != -1 )
    		last_it = popup.insertItem(mod->I18N("Insert record"),2); 
    	    if( n_el->attr("s_com").find("del") != string::npos && row != -1 )
    		last_it = popup.insertItem(mod->I18N("Delete record"),3); 
    	    if( n_el->attr("s_com").find("move") != string::npos && row != -1 )
	    {
    		popup.insertSeparator();
    		last_it = popup.insertItem(mod->I18N("Move Up"),4); 
    		last_it = popup.insertItem(mod->I18N("Move Down"),5);
	    }
	}
	if( last_it >= 0 ) 
	{
	    bool ok;
	    QString text;
	    
	    int rez = popup.exec(QCursor::pos());
	    
	    XMLNode n_el1;
	    if( rez == 1 )
	    {
    		n_el1.name("add");
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Add <%s> record.",
			w_user->text().ascii(), el_path.c_str() );
	    }
	    else if( rez == 2 )
	    {
		n_el1.name("ins");
	    	n_el1.attr("row",TSYS::int2str(row));
    		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Insert <%s> record %d.", 
			w_user->text().ascii(), el_path.c_str(), row );
	    }
	    else if( rez == 3 )
	    {
		n_el1.name("del");
		n_el1.attr("row",TSYS::int2str(row));
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Delete <%s> record %d.", 
			w_user->text().ascii(), el_path.c_str(), row );
	    }
	    else if( rez == 4 || rez == 5 )
	    {
		int r_new = row-1;
                if( rez == 5 )  r_new = row+1;
		n_el1.name("move");
		n_el1.attr("row",TSYS::int2str(row))->attr("to",TSYS::int2str(r_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Move <%s> record from %d to %d.", 
			w_user->text().ascii(), el_path.c_str(), row, r_new );
	    }
	    if( rez >= 0 )
	    {
		ctrCmd(el_path, n_el1, TCntrNode::Set);
		autoUpdTimer->start(100,true);
	    }
	}	
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	autoUpdTimer->start(100,true);	//Redraw
    }
}

void ConfApp::tableSet( int row, int col )
{
    string value;
    if( row < 0 || col < 0 )	return;
    
    try
    {
	QTable *tbl = (QTable *)sender();
	string el_path = sel_path+"/"+tbl->name();

	value = tbl->text(row,col).ascii();	
	if( tbl->item(row,col)->rtti() == 1 )
	{
	
	    XMLNode *n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(tbl->name(),TSYS::Path) );
		
	    bool find_ok = false;
	    XMLNode x_lst("list");
	    ctrCmd(sel_path+"/"+TSYS::strCode( n_el->childGet(col)->attr("select"),TSYS::Path), x_lst, TCntrNode::Get);
	    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
		if( x_lst.childGet(i_el)->text() == value )
		{
		    if( atoi(n_el->childGet(col)->attr("idm").c_str()) )
			value = x_lst.childGet(i_el)->attr("id");
		    find_ok = true;
    		}
	    if( !find_ok ) throw TError(mod->nodePath().c_str(),"Value <%s> no valid!",value.c_str());
	}
	if( tbl->item(row,col)->rtti() == 2 )
	    value = (((QCheckTableItem *)tbl->item(row,col))->isChecked())?"true":"false";
	XMLNode n_el1("set");
	n_el1.attr("row",TSYS::int2str(row))->attr("col",TSYS::int2str(col))->text(value);
    
	Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Set <%s> cell (%d:%d) to: %s.", 
	    w_user->text().ascii(), el_path.c_str(), row, col, value.c_str());
	ctrCmd(el_path, n_el1, TCntrNode::Set);
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
    
    autoUpdTimer->start(100,true);
}

void ConfApp::listBoxGo( QListBoxItem* item )
{
    string path;
    
    QListBox *lbox = item->listBox();
    try
    { 
        //Prev and next
	if( sel_path.size() )   	prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();	
	
	XMLNode &t_c = *TCntrNode::ctrId(&root, TSYS::strEncode(lbox->name(),TSYS::Path) );
	
	//Get branch prefix	
        string br_pref;
        if( t_c.attr("br_pref").size() )br_pref = t_c.attr("br_pref");
        else                         	br_pref = br_pref+lbox->name()+":";											    
	
	//Check branche type 
	if(t_c.attr("mode") == "att")	br_pref.insert(0,"/d");
	else				br_pref.insert(0,"/s");
	//Find selected index
        bool sel_ok = false;
        for( int i_el = 0; i_el < t_c.childSize(); i_el++ )
    	    if( t_c.childGet(i_el)->name() == "el" && t_c.childGet(i_el)->text() == item->text() )
    	    {
		if( t_c.childGet(i_el)->attr("id").size() )
                    path = sel_path+br_pref+TSYS::strCode(t_c.childGet(i_el)->attr("id"),TSYS::PathEl);
		else
                    path = sel_path+br_pref+TSYS::strCode(t_c.childGet(i_el)->text(),TSYS::PathEl);
		sel_ok = true;
	    }
	if( !sel_ok ) throw TError(mod->nodePath().c_str(),"Select element <%s> no present!",item->text().ascii());
	    
	pageDisplay( path );
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::dataTimeChange( const QDateTime & qtm )
{
    XMLNode *n_el;    
    QDateTimeEdit *datat = (QDateTimeEdit *)sender();
    
    try
    {    
	time_t c_tm = time(NULL);
	string path = datat->name();

	struct tm tm_tm;
	tm_tm = *localtime(&c_tm);
	//tm_tm.tm_isdst = 1;
	tm_tm.tm_year = qtm.date().year()-1900;
	tm_tm.tm_mon = qtm.date().month()-1;
	tm_tm.tm_mday = qtm.date().day();
	tm_tm.tm_hour = qtm.time().hour();
	tm_tm.tm_min = qtm.time().minute();
	tm_tm.tm_sec = qtm.time().second();
	string val = TSYS::int2str(mktime(&tm_tm),C_INT_HEX);
	
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path.substr(1),TSYS::Path) );    
    	n_el->text(val);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::editChange( )
{
    QTextEdit *txt_ed = (QTextEdit *)sender();
    
    try
    {    
	string path = txt_ed->name();
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) )->
	    text(txt_ed->text());
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::editLineChange( const QString& txt )
{
    LineEdit *ln_ed = (LineEdit *)sender();
    
    try
    {   
	string path = ln_ed->name();
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) )->text(txt);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::applyButton( )
{
    QButton *button = (QButton *)sender();
    
    string path = button->name();

    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) );    
	Mess->put(mod->nodePath().c_str(),TMess::Info,"%s| Change <%s> to: <%s>!", 
		w_user->text().ascii(), (sel_path+"/"+path).c_str(), n_el->text().c_str() );
	ctrCmd(sel_path+"/"+path, *n_el, TCntrNode::Set);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::cancelButton( )
{
    QButton *button = (QButton *)sender();
    
    string path = button->name();

    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)root, TSYS::strEncode(path,TSYS::Path) );    
	ctrCmd(sel_path+"/"+path, *n_el, TCntrNode::Get);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

//Address convert
string ConfApp::addr2str( void *addr )
{
    char buf[40];
    snprintf(buf,sizeof(buf),"%X",addr);
    
    return buf;
}

void *ConfApp::str2addr( const string &str )
{
    return (void *)strtol(str.c_str(),NULL,16);
}

//*****************************************************************
//******************* ListViewToolTip *****************************
//*****************************************************************
void ListViewToolTip::maybeTip( const QPoint& p )
{
    if( !listView )     return;
    QListViewItem* item = listView->itemAt( p );
    if ( !item )        return;
    QRect itemRect = listView->itemRect( item );
    QRect headerRect = listView->header()->sectionRect( listView->header()->sectionAt( p.x() ) );
    QRect cellRect( headerRect.left(), itemRect.top(), headerRect.width(), itemRect.height() );
    tip( cellRect, item->text( 1 ) );
}

//*****************************************************************
//************************ LineEdit *******************************
//*****************************************************************
LineEdit::LineEdit( QWidget *parent, const char * name, bool prev_dis ) : 
    QWidget( parent, name ), bt_fld(NULL)
{
    box = new QHBoxLayout(this);
    
    ed_fld = new QLineEdit(this);
    connect( ed_fld, SIGNAL( textChanged(const QString&) ), this, SLOT( changed(const QString&) ) );
    box->addWidget(ed_fld);
    
    if( !prev_dis )
    {
	bt_fld = new QPushButton(this);		
	bt_fld->setPixmap(QPixmap(QImage(ok_xpm)));    
	bt_fld->hide();
	//connect( ed_fld, SIGNAL( returnPressed() ), bt_fld, SLOT( animateClick( ) ) );
	connect( bt_fld, SIGNAL( clicked() ), this, SLOT( applySlot() ) );
	box->addWidget(bt_fld);
    }
}

void LineEdit::changed( const QString& str )
{
    if( bt_fld ) bt_fld->show();
    emit textChanged(str);
}

void LineEdit::setText(const QString &txt)
{
    ed_fld->setText(txt);
    ed_fld->setCursorPosition(0);
    if( bt_fld ) bt_fld->hide();
}

QString LineEdit::text() const
{
    return ed_fld->text();
}

void LineEdit::applySlot( )
{
    bt_fld->hide();
    emit apply();    
}

bool LineEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyPress)
    {
	QKeyEvent *keyEvent = (QKeyEvent *)e;
    	if(keyEvent->key() == Key_Enter || keyEvent->key() == Key_Return)
	{
	    bt_fld->animateClick( );
	    return true;
	}
	else if(keyEvent->key() == Key_Escape )
	{
	    emit cancel();
	    return true;	
	}
    }
    return QWidget::event(e);
}
    
//*****************************************************************
//************************ DataTimeEdit ***************************
//*****************************************************************
DateTimeEdit::DateTimeEdit( QWidget *parent, const char * name, bool prev_dis ) : 
    QWidget( parent, name ), bt_fld(NULL)
{
    box = new QHBoxLayout(this);
    
    ed_fld = new QDateTimeEdit(this);
    connect( ed_fld, SIGNAL( valueChanged(const QDateTime&) ), this, SLOT( changed(const QDateTime&) ) );
    box->addWidget(ed_fld);
    if( !prev_dis )
    {
	bt_fld = new QPushButton(this);		
	bt_fld->setPixmap(QPixmap(QImage(ok_xpm)));    
	bt_fld->hide();
	connect( bt_fld, SIGNAL( clicked() ), this, SLOT( applySlot() ) );
	box->addWidget(bt_fld);
    }
}

void DateTimeEdit::changed( const QDateTime& dt )
{
    if( bt_fld ) bt_fld->show();
    emit valueChanged(dt);
}

void DateTimeEdit::setDateTime ( const QDateTime & dt )
{
    ed_fld->setDateTime(dt);
    if( bt_fld ) bt_fld->hide();
}
	    
QDateTime DateTimeEdit::dateTime() const
{
    return ed_fld->dateTime();
}

void DateTimeEdit::applySlot( )
{
    bt_fld->hide();
    emit apply();    
}

bool DateTimeEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyPress)
    {
	QKeyEvent *keyEvent = (QKeyEvent *)e;
    	if(keyEvent->key() == Key_Enter || keyEvent->key() == Key_Return)
	{
	    bt_fld->animateClick( );
	    return true;
	}
	else if(keyEvent->key() == Key_Escape )
	{
	    emit cancel();
	    return true;	
	}
    }
    return QWidget::event(e);
}

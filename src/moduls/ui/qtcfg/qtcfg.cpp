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
#include <qaction.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qgroupbox.h> 
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qlineedit.h>
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
#include "xpm/exit.xpm"
#include "xpm/help.xpm"
#include "xpm/contexthelp.xpm"
#include "xpm/start.xpm"
#include "xpm/stop.xpm"
#include "xpm/reload.xpm"
#include "xpm/ok.xpm"

#include <tmessage.h>
#include <tsys.h>
#include <tsequrity.h>
#include "tuimod.h"
#include "dlguser.h"
#include "qtcfg.h"


using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( TUIMod *owner, ConfApp *parent ) : 
    QMainWindow( 0, "", WDestructiveClose ), own(owner), m_parent(parent), que_sz(20), block_tabs(false)
{   
    setCaption(own->I18N("QT Configurator of OpenSCADA"));
    
    //Centrall widget
    setCentralWidget( new QWidget( this, "CentralWidget" ) );
    QGridLayout *QTCfgLayout = new QGridLayout( centralWidget(), 1, 1, 4, -1 );	    
    
    //Splitter
    QSplitter *splitter = new QSplitter( centralWidget() );
    splitter->setOrientation( QSplitter::Horizontal );	
    
    //Create Navigator tree
    CtrTree = new QListView( splitter );
    CtrTree->setRootIsDecorated( true );
    CtrTree->addColumn(own->I18N("Name"));
    CtrTree->addColumn("descr",0);    
    CtrTree->addColumn("path",0);
    CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );
    CtrTree->setMinimumSize( QSize( 150, 0 ) );
    CtrTree->setMaximumSize( QSize( 400, 32767 ) );
    CtrTree->setResizeMode( QListView::NoColumn );
    connect( CtrTree, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
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
    connect(w_user, SIGNAL(clicked()), this, SLOT(sel_user()));        
    w_user->setPaletteBackgroundColor(QColor(255,0,0));
    gFrameLayout->addWidget( w_user, 0, 1 );
    
    tabs = new QTabWidget( gFrame );
    connect( tabs, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabSelect(QWidget*) ) );
    gFrameLayout->addMultiCellWidget( tabs, 1, 1, 0, 1 );
    //gFrameLayout->addWidget( tabs, 1, 0 );
    
    QTCfgLayout->addWidget( splitter, 0, 0 );    
    
    QListViewItem *root_l_it = new QListViewItem(CtrTree,SYS->Station(),own->I18N("Local station"),SYS->Station());
    CtrTree->insertItem( root_l_it );
    //viewChild( root_l_it );
    
    //Status bar
    statusBar()->message(own->I18N("Ready"), 2000 );
    resize( 800, 600 );
    
    //Create actions
    //Close
    //le->insertItem( "&Close", this, SLOT(close()), CTRL+Key_W );
    //Quit
    QAction *actQuit = new QAction("",QIconSet(QImage(exit_xpm)),own->I18N("&Quit"),CTRL+Key_Q,this);
    actQuit->setToolTip(own->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(own->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(close()));        
    //connect(actQuit, SIGNAL(activated()), qApp, SLOT(closeAllWindows()));
    //New button
    QAction *actNew = new QAction("",QPixmap(QImage(window_new_xpm)),own->I18N("&New"),CTRL+Key_N,this);
    actNew->setToolTip(own->I18N("New window"));
    actNew->setWhatsThis(own->I18N("Open new window"));
    connect(actNew, SIGNAL(activated()), this, SLOT(new_w()));    
    //Up button
    actUp = new QAction("",QIconSet(QImage(up_xpm)),own->I18N("&Up"),ALT+Key_Up,this);
    actUp->setToolTip(own->I18N("Up page"));
    actUp->setWhatsThis(own->I18N("Go to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(up_page()));    
    //Previos page
    actPrev = new QAction("",QPixmap(QImage(back_xpm)),own->I18N("&Previos"),ALT+Key_Left,this);
    actPrev->setToolTip(own->I18N("Previos page"));
    actPrev->setWhatsThis(own->I18N("Go to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(prew_page()));    
    //Previos page
    actNext = new QAction("",QPixmap(QImage(forward_xpm)),own->I18N("&Next"),ALT+Key_Right,this);
    actNext->setToolTip(own->I18N("Next page"));
    actNext->setWhatsThis(own->I18N("Go to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(next_page()));    
    //Update
    QAction *actUpdate = new QAction("",QPixmap(QImage(reload_xpm)),own->I18N("&Update"),Key_F5,this);
    actUpdate->setToolTip(own->I18N("Update current page"));
    actUpdate->setWhatsThis(own->I18N("Button for update a content of the current page."));
    connect(actUpdate, SIGNAL(activated()), this, SLOT(update_page()));        
    //Start of "Auto update"
    actStartUpd = new QAction("",QPixmap(QImage(start_xpm)),own->I18N("&Start"),CTRL+Key_B,this);
    actStartUpd->setToolTip(own->I18N("Start auto update"));
    actStartUpd->setWhatsThis(own->I18N("Button for start of autoupdate content of the current page."));
    connect(actStartUpd, SIGNAL(activated()), this, SLOT(start_autoupd_page()));        
    //Stop of "Auto update"
    actStopUpd = new QAction("",QPixmap(QImage(stop_xpm)),own->I18N("&Stop"),CTRL+Key_E,this);
    actStopUpd->setToolTip(own->I18N("Stop auto update"));
    actStopUpd->setWhatsThis(own->I18N("Button for stop of autoupdate content of the current page."));
    actStopUpd->setEnabled(false);
    connect(actStopUpd, SIGNAL(activated()), this, SLOT(stop_autoupd_page()));        
    
    //Create menu "file"
    QPopupMenu *mn_file = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&File"), mn_file );
    mn_file->insertSeparator();
    actNew->addTo(mn_file);
    mn_file->insertSeparator();
    actQuit->addTo(mn_file);
    //Create menu "view"
    QPopupMenu *mn_view = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&View"), mn_view );    
    actUpdate->addTo(mn_view);
    actStartUpd->addTo(mn_view);
    actStopUpd->addTo(mn_view);    
    //Create menu "go"
    QPopupMenu *mn_go = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&Go"), mn_go );    
    actUp->addTo(mn_go);
    actPrev->addTo(mn_go);
    actNext->addTo(mn_go);    
    //Create menu "help"
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&Help"), help );
    help->insertItem(QPixmap(QImage(help_xpm)), own->I18N("&About"), this, SLOT(about()), Key_F1 );
    help->insertItem(own->I18N("About &Qt"), this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem(QPixmap(QImage(contexthelp_xpm)), own->I18N("What's &This"), this, SLOT(whatsThis()), SHIFT+Key_F1 );
    
    //Tool bar
    QToolBar *toolBar = new QToolBar(own->I18N("OpenSCADA toolbar"), this, DockTop );          
    actNew->addTo(toolBar);
    toolBar->addSeparator();
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
    connect( autoUpdTimer, SIGNAL(timeout()), SLOT(update_page()) );
    
    //Display root page
    viewChild( root_l_it );
    pageDisplay( getItemPath( root_l_it ) );
}

ConfApp::~ConfApp()
{
    for( int i_cnt = 0; i_cnt < childs.size(); i_cnt++ )
	if(childs[i_cnt] != NULL ) childs[i_cnt]->close();
    if( m_parent != NULL ) m_parent->childClose(this);

    ResAlloc::resDelete(hd_res);
}

void ConfApp::new_w()
{
    int i_ch;
    ConfApp *cfg = new ConfApp(own,this);
    cfg->show();
    for(i_ch = 0; i_ch < childs.size(); i_ch++)
	if( childs[i_ch] == NULL ) break;
    if(i_ch < childs.size()) 	childs[i_ch] = cfg;
    else 			childs.push_back(cfg);
}

void ConfApp::up_page()
{
    int i_l = sel_path.rfind("/");
    if( i_l == string::npos || i_l == 0 ) return;
    //Prev and next
    if( sel_path.size() )	prev.insert(prev.begin(),sel_path);
    if( prev.size() >= que_sz )	prev.pop_back();
    next.clear();	
    
    try{ pageDisplay( sel_path.substr(0,i_l) );	} catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::prew_page()
{
    if( !prev.size() ) return;
    next.insert(next.begin(),sel_path);
    string path = prev[0];
    prev.erase(prev.begin());
    
    try{ pageDisplay( path ); } catch(TError err) { postMess(err.what(),4); }   
}

void ConfApp::next_page()
{
    if( !next.size() ) return;
    prev.insert(prev.begin(),sel_path);
    string path = next[0];
    next.erase(next.begin()); 
    
    try{ pageDisplay( path ); } catch(TError err) { postMess(err.what(),4); }       
}

void ConfApp::sel_user()
{
    vector<string> u_list;
    TSequrity &seq = own->owner().owner().Sequrity();    
	
    DlgUser *d_usr = new DlgUser( own );
    seq.usrList(u_list);
    for(int i_l = 0; i_l < u_list.size(); i_l++ )	
	d_usr->users->insertItem(u_list[i_l]);
    
    int rez = d_usr->exec();
    string dl_user   = d_usr->users->currentText();
    string dl_passwd = d_usr->passwd->text();
    delete d_usr;    
    
    if( rez && dl_user != w_user->text() )
    {
	try
	{
	    if( seq.usrAt(dl_user).at().auth(dl_passwd) ) 
	    {
		w_user->setText( dl_user );
		if( dl_user == "root" )	w_user->setPaletteBackgroundColor(QColor(255,0,0));
		else 			w_user->setPaletteBackgroundColor(QColor(0,255,0));
		pageDisplay( getItemPath( CtrTree->firstChild() ) );
	    }
	    else postMess(own->I18N("Auth wrong!!!"),2);
	} catch(TError err) { postMess(err.what(),4); }       
    }
    
}

void ConfApp::update_page()
{
    try{ pageDisplay(sel_path); }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::start_autoupd_page()
{
    actStartUpd->setEnabled(false);
    actStopUpd->setEnabled(true);

    autoUpdTimer->start(1000);
}

void ConfApp::stop_autoupd_page()
{
    actStopUpd->setEnabled(false);
    actStartUpd->setEnabled(true);
    
    autoUpdTimer->stop();
}


void ConfApp::childClose( ConfApp *child )
{
    for( int i_ch = 0; i_ch < childs.size(); i_ch++)
	if( childs[i_ch] == child ) childs[i_ch]=NULL;
}

void ConfApp::about()
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),own->I18N(
        "OpenSCADA Qt based system configurator v%s.\n"
	"Autor: %s\n"
	"License: %s\n"),
        own->modInfo("Version").c_str(),own->modInfo("Autors").c_str(),own->modInfo("License").c_str());
    
    QMessageBox::about( this,caption(),buf);
}

void ConfApp::aboutQt()
{
    QMessageBox::aboutQt( this, own->I18N("Qt Application."));
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

void ConfApp::selectItem( QListViewItem * i )
{    
    try
    { 
        //Prev and next
	if( sel_path.size() )		prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();
	next.clear();	

	pageDisplay( getItemPath( i ) );	
    }
    catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::selectChildRecArea( const string &path, const XMLNode &node, const string &a_path, const XMLNode &root, QWidget *widget )
{
    QVBoxLayout *layout;
    
    //For abbend no named widgets
    QHBoxLayout *l_hbox = NULL;    
    int		i_area = 0;	//Areas counter
    int 	l_pos = 0;

    //View title name
    if( a_path == "/" ) titleLab->setText(string("<p align='center'><i>")+node.text()+"</b></p>");
    

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
	    QWidget *wdg = NULL;
            bool active = false;	    
	    
	    //Check root page
	    if(a_path == "/")
	    {
		//Search avoid tab
		int i_t;
	       	QScrollView *scrl;
		for( i_t = 0; i_t < tabs->count(); i_t++ )
		    if( tabs->tabLabel(tabs->page(i_t)) == t_s.attr("dscr") ) break;		    
		if( i_t < tabs->count() )
		{			
		    //remove free page
		    while( i_t > i_area ) 
		    { 
			block_tabs = true; 
			tabs->removePage(tabs->page(i_area)); 
			block_tabs = false; 
			i_t--; 
		    }
		    scrl = (QScrollView *)tabs->page(i_area);
		    wdg = scrl->viewport()->childAt(0,0);
		    if( wdg != NULL ) delete wdg;
		}
		else
		{
		    //Create new tab
		    scrl = new QScrollView(tabs);
		    tabs->insertTab( scrl, t_s.attr("dscr"), i_area );		    
		}                
		
	     	wdg = new QFrame( scrl );		
    		QVBoxLayout *wdg_lay = new QVBoxLayout( wdg, 6, 4);
		wdg->layout()->setAlignment( Qt::AlignTop );
		
		scrl->addChild(wdg,0,0);
		scrl->setResizePolicy( QScrollView::AutoOneFit );		    
		    
		//If curent page
		if( tabs->currentPageIndex() == i_area )
		{		    
		    selectChildRecArea(path,t_s,a_path+t_s.attr("id")+'/',root,wdg);
		    wdg->show();
		}
		i_area++;
	    }
	    else
	    {
		wdg = new QGroupBox( t_s.attr("dscr"), widget );
		((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical );
	       	wdg->layout()->setMargin( 6 );
       		wdg->layout()->setSpacing( 4 );
		wdg->layout()->setAlignment( Qt::AlignTop );		     

		widget->layout()->add(wdg);
		selectChildRecArea(path,t_s,a_path+t_s.attr("id")+'/',root,wdg);
	    }
	}	
	//View list elements
	else if( t_s.name() == "list" && t_s.attr("hide") != "1" )
	{
            string br_path = SYS->pathCode(a_path+t_s.attr("id"),false);
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    QListBox *lstbox = new QListBox( widget, br_path.c_str() );
	    lstbox->setMinimumSize( QSize( 150, 110 ) );
	    connect( lstbox, SIGNAL( rightButtonPressed(QListBoxItem*,const QPoint&) ),
		this, SLOT( listBoxPopup( QListBoxItem* ) ) );
	    //Go branche signal connect
	    if( t_s.attr("tp") == "br" )
		connect( lstbox, SIGNAL( doubleClicked(QListBoxItem*) ),
		    this, SLOT( listBoxGo(QListBoxItem*) ) );
	    	    
	    lstbox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 0 ) );
	    for( unsigned i_el = 0, c_el = 0; i_el < t_s.childSize(); i_el++ )
		if( t_s.childGet(i_el)->name() == "el")
		    lstbox->insertItem( t_s.childGet(i_el)->text(), c_el++ );
	    
	    QVBoxLayout *vbox = new QVBoxLayout( 0, 0, 6 );	    
	    vbox->addWidget( new QLabel(t_s.attr("dscr")+":",widget) );
	    QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );
	    hbox->addWidget( lstbox );
	    hbox->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	    vbox->addLayout(hbox);
	    widget->layout()->addItem(vbox);
	}
        //View table elements
	else if( t_s.name() == "table" )
	{
            string br_path = SYS->pathCode(a_path+t_s.attr("id"),false);
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    QTable *tbl = new QTable( widget, br_path.c_str() );
	    tbl->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 0 ) );
	    tbl->setMaximumSize( QSize( 32767, 300 ) );
	    tbl->setReadOnly(true);
	    tbl->setSelectionMode(QTable::NoSelection);
	    
	    for( unsigned i_lst = 0, c_lst = 0; i_lst < t_s.childSize(); i_lst++ )	    
                if( t_s.childGet(i_lst)->name() == "list")
		{
		    XMLNode *t_lsel = t_s.childGet(i_lst);
		    tbl->setNumCols(c_lst+1);
		    tbl->horizontalHeader()->setLabel( c_lst, t_lsel->attr("dscr") );
		    //Set elements
                    for( int i_el = 0, l_lst = 0; i_el < t_lsel->childSize(); i_el++ )
			if( t_lsel->childGet(i_el)->name() == "el")			
			{
			    //Lines counter
			    if( tbl->numRows() < l_lst+1 ) tbl->setNumRows( l_lst+1 );
			    //Set element
			    if( t_lsel->attr("tp") == "time" )
			    {
				time_t tm_t = strtol(t_lsel->childGet(i_el)->text().c_str(),NULL,16);
				char *c_tm = ctime( &tm_t );
		    		for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
				    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
				tbl->setText(l_lst,c_lst,c_tm);
			    }
			    else tbl->setText(l_lst,c_lst,t_lsel->childGet(i_el)->text());
			    l_lst++; 
			}		    
		    c_lst++;
		}
		
	    widget->layout()->add( new QLabel(t_s.attr("dscr")+":",widget) );
	    widget->layout()->add( tbl );	    
	}	
	//View standart fields
	else if( t_s.name() == "fld" )
	    basicFields( path, t_s, a_path, root, widget, wr, &l_hbox, l_pos );
	else if( t_s.name() == "comm" )
	{
            string br_path = SYS->pathCode(a_path+SYS->pathCode(t_s.attr("id"),true), false);
	    
	    if( !t_s.childSize() )
	    {
		QPushButton *button = new QPushButton( t_s.attr("dscr"), widget, br_path.c_str() );
		connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	    
		QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );	    
		hbox->addWidget( button );
		hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem(hbox);
	    }
	    else
	    {	
		QHBoxLayout *c_hbox = NULL;    
		int 	c_pos = 0;
		
		QFrame *comm_pan = new QFrame( widget );    
		comm_pan->setFrameShape( QFrame::WinPanel );
		comm_pan->setFrameShadow( QFrame::Raised );	        
    		QVBoxLayout *wdg_lay = new QVBoxLayout( comm_pan, 6, 4);
		comm_pan->layout()->setAlignment( Qt::AlignTop );
		
		QPushButton *button = new QPushButton( t_s.attr("dscr"), comm_pan, br_path.c_str() );
		connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );		
		comm_pan->layout()->add(button);				
		
		for( unsigned i_cf = 0; i_cf < t_s.childSize(); i_cf++)
		{
		    XMLNode &t_scm = *t_s.childGet(i_cf);
		    if( t_scm.name() == "fld" )
			basicFields( path, t_scm, a_path+t_s.attr("id")+'/', root, comm_pan, true, &c_hbox, c_pos, true);
		}
		
		widget->layout()->add(comm_pan);
	    }
	}	
    }
    //Clear delete widgets into tail
    if(a_path == "/")
    {
	while( tabs->count() > i_area )
	{
	    block_tabs = true; 
	    tabs->removePage(tabs->page(i_area));	
	    block_tabs = false; 
	}
	tabs->showPage(tabs->currentPage());
    }
}

void ConfApp::basicFields( const string &path, XMLNode &t_s, const string &a_path, const XMLNode &root, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm )
{    
    string br_path = SYS->pathCode( string((comm)?"b":"")+a_path+SYS->pathCode(t_s.attr("id"),true),false);
    if( !comm ) ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
    //View select fields
    if( t_s.attr("dest") == "select" && wr )
    {			
	QComboBox *comb = new QComboBox( FALSE, widget, br_path.c_str() );
	comb->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, 1, 0 ) );	    
	connect( comb, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
				
	XMLNode *x_lst = SYS->ctrId(&(XMLNode &)root,t_s.attr("select"));  //????
	string br_path = SYS->pathCode( t_s.attr("select"), false );
	ctrCmd(path+"/"+br_path, *x_lst, CTR_GET);
	if( x_lst->name() == "list" )
	    for( unsigned i_el = 0, c_el = 0; i_el < x_lst->childSize(); i_el++ )
		if( x_lst->childGet(i_el)->name() == "el")
		{
		    comb->insertItem( x_lst->childGet(i_el)->text(), c_el++ );
		    if( x_lst->childGet(i_el)->text() == t_s.text() )
			comb->setCurrentItem( c_el-1 );
		}
	
	if(t_s.attr("dscr").size()) 
	{
	    *l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
	    (*l_hbox)->insertWidget( l_pos++, new QLabel(t_s.attr("dscr")+":",widget) );
	    (*l_hbox)->insertWidget( l_pos++, comb );
	    (*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	    widget->layout()->addItem( *l_hbox );
	}
	else
	{
	    if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, comb );
	    else delete comb;
	}		
    }	    
    else
    {		    
	//View boolean fields
	if( t_s.attr("tp") == "bool" )
	{
	    QCheckBox *chBox  = new QCheckBox(t_s.attr("dscr"),widget,br_path.c_str());
	    if( t_s.text() == "true" )	chBox->setChecked(true);
	    else				chBox->setChecked(false);
	    widget->layout()->add(chBox);
	    connect( chBox, SIGNAL( stateChanged(int) ), this, SLOT( checkBoxStChange(int) ) );
	}
	//View edit fields
	else if( t_s.attr("tp") == "str" && (t_s.attr("rows").size() || t_s.attr("cols").size()) )
	{
	    QVBoxLayout *layoutEdit = new QVBoxLayout( 0, 0, 6);		    
	    layoutEdit->addWidget( new QLabel(t_s.attr("dscr")+":", widget ) );		    
	    QTextEdit *edit = new QTextEdit(t_s.text(), "TEST!!!!", widget );
	    edit->setWordWrap(QTextEdit::NoWrap);
	    if( !wr ) edit->setReadOnly( true );
	    layoutEdit->addWidget( edit );
	    widget->layout()->addItem(layoutEdit);
	}
        //View Data-Time fields
	else if( t_s.attr("tp") == "time" )
        {
	    struct tm *tm_tm;
	    time_t tm_t;
	    if( t_s.text().size() ) tm_t = strtol(t_s.text().c_str(),NULL,16);
	    else
	    {
		tm_t = time(NULL);
		t_s.text(TSYS::int2str(tm_t,C_INT_HEX));
	    }
	    tm_tm = localtime(&tm_t);
	    QWidget *val_w;
	    if( !wr )
	    {
    		char *c_tm = ctime( &tm_t );
		for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
		if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';

		val_w = new QLabel( widget );
		val_w->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 0 ) );
		((QLabel *)val_w)->setText(string("<b>")+c_tm+"</b>");
	    }	    
	    else
	    {
		val_w = new QDateTimeEdit( QDateTime(
			    QDate(tm_tm->tm_year+1900,tm_tm->tm_mon+1,tm_tm->tm_mday),
			    QTime(tm_tm->tm_hour,tm_tm->tm_min,tm_tm->tm_sec)), widget, br_path.c_str() );
		connect( val_w, SIGNAL( valueChanged(const QDateTime &) ), this, SLOT( dataTimeChange(const QDateTime&) ) );
	    }	    

	    //Check use label
	    if(t_s.attr("dscr").size()) 
	    {
		*l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		
		(*l_hbox)->insertWidget( l_pos++, new QLabel(t_s.attr("dscr")+":",widget) );
		(*l_hbox)->insertWidget( l_pos++, val_w );
		if( !comm )
		{
		    QPushButton *tm_ok = new QPushButton( widget, br_path.c_str() );		
		    connect( tm_ok, SIGNAL( clicked() ), this, SLOT( applyButton() ) );
		    tm_ok->setPixmap(QPixmap(QImage(ok_xpm)));
		    (*l_hbox)->insertWidget( l_pos++, tm_ok );
		}
		(*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem(*l_hbox);
	    }
	    else
	    {
		if( *l_hbox )
		{	
		    (*l_hbox)->insertWidget( l_pos++, val_w );
		    if( !comm )
		    {
			QPushButton *tm_ok = new QPushButton( widget, br_path.c_str() );		
			connect( tm_ok, SIGNAL( clicked() ), this, SLOT( applyButton() ) );
			tm_ok->setPixmap(QPixmap(QImage(ok_xpm)));		    
			(*l_hbox)->insertWidget( l_pos++, tm_ok );
		    }
		}		    
		else delete val_w;		    
	    }
	}
	//View other string and numberic fields	
	else
	{
	    QWidget *val_w;
	    //View info
	    if( !wr )
	    {
		val_w = new QLabel( widget );
		val_w->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 0 ) );
		((QLabel *)val_w)->setText(string("<b>")+t_s.text()+"</b>");
	    }
	    //View edit
	    else
	    {
		val_w = new QLineEdit( t_s.text(), widget, br_path.c_str() );
		val_w->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0 ) );
		((QLineEdit *)val_w)->setCursorPosition(0);
		connect( val_w, SIGNAL( returnPressed() ), this, SLOT( editReturnPress( ) ) );
		//connect( val_w, SIGNAL( lostFocus() ), this, SLOT( editReturnPress( ) ) );
		// addon parameters			
		int val_n = atoi(t_s.attr("len").c_str());
		if( val_n > 0 ) 
		{
		    ((QLineEdit *)val_w)->setMaxLength( val_n );
		    ((QLineEdit *)val_w)->setFixedWidth( (val_n>40)?15*40:15*val_n );
		}
		
		string tp = t_s.attr("tp");
		if( tp == "dec" || tp == "hex" || tp == "oct" || tp == "real" )
		{
		    ((QLineEdit *)val_w)->setFixedWidth( 5*15 );
		    if( tp == "dec" )	((QLineEdit *)val_w)->setValidator( new QIntValidator(val_w) );
		    else if( tp == "hex" ) 	((QLineEdit *)val_w)->setValidator( new QIntValidator(val_w) );
		    else if( tp == "oct" )	((QLineEdit *)val_w)->setValidator( new QIntValidator(val_w) );
		    else if( tp == "real" )	((QLineEdit *)val_w)->setValidator( new QDoubleValidator(val_w) );
		}
		else ((QLineEdit *)val_w)->setMinimumWidth( 7*15 );		
	    }
	    //Check use label
	    if(t_s.attr("dscr").size()) 
	    {
		*l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		(*l_hbox)->insertWidget( l_pos++, new QLabel(t_s.attr("dscr")+":",widget) );
		(*l_hbox)->insertWidget( l_pos++, val_w );			
		(*l_hbox)->addItem( new QSpacerItem( 0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		widget->layout()->addItem(*l_hbox);
	    }
	    else
	    {
		if( *l_hbox ) (*l_hbox)->insertWidget( l_pos++, val_w );
		else delete val_w;		    
	    }		    			
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
	ctrCmd(path,node,CTR_INFO);
	viewChildRecArea(path,node,"/",i,2);  
    }
    catch(TError err) { postMess(err.what(),4); }
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
    
    //Delete tabs
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
    }
    ctrCmd(sel_path,node,CTR_INFO);
    selectChildRecArea(sel_path,node,"/",node);
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
	catch(TError err) { postMess(err.what(),4); }	    
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
		if( t_c.name() == "list" && t_c.attr("hide") != "1" && t_c.attr("tp") == "br" )
    		{		 
		    //Calc Child groups
                    if( level < 0 ) { grp++; continue; }
		    
		    QListViewItem *it = i;
		    string br_path = SYS->pathCode( area_path+t_c.attr("id"), false );
		    
		    if( it->text(2)[0] == '*' )
		    {
		        if( br_path != it->text(2).remove(0,1) ) continue;
		    }
		    else if( grp > 1 )
		    {
			it = new QListViewItem(i,t_c.attr("dscr"),t_c.attr("dscr"),string("*")+br_path);
			i->insertItem(it);
    		    }
		    
		    ctrCmd(path+"/"+br_path, t_c, CTR_GET);
		    //Process group
		    for( unsigned i_lel = 0; i_lel < t_c.childSize(); i_lel++)
		    {
			XMLNode &t_cl = *t_c.childGet(i_lel);
			if( t_cl.name() == "el" )
			{   
			    if( t_c.attr("mode") == "att" )
				br_path = SYS->pathCode( string("d")+area_path+t_c.attr("id")+'/'+SYS->pathCode(t_cl.text(),true), false);
	    		    else
				br_path = SYS->pathCode( string("s")+area_path+t_c.attr("id")+'/'+t_cl.attr("id"), false);

			    QListViewItem *ch_it = new QListViewItem(it,t_cl.text(),t_c.attr("dscr"),br_path);
    			    it->insertItem(ch_it);
			    			    
			    //Next node for next level
			    if( level-1 > 0 ) 
			    {				
				XMLNode n_lev;
				ctrCmd(path+"/"+br_path,n_lev,CTR_INFO);
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

//void ConfApp::ctrInfo( const string &path, XMLNode &node, TContr *cntr, int level )
void ConfApp::ctrCmd( const string &path, XMLNode &node, int cmd, TContr *cntr, int level )
{
    //Check stations
    if( level == 0 )
    {
	if( SYS->pathLev(path,level++,false) == SYS->Station() ) cntr = SYS;
    	else throw TError("Station error!");
    }
    
    //Go to branch
    string s_br = SYS->pathEncode( cntr->pathLev(path,level,false), false );

    if( (cmd == 0 && s_br.size()) || (cmd > 0 && cntr->pathLev(path,level+1,false).size()) )
    {    
	char t_br = s_br[0];
	if( t_br == 'd' )	ctrCmd( path,node,cmd,&cntr->ctrAt1(s_br.substr(1)).at(),++level );
	else if( t_br == 's' )	ctrCmd( path,node,cmd,&cntr->ctrAt(s_br.substr(1)),++level );
	return;
    }
    if(cmd == CTR_INFO)		cntr->ctrStat(node);
    else if(cmd == CTR_GET)	cntr->ctrDinGet(s_br,&node);
    else if(cmd == CTR_SET)	cntr->ctrDinSet(s_br,&node);
}                          

void ConfApp::postMess( const string &mess, int type )
{
    //Put system message.
    own->mPutS("SYS",(type==4)?MESS_CRIT:(type==3)?MESS_ERR:(type==2)?MESS_WARNING:MESS_INFO,mess);
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

    return( own->owner().owner().Sequrity().access( user, mode, usr, grp, accs) );
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
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path.substr(1),false) );    
	    n_el->text(val);
	    return;
	}
	else
	{	
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path,false) );    
    	    ctrCmd(sel_path+"/"+path, *n_el, CTR_GET);

    	    if( n_el->text() == val ) return;
     	    Mess->put("QT_CONTROL",MESS_INFO,"%s| Set <%s> to <%s>!", w_user->text().ascii(), 
		    (sel_path+"/"+path).c_str(), val.c_str() );
    	    n_el->text(val);
    	    ctrCmd(sel_path+"/"+path, *n_el, CTR_SET);
	}
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::buttonClicked( )
{	
    QButton *button = (QButton *)sender();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(button->name(),false) );    

 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Press <%s>!", w_user->text().ascii(), 
		(sel_path+"/"+button->name()).c_str() );
	ctrCmd(sel_path+"/"+button->name(), *n_el, CTR_SET);
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::editReturnPress( )
{
    XMLNode *n_el;    
    QLineEdit *edit = (QLineEdit *)sender();
    
    //if( !edit->isModified() ) return;
    try
    {    
	string path = edit->name();
	string val = edit->text();
	//Check block element
	if(path[0] == 'b')
	{
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path.substr(1),false) );    
	    n_el->text(val);
	    return;
	}
	else
	{	    
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path,false) );    
	    ctrCmd(sel_path+"/"+path, *n_el, CTR_GET);
	
	    if( n_el->text() == val ) return;
	    Mess->put("QT_CONTROL",MESS_INFO,"%s| Change <%s> from <%s> to <%s>!", 
		    w_user->text().ascii(), (sel_path+"/"+path).c_str(), n_el->text().c_str(), val.c_str() );
	    n_el->text(val);
	    ctrCmd(sel_path+"/"+path, *n_el, CTR_SET);
	}
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::combBoxActivate( const QString& val  )
{
    XMLNode *n_el;    
    QComboBox *comb = (QComboBox *)sender();
    
    try
    {    
	string path = comb->name();
	//Check block element
	if(path[0] == 'b')
	{
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path.substr(1),false) );    
	    n_el->text(val);
	    return;
	}
	else
	{
	    n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path,false) );    
	    ctrCmd(sel_path+"/"+path, *n_el, CTR_GET);
	
    	    if( n_el->text() == val ) return;
     	    Mess->put("QT_CONTROL",MESS_INFO,"%s| Change <%s> from <%s> to <%s>!", 
		    w_user->text().ascii(), (sel_path+"/"+path).c_str(), n_el->text().c_str(), val.ascii() );
    	    n_el->text(val);
    	    ctrCmd(sel_path+"/"+path, *n_el, CTR_SET);	
	}
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::listBoxPopup(QListBoxItem* item)
{
    QPopupMenu popup;
    QListBox *lbox = (QListBox *)sender();
    string el_path = sel_path+"/"+lbox->name();

    try
    {    
	bool p_ed = false;
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(lbox->name(),false) );
	if( !chkAccess(*n_el, w_user->text(), SEQ_WR) || !n_el->attr("s_com").size() ) return;
	
	if( n_el->attr("s_com").find("add") != string::npos )
	{ 
	    popup.insertItem(own->I18N("Add"),1); 
	    p_ed = true; 
	}
	if( n_el->attr("s_com").find("ins") != string::npos && item != NULL )
	{ 
	    popup.insertItem(own->I18N("Insert"),2); 
	    p_ed = true; 
	}
	if( n_el->attr("s_com").find("edit") != string::npos && item != NULL )
	{ 
	    popup.insertItem(own->I18N("Edit"),3); 
	    p_ed = true; 
	}
	if( n_el->attr("s_com").find("del") != string::npos && item != NULL )
	{ 
	    popup.insertSeparator();
	    popup.insertItem(own->I18N("Delete"),4); 
	    p_ed = true; 
	}
	    
	if(p_ed) 
	{
	    bool ok;
	    QString text;
	    
	    int rez = popup.exec(QCursor::pos());
	    if( rez == 1 || rez == 2 || rez == 3 )
	    {
		text = QInputDialog::getText(caption(),"Enter element name:", 
			QLineEdit::Normal,(rez==3)?item->text():"", &ok, this );
		if( !ok || text.isEmpty() ) return;		
	    }
	    
	    XMLNode *n_el1 = n_el->childAdd("el");
	    switch(rez)		
	    {
		case 1:
		    n_el1->attr("do","add",true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Add <%s> element <%s>!", 
			    w_user->text().ascii(), el_path.c_str(), text.ascii() );
		    n_el1->text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    //lbox->insertItem(text);	//Add list item		    
		    break;
		case 2:
		    n_el1->attr("do","ins",true);
		    n_el->attr("id",SYS->int2str(lbox->currentItem()),true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Insert <%s> element <%s>!", 
			    w_user->text().ascii(), el_path.c_str(), text.ascii() );
		    n_el1->text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    //lbox->insertItem(text,lbox->currentItem());	//insert list item
		    break;	    
		case 3:
		    n_el1->attr("do","edit",true);
		    n_el->attr("id",SYS->int2str(lbox->currentItem()),true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Rename <%s> element <%s> to <%s>!", 
			    w_user->text().ascii(), el_path.c_str(), item->text().ascii(), text.ascii() );
		    n_el1->text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    //lbox->changeItem(text,lbox->currentItem());	//Change list item text
		    break;	    		    
		case 4:
		    text = item->text();
		    n_el1->attr("do","del",true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Delete <%s> element <%s>!", 
			    w_user->text().ascii(), el_path.c_str(), text.ascii() );
		    n_el1->text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    //lbox->removeItem(lbox->currentItem());	//Remove list item
		    break;	    		    
	    }
	}
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
}

void ConfApp::listBoxGo( QListBoxItem* item )
{
    string path;
    
    QListBox *lbox = (QListBox *)sender();
    try
    { 
        //Prev and next
	if( sel_path.size() )   	prev.insert(prev.begin(),sel_path);
	if( prev.size() >= que_sz )	prev.pop_back();	
	
	//Check branche type 
	if(SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(lbox->name(),false) )->attr("mode") == "att")    
	    path = sel_path+"/"+string("d")+lbox->name()+":"+item->text().ascii();
	else
	    path = sel_path+"/"+string("s")+lbox->name()+":"+SYS->int2str(lbox->currentItem());
	    
	pageDisplay( path );
    }
    catch(TError err) { postMess(err.what(),4); }
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
	n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path.substr(1),false) );    
    	n_el->text(val);
    }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::applyButton( )
{
    QButton *button = (QButton *)sender();
    
    string path = button->name();

    try
    {    
	XMLNode *n_el = SYS->ctrId(&(XMLNode &)node, SYS->pathEncode(path,false) );    
	Mess->put("QT_CONTROL",MESS_INFO,"%s| Change time <%s>!", 
		w_user->text().ascii(), (sel_path+"/"+path).c_str() );
	ctrCmd(sel_path+"/"+path, *n_el, CTR_SET);
    }catch(TError err) { postMess(err.what(),4); }
    //Redraw
    autoUpdTimer->start(100,true);
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

#include <qtcfg.moc>

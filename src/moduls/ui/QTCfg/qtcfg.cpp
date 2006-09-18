
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

#include "tuimod.h"
#include "dlguser.h"
#include "imgview.h"
#include "qtcfg.h"

#define CH_REFR_TM 200

using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( ) : 
    QMainWindow( 0, "", WDestructiveClose ), que_sz(20), block_tabs(false), pg_info("info"), root(&pg_info)
{   
    string simg;
    QImage timg;
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
    connect( CtrTree, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( pressed(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( expanded(QListViewItem*) ), this, SLOT( viewChild(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( onItem(QListViewItem*) ), this, SLOT( onItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( rightButtonPressed (QListViewItem*,const QPoint&,int) ), this, SLOT( ctrTreePopup(QListViewItem*) ) );
		
    QToolTip::remove( CtrTree );
    new ListViewToolTip( CtrTree );
    
    //Right frame add
    QFrame *gFrame = new QFrame( splitter );    
    gFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 3, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );		    
    gFrame->setFrameShape( QFrame::WinPanel );
    gFrame->setFrameShadow( QFrame::Raised );	        

    QGridLayout *gFrameLayout = new QGridLayout( gFrame, 1, 1, 7, -1 );    
    
    titleIco = new QLabel( gFrame );
    titleIco->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
    gFrameLayout->addWidget( titleIco, 0, 0 );
    
    titleLab = new QLabel( gFrame );
    titleLab->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, 0, 0 ) );		    
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 1 );
    
    simg = TUIS::getIco("identity");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(identity_xpm);    
    w_user = new QPushButton(QIconSet(timg), "root", gFrame );		//!!!! My be not root!
    w_user->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Maximum, 0, 0 ) );
    QToolTip::add( w_user, mod->I18N("Change user."));
    QWhatsThis::add( w_user, mod->I18N("This button change the OpenSCADA system user."));	
    connect(w_user, SIGNAL(clicked()), this, SLOT(userSel()));        
    w_user->setPaletteBackgroundColor(QColor(255,0,0));
    gFrameLayout->addWidget( w_user, 0, 2 );
    
    tabs = new QTabWidget( gFrame );
    connect( tabs, SIGNAL( currentChanged(QWidget*) ), this, SLOT( tabSelect(QWidget*) ) );
    gFrameLayout->addMultiCellWidget( tabs, 1, 1, 0, 2 );
    //gFrameLayout->addWidget( tabs, 1, 0 );
    
    QTCfgLayout->addWidget( splitter, 0, 0 );    
    
    //Status bar
    statusBar()->message(mod->I18N("Ready"), 2000 );
    resize( 800, 600 );
    
    //Create actions
    //Close
    simg = TUIS::getIco("close");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(close_xpm);    
    QAction *actClose = new QAction("",QIconSet(timg),mod->I18N("&Close"),CTRL+Key_W,this);
    actClose->setToolTip(mod->I18N("Close configurator window"));
    actClose->setWhatsThis(mod->I18N("Close OpenSCADA configurator window"));
    connect(actClose, SIGNAL(activated()), this, SLOT(close()));
    //Quit
    simg = TUIS::getIco("exit");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(exit_xpm);    
    QAction *actQuit = new QAction("",QIconSet(timg),mod->I18N("&Quit"),CTRL+Key_Q,this);
    actQuit->setToolTip(mod->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(mod->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(quitSt()));
    //Up button
    simg = TUIS::getIco("up");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(up_xpm);
    actUp = new QAction(mod->I18N("Up"),QIconSet(timg),mod->I18N("&Up"),ALT+Key_Up,this);
    actUp->setToolTip(mod->I18N("Up page"));
    actUp->setWhatsThis(mod->I18N("Go to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(pageUp()));    
    //Previos page
    simg = TUIS::getIco("previous");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(previous_xpm);
    actPrev = new QAction(mod->I18N("Previos"),QIconSet(timg),mod->I18N("&Previos"),ALT+Key_Left,this);
    actPrev->setToolTip(mod->I18N("Previos page"));
    actPrev->setWhatsThis(mod->I18N("Go to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(pagePrev()));    
    //Previos page
    simg = TUIS::getIco("next");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(next_xpm);    
    actNext = new QAction(mod->I18N("Next"),QIconSet(timg),mod->I18N("&Next"),ALT+Key_Right,this);
    actNext->setToolTip(mod->I18N("Next page"));
    actNext->setWhatsThis(mod->I18N("Go to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(pageNext()));    
    //Update
    simg = TUIS::getIco("reload");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(reload_xpm);    
    QAction *actUpdate = new QAction(mod->I18N("Refresh"),QIconSet(timg),mod->I18N("&Refresh"),Key_F5,this);
    actUpdate->setToolTip(mod->I18N("Refresh current page"));
    actUpdate->setWhatsThis(mod->I18N("Button for refreshing a content of the current page."));
    connect(actUpdate, SIGNAL(activated()), this, SLOT(pageRefresh()));        
    //Start of "Auto update"
    simg = TUIS::getIco("start");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(start_xpm);    
    actStartUpd = new QAction(mod->I18N("Start"),QIconSet(timg),mod->I18N("&Start"),CTRL+Key_B,this);
    actStartUpd->setToolTip(mod->I18N("Start cycled refresh"));
    actStartUpd->setWhatsThis(mod->I18N("Button for start of cycled refresh content of the current page."));
    connect(actStartUpd, SIGNAL(activated()), this, SLOT(pageCyclRefrStart()));        
    //Stop of "Auto update"
    simg = TUIS::getIco("stop");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(stop_xpm);        
    actStopUpd = new QAction(mod->I18N("Stop"),QIconSet(timg),mod->I18N("&Stop"),CTRL+Key_E,this);
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
    simg = TUIS::getIco("help");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(help_xpm);    
    help->insertItem(QIconSet(timg), mod->I18N("&About"), this, SLOT(about()), Key_F1 );
    help->insertItem(mod->I18N("About &Qt"), this, SLOT(aboutQt()) );
    help->insertSeparator();
    simg = TUIS::getIco("contexthelp");
    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
    else timg = QImage(contexthelp_xpm);    
    help->insertItem(QIconSet(timg), mod->I18N("What's &This"), this, SLOT(whatsThis()), SHIFT+Key_F1 );
    
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
    
    //Display root page and init external pages
    initHosts();
    pageDisplay( getItemPath(CtrTree->firstChild()) );
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
    SYS->security().at().usrList(u_list);
    d_usr->user(u_list);
    int rez = d_usr->exec();
    string dl_user   = d_usr->user();
    string dl_passwd = d_usr->password();
    delete d_usr;    
    
    if( rez && dl_user != w_user->text() )
    {
	try
	{
	    if( SYS->security().at().usrAt(dl_user).at().auth(dl_passwd) ) 
	    {
		w_user->setText( dl_user );
		if( dl_user == "root" )	w_user->setPaletteBackgroundColor(QColor(255,0,0));
		else 			w_user->setPaletteBackgroundColor(QColor(0,255,0));
		sel_path="";	//Clear last path for correct user change
		pageDisplay( getItemPath( CtrTree->firstChild() ) );
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
        mod->modInfo("Version").c_str(),mod->modInfo("Author").c_str(),mod->modInfo("License").c_str());
    
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
	//Set node icon
	if( node.childGet("id","ico",true) )
        {
	    XMLNode dt_req("get");
	    dt_req.attr("path",sel_path+"/"+TSYS::strEncode(a_path+"ico",TSYS::PathEl))->attr("user",w_user->text());
    	    if( cntrIfCmd(dt_req) ) postMess(dt_req.attr("mcat"),dt_req.text(),4);
	    else
	    {
    		string simg = TSYS::strDecode(dt_req.text(),TSYS::base64);
		QImage img;
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    titleIco->setPixmap(QPixmap(img.smoothScale(32,32)));
		else titleIco->clear();	
	    }
        }else titleIco->clear();	
	
	//Set title
	titleLab->setText(string("<p align='center'><i><b>")+TSYS::strEncode(node.attr("dscr"),TSYS::Html)+"</b></i></p>");

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
	    if( atoi(t_s.attr("acs").c_str())&SEQ_WR ) wr = true;
	
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
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);
		
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
		XMLNode dt_req("get");
		dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text());
		if(cntrIfCmd(dt_req)) postMess(dt_req.attr("mcat"),dt_req.text(),4);
		else
		    for( unsigned i_el = 0, c_el = 0; i_el < dt_req.childSize(); i_el++ )
                	if( dt_req.childGet(i_el)->name() == "el")
                    	    lstbox->insertItem( dt_req.childGet(i_el)->text(), c_el++ );								    
    	    }
	    //View table elements
	    else if( t_s.name() == "table" )
	    {
		string br_path = TSYS::strEncode(a_path+t_s.attr("id"),TSYS::PathEl);
		
		//QLabel *lab;
		QTable *tbl;
		
		if( !refr )
		{		    
		    tbl = new QTable( widget, br_path.c_str() );
		    tbl->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 0, 1 ) );
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
                XMLNode dt_req("get");
		dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text());
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
		
		    int c_col = tbl->currentColumn();
		    int c_row = tbl->currentRow();

		    if( tbl->numCols() != n_col )	tbl->setNumCols(n_col);
		    if( tbl->numRows() != n_row )   	tbl->setNumRows(n_row);		
		
		    for( unsigned i_lst = 0; i_lst < t_s.childSize(); i_lst++ )	    
		    {
			XMLNode *t_linf = t_s.childGet(i_lst);
			if(!t_linf) continue;
			bool c_wr = wr && (atoi(t_linf->attr("acs").c_str())&SEQ_WR);
		    
			tbl->horizontalHeader()->setLabel( i_lst, t_linf->attr("dscr") );
			//Set elements
			for( int i_el = 0; i_el < t_linf->childSize(); i_el++ )
			{
			    //Set element
			    if( t_linf->attr("tp") == "bool" )
			    {			    			    
				tbl->setItem(i_el,i_lst, new QCheckTableItem(tbl,""));
				((QCheckTableItem *)tbl->item(i_el,i_lst))->
				    setChecked(atoi(t_linf->childGet(i_el)->text().c_str()));
				if( !c_wr ) ((QCheckTableItem *)tbl->item(i_el,i_lst))->setEnabled(false);
			    }				    
			    else if( (t_linf->attr("dest") == "select" || t_linf->attr("dest") == "sel_ed") && c_wr )
			    {
				QStringList elms;
				int sel_n = -1;
				bool u_ind = atoi(t_linf->attr("idm").c_str());
				
				XMLNode x_lst("get");
				x_lst.attr("path",sel_path+"/"+TSYS::strEncode( t_linf->attr("select"),TSYS::PathEl))->attr("user",w_user->text());
				if(cntrIfCmd(x_lst)) postMess(x_lst.attr("mcat"),x_lst.text(),4);
				else
				    for( int i_ls = 0; i_ls < x_lst.childSize(); i_ls++ )
				    {
					elms+=x_lst.childGet(i_ls)->text();
					if( (u_ind && x_lst.childGet(i_ls)->attr("id") == t_linf->childGet(i_el)->text()) ||
						(!u_ind && x_lst.childGet(i_ls)->text() == t_linf->childGet(i_el)->text()) )
					    sel_n = i_ls;
				    }
				if( sel_n < 0 )
				{
				    elms.insert(elms.begin(),"");
				    sel_n = 0;
				}
			    
				tbl->setItem(i_el,i_lst, new QComboTableItem(tbl,elms));
				if( t_linf->attr("dest") == "sel_ed" )
			    	    ((QComboTableItem *)tbl->item(i_el,i_lst))->setEditable(true);
				((QComboTableItem *)tbl->item(i_el,i_lst))->setCurrentItem(sel_n);			    
			    }
			    else if( t_linf->attr("tp") == "time" )
			    {
				time_t tm_t = atoi(t_linf->childGet(i_el)->text().c_str());
				char *c_tm = ctime( &tm_t );
				for( int i_ch = 0; i_ch < strlen(c_tm); i_ch++ )
				    if( c_tm[i_ch] == '\n' ) c_tm[i_ch] = '\0';
				tbl->setText(i_el,i_lst,c_tm);
			    }
			    else tbl->setText(i_el,i_lst,t_linf->childGet(i_el)->text());
			}
			if( !c_wr )	tbl->setColumnReadOnly(i_lst,true);
    			tbl->adjustColumn(i_lst);
    		    }		
		    tbl->setCurrentCell( c_row, c_col );
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
		    img = new ImgView( widget, br_path.c_str() );
    		    img->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
		    img->setMinimumSize(200,200);
	    
    		    QVBoxLayout *vbox = new QVBoxLayout( 0, 0, 6 );
		    lab = new QLabel(widget);
    		    vbox->addWidget(lab);
    		    vbox->addWidget(img);
    		    widget->layout()->addItem(vbox);
		    
		    t_s.attr("addr_lab",addr2str(lab));
		    t_s.attr("addr_el",addr2str(img));
		}
		else
		{
		    lab = (QLabel *)str2addr(t_s.attr("addr_lab"));
		    img = (ImgView *)str2addr(t_s.attr("addr_el"));
		}
		//Set image
		lab->setText(t_s.attr("dscr")+":");
		
		XMLNode dt_req("get");
                dt_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text());
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
			button = new QPushButton( widget, br_path.c_str() );
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
			button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
			widget->layout()->add( button );
		    }
		    else
		    {	

			comm_pan = new QGroupBox( t_s.attr("dscr"), widget );
		        comm_pan->setColumnLayout(0, Qt::Vertical );
	       	    	comm_pan->layout()->setMargin( 6 );
			comm_pan->layout()->setSpacing( 4 );		
			comm_pan->layout()->setAlignment( Qt::AlignTop );
		    
			button = new QPushButton( comm_pan, br_path.c_str() );
			button->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed, 0, 0 ) );
			connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );		
		
			comm_pan->layout()->add(button);				
			widget->layout()->add(comm_pan);
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
		button->setText(t_s.attr("dscr"));
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
        data_req.attr("path",sel_path+"/"+br_path)->attr("user",w_user->text());
        if(cntrIfCmd(data_req))
	{ 
	    postMess(data_req.attr("mcat"),data_req.text(),4);
	    data_req.text("");
	}
    }

    //View select fields
    if( (t_s.attr("dest") == "select" || t_s.attr("dest") == "sel_ed") && wr )
    {	
	QLabel *lab     = NULL;
        QComboBox *comb = NULL;
	
	if( !refr )
	{    		
	    comb = new QComboBox( FALSE, widget, br_path.c_str() );
	    QToolTip::add( comb, t_s.attr("id") );
	    if( t_s.attr("dest") == "sel_ed" )
		comb->setEditable(true);
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
	
	XMLNode x_lst("get");
	x_lst.attr("path",sel_path+"/"+TSYS::strEncode( t_s.attr("select"),TSYS::PathEl))->attr("user",w_user->text());	
	if(cntrIfCmd(x_lst)) postMess(x_lst.attr("mcat"),x_lst.text(),4);
	else
	{
    	    bool sel_ok = false;
    	    unsigned i_el,c_el;
    	    for( i_el = 0, c_el = 0; i_el < x_lst.childSize(); i_el++ )
    		if( x_lst.childGet(i_el)->name() == "el")
    		{
        	    comb->insertItem( x_lst.childGet(i_el)->text(), c_el++ );
	    	    bool ind_ok = x_lst.childGet(i_el)->attr("id").size();	//Index present
        	    if( (ind_ok && x_lst.childGet(i_el)->attr("id") == data_req.text()) || 
			    (!ind_ok && x_lst.childGet(i_el)->text() == data_req.text()) )
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
		QToolTip::add( chBox, t_s.attr("id") );
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
	    if( atoi(data_req.text().c_str()) )	chBox->setChecked(true);
    	    else chBox->setChecked(false);
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
		QToolTip::add( edit, t_s.attr("id") );
		if(atoi(t_s.attr("rows").c_str()) < 10)
		{
		    edit->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 0, 0 ) );
		    edit->setFixedHeight(2*edit->currentFont().pointSize()*atoi(t_s.attr("rows").c_str()));
		}
		else
		{
		    edit->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding, 0, 1 ) );
		    edit->setMinimumHeight(atoi(t_s.attr("rows").c_str())*20);		    
		}
		edit->setWordWrap(QTextEdit::NoWrap);
		//edit->setMinimumSize(atoi(t_s.attr("cols").c_str())*8,atoi(t_s.attr("rows").c_str())*20);
		layoutEdit->addWidget( edit );
		
		if( !wr )       edit->setReadOnly( true );    
		else
		{
		    connect( edit, SIGNAL( textChanged() ), this, SLOT( editChange( ) ) );
		    
		    QHBoxLayout *bt_layout = new QHBoxLayout( 0, 0, 6);		    
		    
		    string simg = TUIS::getIco("button_ok");
		    QImage timg;
		    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
		    else timg = QImage(button_ok_xpm);
		    QPushButton *bt_ok = new QPushButton( QIconSet(timg), mod->I18N("Apply"), widget, br_path.c_str() );
		    connect( edit, SIGNAL( modificationChanged(bool) ), bt_ok, SLOT( setShown(bool) ) );
		    bt_ok->setHidden(true);
		    connect( bt_ok, SIGNAL( clicked() ), this, SLOT( applyButton() ) );
		    
		    simg = TUIS::getIco("button_cancel");
		    if(simg.size()) timg.loadFromData((const uchar *)simg.c_str(),simg.size());
		    else timg = QImage(button_cancel_xpm);		    
		    QPushButton *bt_cancel = new QPushButton( QIconSet(timg), mod->I18N("Cancel"), widget, br_path.c_str() );
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
	    edit->setText(data_req.text());	    
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
		val_w = new DateTimeEdit( widget, br_path.c_str(), comm );
		QToolTip::add( val_w, t_s.attr("id") );
	    	connect( val_w, SIGNAL( valueChanged(const QDateTime &) ), this, SLOT( dataTimeChange(const QDateTime&) ) );
		connect( val_w, SIGNAL( apply() ), this, SLOT( applyButton() ) );
		connect( val_w, SIGNAL( cancel() ), this, SLOT( cancelButton() ) );
		if(!wr) val_w->setDisabled(true);

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
		    QToolTip::add( val_r, t_s.attr("id") );
		    val_r->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 1, 0 ) );
		    val_r->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
		}
		//View edit
		else
		{
		    val_w = new LineEdit( widget, br_path.c_str(), comm );		    
		    QToolTip::add( val_w, t_s.attr("id") );
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
	    if( val_r ) val_r->setText(string("<b>")+TSYS::strEncode(data_req.text(),TSYS::Html)+"</b>");
	    if( val_w )	val_w->setText(data_req.text());
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
	string path = getItemPath(i);
	viewChildRecArea(path,i,2);
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
	//Stop refresh
	pageCyclRefrStop();
    
    	block_tabs = true;
        while(tabs->page(0))
        {
            QWidget *tab = tabs->page(0);
            tabs->removePage(tab);
            delete tab;
        }	
    	block_tabs = false;
	sel_path = path;
	
	pg_info.childClean();
	pg_info.attr("path",sel_path)->attr("user",w_user->text());
	if(cntrIfCmd(pg_info)) { postMess(pg_info.attr("mcat"),pg_info.text(),4); return; }
	root = pg_info.childGet(0);
    }
    else
    {    
	//Check the new node structure and the old node
	XMLNode n_node("info");
	n_node.attr("path",sel_path)->attr("user",w_user->text());
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

void ConfApp::ctrTreePopup( QListViewItem * i )
{
    QPopupMenu popup;
    QListView *lview = (QListView *)sender();

    try
    {    
        popup.insertItem(mod->I18N("Remote hosts update"),0);
        int rez = popup.exec(QCursor::pos());
	if( rez == 0 )	initHosts();
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

void ConfApp::viewChildRecArea( const string &path, QListViewItem *i, int level )
{
    //Get icon
    if( i->text(2)[0] != '*' && !(i->pixmap(0)) )
    {
	XMLNode dt_req("get");
	dt_req.attr("path",path+"/%2fico")->attr("user",w_user->text());
        if( !cntrIfCmd(dt_req) )
	//else
	{    
	    string simg = TSYS::strDecode(dt_req.text(),TSYS::base64);
	    QImage img;
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
    		i->setPixmap(0,QPixmap(img.smoothScale(16,16)));
	}
    }
    
    //Get branches groups
    XMLNode br_req("info");
    br_req.attr("path",path+"/%2fbr")->attr("user",w_user->text());
    if( cntrIfCmd(br_req) ) return;
    XMLNode *brs = br_req.childGet(0,true);
    if( !brs )	return;
    
    //Proccess branches groups
    for( int ibr = 0; ibr < brs->childSize(); ibr++ )
    {
	XMLNode *br = brs->childGet(ibr);
	QListViewItem *it = i;
	if( it->text(2)[0] == '*' )
	{
	    if( br->attr("id") != it->text(2).remove(0,1) ) continue;
	}
	else if( brs->childSize() > 1 )
	{
	    it = new QListViewItem(i,br->attr("dscr")+":",br->attr("dscr"),string("*")+br->attr("id"));
	    i->insertItem(it);
    	}
	
	//Get branches
	XMLNode req("get");
	req.attr("path",path+"/"+TSYS::strEncode(br->attr("list"),TSYS::PathEl))->attr("user",w_user->text());
	if( cntrIfCmd(req) ) { postMess(req.attr("mcat"),req.text(),4);	continue; }
	
	for( int i_lel = 0; i_lel < req.childSize(); i_lel++)
	{
	    XMLNode *br_el = req.childGet(i_lel);
	    //Check index-list
	    string br_path = br->attr("id");
	    if( br_el->attr("id").size() )	br_path.append(br_el->attr("id"));
	    else				br_path.append(br_el->text());
	    br_path = TSYS::strEncode(br_path,TSYS::PathEl);
		    
	    QListViewItem *ch_it = new QListViewItem(it,br_el->text(),br->attr("dscr"),br_path);
	    it->insertItem(ch_it);
			    			    
	    //Next node for next level
	    if( level-1 > 0 )
		viewChildRecArea(path+"/"+br_path,ch_it,level-1);
	}
    }    
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
    
    return p_path;
}

int ConfApp::cntrIfCmd( XMLNode &node )
{
    string path = node.attr("path");
    string station = TSYS::pathLev(path,0,false);
    node.attr("path",path.substr(station.size()+1));
    
    //- Check local station request -    
    if( station == SYS->id() )
    {
        SYS->cntrCmd(&node);
	node.attr("path",path);
	return atoi(node.attr("rez").c_str());
    }
    
    //- Check remote station request -
    ExtHost host = mod->extHostGet(w_user->text(),station);
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
		resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),5);
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
	    resp_len = tr.at().messIO(req.c_str(),req.size(),buf,sizeof(buf),5);
	    resp.assign(buf,resp_len);
	    //Get head
	    buf1[0] = 0;
	    sscanf(resp.c_str(),"REZ %d %255s\n",&rez,buf1);
	    if(rez == 1)	{ host.ses_id = -1; continue; }
	    if(rez > 0)	throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error: %d:%s!"),station.c_str(),rez,buf1);	    
	    int head_end = resp.find("\n",0);
	    if(head_end == string::npos)
		throw TError(mod->nodePath().c_str(),mod->I18N("Station <%s> error: Respond broken!"),station.c_str());	    
	    int resp_size = atoi(buf1);	
	    //Wait tail
	    while(resp.size() < resp_size+head_end+sizeof('\n'))
	    {
		resp_len = tr.at().messIO(NULL,0,buf,sizeof(buf),5);
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
	}
	throw;
    }
}                          

void ConfApp::initHosts()
{
    bool it_present = false;
    //- Remove no present hosts -
    QListViewItem *nit = CtrTree->firstChild();
    if(nit)
	do 
	{	
	    if(SYS->id() == nit->text(2).ascii())
	    {
		it_present = true;
		continue;
	    }
	    ExtHost host = mod->extHostGet(w_user->text(),nit->text(2).ascii());
	    if(host.id.size())
	    {
		if(!host.link_ok) viewChild(nit);
		continue;
	    }
	    CtrTree->takeItem(nit);
    	    delete nit;
	}
	while( nit = nit->nextSibling() );
        
    //- Add new hosts -
    //-- Init local host --    
    if(!it_present)
    {
	nit = new QListViewItem(CtrTree,SYS->name(),mod->I18N("Local station"),SYS->id());
	nit->setExpandable(true);
	CtrTree->insertItem(nit);	
	//viewChild(nit);
    }	
    
    //-- Init remote hosts --
    vector<string> list;
    mod->extHostList(w_user->text(),list);
    for(int i_h = 0; i_h < list.size(); i_h++)
    {
	ExtHost host = mod->extHostGet(w_user->text(),list[i_h]);
	//--- Find alreadi present items ---
	it_present = false;
	nit = CtrTree->firstChild();
	do 
	{
	    if(host.id != nit->text(2).ascii())	continue;
	    it_present = true;
	    break;
	}
	while( nit = nit->nextSibling() );
	if(!it_present)
	{
	    nit = new QListViewItem(CtrTree,host.name,mod->I18N("Remote station"),host.id);
	    nit->setExpandable(true);
	    CtrTree->insertItem(nit);
	    //viewChild(nit);
	}
    }
}

void ConfApp::postMess( const string &cat, const string &mess, int type )
{
    //Put system message.
    Mess->put(cat.c_str(),(type==4)?TMess::Crit:(type==3)?TMess::Error:(type==2)?TMess::Warning:TMess::Info,"%s",mess.c_str());
    //QT message
    if( type == 1 )	QMessageBox::information( this,caption(),mess);
    else if( type == 2 )QMessageBox::warning( this,caption(),mess);
    else if( type == 3 )QMessageBox::critical( this,caption(),mess);
    else if( type == 4 )QErrorMessage::qtHandler()->message(mess);
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
	string val = (stat==QButton::On)?"1":"0";
	//Check block element
	if(path[0] == 'b')
	{
	    SYS->ctrId(root,TSYS::strDecode(path.substr(1),TSYS::PathEl) )->text(val);
	    return;
	}
	else
	{
	    XMLNode req("get");
	    req.attr("path",sel_path+"/"+path)->attr("user",w_user->text());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	    
	    
    	    if( req.text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Set <%s> to <%s>!"), w_user->text().ascii(), 
		    (sel_path+"/"+path).c_str(), val.c_str() );    	    
	    
	    req.name("set"); req.text(val);
	    if(cntrIfCmd(req))	postMess(req.attr("mcat"),req.text(),4);
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(CH_REFR_TM,true);
}

void ConfApp::buttonClicked( )
{	
    QButton *button = (QButton *)sender();
    
    XMLNode req();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(button->name(),TSYS::PathEl) );
	
	//Check link
	if( n_el->attr("tp") == "lnk")
	{
	    XMLNode req("get");
	    req.attr("path",sel_path+"/"+button->name())->attr("user",w_user->text());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }
	    
	    string url = TSYS::pathLev(sel_path,0)+"/"+req.text();
	    
    	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Go to link <%s>!"), w_user->text().ascii(),url.c_str());
	    
	    //Prev and next
    	    if( sel_path.size() )       prev.insert(prev.begin(),sel_path);
	    if( prev.size() >= que_sz ) prev.pop_back();
	    next.clear();			
	    
	    pageDisplay( url );	    
	}
	else
	{
	    XMLNode req("set");
            req.attr("path",sel_path+"/"+button->name())->attr("user",w_user->text());
	    //Copy parameters
	    for( int i_ch = 0; i_ch < n_el->childSize(); i_ch++ )
		*(req.childAdd()) = *(n_el->childGet(i_ch));
	
 	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Press <%s>!"), w_user->text().ascii(), 
		(sel_path+"/"+button->name()).c_str() );
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(CH_REFR_TM,true);
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
	
	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );
	
        //Get list for index list check!
	if( n_el->attr("dest") == "select" )
	{
    	    bool find_ok = false;
    	    XMLNode x_lst("get");
	    x_lst.attr("path",sel_path+"/"+TSYS::strEncode( n_el->attr("select"),TSYS::PathEl))->attr("user",w_user->text());
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
	    req.attr("path",sel_path+"/"+path)->attr("user",w_user->text());
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }
	
    	    if( req.text() == val ) return;
     	    Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Change <%s> from <%s> to <%s>!"), 
		    w_user->text().ascii(), (sel_path+"/"+path).c_str(), req.text().c_str(), val.c_str() );
    	    
	    req.name("set"); req.text(val);
	    if(cntrIfCmd(req)) { postMess(req.attr("mcat"),req.text(),4); return; }	    
	}
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(CH_REFR_TM,true);
}

void ConfApp::listBoxPopup(QListBoxItem* item)
{
    QPopupMenu popup;
    QListBox *lbox = (QListBox *)sender();
    string el_path = sel_path+"/"+lbox->name();
    XMLNode *n_el;

    try
    {    
	n_el = SYS->ctrId(root,TSYS::strDecode(lbox->name(),TSYS::PathEl) );
        int last_it = -1;
        if( n_el->attr("tp") == "br" && item != NULL )
        { 
    	    last_it = popup.insertItem(mod->I18N("Go"),0); 
	    popup.insertSeparator();
	}
	if( (atoi(n_el->attr("acs").c_str())&SEQ_WR) && n_el->attr("s_com").size() )
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
		XMLNode x_lst("get");
		x_lst.attr("path",el_path)->attr("user",w_user->text());
		if(cntrIfCmd(x_lst)) { postMess(x_lst.attr("x_lst"),x_lst.text(),4); return; }
		
        	p_text = item->text().ascii();
        	if( ind_m )
            	    for( int i_el = 0; i_el < x_lst.childSize(); i_el++ )
            		if( x_lst.childGet(i_el)->text() == item->text() )
                	{
                    	    p_id = x_lst.childGet(i_el)->attr("id");
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
	    n_el1.attr("path",el_path)->attr("user",w_user->text())->text(p_text);
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
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Add <%s> element <%s:%s>!"), 
	    		w_user->text().ascii(), el_path.c_str(), id.c_str(), text.c_str() );
	    }
	    else if( rez == 2 )
	    {
		n_el1.name("ins");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Insert <%s> element <%s:%s> to %d!"), 
	    		w_user->text().ascii(), el_path.c_str(), id.c_str(), text.c_str(),c_id);
	    }
	    else if( rez == 3 )
	    {
		n_el1.name("edit");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("p_id",(ind_m)?p_id:p_text);
		if( ind_m ) n_el1.attr("id",id);
		n_el1.text(text);
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Set <%s> element %d to <%s:%s>!"), 
	    		w_user->text().ascii(), el_path.c_str(), c_id, id.c_str(), text.c_str());
	    }
	    else if( rez == 4 )
	    {
		n_el1.name("del");
		n_el1.attr("pos",TSYS::int2str(c_id));
		if( ind_m ) n_el1.attr("id",p_id);
		else n_el1.text(item->text());
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Delete <%s> element <%s:%s>!"), 
	    		w_user->text().ascii(), el_path.c_str(), n_el1.attr("id").c_str(), n_el1.text().c_str());
	    }
	    else if( rez == 5 || rez == 6 )
	    {
		int c_new = c_id-1;
		if( rez == 6 )	c_new = c_id+1;		
		n_el1.name("move");
		n_el1.attr("pos",TSYS::int2str(c_id));
		n_el1.attr("to",TSYS::int2str(c_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Move <%s> from %d to %d!"), 
	    		w_user->text().ascii(), el_path.c_str(), c_id, c_new);
	    }
	    if( rez >= 0 ) 
	    {
		if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }
		autoUpdTimer->start(CH_REFR_TM,true);      //Redraw
	    }	    	    
	}
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	autoUpdTimer->start(CH_REFR_TM,true);	//Redraw
    }
}

void ConfApp::tablePopup(int row, int col, const QPoint &pos )
{
    QPopupMenu popup;
    QTable *tbl = (QTable *)sender();
    string el_path = sel_path+"/"+tbl->name();
    
    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(tbl->name(),TSYS::PathEl) );
	
	int last_it = -1;
	if( (atoi(n_el->attr("acs").c_str())&SEQ_WR) && n_el->attr("s_com").size() )
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
	    n_el1.attr("path",el_path)->attr("user",w_user->text());
	    if( rez == 1 )
	    {
    		n_el1.name("add");
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Add <%s> record."),
			w_user->text().ascii(), el_path.c_str() );
	    }
	    else if( rez == 2 )
	    {
		n_el1.name("ins");
	    	n_el1.attr("row",TSYS::int2str(row));
    		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Insert <%s> record %d."), 
			w_user->text().ascii(), el_path.c_str(), row );
	    }
	    else if( rez == 3 )
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
			w_user->text().ascii(), el_path.c_str(), row_addr.c_str() );
	    }
	    else if( rez == 4 || rez == 5 )
	    {
		int r_new = row-1;
                if( rez == 5 )  r_new = row+1;
		n_el1.name("move");
		n_el1.attr("row",TSYS::int2str(row))->attr("to",TSYS::int2str(r_new));
		Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Move <%s> record from %d to %d."), 
			w_user->text().ascii(), el_path.c_str(), row, r_new );
	    }
	    if( rez >= 0 )
	    {
		if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }
		autoUpdTimer->start(CH_REFR_TM,true);
	    }
	}	
    }catch(TError err) 
    { 
	postMess(err.cat,err.mess,4); 
	autoUpdTimer->start(CH_REFR_TM,true);	//Redraw
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

	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(tbl->name(),TSYS::PathEl) );	

	value = tbl->text(row,col).ascii();	
	if( tbl->item(row,col)->rtti() == 1 )
	{
	    bool find_ok = false;
	    XMLNode x_lst("get");
	    x_lst.attr("path",sel_path+"/"+TSYS::strEncode(n_el->childGet(col)->attr("select"),TSYS::PathEl))->attr("user",w_user->text());
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
	if( tbl->item(row,col)->rtti() == 2 )
	    value = (((QCheckTableItem *)tbl->item(row,col))->isChecked())?"1":"0";
	
	//*** Prepare request ***
	XMLNode n_el1("set");
	n_el1.attr("path",el_path)->attr("user",w_user->text())->text(value);
    	//Get current column id
     	for( int i_el = 0; i_el < n_el->childSize(); i_el++ )
	    if( tbl->horizontalHeader()->label(col) == n_el->childGet(i_el)->attr("dscr") )
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
	    w_user->text().ascii(), el_path.c_str(), row_addr.c_str(), n_el1.attr("col").c_str(), value.c_str());
	if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }    
    }
    catch(TError err) { postMess(err.cat,err.mess,4); }
    
    autoUpdTimer->start(CH_REFR_TM,true);
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
	
	XMLNode &t_c = *TCntrNode::ctrId(root,TSYS::strDecode(lbox->name(),TSYS::PathEl) );
	
	XMLNode req("get");
        req.attr("path",sel_path+"/"+lbox->name())->attr("user",w_user->text());
	if(cntrIfCmd(req)) throw TError(mod->nodePath().c_str(),"%s",req.text().c_str(),4);
	
	string br_pref = t_c.attr("br_pref");
	//Find selected index
        bool sel_ok = false;
        for( int i_el = 0; i_el < req.childSize(); i_el++ )
    	    if( req.childGet(i_el)->name() == "el" && req.childGet(i_el)->text() == item->text() )
    	    {
		if( req.childGet(i_el)->attr("id").size() )
                    path = sel_path+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->attr("id"),TSYS::PathEl);
		else
                    path = sel_path+"/"+TSYS::strEncode(br_pref+req.childGet(i_el)->text(),TSYS::PathEl);
		sel_ok = true;
	    }
	if( !sel_ok ) throw TError(mod->nodePath().c_str(),mod->I18N("Select element <%s> no present!"),item->text().ascii());
	    
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
	string val = TSYS::int2str(mktime(&tm_tm));
	
	//Check block element
	if(path[0] == 'b') path.erase(0,1);
	n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );    
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
	SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) )->text(txt_ed->text());
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
	SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) )->text(txt);
    }catch(TError err) { postMess(err.cat,err.mess,4); }
}

void ConfApp::applyButton( )
{
    QButton *button = (QButton *)sender();
    
    string path = button->name();

    try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );    
	Mess->put(mod->nodePath().c_str(),TMess::Info,mod->I18N("%s| Change <%s> to: <%s>!"), 
		w_user->text().ascii(), (sel_path+"/"+path).c_str(), n_el->text().c_str() );
	
	XMLNode n_el1("set");
	n_el1.attr("path",sel_path+"/"+path)->attr("user",w_user->text())->text(n_el->text());
	if(cntrIfCmd(n_el1)) { postMess(n_el1.attr("mcat"),n_el1.text(),4); return; }	
    }catch(TError err) { postMess(err.cat,err.mess,4); }
    //Redraw
    autoUpdTimer->start(CH_REFR_TM,true);
}

void ConfApp::cancelButton( )
{
    QButton *button = (QButton *)sender();
    
    string path = button->name();

    /*try
    {    
	XMLNode *n_el = SYS->ctrId(root,TSYS::strDecode(path,TSYS::PathEl) );    
	cntrIfCmd(sel_path+"/"+path, *n_el, TCntrNode::Get);
    }catch(TError err) { postMess(err.cat,err.mess,4); }*/
    //Redraw
    autoUpdTimer->start(CH_REFR_TM,true);
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
    if(e->type() == QEvent::KeyPress && bt_fld)
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
    if(e->type() == QEvent::KeyPress && bt_fld)
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


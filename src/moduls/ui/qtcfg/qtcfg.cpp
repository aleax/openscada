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

#include "xpm/up.xpm"
#include "xpm/back.xpm"
#include "xpm/forward.xpm"
#include "xpm/window_new.xpm"
#include "xpm/identity.xpm"
#include "xpm/exit.xpm"
#include "xpm/help.xpm"
#include "xpm/contexthelp.xpm"

#include <tmessage.h>
#include <tsys.h>
#include <tsequrity.h>
#include "tuimod.h"
#include "qtcfg.h"

using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( TUIMod *owner, ConfApp *parent ) : 
    QMainWindow( 0, "", WDestructiveClose ), own(owner), m_parent(parent), user("root"), que_sz(20), p_active(false)
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
    
    //QScrollView *vp = new QScrollView(gFrame);
    //QVBox* big_box = new QVBox(vp->viewport());
    //vp->addChild(big_box);
    
    //titleLab = new QLabel( big_box );
    //QFont titleLab_font( titleLab->font() );
    //titleLab_font.setPointSize( 14 );
    //titleLab->setFont( titleLab_font );

    //tabs = new QTabWidget( big_box );
    
    QGridLayout *gFrameLayout = new QGridLayout( gFrame, 1, 1, 7, -1 );    
    titleLab = new QLabel( gFrame );
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 0 );

    tabs = new QTabWidget( gFrame );
    connect( tabs, SIGNAL( selected(const QString&) ), this, SLOT( tabSelect(const QString&) ) );
    gFrameLayout->addWidget( tabs, 1, 0 );

    QTCfgLayout->addWidget( splitter, 0, 0 );    

    QListViewItem *root_l_it = new QListViewItem(CtrTree,SYS->Station(),own->I18N("Local station"),SYS->Station());
    CtrTree->insertItem( root_l_it );
    viewChild( root_l_it );

    //Status bar
    statusBar()->message(own->I18N("Ready"), 2000 );
    resize( 800, 600 );

    //Create actions
    //Quit
    QAction *actQuit = new QAction(own->I18N("&Quit"),CTRL+Key_Q,this);
    actQuit->setIconSet(QPixmap(QImage(exit_xpm)));
    actQuit->setToolTip(own->I18N("Quit OpenSCADA"));
    actQuit->setWhatsThis(own->I18N("Quit from OpenSCADA"));
    connect(actQuit, SIGNAL(activated()), this, SLOT(close()));        
    //connect(actQuit, SIGNAL(activated()), qApp, SLOT(closeAllWindows()));
    //User
    QAction *actUser = new QAction(own->I18N("&User"),CTRL+Key_U,this);
    actUser->setIconSet(QPixmap(QImage(identity_xpm)));
    actUser->setToolTip(own->I18N("Select user"));
    actUser->setWhatsThis(own->I18N("Select OpenSCADA user"));
    connect(actUser, SIGNAL(activated()), this, SLOT(sel_user()));        
    //New button
    QAction *actNew = new QAction(own->I18N("&New"),CTRL+Key_N,this);
    actNew->setIconSet(QPixmap(QImage(window_new_xpm)));
    actNew->setToolTip(own->I18N("New window"));
    actNew->setWhatsThis(own->I18N("Open new window"));
    connect(actNew, SIGNAL(activated()), this, SLOT(new_w()));    
    //Up button
    actUp = new QAction(own->I18N("&Up"),ALT+Key_Up,this);
    actUp->setIconSet(QPixmap(QImage(up_xpm)));
    actUp->setToolTip(own->I18N("Up page"));
    actUp->setWhatsThis(own->I18N("Go to level up"));
    actUp->setEnabled(false);
    connect(actUp, SIGNAL(activated()), this, SLOT(up_page()));    
    //Previos page
    actPrev = new QAction(own->I18N("&Previos"),ALT+Key_Left,this);
    actPrev->setIconSet(QPixmap(QImage(back_xpm)));
    actPrev->setToolTip(own->I18N("Previos page"));
    actPrev->setWhatsThis(own->I18N("Go to previos page"));
    actPrev->setEnabled(false);
    connect(actPrev, SIGNAL(activated()), this, SLOT(prew_page()));    
    //Previos page
    actNext = new QAction(own->I18N("&Next"),ALT+Key_Right,this);
    actNext->setIconSet(QPixmap(QImage(forward_xpm)));
    actNext->setToolTip(own->I18N("Next page"));
    actNext->setWhatsThis(own->I18N("Go to next page"));
    actNext->setEnabled(false);
    connect(actNext, SIGNAL(activated()), this, SLOT(next_page()));    
    
    //Create menu "file"
    QPopupMenu *mn_file = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&File"), mn_file );
    mn_file->insertSeparator();
    actNew->addTo(mn_file);
    mn_file->insertSeparator();
    actQuit->addTo(mn_file);
    //Create menu "go"
    QPopupMenu *mn_go = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&Go"), mn_go );    
    actUp->addTo(mn_go);
    actPrev->addTo(mn_go);
    actNext->addTo(mn_go);
    //Create menu "User"
    QPopupMenu *mn_user = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&User"), mn_user );    
    actUser->addTo(mn_user);
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
    actUser->addTo(toolBar);
    
    //Display curent page
    pageDisplay( getItemPath( root_l_it ) );
}

ConfApp::~ConfApp()
{
    for( int i_cnt = 0; i_cnt < childs.size(); i_cnt++ )
	if(childs[i_cnt] != NULL ) childs[i_cnt]->close();
    if( m_parent != NULL ) m_parent->childClose(this);
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
    
    try
    {
	pageDisplay( sel_path.substr(0,i_l) );	
    } catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::prew_page()
{
    if( !prev.size() ) return;
    next.insert(next.begin(),sel_path);
    string path = prev[0];
    prev.erase(prev.begin());
    
    try
    {
	pageDisplay( path );	
    } catch(TError err) { postMess(err.what(),4); }   
}

void ConfApp::next_page()
{
    if( !next.size() ) return;
    prev.insert(prev.begin(),sel_path);
    string path = next[0];
    next.erase(next.begin()); 
    
    try
    {
	pageDisplay( path );	
    } catch(TError err) { postMess(err.what(),4); }       
}

void ConfApp::sel_user()
{
    postMess("Select user");
}

void ConfApp::childClose( ConfApp *child )
{
    for( int i_ch = 0; i_ch < childs.size(); i_ch++)
	if( childs[i_ch] == child ) childs[i_ch]=NULL;
}

void ConfApp::about()
{
    QMessageBox::about( this,caption(), "OpenSCADA Qt based system configurator.");
}

void ConfApp::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Application.");
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
    if( a_path == "/" ) titleLab->setText(string("<p align='center'><i>")+node.get_text()+"</b></p>");

    //Read node tree and create widgets
    for( unsigned i_cf = 0; i_cf < node.get_child_count(); i_cf++)
    {	
	XMLNode &t_s = *node.get_child(i_cf);

	//Check access to node
	bool wr = false;	
	if( !chkAccess(t_s, user, SEQ_RD) ) continue;
        if( chkAccess(t_s, user, SEQ_WR) ) wr = true;
	
	//View areas
	if( t_s.get_name() == "area" )
	{
	    QWidget *wdg = NULL;
	    //Check root page
	    if(a_path == "/")
	    {
		//Search avoid tab
		int i_t;
		for( i_t = 0; i_t < tabs->count(); i_t++ )
		    if( tabs->tabLabel(tabs->page(i_t)) == t_s.get_attr("dscr") ) break;		    
		if( i_t < tabs->count() )
		{			
		    //remove free page
		    while( i_t > i_area ) { tabs->removePage(tabs->page(i_area)); i_t--; }
		    wdg = ((QScrollView *)tabs->page(i_area++))->viewport()->childAt(0,0);
		}
		if( wdg == NULL )
		{
		    //Create new tab
		    QScrollView *scrl = new QScrollView(tabs);
		    scrl->setResizePolicy( QScrollView::AutoOneFit );
		    wdg = new QFrame( scrl );		
		    scrl->addChild(wdg,0,0);
		    tabs->insertTab( scrl, t_s.get_attr("dscr"), i_area++ );
		
		    QVBoxLayout *wdg_lay = new QVBoxLayout( wdg, 6, 4);
		    wdg->layout()->setAlignment( Qt::AlignTop );
		}
	    }
	    else
	    {
		wdg = new QGroupBox( t_s.get_attr("dscr"), widget );
		((QGroupBox *)wdg)->setColumnLayout(0, Qt::Vertical );
	       	wdg->layout()->setMargin( 6 );
       		wdg->layout()->setSpacing( 4 );
		wdg->layout()->setAlignment( Qt::AlignTop );		     

		widget->layout()->add(wdg);
	    }
	    selectChildRecArea(path,t_s,a_path+t_s.get_attr("id")+'/',root,wdg);
	}	
	//View list elements
	else if( t_s.get_name() == "list" && t_s.get_attr("hide") != "1" )
	{
            string br_path = SYS->pathCode(a_path+t_s.get_attr("id"),false);
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    QListBox *lstbox = new QListBox( widget, br_path.c_str() );
	    lstbox->setMinimumSize( QSize( 150, 110 ) );
	    connect( lstbox, SIGNAL( rightButtonPressed(QListBoxItem*,const QPoint&) ),
		this, SLOT( listBoxPopup( QListBoxItem* ) ) );
	    //Go branche signal connect
	    if( t_s.get_attr("tp") == "br" )
		connect( lstbox, SIGNAL( doubleClicked(QListBoxItem*) ),
		    this, SLOT( listBoxGo(QListBoxItem*) ) );
	    	    
	    lstbox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 0 ) );
	    for( unsigned i_el = 0, c_el = 0; i_el < t_s.get_child_count(); i_el++ )
		if( t_s.get_child(i_el)->get_name() == "el")
		    lstbox->insertItem( t_s.get_child(i_el)->get_text(), c_el++ );
	    
	    QVBoxLayout *vbox = new QVBoxLayout( 0, 0, 6 );	    
	    vbox->addWidget( new QLabel(t_s.get_attr("dscr")+":",widget) );
	    QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );
	    hbox->addWidget( lstbox );
	    hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	    vbox->addLayout(hbox);
	    widget->layout()->addItem(vbox);	    
	}
	//View standart fields
	else if( t_s.get_name() == "fld" )
	{
            string br_path = SYS->pathCode(a_path+SYS->pathCode(t_s.get_attr("id"),true),false);
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    //View select fields
	    if( t_s.get_attr("dest") == "select" && wr )
	    {			
		QComboBox *comb = new QComboBox( FALSE, widget, br_path.c_str() );
		connect( comb, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
				
		XMLNode *x_lst = SYS->ctr_id(&(XMLNode &)root,t_s.get_attr("select"));  //????
                string br_path = SYS->pathCode( t_s.get_attr("select"), false );
		ctrCmd(path+"/"+br_path, *x_lst, CTR_GET);
		if( x_lst->get_name() == "list" )
		    for( unsigned i_el = 0, c_el = 0; i_el < x_lst->get_child_count(); i_el++ )
    			if( x_lst->get_child(i_el)->get_name() == "el")
    			{
                            comb->insertItem( x_lst->get_child(i_el)->get_text(), c_el++ );
    			    if( x_lst->get_child(i_el)->get_text() == t_s.get_text() )
				comb->setCurrentItem( c_el-1 );
			}
		
		if(t_s.get_attr("dscr").size()) 
		{
		    l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		    l_hbox->insertWidget( l_pos++, new QLabel(t_s.get_attr("dscr")+":",widget) );
		    l_hbox->insertWidget( l_pos++, comb );
		    l_hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
		    widget->layout()->addItem( l_hbox );
		}
		else
		{
		    if( l_hbox ) l_hbox->insertWidget( l_pos++, comb );
		    else delete comb;
		}		
    	    }	    
	    else
	    {		    
		//View boolean fields
    		if( t_s.get_attr("tp") == "bool" )
    		{
    		    QCheckBox *chBox  = new QCheckBox(t_s.get_attr("dscr"),widget,br_path.c_str());
    		    if( t_s.get_text() == "true" )	chBox->setChecked(true);
    		    else				chBox->setChecked(false);
    		    widget->layout()->add(chBox);
		    connect( chBox, SIGNAL( stateChanged(int) ), this, SLOT( checkBoxStChange(int) ) );
    		}
		//View edit fields
                else if( t_s.get_attr("tp") == "str" && (t_s.get_attr("rows").size() || t_s.get_attr("cols").size()) )
		{
		    QVBoxLayout *layoutEdit = new QVBoxLayout( 0, 0, 6);		    
                    layoutEdit->addWidget( new QLabel(t_s.get_attr("dscr")+":", widget ) );		    
		    QTextEdit *edit = new QTextEdit(t_s.get_text(), "TEST!!!!", widget );
		    edit->setWordWrap(QTextEdit::NoWrap);
		    if( !wr ) edit->setReadOnly( true );
                    layoutEdit->addWidget( edit );
		    widget->layout()->addItem(layoutEdit);
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
			((QLabel *)val_w)->setText(string("<b>")+t_s.get_text()+"</b>");
		    }
		    //View edit
		    else
		    {
			val_w = new QLineEdit( t_s.get_text(), widget, br_path.c_str() );
			val_w->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed, 1, 0 ) );
			((QLineEdit *)val_w)->setCursorPosition(0);
			connect( val_w, SIGNAL( returnPressed() ), this, SLOT( editReturnPress( ) ) );
			// addon parameters			
			int val_n = atoi(t_s.get_attr("len").c_str());
			if( val_n > 0 ) 
			{
			    ((QLineEdit *)val_w)->setMaxLength( val_n );
			    ((QLineEdit *)val_w)->setFixedWidth( (val_n>40)?15*40:15*val_n );
			}
			else
			{
			    if( t_s.get_attr("tp") == "dec" || t_s.get_attr("tp") == "hex" ||
				t_s.get_attr("tp") == "oct" || t_s.get_attr("tp") == "real" )
			    	((QLineEdit *)val_w)->setFixedWidth( 5*15 );
			    else ((QLineEdit *)val_w)->setMinimumWidth( 7*15 );
			}
		    }
		    //Check use label
		    if(t_s.get_attr("dscr").size()) 
		    {
			l_hbox = new QHBoxLayout( 0, 0, 6 ); l_pos = 0;
		    	l_hbox->insertWidget( l_pos++, new QLabel(t_s.get_attr("dscr")+":",widget) );
			l_hbox->insertWidget( l_pos++, val_w );			
			l_hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
			widget->layout()->addItem(l_hbox);
		    }
		    else
		    {
			if( l_hbox ) l_hbox->insertWidget( l_pos++, val_w );
			else delete val_w;		    
		    }		    			
    		}
	    }
	}
	else if( t_s.get_name() == "comm" )
	{
            string br_path = SYS->pathCode(a_path+SYS->pathCode(t_s.get_attr("id"),true), false);
	
	    QPushButton *button = new QPushButton( t_s.get_attr("dscr"), widget, br_path.c_str() );
	    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	    
	    QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );	    
	    hbox->addWidget( button );
	    hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	    widget->layout()->addItem(hbox);
	}
    }
    //Clear delete widgets into tail
    if(a_path == "/")
    {
	while( i_area > tabs->count() ) tabs->removePage(tabs->page(i_area));    
	tabs->showPage(tabs->currentPage());
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
    p_active = false;
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
    	while(tabs->page(0))
	{
	    QWidget *tab = tabs->page(0);
	    tabs->removePage(tab);
	    delete tab;
	}
	sel_path = path;	
	ctrCmd(sel_path,node,CTR_INFO);
    }
    selectChildRecArea(sel_path,node,"/",node);
    p_active = true;
}

void ConfApp::onItem( QListViewItem * i )
{
    statusBar()->message(getItemPath( i ), 10000 );
}

void ConfApp::tabSelect( const QString &wdg )
{
    //if(p_active) pageDisplay( sel_path );
}

int ConfApp::viewChildRecArea( const string &path, const XMLNode &node, const string &a_path, QListViewItem * i, int level, int grp )
{           
    //Calc number group into node
    if(a_path == "/" && level >= 0 ) 
    	grp = viewChildRecArea( path, node, a_path, i, -1, 0 );

    //Curent node 
    for( unsigned i_cf = 0; i_cf < node.get_child_count(); i_cf++)
    {    
	XMLNode &t_s = *node.get_child(i_cf);
	if( t_s.get_name() == "area" && chkAccess(t_s, user, SEQ_RD) )
	{
    	    string area_path = a_path+t_s.get_attr("id")+'/';
    	    for( unsigned i_el = 0; i_el < t_s.get_child_count(); i_el++)
	    {
		XMLNode &t_c = *t_s.get_child(i_el);
		if( !chkAccess(t_c, user, SEQ_RD) ) continue;
		if( t_c.get_name() == "list" && t_c.get_attr("hide") != "1" && t_c.get_attr("tp") == "br" )
    		{		 
		    //Calc Child groups
                    if( level < 0 ) { grp++; continue; }
		    
		    QListViewItem *it = i;
		    string br_path = SYS->pathCode( area_path+t_c.get_attr("id"), false );
		    
		    if( it->text(2)[0] == '*' )
		    {
		        if( br_path != it->text(2).remove(0,1) ) continue;
		    }
		    else if( grp > 1 )
		    {
			it = new QListViewItem(i,t_c.get_attr("dscr"),t_c.get_attr("dscr"),string("*")+br_path);
			i->insertItem(it);
    		    }
		    
		    ctrCmd(path+"/"+br_path, t_c, CTR_GET);
		    //Process group
		    for( unsigned i_lel = 0; i_lel < t_c.get_child_count(); i_lel++)
		    {
			XMLNode &t_cl = *t_c.get_child(i_lel);
			if( t_cl.get_name() == "el" )
			{   
			    if( t_c.get_attr("mode") == "att" )
				br_path = SYS->pathCode(area_path+t_c.get_attr("id")+'/'+SYS->pathCode(t_cl.get_text(),true), false);
	    		    else
				br_path = SYS->pathCode(area_path+t_c.get_attr("id")+'/'+t_cl.get_attr("id"), false);

			    QListViewItem *ch_it = new QListViewItem(it,t_cl.get_text(),t_c.get_attr("dscr"),br_path);
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
	if( SYS->ctr_path_l(path,level++,false) == SYS->Station() ) cntr = SYS;
    	else throw TError("Station error!");
    }
    
    //Go to branch
    string s_br = SYS->pathEncode( cntr->ctr_path_l(path,level,false), false );
    if( (cmd == 0 && s_br.size()) || (cmd > 0 && cntr->ctr_path_l(path,level+1,false).size()) )
    {                  
	try{ ctrCmd( path,node,cmd,&cntr->ctr_at1(s_br).at(),++level ); }
	catch(TError err)
	{ 
	    //printf("TEST 00: <%s> for <%d>\n",err.what().c_str(),level+1);
	    ctrCmd( path,node,cmd,&cntr->ctr_at(s_br),++level ); 
	}
	return;
    }
    if(cmd == CTR_INFO)		cntr->ctr_info(node);
    else if(cmd == CTR_GET)	cntr->ctr_din_get(s_br,&node);
    else if(cmd == CTR_SET)	cntr->ctr_din_set(s_br,&node);
    else if(cmd == CTR_CMD)	cntr->ctr_cmd_go(s_br,&node,&node);
}                          

void ConfApp::postMess( const string &mess, int type )
{
    //Put system message.
    own->m_put_s("SYS",(type==4)?MESS_CRIT:(type==3)?MESS_ERR:(type==2)?MESS_WARNING:MESS_INFO,mess);
    //QT message
    if( type == 1 )	QMessageBox::information( this,caption(), mess);
    else if( type == 2 )QMessageBox::warning( this,caption(), mess);
    else if( type == 3 )QMessageBox::critical( this,caption(), mess);
    else if( type == 4 )QErrorMessage::qtHandler()->message(mess);
}

bool ConfApp::chkAccess( const XMLNode &fld, string user, char mode )
{
    string s_acc = fld.get_attr("acs");
    string s_usr = fld.get_attr("own");
    string s_grp = fld.get_attr("grp");
    if( !s_acc.size() ) s_acc = "0777";
    int accs = strtol(s_acc.c_str(),NULL,8);
    if( !s_usr.size() ) s_usr = "0";        //root
    int usr = atoi(s_usr.c_str());
    if( !s_grp.size() ) s_grp = "0";        //root
    int grp = atoi(s_grp.c_str());

    return( own->Owner().Owner().Sequrity().access( user, mode, usr, grp, accs) );
}


//-------------------------------------------------------------
//-------------- Self widget's slots --------------------------
//-------------------------------------------------------------
void ConfApp::checkBoxStChange( int stat )
{
    QCheckBox *box = (QCheckBox *)sender();
    
    if(stat==QButton::NoChange) return;
    try
    {    
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(box->name(),false) );    
	ctrCmd(sel_path+"/"+box->name(), *n_el, CTR_GET);

	string val = (stat==QButton::On)?"true":"false";
	if( n_el->get_text() == val ) return;
 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Set <%s> to <%s>!", 
		user.c_str(), (sel_path+"/"+box->name()).c_str(), val.c_str() );
	n_el->set_text(val);
	ctrCmd(sel_path+"/"+box->name(), *n_el, CTR_SET);	
	//Control request
	//ctrCmd(sel_path+"/"+box->name(), *n_el, CTR_GET);
	
    }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::buttonClicked( )
{	
    QButton *button = (QButton *)sender();
    
    try
    {    
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(button->name(),false) );    

 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Press <%s>!", 
		user.c_str(), (sel_path+"/"+button->name()).c_str() );
	ctrCmd(sel_path+"/"+button->name(), *n_el, CTR_CMD);	
	
    }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::editReturnPress( )
{
    QLineEdit *edit = (QLineEdit *)sender();
    
    try
    {    
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(edit->name(),false) );    
	ctrCmd(sel_path+"/"+edit->name(), *n_el, CTR_GET);
	
	if( n_el->get_text() == edit->text() ) return;
 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Change <%s> from <%s> to <%s>!", 
		user.c_str(), (sel_path+"/"+edit->name()).c_str(), n_el->get_text().c_str(), edit->text().ascii() );
	n_el->set_text(edit->text());
	ctrCmd(sel_path+"/"+edit->name(), *n_el, CTR_SET);	
	//Control request
    }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::combBoxActivate( const QString& val  )
{
    QComboBox *comb = (QComboBox *)sender();
    
    try
    {    
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(comb->name(),false) );    
	ctrCmd(sel_path+"/"+comb->name(), *n_el, CTR_GET);
	
	if( n_el->get_text() == val ) return;
 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Change <%s> from <%s> to <%s>!", 
		user.c_str(), (sel_path+"/"+comb->name()).c_str(), n_el->get_text().c_str(), val.ascii() );
	n_el->set_text(val);
	ctrCmd(sel_path+"/"+comb->name(), *n_el, CTR_SET);	
	//Control request
    }catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::listBoxPopup(QListBoxItem* item)
{
    QPopupMenu popup;
    QListBox *lbox = (QListBox *)sender();
    string el_path = sel_path+"/"+lbox->name();

    try
    {    
	bool p_ed = false;
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(lbox->name(),false) );
	if( !chkAccess(*n_el, user, SEQ_WR) || !n_el->get_attr("s_com").size() ) return;
	
	if( n_el->get_attr("s_com").find("add") != string::npos )
	{ 
	    popup.insertItem(own->I18N("Add"),1); 
	    p_ed = true; 
	}
	if( n_el->get_attr("s_com").find("ins") != string::npos && item != NULL )
	{ 
	    popup.insertItem(own->I18N("Insert"),2); 
	    p_ed = true; 
	}
	if( n_el->get_attr("s_com").find("edit") != string::npos && item != NULL )
	{ 
	    popup.insertItem(own->I18N("Edit"),3); 
	    p_ed = true; 
	}
	if( n_el->get_attr("s_com").find("del") != string::npos && item != NULL )
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
	    
	    XMLNode *n_el1 = n_el->add_child("el");
	    switch(rez)		
	    {
		case 1:
		    n_el1->set_attr("do","add",true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Add <%s> element <%s>!",
			user.c_str(), el_path.c_str(), text.ascii() );
		    n_el1->set_text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    lbox->insertItem(text);	//Add list item		    
		    break;
		case 2:
		    n_el1->set_attr("do","ins",true);
		    n_el->set_attr("id",SYS->int2str(lbox->currentItem()),true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Insert <%s> element <%s>!",
			user.c_str(), el_path.c_str(), text.ascii() );
		    n_el1->set_text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    lbox->insertItem(text,lbox->currentItem());	//insert list item
		    break;	    
		case 3:
		    n_el1->set_attr("do","edit",true);
		    n_el->set_attr("id",SYS->int2str(lbox->currentItem()),true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Rename <%s> element <%s> to <%s>!",
			user.c_str(), el_path.c_str(), item->text().ascii(), text.ascii() );
		    n_el1->set_text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    lbox->changeItem(text,lbox->currentItem());	//Change list item text
		    break;	    		    
		case 4:
		    text = item->text();
		    n_el1->set_attr("do","del",true);
		    Mess->put("QT_CONTROL",MESS_INFO,"%s| Delete <%s> element <%s>!",
			user.c_str(), el_path.c_str(), text.ascii() );
		    n_el1->set_text(text);
		    ctrCmd(el_path, *n_el, CTR_SET);
		    lbox->removeItem(lbox->currentItem());	//Remove list item
		    break;	    		    
	    }
	}
    }catch(TError err) { postMess(err.what(),4); }
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
	if(SYS->ctr_id(&(XMLNode &)node, SYS->pathEncode(lbox->name(),false) )->get_attr("mode") == "att")    
	    path = sel_path+"/"+lbox->name()+":"+item->text().ascii();
	else
	    path = sel_path+"/"+lbox->name()+":"+SYS->int2str(lbox->currentItem());    
	    
	pageDisplay( path );
    }
    catch(TError err) { postMess(err.what(),4); }
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

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
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qlistview.h>
#include <qtoolbox.h>
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

#include <tmessage.h>
#include <tsys.h>
#include <tsequrity.h>
#include "tuimod.h"
#include "qtcfg.h"

using namespace QTCFG;

//==============================================================================
//================= QTCFG::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp( TUIMod *owner ) : QMainWindow( 0, "", WDestructiveClose ), own(owner), user("root")
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
    CtrTree->addColumn(own->I18N("Description"));    
    CtrTree->addColumn("path",0);
    CtrTree->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding, 2, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );		    
    CtrTree->setMinimumSize( QSize( 150, 0 ) );
    CtrTree->setMaximumSize( QSize( 400, 32767 ) );
    CtrTree->setResizeMode( QListView::NoColumn );

    //Right frame add
    QFrame *gFrame = new QFrame( splitter, "GFrame" );
    gFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, 3, 0, CtrTree->sizePolicy().hasHeightForWidth() ) );		    
    gFrame->setFrameShape( QFrame::StyledPanel );
    gFrame->setFrameShadow( QFrame::Raised );	    
    QGridLayout *gFrameLayout = new QGridLayout( gFrame, 1, 1, 4, -1 );
    
    titleLab = new QLabel( gFrame );
    QFont titleLab_font( titleLab->font() );
    titleLab_font.setPointSize( 14 );
    titleLab->setFont( titleLab_font );
    gFrameLayout->addWidget( titleLab, 0, 0 );

    tabs = new QTabWidget( gFrame );
    gFrameLayout->addWidget( tabs, 1, 0 );

    QTCfgLayout->addWidget( splitter, 0, 0 );
    
    //Create Popup menu
    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&File"), file );
    file->insertSeparator();
    file->insertItem(own->I18N("&Quit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
    
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem(own->I18N("&Help"), help );
    help->insertItem(own->I18N("&About"), this, SLOT(about()), Key_F1 );
    help->insertItem(own->I18N("About &Qt"), this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem(own->I18N("What's &This"), this, SLOT(whatsThis()), SHIFT+Key_F1 );

    QListViewItem *root_l_it = new QListViewItem(CtrTree,SYS->Station(),"Local station",SYS->Station());
    CtrTree->insertItem( root_l_it );
    viewChild( root_l_it );
    selectItem( root_l_it );    
	    
    statusBar()->message("Ready", 2000 );
    resize( 800, 600 );

    //Connect signals
    connect( CtrTree, SIGNAL( currentChanged(QListViewItem*) ), this, SLOT( selectItem(QListViewItem*) ) );
    connect( CtrTree, SIGNAL( expanded(QListViewItem*) ), this, SLOT( viewChild(QListViewItem*) ) );
}

ConfApp::~ConfApp()
{

}

void ConfApp::about()
{
    QMessageBox::about( this,"OpenSCADA", "OpenSCADA Qt based system configurator.");
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
	//Delete tabs
	while(tabs->page(0))
	{
	    QWidget *tab = tabs->page(0);
    	    tabs->removePage(tab);
	    delete tab;
	}
    
	//XMLNode	node;
	sel_path = getItemPath( i );    
	ctrCmd(sel_path,node,CTR_INFO);    
	selectChildRecArea(sel_path,node,"/",node);
	if(tabs->page(0)) tabs->showPage(tabs->page(tabs->currentPageIndex()));  //Show curent page
    }
    catch(TError err) { postMess(err.what(),4); }
}

void ConfApp::selectChildRecArea( const string &path, const XMLNode &node, const string &a_path, const XMLNode &root, QWidget *widget )
{
    QVBoxLayout *layout;
    
    //For abbend no named widgets
    QHBoxLayout *l_hbox = NULL;
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
	    QWidget *wdg;
	    if(a_path == "/")
	    {
		wdg = new QWidget( tabs );
		QVBoxLayout *wdg_lay = new QVBoxLayout( wdg, 6, 4);
		wdg->layout()->setAlignment( Qt::AlignTop );
		
                tabs->insertTab( wdg, t_s.get_attr("dscr") );
	    }else
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
            string br_path = xpathCode(a_path+t_s.get_attr("id"));
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    QListBox *lstbox = new QListBox(widget);
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
            string br_path = xpathCode(a_path+t_s.get_attr("id"));
            ctrCmd(path+"/"+br_path, t_s, CTR_GET);
	    
	    //View select fields
	    if( t_s.get_attr("dest") == "select" && wr )
	    {			
		QComboBox *comb = new QComboBox( FALSE, widget, br_path.c_str() );
		connect( comb, SIGNAL( activated(const QString&) ), this, SLOT( combBoxActivate( const QString& ) ) );
				
		XMLNode *x_lst = SYS->ctr_id(&(XMLNode &)root,t_s.get_attr("select"));  //????
                string br_path = xpathCode( t_s.get_attr("select") );
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
            string br_path = xpathCode(a_path+t_s.get_attr("id"));
	
	    QPushButton *button = new QPushButton( t_s.get_attr("dscr"), widget, br_path.c_str() );
	    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
	    
	    QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6 );	    
	    hbox->addWidget( button );
	    hbox->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
	    widget->layout()->addItem(hbox);
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

void ConfApp::viewChildRecArea( const string &path, const XMLNode &node, const string &a_path, QListViewItem * i, int level )
{       
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
		    level--;
		    string br_path = xpathCode( area_path+t_c.get_attr("id") );
		    ctrCmd(path+"/"+br_path, t_c, CTR_GET);
		    for( unsigned i_lel = 0; i_lel < t_c.get_child_count(); i_lel++)
		    {
			XMLNode &t_cl = *t_c.get_child(i_lel);
			if( t_cl.get_name() == "el" )
			{   
			    if( t_c.get_attr("mode") == "att" ) 
				br_path = xpathCode(area_path+t_c.get_attr("id")+'/'+t_cl.get_text());
	    		    else 	                   	
				br_path = xpathCode(area_path+t_c.get_attr("id")+'/'+t_cl.get_attr("id"));
			    
			    QListViewItem *ch_it = new QListViewItem(i,t_cl.get_text(),t_c.get_attr("dscr"),br_path);
    			    i->insertItem(ch_it);
			    
			    if( level > 0 ) 
			    {				
				XMLNode n_lev;
				ctrCmd(path+"/"+br_path,n_lev,CTR_INFO);
				viewChildRecArea(path+"/"+br_path,n_lev,"/",ch_it,level);  
			    }
			}
		    }
		}
	    }
	    viewChildRecArea( path, t_s, area_path, i, level );
	}
    }
}

string ConfApp::getItemPath( QListViewItem * i )
{
    QListViewItem *it_next = i;
    string p_path = "";    
    do
    {
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
	if( SYS->ctr_path_l(path,level++) == SYS->Station() ) cntr = SYS;
    	else throw TError("Station error!");
    }
    
    //Go to branch
    string s_br = xpathEncode( cntr->ctr_path_l(path,level) );
    if( (cmd == 0 && s_br.size()) || (cmd > 0 && cntr->ctr_path_l(path,level+1).size()) )
    {
	try{ ctrCmd( path,node,cmd,&cntr->ctr_at1(s_br).at(),++level ); }
	catch(TError err){ ctrCmd( path,node,cmd,&cntr->ctr_at(s_br),++level ); }	
	return;
    }
    if(cmd == CTR_INFO)		cntr->ctr_info(node);
    else if(cmd == CTR_GET)	cntr->ctr_din_get(s_br,&node);
    else if(cmd == CTR_SET)	cntr->ctr_din_set(s_br,&node);
    else if(cmd == CTR_CMD)	cntr->ctr_cmd_go(s_br,&node,&node);
}                          

string ConfApp::xpathCode( const string &in )
{
    string path = in;
    for( unsigned i_sz = 0; i_sz < path.size(); i_sz++ )
    	if( path[i_sz] == '/' ) path[i_sz] = ':';
    
    return path;
}

string ConfApp::xpathEncode( const string &in )
{
    string path = in;
    for( unsigned i_sz = 0; i_sz < path.size(); i_sz++ )
    	if( path[i_sz] == ':' ) path[i_sz] = '/';
    
    return path;
}

void ConfApp::postMess( const string &mess, int type )
{
    //Put system message.
    own->m_put_s("SYS",(type==4)?MESS_CRIT:(type==3)?MESS_ERR:(type==2)?MESS_WARNING:MESS_INFO,mess);
    //QT message
    if( type == 1 )	QMessageBox::information( this,"OpenSCADA", mess);
    else if( type == 2 )QMessageBox::warning( this,"OpenSCADA", mess);
    else if( type == 3 )QMessageBox::critical( this,"OpenSCADA", mess);
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

//Self widget's slots
void ConfApp::checkBoxStChange( int stat )
{
    QCheckBox *box = (QCheckBox *)sender();
    
    if(stat==QButton::NoChange) return;
    try
    {    
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, xpathEncode(box->name()) );    
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
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, xpathEncode(button->name()) );    

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
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, xpathEncode(edit->name()) );    
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
	XMLNode *n_el = SYS->ctr_id(&(XMLNode &)node, xpathEncode(comb->name()) );    
	ctrCmd(sel_path+"/"+comb->name(), *n_el, CTR_GET);
	
	if( n_el->get_text() == val ) return;
 	Mess->put("QT_CONTROL",MESS_INFO,"%s| Change <%s> from <%s> to <%s>!", 
		user.c_str(), (sel_path+"/"+comb->name()).c_str(), n_el->get_text().c_str(), val.ascii() );
	n_el->set_text(val);
	ctrCmd(sel_path+"/"+comb->name(), *n_el, CTR_SET);	
	//Control request
    }catch(TError err) { postMess(err.what(),4); }
}
#include <qtcfg.moc>

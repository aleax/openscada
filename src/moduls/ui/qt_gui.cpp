/* Test Modul
** ==============================================================
*/

#include <getopt.h>
#include <string>
#include <unistd.h>
#include <signal.h>

#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qapplication.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "qt_gui.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "QT_GUI"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "QT based OpenSCADA GUI: Configurator, ARM."
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    SAtMod module( int n_mod )
    {
    	SAtMod AtMod;

	if(n_mod==0)
	{
	    AtMod.name  = NAME_MODUL;
	    AtMod.type  = NAME_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.name  = "";

	return( AtMod );
    }

    TModule *attach( SAtMod &AtMod, string source )
    {
	QT_GUI::TUIMod *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new QT_GUI::TUIMod( source );       

	return ( self_addr );
    }    
    /*
    TModule *attach( char *FName, int n_mod )
    {
	QT_GUI::TUIMod *self_addr;
	if(n_mod==0) self_addr = new QT_GUI::TUIMod( FName );
	else         self_addr = NULL;
	return ( self_addr );
    }
    */
}

using namespace QT_GUI;

//==============================================================================
//================= QT_GUI::TUIMod =============================================
//==============================================================================

TUIMod::TUIMod( string name ) : run_st(false)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    Source    = name;
}

TUIMod::~TUIMod()
{
    if( run_st )
    {
	SYS->event_wait( run_st, false, string(NAME_MODUL)+": The GUI is closing... . Manualy close please!");
	pthread_join(pthr_tsk,NULL);
    }
}

string TUIMod::mod_info( const string name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TUIMod::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

void TUIMod::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "\n",NAME_MODUL);
}

void TUIMod::mod_CheckCommandLine( )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TUIMod::mod_connect(  )
{
    TModule::mod_connect(  );
}

void TUIMod::Start()
{
    if( run_st ) return;
    pthread_attr_t pthr_attr;
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( run_st, true, string(NAME_MODUL)+": The configurator is starting....",5) )
       	throw TError("%s: The configurator no started!",NAME_MODUL);   
}

void TUIMod::Stop()
{
    if( run_st)
    {
	if( SYS->event_wait( run_st, false, string(NAME_MODUL)+": The configurator is stoping....",5) )
	    throw TError("%s: The configurator no stoped!",NAME_MODUL);   
	pthread_join(pthr_tsk,NULL);
    }	
}

void *TUIMod::Task( void *CfgM )
{
    //struct sigaction sa;
    //memset (&sa, 0, sizeof(sa));
    //sa.sa_handler= SYS->sighandler;
    //sigaction(SIGALRM,&sa,NULL);

    TUIMod *Cfg = (TUIMod *)CfgM;

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Thread <%d>!",NAME_MODUL,getpid() );
#endif
    
    
    Cfg->run_st = true;

    QApplication app(0, NULL);
    AWPApp *mw = new AWPApp();
    mw->setCaption( "OpenSCADA automatic work place." );
    mw->show();
    app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );

    //NewWidget NWidg;
    //NWidg.setGeometry( 100, 100, 200, 120 );
    //app1.setMainWidget( &NWidg );
    //NWidg.setCaption("QT OpenSCADA UI manager!");
    //NWidg.show();
    
    app.exec();
    Cfg->run_st = false;
    
    return(NULL);
}

//==============================================================================
//================= QT_GUI::NewWidget ==========================================
//==============================================================================
/*
NewWidget::NewWidget( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    //Vertical box
    QBoxLayout *topLayout = new QVBoxLayout( this, 5 );
    
    // make menu
    QMenuBar *menubar = new QMenuBar( this );
    menubar->setSeparator( QMenuBar::InWindowsStyle );
    QPopupMenu* popup;
    popup = new QPopupMenu( this );
    popup->insertItem( "&Quit", qApp, SLOT(quit()) );
    menubar->insertItem( "&File", popup );
    // assign menuBar
    topLayout->setMenuBar( menubar );
 
    // Horisontal box
    QBoxLayout *buttons1 = new QHBoxLayout( topLayout );
    for ( int i = 1; i <= 4; i++ ) 
    {
	// New button
    	QPushButton* but = new QPushButton( this );
	QString s;
	s.sprintf( "Button %d", i );
	but->setText( s );
	buttons1->addWidget( but, 10 );
    }

    QBoxLayout *buttons2 = new QHBoxLayout( topLayout );
    QPushButton* but = new QPushButton( "Button five", this );
    buttons2->addWidget( but );
    but = new QPushButton( "Button 6", this );
    buttons2->addWidget( but );
    buttons2->addStretch( 10 );

    QMultiLineEdit *bigWidget = new QMultiLineEdit( this );
    bigWidget->setText( "This widget will get all the remaining space" );
    bigWidget->setFrameStyle( QFrame::Panel | QFrame::Plain );
    topLayout->addWidget( bigWidget );


    const int numRows = 3;
    const int labelCol = 0;
    const int linedCol = 1;
    const int multiCol = 2;

    QGridLayout *grid = new QGridLayout( topLayout, 0, 0, 10 );
    for( int row = 0; row < numRows; row++ ) 
    {
	QLineEdit *ed = new QLineEdit( this );
	grid->addWidget( ed, row, linedCol ); 
	
	QString s;
	s.sprintf( "Line &%d", row+1 );
	QLabel *label = new QLabel( ed, s, this );
	grid->addWidget( label, row, labelCol );
    }
 
    QMultiLineEdit *med = new QMultiLineEdit( this );
    grid->addMultiCellWidget( med, 0, -1, multiCol, multiCol );

    grid->setColStretch( linedCol, 10 );
    grid->setColStretch( multiCol, 20 );

    QLabel* sb = new QLabel( this );
    sb->setText( "Let's pretend this is a status bar" );
    sb->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    sb->setFixedHeight( sb->sizeHint().height() );

    sb->setAlignment( AlignVCenter | AlignLeft );
    topLayout->addWidget( sb );
	    
    topLayout->activate();    
}

NewWidget::~NewWidget(  )
{

}
*/
//==============================================================================
//================= QT_GUI::ConfApp ============================================
//==============================================================================
ConfApp::ConfApp() : QMainWindow( 0, "Configurator of OpenSCADA", WDestructiveClose )
{
    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    file->insertSeparator();
    int id;
    id = file->insertItem( "&Configurator", this, SLOT(conf()), CTRL+Key_C );
    //file->setWhatsThis( id, fileOpenText );
    id = file->insertItem( "&Work place",   this, SLOT(w_place()), CTRL+Key_W );
    //file->setWhatsThis( id, fileSaveText );	      
    file->insertSeparator();
    file->insertItem( "&Close", this, SLOT(close()), CTRL+Key_W );
    file->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
    
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, SLOT(about()), Key_F1 );
    help->insertItem( "About &Qt", this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1 );

    statusBar()->message( "Ready", 2000 );
    resize( 450, 600 );
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

void ConfApp::conf()
{
    ConfApp *cfg = new ConfApp;
    cfg->setCaption("OpenSCADA Configurator.");
    cfg->show();    
}

void ConfApp::w_place()
{
    AWPApp *awp = new AWPApp;
    awp->setCaption("OpenSCADA automatic work place.");
    awp->show();        
}

void ConfApp::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

//==============================================================================
//================= QT_GUI::AWPApp ============================================
//==============================================================================
AWPApp::AWPApp() : QMainWindow( 0, "Automatic work place of OpenSCADA", WDestructiveClose )
{
    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    file->insertSeparator();
    int id;
    id = file->insertItem( "&Configurator", this, SLOT(conf()), CTRL+Key_C );
    //file->setWhatsThis( id, fileOpenText );
    id = file->insertItem( "&Work place",   this, SLOT(w_place()), CTRL+Key_W );
    //file->setWhatsThis( id, fileSaveText );	      
    file->insertSeparator();
    file->insertItem( "&Close", this, SLOT(close()), CTRL+Key_W );
    file->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
    
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, SLOT(about()), Key_F1 );
    help->insertItem( "About &Qt", this, SLOT(aboutQt()) );
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1 );

    statusBar()->message( "Ready", 2000 );
    resize( 450, 600 );
}

AWPApp::~AWPApp()
{

}

void AWPApp::about()
{
    QMessageBox::about( this,"OpenSCADA", "OpenSCADA Qt based automatic work place.");
}

void AWPApp::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Application.");
}

void AWPApp::conf()
{
    ConfApp *cfg = new ConfApp;
    cfg->setCaption("OpenSCADA Configurator.");
    cfg->show();    
}

void AWPApp::w_place()
{
    AWPApp *awp = new AWPApp;
    awp->setCaption("OpenSCADA automatic work place.");
    awp->show();        
}

void AWPApp::closeEvent( QCloseEvent* ce )
{
    ce->accept();
}

#include <qt_gui.moc>

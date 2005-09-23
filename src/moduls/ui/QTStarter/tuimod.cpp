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
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>       

#include <qtextcodec.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qtoolbar.h> 
#include <qaction.h> 
#include <qimage.h> 
#include <qpushbutton.h> 
#include <qlayout.h> 
#include <qframe.h>

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

#include "xpm/oscada_qt.xpm"
#include "xpm/exit.xpm"

//============ Modul info! =====================================================
#define MOD_ID      "QTStarter"
#define MOD_NAME    "QT GUI starter"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow the QT GUI starter. Single for all QT GUI modules!"
#define LICENSE     "GPL"
//==============================================================================

QTStarter::TUIMod *QTStarter::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
    	TModule::SAt AtMod;

	if(n_mod==0)
	{
	    AtMod.id	= MOD_ID;
	    AtMod.type  = MOD_TYPE;
    	    AtMod.t_ver = VER_TYPE;
	}
	else
	    AtMod.id	= "";

	return( AtMod );
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	QTStarter::TUIMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = QTStarter::mod = new QTStarter::TUIMod( source );

	return ( self_addr );
    }    
}

using namespace QTStarter;

//==============================================================================
//================= QTStarter::TUIMod ==========================================
//==============================================================================

TUIMod::TUIMod( string name ) : end_run(false)
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    Vers      	= VERSION;
    Autors    	= AUTORS;
    DescrMod  	= DESCRIPTION;
    License   	= LICENSE;
    Source    	= name;
}

TUIMod::~TUIMod()
{
    if( run_st ) modStop();
}

void TUIMod::modLoad( )
{
    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
        {"help"    ,0,NULL,'h'},
        {NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
        next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
        switch(next_opt)
        {
            case 'h': fprintf(stdout,optDescr().c_str()); break;
            case -1 : break;
        }
    } while(next_opt != -1);
    
    //========== Load parameters from config file =============
    try { start_mod = ctrId(&SYS->cfgRoot(),nodePath())->childGet("id","startMod")->text(); }
    catch(...) {  }															        
}

void TUIMod::postEnable( )
{
    TModule::postEnable( );
    
    //Set QT environments    
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale () ); //codepage for QT across QString recode!
}

void TUIMod::modStart()
{
    if( run_st ) return;
    pthread_attr_t pthr_attr;
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
       	throw TError(nodePath().c_str(),"QT starter no started!");   
}

void TUIMod::modStop()
{
    if( run_st )
    {
	end_run = true;
	qApp->closeAllWindows();
	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),"QT starter no stoped!");
	pthread_join(pthr_tsk,NULL);
    }	
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),I18N(
        "======================= The module <%s:%s> options =======================\n"
        "---------- Parameters of the module section <%s> in config file ----------\n"
        "startMod = <moduls> Parameter test:\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
	
    return buf;
}
																													

void TUIMod::callQTModule( )
{
    QObject *obj = (QObject *)sender();
    if( string("*exit*") == obj->name() ) SYS->stop();
    else callQTModule(obj->name());
}

void TUIMod::callQTModule( const string &nm )
{
    vector<string> list;
    
    AutoHD<TModule> qt_mod = owner().modAt(nm);
    QMainWindow *(TModule::*openWindow)( );
    qt_mod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
    QMainWindow *new_wnd = ((&qt_mod.at())->*openWindow)( );

    //Make QT starter toolbar
    QToolBar *toolBar = new QToolBar(I18N("QTStarter toolbar"), new_wnd, DockTop );    
    owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
        if( owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
            owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
	AutoHD<TModule> qt_mod = owner().modAt(list[i_l]);
	
	QPixmap icon;
	if( owner().modAt(list[i_l]).at().modFuncPresent("QPixmap TUIMod::icon();") )
	{
	    QPixmap (TModule::*getIcon)();
	    owner().modAt(list[i_l]).at().modFunc("QPixmap TUIMod::icon();",(void (TModule::**)()) &getIcon);
    	    icon = ((&owner().modAt(list[i_l]).at())->*getIcon)( );
	}
	else icon = QImage(oscada_qt_xpm);     
	QAction *act_1 = new QAction(qt_mod.at().modName(),icon,qt_mod.at().modName(),CTRL+SHIFT+Key_1,new_wnd,list[i_l].c_str());
	act_1->setToolTip(mod->I18N("Call QT GUI programm: '")+qt_mod.at().modName()+"'");
	act_1->setWhatsThis( qt_mod.at().modInfo("Descript") );
	QObject::connect(act_1, SIGNAL(activated()), this, SLOT(callQTModule()));
	
	act_1->addTo(toolBar);
    }
    
    new_wnd->show();
}

void TUIMod::startDialog( )
{
    vector<string> list;

    QMainWindow *new_wnd = new QMainWindow( );
    new_wnd->setCaption(mod->I18N("QT Starter dialog"));
    new_wnd->setIcon(QPixmap(QImage(oscada_qt_xpm)));
					
    new_wnd->setCentralWidget( new QWidget( new_wnd, "CentralWidget" ) );
    QVBoxLayout *new_wnd_lay = new QVBoxLayout( new_wnd->centralWidget(), 6, 4);
    new_wnd_lay->setAlignment( Qt::AlignTop );
    
    owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
        if( owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
            owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
        QPixmap icon;
        if( owner().modAt(list[i_l]).at().modFuncPresent("QPixmap TUIMod::icon();") )
        {
            QPixmap (TModule::*getIcon)();
            owner().modAt(list[i_l]).at().modFunc("QPixmap TUIMod::icon();",(void (TModule::**)()) &getIcon);
            icon = ((&owner().modAt(list[i_l]).at())->*getIcon)( );
        }
        else icon = QImage(oscada_qt_xpm);										    
    
	AutoHD<TModule> qt_mod = owner().modAt(list[i_l]);	
	QPushButton *butt = new QPushButton( icon, qt_mod.at().modName(), new_wnd->centralWidget(),list[i_l].c_str());
	QObject::connect(butt, SIGNAL(clicked()), this, SLOT(callQTModule()));	
	new_wnd_lay->addWidget( butt, 0, 0 );
    }
    
    QFrame *gFrame = new QFrame( new_wnd->centralWidget() );
    gFrame->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding, 3, 0 ) );
    gFrame->setFrameShape( QFrame::HLine );
    gFrame->setFrameShadow( QFrame::Raised );
    new_wnd_lay->addWidget( gFrame, 0, 0 );
    
    QPushButton *butt = new QPushButton( QPixmap(QImage(exit_xpm)),"Exit from system", new_wnd->centralWidget(),"*exit*");
    QObject::connect(butt, SIGNAL(clicked()), this, SLOT(callQTModule()));
    new_wnd_lay->addWidget( butt, 0, 0 );
    
    new_wnd->show();
}

void *TUIMod::Task( void *CfgM )
{
    vector<string> list;
    TUIMod *Cfg = (TUIMod *)CfgM;
    bool first_ent = true;

#if OSC_DEBUG
    Mess->put(Cfg->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif    
    
    Cfg->run_st = true;

    while(!Cfg->end_run && !SYS->stopSignal( ))
    {
	int op_wnd = 0;
	if( !qApp ) qApp = new QApplication( (int)SYS->argc,(char **)SYS->argv );
	//------------- Start external modules ----------------
	if( first_ent )
	{
	    Cfg->owner().modList(list);
	    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
    		if( Cfg->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
			Cfg->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
		{
		    //Search module into start list
		    int i_el = 0;
		    string s_el;
		    while( TSYS::strSepParse(Cfg->start_mod,i_el,';').size() )
		    {
			if( TSYS::strSepParse(Cfg->start_mod,i_el,';') == list[i_l] )	break;
			i_el++;
		    }
		    if( s_el.size() || !i_el ) 
		    {
			Cfg->callQTModule(list[i_l]);
			op_wnd++;
		    }
		}
	}
	//-------------- Start call dialog --------------------
	if(!op_wnd) Cfg->startDialog( );
	
	qApp->connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );    
	qApp->exec();
	delete qApp;
	qApp = NULL;
	first_ent = false;	
    }
    
    Cfg->run_st = false;
    
    return(NULL);
}


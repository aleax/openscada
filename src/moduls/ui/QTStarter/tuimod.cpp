
//OpenSCADA system module UI.QTStarter file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
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

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QImage>
#include <QPushButton>
#include <QLayout>
#include <QFrame>
#include <QVBoxLayout>
#include <QTextCodec>

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

//============ Modul info! =====================================================
#define MOD_ID      "QTStarter"
#define MOD_NAME    "QT GUI starter"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define VERSION     "1.0.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Allow QT GUI starter. It is single for all QT GUI modules!"
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

	return AtMod;
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	QTStarter::TUIMod *self_addr = NULL;

	if( AtMod.id == MOD_ID && AtMod.type == MOD_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = QTStarter::mod = new QTStarter::TUIMod( source );

	return self_addr;
    }    
}

using namespace QTStarter;

//==============================================================================
//================= QTStarter::TUIMod ==========================================
//==============================================================================

TUIMod::TUIMod( string name ) : end_run(false), dem_mode(false)
{
    mId		= MOD_ID;
    mName       = MOD_NAME;
    mType  	= MOD_TYPE;
    mVers      	= VERSION;
    mAutor    	= AUTORS;
    mDescr  	= DESCRIPTION;
    mLicense   	= LICENSE;
    mSource    	= name;
}

TUIMod::~TUIMod()
{
    if( run_st ) modStop();
}

void TUIMod::modLoad( )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Load module."));
#endif

    //========== Load parameters from command line ============
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
        {"help"    ,0,NULL,'h'},
	{"demon"   ,0,NULL,'d'},
        {NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
        next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
        switch(next_opt)
        {
            case 'h': fprintf(stdout,optDescr().c_str()); break;
	    case 'd': dem_mode = true; break;
            case -1 : break;
        }
    } while(next_opt != -1);
    
    //========== Load parameters from config file =============
    start_mod = TBDS::genDBGet(nodePath()+"StartMod",start_mod);
}

void TUIMod::modSave( )
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Save module."));
#endif

    TBDS::genDBSet(nodePath()+"StartMod",start_mod);
}

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);
    
    //Set QT environments    
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale () ); //codepage for QT across QString recode!
}

void TUIMod::modStart()
{
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Start module."));
#endif

    if( run_st || dem_mode ) return;
    pthread_attr_t pthr_attr;
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( TSYS::eventWait( run_st, true, nodePath()+"start",5) )
       	throw TError(nodePath().c_str(),_("QT starter no started!"));   
}

void TUIMod::modStop()
{    
#if OSC_DEBUG
    mess_debug(nodePath().c_str(),_("Stop module."));
#endif

    if( run_st )
    {
	end_run = true;
	emit qApp->closeAllWindows();
	
	printf("TEST 100\n");
	
	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),_("QT starter no stoped!"));
	pthread_join(pthr_tsk,NULL);
    }	
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];
    
    snprintf(buf,sizeof(buf),_(
        "======================= The module <%s:%s> options =======================\n"
        "---------- Parameters of the module section <%s> in config file ----------\n"
        "StartMod  <moduls>    Start modules list (sep - ';').\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
	
    return buf;
}

void *TUIMod::Task( void * )
{
    vector<string> list;
    bool first_ent = true;

#if OSC_DEBUG
    mess_debug(mod->nodePath().c_str(),_("Thread <%d> started!"),getpid());
#endif        

    QApplication *QtApp = new QApplication( (int&)SYS->argc,(char **)SYS->argv );
    QtApp->setQuitOnLastWindowClosed(false);
    WinControl *winCntr = new WinControl();
    mod->run_st = true;
    
    int op_wnd = 0;
    //------------- Start external modules ----------------
    mod->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
	if( mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
		mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
	{
	    //Search module into start list
	    int i_el = 0;
	    string s_el;
	    while( TSYS::strSepParse(mod->start_mod,i_el,';').size() )
	    {
		if( TSYS::strSepParse(mod->start_mod,i_el,';') == list[i_l] )	break;
		i_el++;
	    }
	    if( s_el.size() || !i_el ) 
	    {
		if(winCntr->callQTModule(list[i_l]))	op_wnd++;
	    }
	}
    //-------------- Start call dialog --------------------
    if(!op_wnd) winCntr->startDialog( );
	
    QObject::connect( QtApp, SIGNAL(lastWindowClosed()), winCntr, SLOT(lastWinClose()) );
    QtApp->exec();
    delete winCntr;
    delete QtApp;
    first_ent = false;	
    
    mod->run_st = false;
    
    return NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if( opt->name() == "info" )
    {
        TUI::cntrCmdProc(opt);
        if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
    	    ctrMkNode("fld",opt,-1,"/prm/cfg/st_mod",_("Start QT modules (sep - ';')"),0660,"root","root",1,"tp","str");
    	    ctrMkNode("comm",opt,-1,"/prm/cfg/load",_("Load"),0660);
    	    ctrMkNode("comm",opt,-1,"/prm/cfg/save",_("Save"),0660);
	}
        ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }
    //Process command to page
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/st_mod" )
    {
	if( ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )	opt->setText(start_mod);
	if( ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	start_mod = opt->text();
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )	opt->setText(optDescr());
    else if( a_path == "/prm/cfg/load" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modLoad();
    else if( a_path == "/prm/cfg/save" && ctrChkNode(opt,"set",0660,"root","root",SEQ_WR) )	modSave();
    else TUI::cntrCmdProc(opt);
}		    

//=========================== Windows control =====================
void WinControl::callQTModule( )
{
    QObject *obj = (QObject *)sender();
    if( string("*exit*") == obj->objectName().toAscii().data() ) SYS->stop();
    else 
    {
	try{ callQTModule(obj->objectName().toAscii().data()); }
	catch(TError err) {  }
    }
}

void WinControl::lastWinClose( )
{
    if(mod->endRun() || SYS->stopSignal( ))	
	emit qApp->quit();
    else startDialog( );
}

bool WinControl::callQTModule( const string &nm )
{
    vector<string> list;
    
    AutoHD<TModule> qt_mod = mod->owner().modAt(nm);
    QMainWindow *(TModule::*openWindow)( );
    qt_mod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
    QMainWindow *new_wnd = ((&qt_mod.at())->*openWindow)( );
    if(!new_wnd) return false;

    //Make QT starter toolbar
    QToolBar *toolBar = new QToolBar(_("QTStarter toolbar"), new_wnd);
    toolBar->setObjectName("QTStarterTool");
    new_wnd->addToolBar(toolBar);
    //, Qt::DockTop );    
    mod->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
        if( mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
            mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
	AutoHD<TModule> qt_mod = mod->owner().modAt(list[i_l]);
	
	QIcon icon;
	if( mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();") )
	{
	    QIcon(TModule::*iconGet)();
	    mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
    	    icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
	}
	else icon = QIcon(":/images/oscada_qt.png");
	QAction *act_1 = new QAction(icon,qt_mod.at().modName().c_str(),new_wnd);
	act_1->setObjectName(list[i_l].c_str());
	act_1->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_1);
	act_1->setToolTip(qt_mod.at().modName().c_str());
	act_1->setWhatsThis(qt_mod.at().modInfo("Descript").c_str());
	QObject::connect(act_1, SIGNAL(activated()), this, SLOT(callQTModule()));
	
	toolBar->addAction(act_1);
    }
    
    new_wnd->show();

    return true;
}

void WinControl::startDialog( )
{
    vector<string> list;

    QMainWindow *new_wnd = new QMainWindow( );
    new_wnd->setWindowTitle(_("OpenSCADA system QT-starter"));
    new_wnd->setWindowIcon(QIcon(":/images/oscada_qt.png"));
					
    new_wnd->setCentralWidget( new QWidget(new_wnd) );
    QVBoxLayout *new_wnd_lay = new QVBoxLayout(new_wnd->centralWidget());
    new_wnd_lay->setMargin(6);
    new_wnd_lay->setSpacing(4);
    
    mod->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
        if( mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
            mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
        QIcon icon;
        if( mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();") )
        {
            QIcon (TModule::*iconGet)();
            mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
            icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
        }
        else icon = QIcon(":/images/oscada_qt.png");
    
	AutoHD<TModule> qt_mod = mod->owner().modAt(list[i_l]);	
	QPushButton *butt = new QPushButton(icon,qt_mod.at().modName().c_str(),new_wnd->centralWidget());
	butt->setObjectName(list[i_l].c_str());
	QObject::connect(butt, SIGNAL(clicked(bool)), this, SLOT(callQTModule()));
	new_wnd_lay->addWidget( butt, 0, 0 );
    }
    
    new_wnd_lay->addItem( new QSpacerItem( 20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    
    QFrame *gFrame = new QFrame( new_wnd->centralWidget() );
    gFrame->setFrameShape(QFrame::HLine);
    gFrame->setFrameShadow(QFrame::Raised);
    new_wnd_lay->addWidget(gFrame,0,0);
    
    QPushButton *butt = new QPushButton(QIcon(":/images/exit.png"),_("Exit from system"), new_wnd->centralWidget());
    butt->setObjectName("*exit*");
    QObject::connect(butt, SIGNAL(clicked(bool)), this, SLOT(callQTModule()));
    new_wnd_lay->addWidget( butt, 0, 0 );
    
    new_wnd->show();
}

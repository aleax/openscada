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

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

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

TUIMod::TUIMod( string name )
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
    if( run_st)
    {
	qApp->closeAllWindows();
	if( TSYS::eventWait( run_st, false, nodePath()+"stop",5) )
	    throw TError(nodePath().c_str(),"QT starter no stoped!");
	pthread_join(pthr_tsk,NULL);
    }	
}

void *TUIMod::Task( void *CfgM )
{
    TUIMod *Cfg = (TUIMod *)CfgM;

#if OSC_DEBUG
    Mess->put(Cfg->nodePath().c_str(),TMess::Debug,Mess->I18N("Thread <%d> started!"),getpid() );
#endif    
    
    Cfg->run_st = true;

    QApplication app( (int)SYS->argc,(char **)SYS->argv );
    //------------- Start all external modules ----------------
    vector<string> list;
    Cfg->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
        if( Cfg->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" )
	{
	    AutoHD<TModule> qt_mod = Cfg->owner().modAt(list[i_l]);
	    printf("Start QT GUI: <%s>\n",qt_mod.at().modId().c_str());
	    QMainWindow *(TModule::*openWindow)( );
	    qt_mod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
	    
	    
	    
	    QMainWindow *new_wnd = ((&qt_mod.at())->*openWindow)( );
	    QToolBar *toolBar = new QToolBar(mod->I18N("QTStarter toolbar"), new_wnd, DockTop );
	    
	    QAction *act_1 = new QAction(qt_mod.at().modName(),*new_wnd->icon(),qt_mod.at().modName(),CTRL+SHIFT+Key_1,new_wnd);
    	    act_1->setToolTip(mod->I18N("Call QT GUI programm: '")+qt_mod.at().modName()+"'");
	    //act_1->setWhatsThis(mod->I18N( "Button for refreshing a content of the current page."));
	    act_1->addTo(toolBar);
		    
	    new_wnd->show();
	}
    //---------------------------------------------------------
    app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );    
    app.exec();
    
    Cfg->run_st = false;
    
    return(NULL);
}


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

#include <qtextcodec.h>
#include <qapplication.h>

#include <tsys.h>
#include <tkernel.h>
#include <tmessage.h>
#include "qtcfg.h"
#include "tuimod.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "QTCfg"
#define NAME_TYPE   "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "0.0.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "QT based OpenSCADA Configurator."
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

    TModule *attach( const SAtMod &AtMod, const string &source )
    {
	QTCFG::TUIMod *self_addr = NULL;

	if( AtMod.name == NAME_MODUL && AtMod.type == NAME_TYPE && AtMod.t_ver == VER_TYPE )
	    self_addr = new QTCFG::TUIMod( source );       

	return ( self_addr );
    }    
}

using namespace QTCFG;

//==============================================================================
//================= QTCFG::TUIMod =============================================
//==============================================================================

TUIMod::TUIMod( string name ) : cfapp(NULL)
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
    if( run_st ) stop();
}

string TUIMod::mod_info( const string &name )
{
    if( name == "SubType" ) return(SUB_TYPE);
    else return( TModule::mod_info( name) );
}

void TUIMod::mod_info( vector<string> &list )
{
    TModule::mod_info(list);
    list.push_back("SubType");
}

string TUIMod::opt_descr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),I18N(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n\n"),
	NAME_TYPE,NAME_MODUL,NAME_MODUL);

    return(buf);
}

void TUIMod::mod_CheckCommandLine( )
{
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
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TUIMod::mod_connect(  )
{
    TModule::mod_connect(  );
    
    //Set QT environments    
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale () ); //codepage for QT across QString recode!
}

void TUIMod::start()
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

void TUIMod::stop()
{
    if( run_st)
    {
	if( cfapp != NULL ) cfapp->close();
	if( SYS->event_wait( run_st, false, string(NAME_MODUL)+": The configurator is stoping....",5) )
	    throw TError("%s: The configurator no stoped!",NAME_MODUL);   
	pthread_join(pthr_tsk,NULL);
    }	
}

void *TUIMod::Task( void *CfgM )
{
    TUIMod *Cfg = (TUIMod *)CfgM;

#if OSC_DEBUG
    Cfg->m_put("DEBUG",MESS_DEBUG,"Thread <%d>!",getpid() );
#endif    
    
    Cfg->run_st = true;

    QApplication app( (int)SYS->argc,(char **)SYS->argv );
    Cfg->cfapp = new ConfApp(Cfg);
    Cfg->cfapp->show();
    app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );    
    app.exec();
    
    Cfg->cfapp = NULL;
    Cfg->run_st = false;
    
    return(NULL);
}


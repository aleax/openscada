
//OpenSCADA system module UI.QTCfg file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2004-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <QMessageBox>
#include <QErrorMessage>
#include <QIcon>

#include <tsys.h>
#include <tmess.h>
#include "qtcfg.h"
#include "selfwidg.h"
#include "tuimod.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"QTCfg"
#define MOD_NAME	"System configurator (QT)"
#define MOD_TYPE	"UI"
#define VER_TYPE	VER_UI
#define SUB_TYPE	"QT"
#define VERSION		"1.8.1"
#define AUTORS		"Roman Savochenko"
#define DESCRIPTION	"Allow the QT based OpenSCADA system configurator."
#define LICENSE		"GPL"
//*************************************************

QTCFG::TUIMod *QTCFG::mod;

extern "C"
{
    TModule::SAt module( int n_mod )
    {
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

    TModule *attach( const TModule::SAt &AtMod, const string &source )
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new QTCFG::TUIMod( source );
	return NULL;
    }
}

using namespace QTCFG;

//*************************************************
//* TUIMod                                        *
//*************************************************
TUIMod::TUIMod( string name ) : start_path(string("/")+SYS->id()), end_run(false)
{
    mId		= MOD_ID;
    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= VERSION;
    mAutor	= AUTORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    mod		= this;

    //Public export functions
    modFuncReg( new ExpFunc("QIcon icon();","Module QT-icon",(void(TModule::*)( )) &TUIMod::icon) );
    modFuncReg( new ExpFunc("QMainWindow *openWindow();","Start QT GUI.",(void(TModule::*)( )) &TUIMod::openWindow) );
}

TUIMod::~TUIMod( )
{
    if( run_st ) modStop();
}

string TUIMod::modInfo( const string &name )
{
    if( name == "SubType" ) return SUB_TYPE;
    else return TModule::modInfo( name);
}

void TUIMod::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section <%s> in config file ----------\n"
	"StartPath  <path>    Configurator start path.\n"
	"StartUser  <user>    No password requested start user.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TUIMod::load_( )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Load module."));
#endif

    //- Load parameters from command line -
    int next_opt;
    const char *short_opt="h";
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
	    case 'h': fprintf(stdout,"%s",optDescr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //- Load parameters from config file and DB -
    setStartPath( TBDS::genDBGet(nodePath()+"StartPath",startPath()) );
    setStartUser( TBDS::genDBGet(nodePath()+"StartUser",startUser()) );
}

void TUIMod::save_( )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Save module."));
#endif

    //- Save parameters to DB -
    TBDS::genDBSet( nodePath()+"StartPath", startPath() );
    TBDS::genDBSet( nodePath()+"StartUser", startUser() );
}

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);
}

QIcon TUIMod::icon( )
{
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("UI.QTCfg").c_str())) ico_t.load(":/images/oscada_cfg.png");
    return QPixmap::fromImage(ico_t);
}

QMainWindow *TUIMod::openWindow( )
{
    string user_open = startUser();
    if(!SYS->security().at().usrPresent(user_open))
	while(true)
	{
	    DlgUser d_usr;
	    int rez = d_usr.exec();
	    if( rez == DlgUser::SelCancel )     return NULL;
	    if( rez == DlgUser::SelErr )
	    {
		postMess(nodePath().c_str(),_("Auth is wrong!!!"));
		continue;
	    }
	    user_open = d_usr.user().toAscii().data();
	    break;
	}
    return new ConfApp(user_open);
}

void TUIMod::modStart( )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Start module."));
#endif

    end_run = false;
    run_st  = true;
}

void TUIMod::modStop( )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Stop module."));
#endif

    end_run = true;

    for( int i_w = 0; i_w < cfapp.size(); i_w++ )
	while(cfapp[i_w]) usleep(STD_WAIT_DELAY*1000);

    run_st = false;
}

void TUIMod::regWin( QMainWindow *win )
{
    int i_w;
    for( i_w = 0; i_w < cfapp.size(); i_w++ )
	if( cfapp[i_w] == NULL ) break;
    if( i_w == cfapp.size() )	cfapp.push_back((QMainWindow*)NULL);
    cfapp[i_w] = win;
}

void TUIMod::unregWin( QMainWindow *win )
{
    for( int i_w = 0; i_w < cfapp.size(); i_w++ )
	if( cfapp[i_w] == win )	cfapp[i_w] = NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //- Get page info -
    if( opt->name() == "info" )
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_path",_("Configurator start path"),0664,"root","root",1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Configurator start user"),0664,"root","root",3,"tp","str","dest","select","select","/prm/cfg/u_lst");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),0660,"root","root",1,"tp","lnk");
	}
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),0440,"root","root",3,"tp","str","cols","90","rows","5");
	return;
    }

    //- Process command to page -
    string a_path = opt->attr("path");
    if( a_path == "/prm/cfg/start_path" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( startPath() );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setStartPath( opt->text() );
    }
    else if( a_path == "/prm/cfg/start_user" )
    {
	if( ctrChkNode(opt,"get",0664,"root","root",SEQ_RD) )	opt->setText( startUser() );
	if( ctrChkNode(opt,"set",0664,"root","root",SEQ_WR) )	setStartUser( opt->text() );
    }
    else if( a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",0660,"root","root",SEQ_RD) )
    {
	SYS->transport().at().setSysHost(false);
	opt->setText("/Transport");
    }
    else if( a_path == "/help/g_help" && ctrChkNode(opt,"get",0440) )   opt->setText(optDescr());
    else if( a_path == "/prm/cfg/u_lst" && ctrChkNode(opt) )
    {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	opt->childAdd("el")->setText("");
	for(int i_u = 0; i_u < ls.size(); i_u++)
	    opt->childAdd("el")->setText(ls[i_u]);
    }
    else TUI::cntrCmdProc(opt);
}

void TUIMod::postMess( const string &cat, const string &mess, TUIMod::MessLev type, QWidget *parent )
{
    //- Put system message. -
    message(cat.c_str(),(type==TUIMod::Crit)?TMess::Crit:
	(type==TUIMod::Error)?TMess::Error:
	(type==TUIMod::Warning)?TMess::Warning:TMess::Info,"%s",mess.c_str());

    //- QT message -
    switch(type)
    {
	case TUIMod::Info:
	    QMessageBox::information(parent,_(MOD_NAME),mess.c_str());	break;
	case TUIMod::Warning:
	    QMessageBox::warning(parent,_(MOD_NAME),mess.c_str());	break;
	case TUIMod::Error:
	    QMessageBox::critical(parent,_(MOD_NAME),mess.c_str());	break;
	case TUIMod::Crit:
	    QErrorMessage::qtHandler()->showMessage(mess.c_str());	break;
    }
}

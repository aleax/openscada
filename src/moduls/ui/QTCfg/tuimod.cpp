
//OpenSCADA system module UI.QTCfg file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2004-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#define MOD_NAME	_("System configurator (Qt)")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"Qt"
#define MOD_VER		"2.1.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow the Qt based OpenSCADA system configurator.")
#define LICENSE		"GPL2"
//*************************************************

QTCFG::TUIMod *QTCFG::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_QTCfg_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_QTCfg_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new QTCFG::TUIMod(source);
	return NULL;
    }
}

using namespace QTCFG;

//*************************************************
//* TUIMod                                        *
//*************************************************
TUIMod::TUIMod( string name ) : TUI(MOD_ID), start_path(string("/")+SYS->id()), end_run(false)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //Public export functions
    modFuncReg(new ExpFunc("QIcon icon();","Module Qt-icon",(void(TModule::*)( )) &TUIMod::icon));
    modFuncReg(new ExpFunc("QMainWindow *openWindow();","Start Qt GUI.",(void(TModule::*)( )) &TUIMod::openWindow));
}

TUIMod::~TUIMod( )
{
    if(run_st) modStop();
}

string TUIMod::modInfo( const string &name )
{
    if(name == "SubType") return SUB_TYPE;
    else return TModule::modInfo(name);
}

void TUIMod::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TUIMod::optDescr( )
{
    return TSYS::strMess(_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"StartPath  <path>    Configurator start path.\n"
	"StartUser  <user>    No password requested start user.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
}

void TUIMod::load_( )
{
    mess_debug(nodePath().c_str(),_("Load module."));

    //> Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
        if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //> Load parameters from config-file and DB
    setStartPath(TBDS::genDBGet(nodePath()+"StartPath",startPath()));
    setStartUser(TBDS::genDBGet(nodePath()+"StartUser",startUser()));
}

void TUIMod::save_( )
{
    mess_debug(nodePath().c_str(),_("Save module."));

    //> Save parameters to DB
    TBDS::genDBSet(nodePath()+"StartPath", startPath());
    TBDS::genDBSet(nodePath()+"StartUser", startUser());
}

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);
}

QIcon TUIMod::icon( )
{
    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("UI.QTCfg",NULL,true).c_str())) ico_t.load(":/images/oscada_cfg.png");
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
	    if(rez == DlgUser::SelCancel) return NULL;
	    if(rez == DlgUser::SelErr)
	    {
		postMess(nodePath().c_str(),_("Auth is wrong!!!"));
		continue;
	    }
	    user_open = d_usr.user().toStdString();
	    break;
	}
    return new ConfApp(user_open);
}

void TUIMod::modStart( )
{
    mess_debug(nodePath().c_str(), _("Start module."));

    end_run = false;
    run_st  = true;
}

void TUIMod::modStop( )
{
    mess_debug(nodePath().c_str(), _("Stop module."));

    end_run = true;

    for(unsigned i_w = 0; i_w < cfapp.size(); i_w++)
	while(cfapp[i_w]) TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
    TSYS::sysSleep(STD_WAIT_DELAY*1e-3);

    run_st = false;
}

void TUIMod::regWin( QMainWindow *win )
{
    unsigned i_w;
    for(i_w = 0; i_w < cfapp.size(); i_w++)
	if(cfapp[i_w] == NULL) break;
    if(i_w == cfapp.size())	cfapp.push_back((QMainWindow*)NULL);
    cfapp[i_w] = win;
}

void TUIMod::unregWin( QMainWindow *win )
{
    for(unsigned i_w = 0; i_w < cfapp.size(); i_w++)
	if(cfapp[i_w] == win) cfapp[i_w] = NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	{
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_path",_("Configurator start path"),RWRWR_,"root",SUI_ID,1,"tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Configurator start user"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/prm/cfg/u_lst");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),RWRW__,"root",SUI_ID,1,"tp","lnk");
	}
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/start_path")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startPath());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartPath(opt->text());
    }
    else if(a_path == "/prm/cfg/start_user" )
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startUser());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartUser(opt->text());
    }
    else if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) opt->setText("/Transport");
    else if(a_path == "/prm/cfg/u_lst" && ctrChkNode(opt))
    {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	opt->childAdd("el")->setText("");
	for(unsigned i_u = 0; i_u < ls.size(); i_u++)
	    opt->childAdd("el")->setText(ls[i_u]);
    }
    else TUI::cntrCmdProc(opt);
}

void TUIMod::postMess( const string &cat, const string &mess, TUIMod::MessLev type, QWidget *parent )
{
    //> Put system message.
    message(cat.c_str(),(type==TUIMod::Crit)?TMess::Crit:
	(type==TUIMod::Error)?TMess::Error:
	(type==TUIMod::Warning)?TMess::Warning:TMess::Info,"%s",mess.c_str());

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(_(MOD_NAME));
    msgBox.setTextFormat(Qt::PlainText);
    msgBox.setText(mess.c_str());
    switch(type)
    {
	case TUIMod::Info:	msgBox.setIcon(QMessageBox::Information);	break;
	case TUIMod::Warning:	msgBox.setIcon(QMessageBox::Warning);		break;
	case TUIMod::Error:
	case TUIMod::Crit:	msgBox.setIcon(QMessageBox::Critical);		break;
    }
    msgBox.exec();
}

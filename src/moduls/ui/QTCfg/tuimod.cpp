
//OpenSCADA module UI.QTCfg file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2004-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include "qtcfg.h"
#include "selfwidg.h"
#include "tuimod.h"

#include <tmess.h>

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"QTCfg"
#define MOD_NAME	_("Program configurator (Qt)")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"Qt"
#define MOD_VER		"5.6.8"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides the Qt-based configurator of OpenSCADA.")
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
TUIMod::TUIMod( string name ) : TUI(MOD_ID), mTmConChk(dataRes()), mStartUser(dataRes()), mStartPath(dataRes()), mToolTipLim(150), mEndRun(false)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    setTmConChk("10:600");

    //Public export functions
    modFuncReg(new ExpFunc("QIcon icon();","Module Qt-icon",(void(TModule::*)( )) &TUIMod::icon));
    modFuncReg(new ExpFunc("QMainWindow *openWindow();","Start Qt GUI.",(void(TModule::*)( )) &TUIMod::openWindow));
}

TUIMod::~TUIMod( )
{
    if(runSt) modStop();

    if(!SYS->stopSignal()) TSYS::sysSleep(3);
}

void TUIMod::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TUIMod::modInfo( const string &iname )
{
    string name = TSYS::strParse(iname, 0, ":");

    if(name == "SubType") return SUB_TYPE;

    return TModule::modInfo(name);
}

string TUIMod::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Module <%s:%s> options =======================\n"
	"---- Parameters of the module section '%s' of the configuration file ----\n"
	"StartPath  <path>       Initial page path of the configurator.\n"
	"StartUser  <user>       Starting user without password.\n"
	"ToolTipLim <chars>      ToolTip limit in chars, by default 150. Set zero for disable.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());
}

void TUIMod::load_( )
{
    mess_debug(nodePath().c_str(), _("Loading the module."));

    //Load parameters from command line

    //Load parameters from config-file and DB
    setTmConChk(TBDS::genPrmGet(nodePath()+"TmConChk",tmConChk()));
    setStartPath(TBDS::genPrmGet(nodePath()+"StartPath",startPath()));
    setStartUser(TBDS::genPrmGet(nodePath()+"StartUser",startUser()));
    setToolTipLim(s2i(TBDS::genPrmGet(nodePath()+"ToolTipLim",i2s(toolTipLim()))));
}

void TUIMod::save_( )
{
    mess_debug(nodePath().c_str(),_("Saving the module."));

    //Save parameters to DB
    TBDS::genPrmSet(nodePath()+"TmConChk", tmConChk());
    TBDS::genPrmSet(nodePath()+"StartPath", startPath());
    TBDS::genPrmSet(nodePath()+"StartUser", startUser());
    TBDS::genPrmSet(nodePath()+"ToolTipLim",i2s(toolTipLim()));
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
	while(true) {
	    DlgUser d_usr;
	    int rez = d_usr.exec();
	    if(rez == DlgUser::SelCancel) return NULL;
	    if(rez == DlgUser::SelErr) {
		postMess(nodePath().c_str(), _("Error authentication!!!"));
		continue;
	    }
	    user_open = d_usr.user().toStdString();
	    break;
	}
    return new ConfApp(user_open);
}

void TUIMod::setTmConChk( const string &vl )
{
    mTmConChk = i2s(vmax(1,vmin(100,s2i(TSYS::strParse(vl,0,":"))))) + ":" +
		i2s(vmax(1,vmin(1000,s2i(TSYS::strParse(vl,1,":")))));
    modif();
}

void TUIMod::setToolTipLim( int vl )	{ mToolTipLim = vmax(0, vl); modif(); }

void TUIMod::modStart( )
{
    mess_debug(nodePath().c_str(), _("Starting the module."));

    mEndRun = false;
    runSt   = true;
}

void TUIMod::modStop( )
{
    mess_debug(nodePath().c_str(), _("Stopping the module."));

    mEndRun = true;

    for(unsigned iW = 0; iW < cfapp.size(); iW++)
	while(cfapp[iW]) {
	    if(!SYS->mainThr.freeStat()) qApp->processEvents();	//!!!! Else can lock here the main thread
	    TSYS::sysSleep(prmWait_DL);
	}
    TSYS::sysSleep(prmWait_DL);

    runSt = false;
}

void TUIMod::regWin( QMainWindow *win )
{
    unsigned iW;
    for(iW = 0; iW < cfapp.size(); iW++)
	if(cfapp[iW] == NULL) break;
    if(iW == cfapp.size())	cfapp.push_back((QMainWindow*)NULL);
    cfapp[iW] = win;
}

void TUIMod::unregWin( QMainWindow *win )
{
    for(unsigned iW = 0; iW < cfapp.size(); iW++)
	if(cfapp[iW] == win) cfapp[iW] = NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //Get the page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"))) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/tmConChk",_("Timeouts of checking connections '{fail}:{good}', seconds"),RWRWR_,"root",SUI_ID,1, "tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/startPath",_("Initial path of the configurator"),RWRWR_,"root",SUI_ID,1, "tp","str");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/startUser",_("Initial user of the configurator"),RWRWR_,"root",SUI_ID,3,
		"tp","str", "dest","select", "select","/prm/cfg/u_lst");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/toolTipLim",_("ToolTip limit, zero to disable"),RWRWR_,"root",SUI_ID,1, "tp","int");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to the configuration of the list of remote stations"),RWRW__,"root",SUI_ID,1, "tp","lnk");
	}
	return;
    }

    //Process command to the page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/tmConChk") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(tmConChk());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setTmConChk(opt->text());
    }
    else if(a_path == "/prm/cfg/startPath") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startPath());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartPath(opt->text());
    }
    else if(a_path == "/prm/cfg/startUser") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startUser());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartUser(opt->text());
    }
    else if(a_path == "/prm/cfg/toolTipLim") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(toolTipLim()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setToolTipLim(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) opt->setText("/Transport");
    else if(a_path == "/prm/cfg/u_lst" && ctrChkNode(opt)) {
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
    //Put the program message
    message(cat.c_str(), (type==TUIMod::Crit)?TMess::Crit:
	(type==TUIMod::Error)?TMess::Error:
	(type==TUIMod::Warning)?TMess::Warning:TMess::Info, "%s", mess.c_str());

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(_(MOD_NAME));
    msgBox.setTextFormat(Qt::PlainText);
    msgBox.setText(mess.c_str());
    switch(type) {
	case TUIMod::Info:	msgBox.setIcon(QMessageBox::Information);	break;
	case TUIMod::Warning:	msgBox.setIcon(QMessageBox::Warning);		break;
	case TUIMod::Error:
	case TUIMod::Crit:	msgBox.setIcon(QMessageBox::Critical);		break;
    }
    msgBox.exec();
}

string TUIMod::setHelp( const string &help, const string &addr, QWidget *w )
{
    string helpL = TSYS::strEncode(help, TSYS::Limit, i2s(toolTipLim()));

    if(!w)
	return (helpL != help) ?
	    "<body style='white-space: pre-wrap;'>"+TSYS::strEncode(helpL+"...",TSYS::Html)+"<i><b>Shift+F1</b></i></body>" :
	    help;

    w->setStatusTip(addr.c_str());
    size_t itPos = addr.rfind("/");
    w->setWhatsThis(("<body style='white-space: pre-wrap;'>"+TSYS::strEncode(help,TSYS::Html)+(help.size()?"\n":"")+
	"<i><b>"+_("Page")+"</b></i>:&nbsp;"+addr.substr(0,itPos)+"\n"+
	"<i><b>"+_("Item")+"</b></i>:&nbsp;"+TSYS::strDecode((itPos==string::npos)?"":addr.substr(itPos+1),TSYS::PathEl)+"</body>").c_str());
    if(help.size()) {
	if(helpL != help)
	    w->setToolTip(("<body style='white-space: pre-wrap;'>"+TSYS::strEncode(helpL+"...",TSYS::Html)+"<i><b>Shift+F1</b></i></body>").c_str());
	else w->setToolTip(help.c_str());
    }
    return "";
}


//OpenSCADA system module UI.VISION file: tvision.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2014 by Roman Savochenko (rom_as@oscada.org)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <sys/types.h>
#include <unistd.h>

#include <tsys.h>
#include <tmess.h>

#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QErrorMessage>

#include "vis_devel.h"
#include "vis_run.h"
#include "vis_widgs.h"
#include "vis_shapes.h"
#include "vis_shape_elfig.h"
#include "tvision.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Vision"
#define MOD_NAME	_("Operation user interface (Qt)")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"Qt"
#define MOD_VER		"1.3.0"
#define AUTHORS		_("Roman Savochenko")
#define DEVELOPERS	_("Roman Savochenko, Lysenko Maxim, Yashina Kseniya")
#define DESCRIPTION	_("Visual operation user interface.")
#define LICENSE		"GPL2"
//*************************************************

VISION::TVision *VISION::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_Vision_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
        return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_Vision_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE))
	    return new VISION::TVision( source );
	return NULL;
    }
}

using namespace VISION;

//*************************************************
//* QTCFG::TVision                                *
//*************************************************
TVision::TVision( string name ) : TUI(MOD_ID), mStatusEn(true), mWinPosCntrSave(true), mExitLstRunPrjCls(true), end_run(false),
    mRestTime(60), mCachePgLife(1), vca_station("."), mPlayCom("play -q %f"), mScrnCnt(0)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //Export functions
    modFuncReg(new ExpFunc("QIcon icon();","Module Qt-icon",(void(TModule::*)( )) &TVision::icon));
    modFuncReg(new ExpFunc("QMainWindow *openWindow();","Start Qt GUI.",(void(TModule::*)( )) &TVision::openWindow));
}

TVision::~TVision( )
{
    //Free widget's shapes
    for(unsigned i_sw = 0; i_sw < shapesWdg.size(); i_sw++) delete shapesWdg[i_sw];
    shapesWdg.clear();
}

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
    list.push_back(_("Developers"));
}

string TVision::modInfo( const string &name )
{
    if(name == "SubType")		return SUB_TYPE;
    else if(name == _("Developers"))	return _(DEVELOPERS);
    return TModule::modInfo(name);
}

string TVision::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"StartUser   <user>    No password requested start user.\n"
	"UserPass    <pass>    User password for no local start.\n"
	"RunPrjs     <list>    Run projects list on the module start.\n"
	"RunPrjsSt    {0;1}    Display status for run projects (default = 1).\n"
	"WinPosCntrSave {0;1}  Windows position control and save (default = 1).\n"
	"ExitLstRunPrjCls {0;1}Exit on last run project close (default = 1).\n"
	"CachePgLife <hours>   Cached pages lifetime.\n"
	"VCAstation  <id>      VCA station id ('.' - local).\n"
	"RestoreTime <seconds> Restore connection time.\n"
	"PlayCom     <cmd>     Audio alarms' files play command.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TVision::load_( )
{
    mess_debug(nodePath().c_str(),_("Load module."));

    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters from config-file and DB
    setStartUser(TBDS::genDBGet(nodePath()+"StartUser",""));
    setUserPass(TBDS::genDBGet(nodePath()+"UserPass",""));
    setRunPrjs(TBDS::genDBGet(nodePath()+"RunPrjs",""));
    setRunPrjsSt(s2i(TBDS::genDBGet(nodePath()+"RunPrjsSt","1")));
    setWinPosCntrSave(s2i(TBDS::genDBGet(nodePath()+"WinPosCntrSave",i2s(winPosCntrSave()))));
    setExitLstRunPrjCls(s2i(TBDS::genDBGet(nodePath()+"ExitLstRunPrjCls",i2s(exitLstRunPrjCls()))));
    setCachePgLife(s2r(TBDS::genDBGet(nodePath()+"CachePgLife",r2s(cachePgLife()))));
    setVCAStation(TBDS::genDBGet(nodePath()+"VCAstation","."));
    setRestoreTime(s2i(TBDS::genDBGet(nodePath()+"RestoreTime",i2s(restoreTime()))));
    setPlayCom(TBDS::genDBGet(nodePath()+"PlayCom",playCom()));
}

void TVision::save_( )
{
    mess_debug(nodePath().c_str(),_("Save module."));

    //Save parameters to DB
    TBDS::genDBSet(nodePath()+"StartUser", startUser());
    TBDS::genDBSet(nodePath()+"UserPass", userPass());
    TBDS::genDBSet(nodePath()+"RunPrjs", runPrjs());
    TBDS::genDBSet(nodePath()+"RunPrjsSt", i2s(runPrjsSt()));
    TBDS::genDBSet(nodePath()+"WinPosCntrSave", i2s(winPosCntrSave()));
    TBDS::genDBSet(nodePath()+"ExitLstRunPrjCls", i2s(exitLstRunPrjCls()));
    TBDS::genDBSet(nodePath()+"CachePgLife", r2s(cachePgLife()));
    TBDS::genDBSet(nodePath()+"VCAstation", VCAStation());
    TBDS::genDBSet(nodePath()+"RestoreTime",i2s(restoreTime()));
    TBDS::genDBSet(nodePath()+"PlayCom", playCom());
}

void TVision::postEnable( int flag )
{
   TModule::postEnable(flag);
}

string TVision::uiPropGet( const string &prop, const string &user )
{
    ResAlloc res(nodeRes(), false);

    XMLNode prmNd;
    try {
	prmNd.load(TBDS::genDBGet(nodePath()+"uiProps","",user));
	return prmNd.attr(prop);
    }
    catch(TError err)	{ }

    return "";
}

void TVision::uiPropSet( const string &prop, const string &vl, const string &user )
{
    ResAlloc res(nodeRes(), true);

    XMLNode prmNd("UI");
    try { prmNd.load(TBDS::genDBGet(nodePath()+"uiProps","",user)); }
    catch(TError err)	{ }
    prmNd.setAttr(prop,vl);
    TBDS::genDBSet(nodePath()+"uiProps",prmNd.save(XMLNode::BrAllPast),user);
}

QIcon TVision::icon()
{
    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("UI.Vision",NULL,true).c_str())) ico_t.load(":/images/vision.png");
    return QPixmap::fromImage(ico_t);
}

QMainWindow *TVision::openWindow( )
{
#if QT_VERSION >= 0x040600
    //Get allowed screens count
    mScrnCnt = QDesktopWidget().screenCount();
#endif

    //Register support widget's shapes
    if(shapesWdg.empty()) {
	shapesWdg.push_back(new ShapeElFigure);
	shapesWdg.push_back(new ShapeFormEl);
	shapesWdg.push_back(new ShapeText);
	shapesWdg.push_back(new ShapeMedia);
	shapesWdg.push_back(new ShapeDiagram);
	shapesWdg.push_back(new ShapeProtocol);
	shapesWdg.push_back(new ShapeDocument);
	shapesWdg.push_back(new ShapeBox);
	shapesWdg.push_back(new ShapeFunction);
    }

    string user_open = startUser( );
    string user_pass = userPass( );

    //Check for start user set OK
    int err = 0;
    XMLNode req("get");
    req.setAttr("path",string("/Security/")+user_open+"/%2fauth")->setAttr("password",user_pass);
    if(!((VCAStation() == "." && SYS->security().at().usrPresent(startUser())) ||
	    (!(err=mod->cntrIfCmd(req,startUser(),userPass(),VCAStation(),true)) && s2i(req.text()))))
	while(true) {
	    if(err == 10)	{ postMess(nodePath().c_str(),_("Error connection to remote station!")); return NULL; }
	    DlgUser d_usr(startUser().c_str(),userPass().c_str(),VCAStation().c_str());
	    int rez = d_usr.exec();
	    if(rez == DlgUser::SelCancel) return NULL;
	    if(rez == DlgUser::SelErr) {
		postMess(nodePath().c_str(),_("Auth wrong!!!"));
		continue;
	    }
	    user_open = d_usr.user().toStdString();
	    user_pass = d_usr.password().toStdString();
	    break;
	}

    //Check for run projects need
    string sprj;
    unsigned screen;
    VisRun *fsess = NULL;
    for(int p_off = 0; (sprj=TSYS::strSepParse(runPrjs(),0,';',&p_off)).size(); ) {
	// Screen number take
	screen = 0;
	size_t iSep = sprj.find("-");
	if(iSep != string::npos) screen = s2i(sprj.substr(iSep+1));
	sprj = sprj.substr(0,iSep);

	// Check and extract for "sess_" or "proj_" prefixes
	bool isSess = false;
	if(sprj.compare(0,4,"ses_") == 0) { isSess = true; sprj.erase(0,4); }
	else if(sprj.compare(0,4,"prj_") == 0) sprj.erase(0,4);

	//QDesktopWidget().screen(1)
	// Find for already opened run window
	unsigned i_w;
	for(i_w = 0; i_w < mn_winds.size(); i_w++)
	    if(qobject_cast<VisRun*>(mn_winds[i_w]) &&
		    ((isSess && ((VisRun*)mn_winds[i_w])->workSess() == sprj) || (!isSess && ((VisRun*)mn_winds[i_w])->srcProject() == sprj)) &&
		    ((VisRun*)mn_winds[i_w])->screen() == screen)
		break;
	if(i_w < mn_winds.size()) continue;
	VisRun *sess = new VisRun((isSess?"/ses_":"/prj_")+sprj, user_open, user_pass, VCAStation(), true, screen);
	sess->show();
	sess->raise();
	sess->activateWindow();
	if(!fsess) fsess = sess;
    }

    return fsess ? (QMainWindow*)fsess : (QMainWindow*)new VisDevelop(user_open, user_pass, VCAStation());
}

void TVision::modStart( )
{
    mess_debug(nodePath().c_str(),_("Start module."));

    end_run = false;
    run_st  = true;
}

void TVision::modStop( )
{
    mess_debug(nodePath().c_str(),_("Stop module."));

    end_run = true;

    for(unsigned i_w = 0; i_w < mn_winds.size(); i_w++)
	while(mn_winds[i_w]) TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
    TSYS::sysSleep(STD_WAIT_DELAY*1e-3);

    run_st = false;
}

WdgShape *TVision::getWdgShape( const string &iid )
{
    for(unsigned i_sw = 0; i_sw < shapesWdg.size(); i_sw++)
	if(shapesWdg[i_sw]->id() == iid)
	    return shapesWdg[i_sw];

    return NULL;
}

void TVision::regWin( QMainWindow *mwd )
{
    unsigned i_w;
    for(i_w = 0; i_w < mn_winds.size(); i_w++)
	if(mn_winds[i_w] == NULL) break;
    if(i_w == mn_winds.size()) mn_winds.push_back((QMainWindow*)NULL);
    mn_winds[i_w] = mwd;
}

void TVision::unregWin( QMainWindow *mwd )
{
    for(unsigned i_w = 0; i_w < mn_winds.size(); i_w++)
	if(mn_winds[i_w] == mwd) mn_winds[i_w] = NULL;
}

void TVision::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/disp_n",_("Display number"),R_R_R_,"root",SUI_ID,1,"tp","dec");
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"))) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/stationVCA",_("VCA engine station"),RWRWR_,"root",SUI_ID,4,"tp","str","idm","1","dest","select","select","/prm/cfg/vca_lst");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to remote stations list configuration"),RWRW__,"root",SUI_ID,1,"tp","lnk");
	    if(VCAStation() == ".")
		ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Start user"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/prm/cfg/u_lst");
	    else {
		ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Start user"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/u_pass",_("User password"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/restTm",_("Restore connection timeout, s"),RWRWR_,"root",SUI_ID,3,"tp","dec","min","1","max","1000");
	    }
	    ctrMkNode("fld",opt,-1,"/prm/cfg/cachePgLife",_("Cached pages lifetime"),RWRWR_,"root",SUI_ID,2,"tp","real",
		"help",_("The time in hours for close pages from cache by inactive.\nFor zero time pages will not closed."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/run_prj",_("Run projects list"),RWRWR_,"root",SUI_ID,4,"tp","str","dest","sel_ed","select","/prm/cfg/r_lst",
		"help",_("Automatic started projects separated by symbol ';'.\n"
			 "For opening a project's window to need display (1) use the project name format: 'PrjName-1'.\n"
			 "For connect to background or other opened session use \"ses_{SesID}\"."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/run_prj_st",_("Run projects status display"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/winPos_cntr_save",_("Windows position control and save"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/exit_on_lst_run_prj_cls",_("Exit on last run project close"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	}
	if(ctrMkNode("area",opt,2,"/alarm",_("Alarms"),R_R_R_,"root",SUI_ID))
	    ctrMkNode("fld",opt,-1,"/alarm/plComm",_("Play command"),RWRWR_,"root",SUI_ID,4,"tp","str","dest","sel_ed","select","/alarm/plComLs","help",
		    _("Command line for call sounds play.\n"
		    "Use %f for source file name inserting. If source file is not used play sample is sent to pipe."));
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/disp_n" && ctrChkNode(opt))		opt->setText(i2s(mScrnCnt));
    else if(a_path == "/prm/cfg/start_user") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startUser());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartUser(opt->text());
    }
    else if(a_path == "/prm/cfg/u_pass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText("*******");
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setUserPass(opt->text());
    }
    else if(a_path == "/prm/cfg/restTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(restoreTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setRestoreTime(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/cachePgLife") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(r2s(cachePgLife()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCachePgLife(s2r(opt->text()));
    }
    else if(a_path == "/prm/cfg/run_prj") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(runPrjs());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setRunPrjs(opt->text());
    }
    else if(a_path == "/prm/cfg/r_lst" && ctrChkNode(opt)) {
	string rPrjs = runPrjs();
	// Sessions and projects list request
	XMLNode req("CntrReqs");
	req.childAdd("get")->setAttr("path","%2fses%2fses")->setAttr("chkUserPerm","1");
	req.childAdd("get")->setAttr("path","%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm","1");
	cntrIfCmd(req, startUser(), userPass(), VCAStation());
	XMLNode *reqN = req.childGet(0);
	for(unsigned i_ch = 0; i_ch < reqN->childSize(); i_ch++)
	    if(SYS->security().at().access(startUser(),SEC_WR,"root","root",RWRWR_) ||
		    reqN->childGet(i_ch)->attr("user") == startUser())
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+"ses_"+reqN->childGet(i_ch)->text());
	reqN = req.childGet(1);
	for(unsigned i_ch = 0; i_ch < reqN->childSize(); i_ch++)
	    if(SYS->security().at().access(startUser(),SEC_WR,"root","root",RWRWR_))
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+reqN->childGet(i_ch)->attr("id"));
    }
    else if(a_path == "/prm/cfg/winPos_cntr_save") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(winPosCntrSave()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setWinPosCntrSave(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/exit_on_lst_run_prj_cls") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(exitLstRunPrjCls()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setExitLstRunPrjCls(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/run_prj_st") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(runPrjsSt()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setRunPrjsSt(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/stationVCA") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(VCAStation());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setVCAStation(opt->text());
    }
    else if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) opt->setText("/Transport");
    else if(a_path == "/prm/cfg/u_lst" && ctrChkNode(opt)) {
	vector<string> ls;
	SYS->security().at().usrList(ls);
	opt->childAdd("el")->setText("");
	for(unsigned i_u = 0; i_u < ls.size(); i_u++)
	    opt->childAdd("el")->setText(ls[i_u]);
    }
    else if(a_path == "/prm/cfg/vca_lst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",".")->setText("Local");
	vector<string> lst;
	SYS->transport().at().extHostList("*",lst);
	for(unsigned i_ls = 0; i_ls < lst.size(); i_ls++)
	    opt->childAdd("el")->setAttr("id",lst[i_ls])->
		setText(SYS->transport().at().extHostGet("*",lst[i_ls]).name);
    }
    else if(a_path == "/alarm/plComm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(playCom());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setPlayCom(opt->text());
    }
    else if(a_path == "/alarm/plComLs" && ctrChkNode(opt)) opt->childAdd("el")->setText("play %f");
    else TUI::cntrCmdProc(opt);
}

void TVision::postMess( const QString &cat, const QString &mess, TVision::MessLev type, QWidget *parent )
{
    //Put system message
    message(cat.toStdString().c_str(), (type==TVision::Crit) ? TMess::Crit :
			(type==TVision::Error)?TMess::Error:
			(type==TVision::Warning)?TMess::Warning:TMess::Info,"%s",mess.toStdString().c_str());
    //QT message
    switch(type)
    {
	case TVision::Info:	QMessageBox::information(parent,_(MOD_NAME),mess);	break;
	case TVision::Warning:	QMessageBox::warning(parent,_(MOD_NAME),mess);		break;
	case TVision::Error:	QMessageBox::critical(parent,_(MOD_NAME),mess);		break;
	case TVision::Crit:	QErrorMessage::qtHandler()->showMessage(mess);		break;
    }
}

int TVision::cntrIfCmd( XMLNode &node, const string &user, const string &password, const string &VCAStat, bool glob )
{
    //Check for local VCAEngine path
    if(!glob) node.setAttr("path","/UI/VCAEngine"+node.attr("path"));

    //Local station request
    if(VCAStat.empty() || VCAStat == ".") {
	node.setAttr("user",user);
	SYS->cntrCmd(&node);
	return s2i(node.attr("rez"));
    }

    //Request remote host
    try {
	TTransportS::ExtHost host = SYS->transport().at().extHostGet("*",VCAStat);
	AutoHD<TTransportOut> tr = SYS->transport().at().extHost(host,"UIVision");
	if(!tr.at().startStat()) tr.at().start();

	bool trUser = (user.empty()||user==host.user);
	node.setAttr("rqDir",trUser?"0":"1")->
	    setAttr("rqUser",trUser?host.user:user)->
	    setAttr("rqPass",trUser?host.pass:password);
	tr.at().messProtIO(node,"SelfSystem");

	return s2i(node.attr("rez"));
    }
    catch(TError err) {
	node.childClear();
	node.setAttr("rez", "10");
	node.setAttr("mcat", err.cat);
	node.setText(err.mess);
	return 10;
    }
}

QWidget *TVision::getFocusedWdg( QWidget *wcntr )
{
    while(wcntr->focusProxy()) wcntr = wcntr->focusProxy();

    return wcntr;
}

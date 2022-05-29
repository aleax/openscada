
//OpenSCADA module UI.VISION file: tvision.cpp
/***************************************************************************
 *   Copyright (C) 2006-2022 by Roman Savochenko (roman@oscada.org)
 *                 2005-2006 by Evgen Zaichuk
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

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QErrorMessage>

#include "vis_run.h"
#include "vis_shapes.h"
#include "vis_shape_elfig.h"
#include "vis_widgs.h"
#include "vis_devel.h"

//#include <tmess.h>

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"Vision"
#define MOD_NAME	trS("Operation user interface (Qt)")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define SUB_TYPE	"Qt"
#define MOD_VER		"8.0.23"
#define AUTHORS		trS("Roman Savochenko, Maxim Lysenko (2006-2012), Kseniya Yashina (2006-2007), Evgen Zaichuk (2005-2006)")
#define DESCRIPTION	trS("Visual operation user interface, based on the Qt library - front-end to the VCA engine.")
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
TVision::TVision( string name ) : TUI(MOD_ID), mVCAStation(dataRes()), mUserStart(dataRes()), mUserPass(dataRes()),
    mExitLstRunPrjCls(true), mDropCommonWdgStls(true), mEndRun(false), mRestTime(30), mCachePgLife(1), mCachePgSz(10), mScrnCnt(0)
{
    mVCAStation = ".";
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    //Export functions
    modFuncReg(new ExpFunc("QIcon icon();","Module Qt-icon",(void(TModule::*)( )) &TVision::icon));
    modFuncReg(new ExpFunc("QMainWindow *openWindow();","Start Qt GUI.",(void(TModule::*)( )) &TVision::openWindow));
}

TVision::~TVision( )
{
    //Free widget's shapes
    for(unsigned iSw = 0; iSw < shapesWdg.size(); iSw++)
	delete shapesWdg[iSw];
	//shapesWdg[iSw]->deleteLater();	//!!!! Goes to crashes at the module disconnection
    shapesWdg.clear();

    if(!SYS->stopSignal()) TSYS::sysSleep(5);
}

string TVision::optDescr( )
{
    return TSYS::strMess(_(
	"======================= Module <%s:%s> options =======================\n"
	"---- Parameters of the module section '%s' of the configuration file ----\n"
	"StartUser  <user>       Start-up, no-password, user.\n"
	"UserPass   <pass>       User password for non-local start.\n"
	"RunPrjs    <list>       List of projects to be launched at the start of the module.\n"
	"ExitLstRunPrjCls <0|1>  Exit closing the last completed project (by default 1).\n"
	"DropCommonWdgStls <0|1> Reset widget styles to common for some specific widgets in runtime, like to buttons (default 1).\n"
	"CachePgLife <hours>     Lifetime of the pages in the cache (by default 1).\n"
	"CachePgSz  <numb>       Maximum number of the pages in the cache (by default 10).\n"
	"VCAstation <id>         The station with the VCA engine ('.' is local).\n"
	"RestoreTime <seconds>   Connection recovery time.\n\n"),
	MOD_TYPE, MOD_ID, nodePath().c_str());
}

void TVision::load_( )
{
    mess_debug(nodePath().c_str(), _("Loading the module."));

    //Load parameters from command line

    //Load parameters from config-file and DB
    setUserStart(TBDS::genPrmGet(nodePath()+"StartUser",""));
    setUserPass(TBDS::genPrmGet(nodePath()+"UserPass",""));
    setRunPrjs(TBDS::genPrmGet(nodePath()+"RunPrjs",""));
    setExitLstRunPrjCls(s2i(TBDS::genPrmGet(nodePath()+"ExitLstRunPrjCls",i2s(exitLstRunPrjCls()))));
    setDropCommonWdgStls(s2i(TBDS::genPrmGet(nodePath()+"DropCommonWdgStls",i2s(dropCommonWdgStls()))));
    setCachePgLife(s2r(TBDS::genPrmGet(nodePath()+"CachePgLife",r2s(cachePgLife()))));
    setCachePgSz(s2i(TBDS::genPrmGet(nodePath()+"CachePgSz",i2s(cachePgSz()))));
    setVCAStation(TBDS::genPrmGet(nodePath()+"VCAstation","."));
    setRestoreTime(s2i(TBDS::genPrmGet(nodePath()+"RestoreTime",i2s(restoreTime()))));
}

void TVision::save_( )
{
    mess_debug(nodePath().c_str(),_("Saving the module."));

    //Save parameters to DB
    TBDS::genPrmSet(nodePath()+"StartUser", userStart());
    TBDS::genPrmSet(nodePath()+"UserPass", userPass());
    TBDS::genPrmSet(nodePath()+"RunPrjs", runPrjs());
    TBDS::genPrmSet(nodePath()+"ExitLstRunPrjCls", i2s(exitLstRunPrjCls()));
    TBDS::genPrmSet(nodePath()+"DropCommonWdgStls", i2s(dropCommonWdgStls()));
    TBDS::genPrmSet(nodePath()+"CachePgLife", r2s(cachePgLife()));
    TBDS::genPrmSet(nodePath()+"CachePgSz", i2s(cachePgSz()));
    TBDS::genPrmSet(nodePath()+"VCAstation", VCAStation());
    TBDS::genPrmSet(nodePath()+"RestoreTime",i2s(restoreTime()));
}

void TVision::postEnable( int flag )
{
   TModule::postEnable(flag);
}

string TVision::uiPropGet( const string &prop, const string &user )
{
    MtxAlloc res(mnWindsRes, true);

    XMLNode prmNd;
    try {
	prmNd.load(TBDS::genPrmGet(nodePath()+"uiProps","",user));
	return prmNd.attr(prop);
    } catch(TError &err) { }

    return "";
}

void TVision::uiPropSet( const string &prop, const string &vl, const string &user )
{
    MtxAlloc res(mnWindsRes, true);

    XMLNode prmNd("UI");
    try { prmNd.load(TBDS::genPrmGet(nodePath()+"uiProps","",user)); }
    catch(TError &err)	{ }
    prmNd.setAttr(prop,vl);
    TBDS::genPrmSet(nodePath()+"uiProps",prmNd.save(XMLNode::BrAllPast),user);
}

QIcon TVision::icon( )
{
    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("UI.Vision",NULL,true).c_str())) ico_t.load(":/images/vision.png");
    return QPixmap::fromImage(ico_t);
}

QMainWindow *TVision::openWindow( )
{
    //Get allowed screens count
#if QT_VERSION >= 0x050000
    mScrnCnt = QApplication::screens().size();
#elif QT_VERSION >= 0x040600
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

    string user_open = userStart();
    string user_pass = userPass();

    //Check for start user set OK
    int err = 0;
    XMLNode req("get"); req.setAttr("path", "/%2fgen%2fid");
    if(!((VCAStation() == "." && SYS->security().at().usrPresent(userStart())) ||
	    (VCAStation() != "." && !(err=mod->cntrIfCmd(req,userStart(),userPass(),VCAStation(),true)))))
										//!!! But for remote same the request has the athentification
	while(true) {
	    if(err == TError::Tr_Connect) { postMess(nodePath().c_str(),_("Error connecting to remote station!")); return NULL; }
	    DlgUser d_usr(userStart().c_str(), userPass().c_str(), VCAStation().c_str());
	    int rez = d_usr.exec();
	    if(rez == DlgUser::SelCancel) return NULL;
	    if(rez == DlgUser::SelErr) {
		postMess(nodePath().c_str(), QString(_("Error authenticating the user '%1'!!!")).arg(d_usr.user()));
		continue;
	    }
	    user_open = d_usr.user().toStdString();
	    user_pass = d_usr.password().toStdString();
	    break;
	}
    if(!user_open.size()) user_open = req.attr("user");

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

	// Check and extract for "ses_" or "proj_" prefixes
	bool isSess = false;
	if(sprj.compare(0,4,"ses_") == 0) { isSess = true; sprj.erase(0,4); }
	else if(sprj.compare(0,4,"prj_") == 0) sprj.erase(0,4);

	//QDesktopWidget().screen(1)
	// Find for already opened run window
	MtxAlloc res(mnWindsRes, true);
	bool openRunOK = false;
	for(unsigned iW = 0; !openRunOK && iW < mnWinds.size(); iW++)
	    openRunOK = (qobject_cast<VisRun*>(mnWinds[iW]) &&
		    ((isSess && ((VisRun*)mnWinds[iW])->workSess() == sprj) || (!isSess && ((VisRun*)mnWinds[iW])->srcProject() == sprj)) &&
		    ((VisRun*)mnWinds[iW])->screen() == screen);
	res.unlock();
	if(openRunOK) continue;

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
    mess_debug(nodePath().c_str(),_("Starting the module."));

    mEndRun = false;
    runSt  = true;
}

void TVision::modStop( )
{
    mess_debug(nodePath().c_str(),_("Stopping the module."));

    mEndRun = true;

    MtxAlloc res(mnWindsRes, true);
    for(unsigned iW = 0; iW < mnWinds.size(); iW++)
	while(mnWinds[iW]) {
	    res.unlock();
	    if(!SYS->mainThr.freeStat()) qApp->processEvents();	//!!!! Else can lock here the main thread, especially at child windows open
	    TSYS::sysSleep(prmWait_DL);
	    res.lock();
	}
    TSYS::sysSleep(prmWait_DL);

    runSt = false;
}

WdgShape *TVision::getWdgShape( const string &iid )
{
    for(unsigned iSw = 0; iSw < shapesWdg.size(); iSw++)
	if(shapesWdg[iSw]->id() == iid)
	    return shapesWdg[iSw];

    return NULL;
}

void TVision::regWin( QMainWindow *mwd )
{
    MtxAlloc res(mnWindsRes, true);
    unsigned iW;
    for(iW = 0; iW < mnWinds.size(); iW++)
	if(mnWinds[iW] == NULL) break;
    if(iW == mnWinds.size()) mnWinds.push_back((QMainWindow*)NULL);
    mnWinds[iW] = mwd;
}

void TVision::unregWin( QMainWindow *mwd )
{
    MtxAlloc res(mnWindsRes, true);
    for(unsigned iW = 0; iW < mnWinds.size(); iW++)
	if(mnWinds[iW] == mwd) mnWinds[iW] = NULL;
}

void TVision::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	ctrMkNode("fld",opt,-1,"/prm/st/disp_n",_("Number of individual displays"),R_R_R_,"root",SUI_ID,1,"tp","dec");
	ctrMkNode("list",opt,-1,"/prm/st/mnWinds",_("Main windows"),R_R_R_,"root",SUI_ID);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options"))) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/stationVCA",_("Station of the VCA engine"),RWRWR_,"root",SUI_ID,4,"tp","str","idm","1","dest","select","select","/prm/cfg/vca_lst");
	    ctrMkNode("comm",opt,-1,"/prm/cfg/host_lnk",_("Go to the remote stations list configuration"),RWRW__,"root",SUI_ID,1,"tp","lnk");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/start_user",_("Starting user"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/prm/cfg/u_lst");
	    if(VCAStation() != ".") {
		ctrMkNode("fld",opt,-1,"/prm/cfg/u_pass",_("Password of the user"),RWRWR_,"root",SUI_ID,1,"tp","str");
		ctrMkNode("fld",opt,-1,"/prm/cfg/restTm",_("Connection recovery time, seconds"),RWRWR_,"root",SUI_ID,3,"tp","dec","min","1","max","1000");
	    }
	    ctrMkNode("fld",opt,-1,"/prm/cfg/cachePgLife",_("Lifetime and maximum number of the cached pages"),RWRWR_,"root",SUI_ID,2,"tp","real",
		"help",_("The time is specified in hours, which defines the inactivity interval for closing pages in the cache.\n"
			"Zero value of the time excludes the closing of pages in the cache."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/cachePgSz","",RWRWR_,"root",SUI_ID,2,"tp","dec",
		"help",_("The number defines a limit of pages in the cache.\n"
			"Zero value of the number excludes the cache limit."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/run_prj",_("List of the projects for launch"),RWRWR_,"root",SUI_ID,4,"tp","str","dest","sel_ed","select","/prm/cfg/r_lst",
		"help",_("Automatically executed projects, divided by the symbol ';'.\n"
			 "To open the project window on the desired display (1), use the project name format: 'PrjName-1'.\n"
			 "To connect to the background or another open session use \"ses_{SesID}\"."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/exit_on_lst_run_prj_cls",_("Exit when closing the last running project"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/drop_common_wdg_stls",_("Reset widget styles to common"),RWRWR_,"root",SUI_ID,2,"tp","bool",
		"help",_("Required for some widget styles like \"gtk\" in some specific widgets in run mode, like buttons for the background color and images."));
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/disp_n" && ctrChkNode(opt))		opt->setText(i2s(mScrnCnt));
    else if(a_path == "/prm/st/mnWinds" && ctrChkNode(opt)) {
	string rez;
	MtxAlloc res(mnWindsRes, true);
	for(unsigned iW = 0; iW < mnWinds.size(); iW++)
	    if(dynamic_cast<VisDevelop*>(mnWinds[iW]))	opt->childAdd("el")->setText(TSYS::strMess(_("%d: Development by \"%s\"."),iW,((VisDevelop*)mnWinds[iW])->user().c_str()));
	    else if(dynamic_cast<VisRun*>(mnWinds[iW]))	{
		opt->childAdd("el")->setText(TSYS::strMess(_("%d: Running \"%s:%s\" from \"%s\" - %s, updating period %s(%s), cached pages %d and resources %d."),iW,
		    ((VisRun*)mnWinds[iW])->workSess().c_str(),
		    ((VisRun*)mnWinds[iW])->srcProject().c_str(),
		    ((VisRun*)mnWinds[iW])->user().c_str(),
		    ((VisRun*)mnWinds[iW])->connOK()?_("Connected"):_("Disconnected"),
		    tm2s(((VisRun*)mnWinds[iW])->planePer*1e-3).c_str(),tm2s(((VisRun*)mnWinds[iW])->period()*1e-3).c_str(),
		    ((VisRun*)mnWinds[iW])->cachePgSz(),((VisRun*)mnWinds[iW])->cacheResSz()));
	    }
    }
    else if(a_path == "/prm/cfg/start_user") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(userStart());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setUserStart(opt->text());
    }
    else if(a_path == "/prm/cfg/u_pass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(string(userPass().size(),'*')/* "*******" */);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(opt->text().find(SEC_HASH_MAGIC) == 0)
		setUserPass(opt->text().substr(strlen(SEC_HASH_MAGIC)));
	    else setUserPass(opt->text());
	}
    }
    else if(a_path == "/prm/cfg/restTm") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(restoreTime()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setRestoreTime(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/cachePgLife") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(r2s(cachePgLife()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCachePgLife(s2r(opt->text()));
    }
    else if(a_path == "/prm/cfg/cachePgSz") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(cachePgSz()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCachePgSz(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/run_prj") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(runPrjs());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setRunPrjs(opt->text());
    }
    else if(a_path == "/prm/cfg/r_lst" && ctrChkNode(opt)) {
	string rPrjs = runPrjs();
	// Sessions and projects list request
	XMLNode req("CntrReqs");
	req.childAdd("get")->setAttr("path", "%2fses%2fses")->setAttr("chkUserPerm", "1");
	req.childAdd("get")->setAttr("path", "%2fprm%2fcfg%2fprj")->setAttr("chkUserPerm", "1");
	cntrIfCmd(req, userStart(), userPass(), VCAStation());
	XMLNode *reqN = req.childGet(0);
	for(unsigned iCh = 0; iCh < reqN->childSize(); iCh++)
	    opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+"ses_"+reqN->childGet(iCh)->text());
	    /*if(SYS->security().at().access(userStart(),SEC_WR,"root","root",RWRWR_) ||
		    reqN->childGet(iCh)->attr("user") == userStart())
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+"ses_"+reqN->childGet(iCh)->text());*/
	reqN = req.childGet(1);
	for(unsigned iCh = 0; iCh < reqN->childSize(); iCh++)
	    opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+reqN->childGet(iCh)->attr("id"));
	    /*if(SYS->security().at().access(userStart(),SEC_WR,"root","root",RWRWR_))
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+reqN->childGet(iCh)->attr("id"));*/
    }
    else if(a_path == "/prm/cfg/exit_on_lst_run_prj_cls") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(exitLstRunPrjCls()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setExitLstRunPrjCls(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/drop_common_wdg_stls") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(dropCommonWdgStls()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setDropCommonWdgStls(s2i(opt->text()));
    }
    else if(a_path == "/prm/cfg/stationVCA") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(VCAStation());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	{ setVCAStation(opt->text()); setUserStart(""); setUserPass(""); }
    }
    else if(a_path == "/prm/cfg/host_lnk" && ctrChkNode(opt,"get",RWRW__,"root",SUI_ID,SEC_RD)) opt->setText("/Transport");
    else if(a_path == "/prm/cfg/u_lst" && ctrChkNode(opt)) {
	XMLNode req("get");
	req.setAttr("path","/Security/%2fusgr%2fusers");
	opt->childAdd("el")->setText("");
	if(!mod->cntrIfCmd(req,"","",VCAStation(),true))
	    for(unsigned iU = 0; iU < req.childSize(); iU++)
		opt->childAdd("el")->setText(req.childGet(iU)->text());
    }
    else if(a_path == "/prm/cfg/vca_lst" && ctrChkNode(opt)) {
	opt->childAdd("el")->setAttr("id",".")->setText("Local");
	vector<TTransportS::ExtHost> lst;
	SYS->transport().at().extHostList("*", lst);
	for(unsigned iLs = 0; iLs < lst.size(); iLs++)
	    opt->childAdd("el")->setAttr("id", lst[iLs].id)->setText(lst[iLs].name);
    }
    else TUI::cntrCmdProc(opt);
}

void TVision::postMess( const QString &cat, const QString &mess, TVision::MessLev type, QWidget *parent )
{
    //Put system message
    message(cat.toStdString().c_str(), (type==TVision::Crit) ? TMess::Crit :
			(type==TVision::Error)?TMess::Error:
			(type==TVision::Warning)?TMess::Warning:TMess::Info,"%s",mess.toStdString().c_str());

    //QT message
    switch(type) {
	case TVision::Info:	QMessageBox::information(parent, MOD_NAME.c_str(), mess);break;
	case TVision::Warning:	QMessageBox::warning(parent, MOD_NAME.c_str(), mess);	break;
	case TVision::Error:	QMessageBox::critical(parent, MOD_NAME.c_str(), mess);	break;
	case TVision::Crit:	QErrorMessage::qtHandler()->showMessage(mess);	break;
    }
}

int TVision::cntrIfCmd( XMLNode &node, const string &user, const string &password, const string &VCAStat, bool glob )
{
    //Check for local VCAEngine path
    if(!glob) node.setAttr("path", "/UI/VCAEngine"+node.attr("path"));
    bool isLoc = (VCAStat.empty() || VCAStat == ".");
    node.setAttr("path", "/"+(isLoc?SYS->id():VCAStat)+node.attr("path"));
    if(!isLoc) node.setAttr("lang", Mess->langCode(user));

    try {
	int rez = SYS->transport().at().cntrIfCmd(node, "UIVision", (isLoc?user:("\n"+user+"\n"+password)));
	//Password's hash processing
	if(node.attr("pHash").size() && userStart() == user && userPass() != (SEC_HASH_MAGIC+node.attr("pHash"))) {
	    setUserPass(SEC_HASH_MAGIC + node.attr("pHash"));
	    node.setAttr("pHash", "");
	}
	return rez;
    } catch(TError &err) {
	node.childClear();
	node.setAttr("mcat", err.cat)->setAttr("rez", i2s(TError::Tr_Connect))->setText(err.mess);
    }

    return s2i(node.attr("rez"));
}

QWidget *TVision::getFocusedWdg( QWidget *wcntr )
{
    while(wcntr->focusProxy()) wcntr = wcntr->focusProxy();

    return wcntr;
}

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TVision::modInfo( const string &name )
{
    if(name == "SubType")	return SUB_TYPE;

    return TModule::modInfo(name);
}

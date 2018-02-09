
//OpenSCADA system module UI.VISION file: tvision.cpp
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2018 by Roman Savochenko (rom_as@oscada.org)
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

#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QErrorMessage>

#include <tsys.h>
#include <tmess.h>

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
#define MOD_VER		"4.11.0"
#define AUTHORS		_("Roman Savochenko, Maxim Lysenko (2006-2012), Kseniya Yashina (2006-2007), Evgen Zaichuk (2005-2006)")
#define DESCRIPTION	_("Visual operation user interface, based on Qt library - front-end to VCA engine.")
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
    mExitLstRunPrjCls(true), mEndRun(false), mRestTime(30), mCachePgLife(1), mScrnCnt(0)
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
    for(unsigned i_sw = 0; i_sw < shapesWdg.size(); i_sw++) delete shapesWdg[i_sw];
    shapesWdg.clear();
}

string TVision::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= Module <%s:%s> options =======================\n"
	"---- Parameters of the module section '%s' of the configuration file ----\n"
	"StartUser  <user>       Start-up, no-password, user.\n"
	"UserPass   <pass>       User password for non-local start.\n"
	"RunPrjs    <list>       List of projects to be launched at the start of the module.\n"
	"ExitLstRunPrjCls <0|1>  Exit closing the last completed project (default = 1).\n"
	"CachePgLife <hours>     The lifetime of pages in the cache.\n"
	"VCAstation <id>         The station with the VCA engine ('.' Is local).\n"
	"RestoreTime <seconds>   Connection recovery time.\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TVision::load_( )
{
    mess_debug(nodePath().c_str(),_("Load module."));

    //Load parameters from command line
    if(s2i(SYS->cmdOpt("h")) || s2i(SYS->cmdOpt("help"))) fprintf(stdout, "%s", optDescr().c_str());

    //Load parameters from config-file and DB
    setUserStart(TBDS::genDBGet(nodePath()+"StartUser",""));
    setUserPass(TBDS::genDBGet(nodePath()+"UserPass",""));
    setRunPrjs(TBDS::genDBGet(nodePath()+"RunPrjs",""));
    setExitLstRunPrjCls(s2i(TBDS::genDBGet(nodePath()+"ExitLstRunPrjCls",i2s(exitLstRunPrjCls()))));
    setCachePgLife(s2r(TBDS::genDBGet(nodePath()+"CachePgLife",r2s(cachePgLife()))));
    setVCAStation(TBDS::genDBGet(nodePath()+"VCAstation","."));
    setRestoreTime(s2i(TBDS::genDBGet(nodePath()+"RestoreTime",i2s(restoreTime()))));
}

void TVision::save_( )
{
    mess_debug(nodePath().c_str(),_("Save module."));

    //Save parameters to DB
    TBDS::genDBSet(nodePath()+"StartUser", userStart());
    TBDS::genDBSet(nodePath()+"UserPass", userPass());
    TBDS::genDBSet(nodePath()+"RunPrjs", runPrjs());
    TBDS::genDBSet(nodePath()+"ExitLstRunPrjCls", i2s(exitLstRunPrjCls()));
    TBDS::genDBSet(nodePath()+"CachePgLife", r2s(cachePgLife()));
    TBDS::genDBSet(nodePath()+"VCAstation", VCAStation());
    TBDS::genDBSet(nodePath()+"RestoreTime",i2s(restoreTime()));
}

void TVision::postEnable( int flag )
{
   TModule::postEnable(flag);
}

string TVision::uiPropGet( const string &prop, const string &user )
{
    MtxAlloc res(dataRes(), true);

    XMLNode prmNd;
    try {
	prmNd.load(TBDS::genDBGet(nodePath()+"uiProps","",user));
	return prmNd.attr(prop);
    } catch(TError &err) { }

    return "";
}

void TVision::uiPropSet( const string &prop, const string &vl, const string &user )
{
    MtxAlloc res(dataRes(), true);

    XMLNode prmNd("UI");
    try { prmNd.load(TBDS::genDBGet(nodePath()+"uiProps","",user)); }
    catch(TError &err)	{ }
    prmNd.setAttr(prop,vl);
    TBDS::genDBSet(nodePath()+"uiProps",prmNd.save(XMLNode::BrAllPast),user);
}

QIcon TVision::icon( )
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

    string user_open = userStart();
    string user_pass = userPass();

    //Check for start user set OK
    int err = 0;
    XMLNode req("get"); req.setAttr("path", "/%2fgen%2fid");
    if(!((VCAStation() == "." && SYS->security().at().usrPresent(userStart())) ||
	    (VCAStation() != "." && !(err=mod->cntrIfCmd(req,userStart(),userPass(),VCAStation(),true)))))
										//!!! But for remote same the request has the athentification
	while(true) {
	    if(err == 10)	{ postMess(nodePath().c_str(),_("Error connection to remote station!")); return NULL; }
	    DlgUser d_usr(userStart().c_str(), userPass().c_str(), VCAStation().c_str());
	    int rez = d_usr.exec();
	    if(rez == DlgUser::SelCancel) return NULL;
	    if(rez == DlgUser::SelErr) {
		postMess(nodePath().c_str(), QString(_("Authentication error for user '%1'!!!")).arg(d_usr.user()));
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
	MtxAlloc res(dataRes(), true);
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
    mess_debug(nodePath().c_str(),_("Start module."));

    mEndRun = false;
    runSt  = true;
}

void TVision::modStop( )
{
    mess_debug(nodePath().c_str(),_("Stop module."));

    mEndRun = true;

    MtxAlloc res(dataRes(), true);
    for(unsigned i_w = 0; i_w < mnWinds.size(); i_w++)
	while(mnWinds[i_w]) {
	    res.unlock();
	    TSYS::sysSleep(STD_WAIT_DELAY*1e-3);
	    res.lock();
	}
    TSYS::sysSleep(STD_WAIT_DELAY*1e-3);

    runSt = false;
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
    MtxAlloc res(dataRes(), true);
    unsigned i_w;
    for(i_w = 0; i_w < mnWinds.size(); i_w++)
	if(mnWinds[i_w] == NULL) break;
    if(i_w == mnWinds.size()) mnWinds.push_back((QMainWindow*)NULL);
    mnWinds[i_w] = mwd;
}

void TVision::unregWin( QMainWindow *mwd )
{
    MtxAlloc res(dataRes(), true);
    for(unsigned i_w = 0; i_w < mnWinds.size(); i_w++)
	if(mnWinds[i_w] == mwd) mnWinds[i_w] = NULL;
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
	    ctrMkNode("fld",opt,-1,"/prm/cfg/cachePgLife",_("Lifetime of the cached pages"),RWRWR_,"root",SUI_ID,2,"tp","real",
		"help",_("The time is specified in hours, which defines the inactivity interval for closing pages in the cache.\nA zero value of time excludes the closing of pages in the cache."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/run_prj",_("List of the projects for launch"),RWRWR_,"root",SUI_ID,4,"tp","str","dest","sel_ed","select","/prm/cfg/r_lst",
		"help",_("Automatically executed projects, divided by the symbol ';'.\n"
			 "To open the project window on the desired display (1), use the project name format: 'PrjName-1'.\n"
			 "To connect to the background or another open session use \"ses_{SesID}\"."));
	    ctrMkNode("fld",opt,-1,"/prm/cfg/exit_on_lst_run_prj_cls",_("Exit when closing the last running project"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/st/disp_n" && ctrChkNode(opt))		opt->setText(i2s(mScrnCnt));
    else if(a_path == "/prm/st/mnWinds" && ctrChkNode(opt)) {
	string rez;
	MtxAlloc res(dataRes(), true);
	for(unsigned iW = 0; iW < mnWinds.size(); iW++)
	    if(dynamic_cast<VisDevelop*>(mnWinds[iW]))	opt->childAdd("el")->setText(TSYS::strMess(_("%d: Development by \"%s\""),iW,((VisDevelop*)mnWinds[iW])->user().c_str()));
	    else if(dynamic_cast<VisRun*>(mnWinds[iW]))	{
		opt->childAdd("el")->setText(TSYS::strMess(_("%d: Running \"%s:%s\" from \"%s\" - %s"),iW,
		    ((VisRun*)mnWinds[iW])->workSess().c_str(),
		    ((VisRun*)mnWinds[iW])->srcProject().c_str(),
		    ((VisRun*)mnWinds[iW])->user().c_str(),
		    ((VisRun*)mnWinds[iW])->connOK()?_("Connected"):_("Disconnected")));
	    }
    }
    else if(a_path == "/prm/cfg/start_user") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(userStart());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setUserStart(opt->text());
    }
    else if(a_path == "/prm/cfg/u_pass") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(string(userPass().size(),'*')/* "*******" */);
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(opt->text().compare(0,TSecurity::pHashMagic.size(),TSecurity::pHashMagic) == 0)
		setUserPass(opt->text().substr(TSecurity::pHashMagic.size()));
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
	for(unsigned i_ch = 0; i_ch < reqN->childSize(); i_ch++)
	    opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+"ses_"+reqN->childGet(i_ch)->text());
	    /*if(SYS->security().at().access(userStart(),SEC_WR,"root","root",RWRWR_) ||
		    reqN->childGet(i_ch)->attr("user") == userStart())
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+"ses_"+reqN->childGet(i_ch)->text());*/
	reqN = req.childGet(1);
	for(unsigned i_ch = 0; i_ch < reqN->childSize(); i_ch++)
	    opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+reqN->childGet(i_ch)->attr("id"));
	    /*if(SYS->security().at().access(userStart(),SEC_WR,"root","root",RWRWR_))
		opt->childAdd("el")->setText((rPrjs.size()?rPrjs+";":"")+reqN->childGet(i_ch)->attr("id"));*/
    }
    else if(a_path == "/prm/cfg/exit_on_lst_run_prj_cls") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(exitLstRunPrjCls()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setExitLstRunPrjCls(s2i(opt->text()));
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
	case TVision::Info:	QMessageBox::information(parent, _(MOD_NAME), mess);	break;
	case TVision::Warning:	QMessageBox::warning(parent, _(MOD_NAME), mess);	break;
	case TVision::Error:	QMessageBox::critical(parent, _(MOD_NAME), mess);	break;
	case TVision::Crit:	QErrorMessage::qtHandler()->showMessage(mess);		break;
    }
}

int TVision::cntrIfCmd( XMLNode &node, const string &user, const string &password, const string &VCAStat, bool glob )
{
    //Check for local VCAEngine path
    if(!glob) node.setAttr("path", "/UI/VCAEngine"+node.attr("path"));
    bool isLoc = (VCAStat.empty() || VCAStat == ".");
    node.setAttr("path", "/"+(isLoc?SYS->id():VCAStat)+node.attr("path"));

    try {
	int rez = SYS->transport().at().cntrIfCmd(node, "UIVision", (isLoc?user:("\n"+user+"\n"+password)));
	//Password's hash processing
	if(node.attr("pHash").size() && userStart() == user && userPass() != (TSecurity::pHashMagic+node.attr("pHash"))) {
	    setUserPass(TSecurity::pHashMagic + node.attr("pHash"));
	    node.setAttr("pHash", "");
	}
	return rez;
    } catch(TError &err) {
	node.childClear();
	node.setAttr("mcat", err.cat)->setAttr("rez", "10")->setText(err.mess);
    }

    return s2i(node.attr("rez"));
}

QWidget *TVision::getFocusedWdg( QWidget *wcntr )
{
    while(wcntr->focusProxy()) wcntr = wcntr->focusProxy();

    return wcntr;
}

#undef _
#define _(mess) mod->I18N(mess, lang.c_str())

void TVision::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TVision::modInfo( const string &iname )
{
    string  name = TSYS::strParse(iname, 0, ":"),
	    lang = TSYS::strParse(iname, 1, ":");

    if(name == "SubType")	return SUB_TYPE;

    if(lang.size()) {
	if(name == "Name")	return _("Operation user interface (Qt)");
	if(name == "Author")	return _("Roman Savochenko, Maxim Lysenko (2006-2012), Kseniya Yashina (2006-2007), Evgen Zaichuk (2005-2006)");
	if(name == "Description") return _("Visual operation user interface, based on Qt library - front-end to VCA engine.");
    }

    return TModule::modInfo(name);
}

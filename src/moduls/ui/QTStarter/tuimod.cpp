
//OpenSCADA module UI.QTStarter file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2005-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <dirent.h>
#include <sys/stat.h>

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QImage>
#include <QPushButton>
#include <QLayout>
#include <QFrame>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QTimer>
#include <QLocale>
#include <QMessageBox>
#include <QWhatsThis>
#include <QLabel>
#include <QSplashScreen>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QStyleFactory>

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"QTStarter"
#define MOD_NAME	_("Qt GUI starter")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#ifdef EN_QtMainThrd
#define SUB_TYPE	"MainThr"
#else
#define SUB_TYPE	""
#endif
#define MOD_VER		"4.2.1"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Provides the Qt GUI starter. Qt-starter is the only and compulsory component for all GUI modules based on the Qt library.")
#define LICENSE		"GPL2"
//*************************************************

QTStarter::TUIMod *QTStarter::mod;

extern "C"
{
#ifdef MOD_INCL
    TModule::SAt ui_QTStarter_module( int n_mod )
#else
    TModule::SAt module( int n_mod )
#endif
    {
	if(n_mod == 0)	return TModule::SAt(MOD_ID, MOD_TYPE, VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_QTStarter_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if(AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE)) return new QTStarter::TUIMod(source);
	return NULL;
    }
}

using namespace QTStarter;

//*************************************************
//* TUIMod                                        *
//*************************************************
TUIMod::TUIMod( string name ) : TUI(MOD_ID), hideMode(false), mEndRun(false), mStartCom(false), mCloseToTray(false),
    mStartMod(dataRes()), mStyle(dataRes()), mPalette(dataRes()), mStyleSheets(dataRes()), qtArgC(0), qtArgEnd(0), QtApp(NULL), splash(NULL)
{
    mod = this;

    modInfoMainSet(MOD_NAME, MOD_TYPE, MOD_VER, AUTHORS, DESCRIPTION, LICENSE, name);

    //Qt massages not for compile but for indexing by gettext
#if 0
    char mess[][100] = {
	_("Could not read image data"),
	_("&Yes"),_("&No"),_("&Cancel"),_("&OK"),_("Apply"),_("Close"),_("Back"),_("Forward"),_("Parent Directory"),
	_("Look in:"),_("Computer"),_("File"),_("Folder"),_("File &name:"),_("Open"),_("&Open"),_("Cancel"),_("Save"),_("&Save"),_("Save As"),_("Date Modified"),_("All Files (*)"),
	_("Create New Folder"),_("List View"),_("Detail View"),_("Files of type:"),_("New Folder"),_("&New Folder"),_("Show &hidden files"),_("&Delete"),_("&Rename"),_("Remove"),
	_("&Undo"),_("&Redo"),_("Cu&t"),_("&Copy"),_("&Paste"),_("Delete"),_("Select All"),_("Insert Unicode control character"),
	_("Size"),_("Drive"),_("Go back"),_("Go forward"),_("Go to the parent directory"),_("Create a New Folder"),_("Change to list view mode"),_("Change to detail view mode"),
	_("Destination file exists"),
	_("%1 bytes"),_("%1 KB"),_("%1 MB"),
	_("Are sure you want to delete '%1'?"),_("%1 already exists.\nDo you want to replace it?"),_("Recent Places"),
	_("<h3>About Qt</h3><p>This program uses Qt version %1.</p>"),
	_("<p>Qt is a C++ toolkit for cross-platform application development.</p><p>Qt provides single-source portability across MS&nbsp;Windows, Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix variants. Qt is also available for embedded devices as Qt for Embedded Linux and Qt for Windows CE.</p><p>Qt is available under three different licensing options designed to accommodate the needs of our various users.</p><p>Qt licensed under our commercial license agreement is appropriate for development of proprietary/commercial software where you do not want to share any source code with third parties or otherwise cannot comply with the terms of the GNU LGPL version 2.1 or GNU GPL version 3.0.</p><p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the development of Qt applications (proprietary or open source) provided you can comply with the terms and conditions of the GNU LGPL version 2.1.</p><p>Qt licensed under the GNU General Public License version 3.0 is appropriate for the development of Qt applications where you wish to use such applications in combination with software subject to the terms of the GNU GPL version 3.0 or where you are otherwise willing to comply with the terms of the GNU GPL version 3.0.</p><p>Please see <a href=\"http://qt.digia.com/product/licensing\">qt.digia.com/product/licensing</a> for an overview of Qt licensing.</p><p>Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).</p><p>Qt is a Digia product. See <a href=\"http://qt.digia.com/\">qt.digia.com</a> for more information.</p>"),
	_("Hu&e:"),_("&Sat:"),_("&Val:"),_("&Red:"),_("&Green:"),_("Bl&ue:"),_("A&lpha channel:"),_("&Basic colors"),_("&Custom colors"),_("&Add to Custom Colors"),_("Select color"),
	_("&Restore"),_("&Move"),_("&Size"),_("Mi&nimize"),_("Ma&ximize"),_("Stay on &Top"),_("&Close"),_("Select Color"),
	_("Form"),_("Printer"),_("&Name:"),_("P&roperties"),_("Location:"),_("Preview"),_("Type:"),_("Output &file:"),_("Print range"),_("Print all"),_("Current Page"),
	_("Pages from"),_("to"),_("Selection"),_("Output Settings"),_("Copies:"),_("Collate"),_("Reverse"),_("Copies"),_("Color Mode"),_("Color"),_("Grayscale"),
	_("Duplex Printing"),_("None"),_("Long side"),_("Short side"),_("Options"),_("&Options >>"),_("&Options <<"),_("&Print"),_("Print"),_("Print to File (PDF)"),_("Print to File (Postscript)"),
	_("Local file"),_("Write %1 file"),_("Paper"),_("Page size:"),_("Width:"),_("Height:"),_("Paper source:"),_("Orientation"),_("Portrait"),_("Landscape"),_("Reverse landscape"),
	_("Reverse portrait"),_("Margins"),_("top margin"),_("left margin"),_("right margin"),_("bottom margin"),_("Points (pt)"),_("Inches (in)"),
	_("Executive"),_("Folio"),_("Ledger"),_("Legal"),_("Letter"),_("Tabloid"),_("US Common #10 Envelope"),_("Custom"),
	_("Millimeters (mm)"),_("Centimeters (cm)"),_("Page"),_("Advanced"),
	_("Mon"),
    };
#endif

    //Look and Feels DB structure
    elLF.fldAdd(new TFld("NAME",_("Name"),TFld::String,TCfg::Key,OBJ_NM_SZ));
    elLF.fldAdd(new TFld("STYLE",_("Style"),TFld::String,0,"20"));
    elLF.fldAdd(new TFld("PALETTE",_("Palette"),TFld::String,0,"1000"));
    elLF.fldAdd(new TFld("STL_SHTS",_("Style Sheets"),TFld::String,0,"100000"));
}

TUIMod::~TUIMod( )
{
    if(runSt) modStop();
}

string TUIMod::modInfo( const string &iname )
{
    string name = TSYS::strParse(iname, 0, ":");

    if(name == "SubType") return SUB_TYPE;

    return TModule::modInfo(name);
}

void TUIMod::modInfo( vector<string> &list )
{
    TModule::modInfo(list);
    list.push_back("SubType");
}

string TUIMod::style( bool mant )	{ return mant ? mStyle.getVal() : (mStyle.getVal().size() ? mStyle.getVal() : SYS->cmdOpt("style")); }

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(!(flag&TCntrNode::NodeConnect))	return;

    //Set up Qt environments
    qtArgC = qtArgEnd = 0;
    toQtArg(SYS->cmdOpt("").c_str());
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());	//codepage for Qt across QString recode!
#endif

    //Check command line for options no help and no daemon
    hideMode = s2i(SYS->cmdOpt("h")) || s2i(SYS->cmdOpt("help")) || s2i(SYS->cmdOpt("demon")) || s2i(SYS->cmdOpt("daemon")) || s2i(SYS->cmdOpt("nox11"));
    string argCom, argVl;
    for(int off = 0; (argCom=TSYS::strParse("sync:widgetcount:qws:style:stylesheet:session:reverse:graphicssystem:display:geometry",0,":",&off)).size(); )
	if((argVl=SYS->cmdOpt(argCom)).size())	toQtArg(argCom.c_str(), argVl.c_str());

#ifdef EN_QtMainThrd
    if(hideMode) return;

    //Init locale setLocale
    QLocale::setDefault(QLocale(Mess->lang().c_str()));

    //Qt application object init
    QtApp = new StApp(mod->qtArgC, (char**)&mod->qtArgV);
    QtApp->setApplicationName(PACKAGE_STRING);
    QtApp->setQuitOnLastWindowClosed(false);

    //Create I18N translator
    I18NTranslator translator;
    QtApp->installTranslator(&translator);

    splashSet(SPLSH_START);
#else
    //Start main Qt thread if no help and no daemon
    if(!(runSt || hideMode)) {
	mEndRun = false;
	SYS->taskCreate(nodePath('.',true), 0, Task, this);
    }
#endif
}

void TUIMod::preDisable( int flag )
{
    if(SYS->stopSignal() == SIGUSR2) throw err_sys(_("Hold at reloading in different project."));
}

void TUIMod::postDisable( int flag )
{
#ifdef EN_QtMainThrd
    if(hideMode) return;

    splashSet(SPLSH_NULL);

    //Qt application object free
    if(QtApp) delete QtApp;
#else
    if(runSt) SYS->taskDestroy(nodePath('.',true), &mEndRun, 10, true);
#endif
}

void TUIMod::load_( )
{
    mess_debug(nodePath().c_str(),_("Load module."));

    //Load parameters from command line
    if(s2i(SYS->cmdOpt("h")) || s2i(SYS->cmdOpt("help")))
	fprintf(stdout, "%s", optDescr().c_str());

    //Load parameters from config-file
    setStartMod(TBDS::genDBGet(nodePath()+"StartMod",startMod()));
    setCloseToTray(s2i(TBDS::genDBGet(nodePath()+"CloseToTray",i2s(closeToTray()))));
    setStyle(TBDS::genDBGet(nodePath()+"Style",style()));
    setPalette(TBDS::genDBGet(nodePath()+"Palette",palette()));
    setStyleSheets(TBDS::genDBGet(nodePath()+"StyleSheets",styleSheets()));
}

void TUIMod::save_( )
{
    mess_debug(nodePath().c_str(),_("Save module."));

    TBDS::genDBSet(nodePath()+"StartMod", startMod());
    TBDS::genDBSet(nodePath()+"CloseToTray", i2s(closeToTray()));
    TBDS::genDBSet(nodePath()+"Style", style());
    TBDS::genDBSet(nodePath()+"Palette", palette());
    TBDS::genDBSet(nodePath()+"StyleSheets", styleSheets());
}

void TUIMod::modStart( )
{
#ifdef EN_QtMainThrd
    if(runSt || hideMode)	return;
    mess_debug(nodePath().c_str(), _("Starting the module..."));

    runSt = true;

    //Start external modules
    QtApp->stExec();

    splashSet(SPLSH_STOP);

    runSt = false;
#else
    mess_debug(nodePath().c_str(), _("Starting the module..."));

    mStartCom = true;
#endif

}

void TUIMod::modStop( )
{
#ifndef EN_QtMainThrd
    mess_debug(nodePath().c_str(), _("Stopping the module..."));

    mStartCom = false;
#endif
}

void TUIMod::splashSet( SplashFlag flg )
{
    if(flg == SPLSH_NULL) {
	if(splash) delete splash;
	splash = NULL;
    }
    else {
	QImage ico_t;
	if(!ico_t.load(TUIS::icoGet(SYS->id()+((flg==SPLSH_STOP)?"_splash_exit":"_splash"),NULL,true).c_str())) ico_t.load(":/images/splash.png");
	if(splash) splashSet(SPLSH_NULL);
	splash = new QSplashScreen(QPixmap::fromImage(ico_t));
	splash->show();
	QFont wFnt = splash->font();
	wFnt.setPixelSize(10);
	splash->setFont(wFnt);
#ifdef EN_QtMainThrd
	for(int iTr = 0; iTr < 10; iTr++) {
	    QtApp->processEvents();
	    TSYS::sysSleep(0.1);
	}
#endif
    }
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf, sizeof(buf), _(
	"======================= Module <%s:%s> options =======================\n"
	"----------- Qt debug commandline options ----------\n"
	"    --noX11             Prevent the launch of Qt, preferably for a clean console.\n"
	"    --sync              Switch to Sync X11 for debugging.\n"
	"    --widgetcount       Print debug messages at output, the number of widgets\n"
	"                        left unselected and their maximum number.\n"
	"----------- Qt commandline options ----------------\n"
	"    --qws               Do this with Qt server software for embedded Linux.\n"
	"    --style=<name>      Sets the GUI style to <name> (windows, platinum, plastique, ...).\n"
	"    --stylesheet=<path> Set the style sheet from the file by <path>.\n"
	"    --session=<name>    Restore from the previous session with the specified <name>.\n"
	"    --reverse           Set the layout in Qt::RightToLeft.\n"
	"    --graphicssystem=<nm> Install rendering mechanism for screen widgets and QPixmaps (raster, opengl).\n"
	"    --display=<nm>      Set the X display (typically $DISPLAY).\n"
	"    --geometry=<geom>   Set the client geometry of the first display window.\n"
	"---- Parameters of the module section '%s' of the configuration file ----\n"
	"StartMod   <moduls>     List of the modules that are started, separated ';'.\n"
	"CloseToTray <0|1>       Closing all windows or starting without Qt modules to the system tray.\n"
	"Style      <name>       The GUI style of Qt.\n"
	"Palette    <colors>     Twenty colors of the palette separated by symbol ',' in three lines for active, disabled and inactive groups.\n"
	"StyleSheets <CSS>       Rules of the Cascade Style Sheets.\n\n"),
	MOD_TYPE, MOD_ID, nodePath().c_str());

    return buf;
}

void TUIMod::toQtArg( const char *nm, const char *arg )
{
    string plStr = nm;
    if(qtArgC) plStr.insert(0, "-");

    //Name process
    if(qtArgC >= (int)(sizeof(qtArgV)/sizeof(char*)) || (qtArgEnd+plStr.size()+1) > sizeof(qtArgBuf)) return;
    strcpy(qtArgBuf+qtArgEnd, plStr.c_str());
    qtArgV[qtArgC++] = qtArgBuf+qtArgEnd;
    qtArgEnd += plStr.size()+1;

    //Argument process
    if(arg) {
	plStr = arg;
	if(qtArgC >= (int)(sizeof(qtArgV)/sizeof(char*)) || (qtArgEnd+plStr.size()+1) > sizeof(qtArgBuf)) return;
	strcpy(qtArgBuf+qtArgEnd, plStr.c_str());
	qtArgV[qtArgC++] = qtArgBuf+qtArgEnd;
	qtArgEnd += plStr.size()+1;
    }
}

#ifndef EN_QtMainThrd
void *TUIMod::Task( void * )
{
    vector<string> list;
    QImage ico_t;
    time_t st_time = time(NULL);
    vector<TMess::SRec> recs;

    //Init locale setLocale
    QLocale::setDefault(QLocale(Mess->lang().c_str()));


    //Qt application object init
    mod->QtApp = new StApp(mod->qtArgC, (char**)&mod->qtArgV);
    mod->QtApp->setApplicationName(PACKAGE_STRING);
    mod->QtApp->setQuitOnLastWindowClosed(false);
    mod->runSt = true;

    //Create I18N translator
    I18NTranslator translator;
    mod->QtApp->installTranslator(&translator);

    ret:
    mod->splashSet(SPLSH_START);
    while(!mod->startCom() && !mod->endRun()) {
	SYS->archive().at().messGet(st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM);
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess += QString("\n%1").arg(recs[i_m].mess.c_str());
	recs.clear();
	mod->splash->showMessage(mess, Qt::AlignBottom|Qt::AlignLeft);
	mod->QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }

    mod->QtApp->stExec();

    if(SYS->stopSignal() == SIGUSR2) { mod->mStartCom = false; goto ret; }

    mod->splashSet(SPLSH_STOP);
    st_time = time(NULL);
    while(!mod->endRun()) {
	SYS->archive().at().messGet(st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM);
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess += QString("\n%1").arg(recs[i_m].mess.c_str());
	recs.clear();
	mod->splash->showMessage(mess,Qt::AlignBottom|Qt::AlignLeft);
	mod->QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }
    mod->splashSet(SPLSH_NULL);

    //Qt application object free
    delete mod->QtApp; mod->QtApp = NULL;

    mod->runSt = false;

    return NULL;
}
#endif

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,-1,"/prm/cfg",_("Module options"))) {
	    ctrMkNode("fld",opt,-1,"/prm/cfg/st_mod",_("Qt modules for startup, separated by ';'"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/prm/cfg/lsQtMod");
	    ctrMkNode("fld",opt,-1,"/prm/cfg/closeToTray",_("Collapse or startup to the system tray"),RWRWR_,"root",SUI_ID,1,"tp","bool");
	    if(ctrMkNode("area",opt,-1,"/prm/LF",_("Look and feels"))) {
		ctrMkNode("fld",opt,-1,"/prm/LF/prfl",_("Known profiles"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","select","select","/prm/LF/prflLs");
		ctrMkNode("fld",opt,-1,"/prm/LF/stl",_("Widgets style"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/prm/LF/stlLs");
		ctrMkNode("fld",opt,-1,"/prm/LF/plt",_("Palette"),RWRWR_,"root",SUI_ID,3,"tp","str","rows","4","SnthHgl","1");
		ctrMkNode("fld",opt,-1,"/prm/LF/stlSheets",_("Style Sheets"),RWRWR_,"root",SUI_ID,3,"tp","str","rows","5","SnthHgl","1");
	    }
	}
	return;
    }

    //Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/st_mod") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startMod());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartMod(opt->text());
    }
    else if(a_path == "/prm/cfg/lsQtMod" && ctrChkNode(opt)) {
	vector<string> list;
	mod->owner().modList(list);
	for(unsigned iL = 0; iL < list.size(); iL++)
	    if(mod->owner().modAt(list[iL]).at().modInfo("SubType") == "Qt" &&
		    mod->owner().modAt(list[iL]).at().modFuncPresent("QMainWindow *openWindow();"))
		opt->childAdd("el")->setText(list[iL]);
    }
    else if(a_path == "/prm/cfg/closeToTray") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(i2s(closeToTray()));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setCloseToTray(s2i(opt->text()));
    }
    else if(a_path == "/prm/LF/prfl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(_("<Select a profile to combine>"));
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR)) {
	    if(opt->text() == _("<Clear>")) { setStyle(""); setPalette(""); setStyleSheets(""); }
	    else if(opt->text() == _("<Read back>")) {
		if(QtApp) {
		    if(!style(true).size())	setStyle(QtApp->origStl);
		    if(!styleSheets().size())	setStyleSheets(QtApp->styleSheet().toStdString());
		    if(!palette().size()) {
			QPalette tPlt = QtApp->palette();
			string tRez;
			for(int iG = 0; iG < 3; iG++) {
			    for(int iC = 0; iC < 20; iC++)
				tRez += tPlt.color((QPalette::ColorGroup)iG,(QPalette::ColorRole)iC).name().toStdString() + ((iC<19)?", ":"");
			    tRez += (iG < 3) ? "\n" : "";
			}
			setPalette(tRez);
		    }
		}
	    }
	    else {
		TConfig cEl(&elLF);
		cEl.cfg("NAME").setS(opt->text());
		if(SYS->db().at().dataGet("",nodePath()+"LookFeel",cEl,true,true)) {
		    string tVl;
		    if((tVl=cEl.cfg("STYLE").getS()).size())
			if(style(true).empty()) setStyle(tVl);
		    if((tVl=cEl.cfg("PALETTE").getS()).size()) {
			string tPlt = palette(), tRez, tVl1;
			for(int iG = 0; iG < 3; iG++) {
			    for(int iC = 0; iC < 20; iC++)
				tRez += ((tVl1=sTrm(TSYS::strParse(TSYS::strLine(tPlt,iG),iC,","))).size() ? tVl1 :
						sTrm(TSYS::strParse(TSYS::strLine(tVl,iG),iC,","))) + ((iC<19)?", ":"");
			    tRez += (iG < 3) ? "\n" : "";
			}
			setPalette(tRez);
		    }
		    if((tVl=cEl.cfg("STL_SHTS").getS()).size()) {
			if(!styleSheets().size())	setStyleSheets(tVl);
			else if(styleSheets().find(tVl) == string::npos)
			    setStyleSheets(styleSheets()+"\n"+tVl);
		    }
		}
	    }
	}
    }
    else if(a_path == "/prm/LF/prflLs" && ctrChkNode(opt)) {
	opt->childAdd("el")->setText(_("<Clear>"));
	opt->childAdd("el")->setText(_("<Read back>"));
	TConfig cEl(&elLF);
	for(int fld_cnt = 0; SYS->db().at().dataSeek("",nodePath()+"LookFeel",fld_cnt++,cEl,true); )
	    opt->childAdd("el")->setText(cEl.cfg("NAME").getS());
    }
    else if(a_path == "/prm/LF/stl") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(style());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStyle(opt->text());
    }
    else if(a_path == "/prm/LF/stlLs" && ctrChkNode(opt)) {
	QStringList sls = QStyleFactory::keys();
	for(unsigned iL = 0; iL < sls.size(); iL++)
	    opt->childAdd("el")->setText(sls[iL].toStdString());
    }
    else if(a_path == "/prm/LF/plt") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(palette());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setPalette(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    opt->setAttr("font", "Courier");
	    opt->childAdd("rule")->setAttr("expr", "#[0-9a-fA-F]{6}")->setAttr("color", "blue");
	}
    }
    else if(a_path == "/prm/LF/stlSheets") {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(styleSheets());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStyleSheets(opt->text());
	if(ctrChkNode(opt,"SnthHgl",RWRWR_,"root",SUI_ID,SEC_RD)) {
	    opt->setAttr("font", "Courier");
	    opt->childAdd("blk")->setAttr("beg", "/\\*")->setAttr("end", "\\*/")->setAttr("color", "gray")->setAttr("font_italic", "1");
	    opt->childAdd("blk")->setAttr("beg", "\\{")->setAttr("end", "\\}")->setAttr("color", "#666666")->
		childAdd("rule")->setAttr("expr", ":[^;]+")->setAttr("color", "blue");
	    opt->childAdd("rule")->setAttr("expr", "(\\.|#)\\w+\\s")->setAttr("color", "darkorange");
	}
    }
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* StApp                                         *
//*************************************************
StApp::StApp( int &argv, char **args ) : QApplication(argv, args), origStl(mod->dataRes()),
    inExec(false), trayMenu(NULL), tray(NULL), stDlg(NULL), initExec(false)
{
    startTimer(STD_WAIT_DELAY);
}

StApp::~StApp( )
{
    stClear();
}

void StApp::saveState( QSessionManager &manager )
{
    manager.setRestartHint(QSessionManager::RestartNever);
}

int StApp::stExec( )
{
    stClear();

    QObject::connect(this, SIGNAL(lastWindowClosed()), this, SLOT(lastWinClose()));

    inExec = true;
    int rez = exec();
    inExec = false;
    return rez;
}

void StApp::stClear( )
{
    QObject::disconnect(this, SIGNAL(lastWindowClosed()), this, SLOT(lastWinClose()));

    if(tray)		{ delete tray; tray = NULL; }
    if(trayMenu)	{ delete trayMenu; trayMenu = NULL; }
    if(stDlg)		{ delete stDlg; stDlg = NULL; }

    initExec = false;
}

void StApp::timerEvent( QTimerEvent *event )
{
    if(!inExec)	return;
    if(!initExec) {
	initExec = true;	//!!: Set to the begin but here can be a multiple entry from processEvents() manual call, observed on QTCfg.

	QStyle *appStl = QStyleFactory::create(mod->style().c_str());
	if(appStl)	QApplication::setStyle(appStl);
	origStl = style()->objectName().toStdString();
	if(mod->palette().size()) {
	    QPalette plt = palette();
	    string cGrp, cRl, tVl;
	    for(int off = 0, iGrp = 0; ((cGrp=TSYS::strLine(mod->palette(),0,&off)).size() || off < mod->palette().size()) && iGrp < 4; iGrp++)
		for(int off1 = 0, iRl = 0; ((cRl=TSYS::strParse(cGrp,0,",",&off1)).size() || off1 < cGrp.size()) && iRl < 20; iRl++) {
		    if(!(tVl=sTrm(cRl)).size())	continue;
		    plt.setColor((QPalette::ColorGroup)iGrp, (QPalette::ColorRole)iRl, tVl.c_str());
		}
	    setPalette(plt);
	}

	if(mod->styleSheets().size()) setStyleSheet(mod->styleSheets().c_str());

	//!!!! Disable the native menu bar, mostly for Unity, Maemo (possible) where a problem in build QTStarter menu is
	//QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

	vector<string> list;
	mod->owner().modList(list);

	//Start external modules
	int op_wnd = 0;
	if(SYS->prjCustMode() || SYS->prjNm().size())
	    for(unsigned iL = 0; iL < list.size(); iL++)
		if(mod->owner().modAt(list[iL]).at().modInfo("SubType") == "Qt" &&
		    mod->owner().modAt(list[iL]).at().modFuncPresent("QMainWindow *openWindow();"))
		{
		    AutoHD<TModule> QtMod = mod->owner().modAt(list[iL]);

		    // Search module into the start list
		    int iOff = 0;
		    string sEl;
		    while((sEl=TSYS::strSepParse(mod->startMod(),0,';',&iOff)).size() && sEl != list[iL]) ;
		    if(mod->startMod().size() && (!sEl.empty() || !iOff) && callQtModule(list[iL])) op_wnd++;
		}

	//delete splash;
	mod->splashSet(TUIMod::SPLSH_NULL);

	//Start the call dialog or to system tray
	if(!op_wnd) {
	    if(mod->closeToTray() && (SYS->prjNm().size() || SYS->prjCustMode())) createTray();
	    else startDialog();
	}
	return;
    }

#ifdef EN_QtMainThrd
    if(!SYS->stopSignal()) return;
#else
    if(!mod->endRun() && mod->startCom()) return;
#endif
    QWidgetList wl = topLevelWidgets();
    for(int iW = 0; iW < wl.size(); iW++) wl[iW]->setProperty("forceClose", true);
    closeAllWindows();

    if(mod->startStat() && !activeWindow()) quit();
}

void StApp::createTray( )
{
    QImage ico_t;
    if(SYS->prjNm().size())	ico_t.load(TUIS::icoGet(SYS->prjNm(),NULL,true).c_str());
    if(ico_t.isNull() && SYS->prjCustMode())	ico_t.load(TUIS::icoGet(SYS->id(),NULL,true).c_str());
    if(ico_t.isNull())		ico_t.load(":/images/oscada_qt.png");
    if(!tray)	tray = new QSystemTrayIcon(QPixmap::fromImage(ico_t));
    QObject::connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayAct(QSystemTrayIcon::ActivationReason)));
    tray->setToolTip(QString(_("OpenSCADA Project: %1")).arg((SYS->prjCustMode()?SYS->name():SYS->prjNm()).c_str()));
    if(!trayMenu) trayMenu = new QMenu();
    else trayMenu->clear();
    QAction *tAct = trayMenu->addAction(QIcon(":/images/oscada_qt.png"), "QTStarter");
    trayMenu->addSeparator();
    QObject::connect(tAct, SIGNAL(triggered()), this, SLOT(startDialog()));
    makeStarterMenu(trayMenu);
    trayMenu->addSeparator();
    tAct = trayMenu->addAction(QIcon(":/images/exit.png"), _("Exit the program"));
    tAct->setObjectName("*exit*");
    QObject::connect(tAct, SIGNAL(triggered()), this, SLOT(callQtModule()));
    tray->setContextMenu(trayMenu);
    tray->show();
}

void StApp::callQtModule( )
{
    QObject *obj = (QObject *)sender();
    if(obj->objectName() == "*exit*")	SYS->stop();
    else {
	try{ callQtModule(obj->objectName().toStdString()); }
	catch(TError &err) {  }
    }
}

void StApp::makeStarterMenu( QWidget *mn )
{
    if(!mn) {
	QMainWindow *w = dynamic_cast<QMainWindow*>(sender());
	if(!w) return;
	mn = new QMenu("QTStarter");
	w->menuBar()->addMenu((QMenu*)mn);
    }

    vector<string> list;
    mod->owner().modList(list);
    for(unsigned iL = 0; iL < list.size(); iL++)
	if(mod->owner().modAt(list[iL]).at().modInfo("SubType") == "Qt" &&
	    mod->owner().modAt(list[iL]).at().modFuncPresent("QMainWindow *openWindow();"))
	{
	    AutoHD<TModule> QtMod = mod->owner().modAt(list[iL]);

	    QIcon icon;
	    if(mod->owner().modAt(list[iL]).at().modFuncPresent("QIcon icon();")) {
		QIcon(TModule::*iconGet)();
		mod->owner().modAt(list[iL]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
		icon = ((&mod->owner().modAt(list[iL]).at())->*iconGet)( );
	    } else icon = QIcon(":/images/oscada_qt.png");
	    QAction *act = new QAction(icon, QtMod.at().modName().c_str(), mn);
	    act->setObjectName(list[iL].c_str());
	    //act_1->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_1);
	    act->setToolTip(QtMod.at().modName().c_str());
	    act->setWhatsThis(QtMod.at().modInfo("Description").c_str());
	    QObject::connect(act, SIGNAL(triggered()), this, SLOT(callQtModule()));
	    mn->addAction(act);
	}
}

void StApp::lastWinClose( )
{
#ifdef EN_QtMainThrd
    if(SYS->stopSignal())	quit();
#else
    if(!mod->startCom() || mod->endRun() || SYS->stopSignal())	quit();
#endif
    else if(mod->closeToTray()) createTray();
    else startDialog();
}

void StApp::trayAct( QSystemTrayIcon::ActivationReason reason )
{
    if((reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::MiddleClick) && !activeWindow())
	startDialog();
}

bool StApp::callQtModule( const string &nm )
{
    vector<string> list;

    setProperty("closeToTray", mod->closeToTray());

    AutoHD<TModule> QtMod = mod->owner().modAt(nm);
    QMainWindow *(TModule::*openWindow)( );
    QtMod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
    QMainWindow *new_wnd = ((&QtMod.at())->*openWindow)( );
    if(!new_wnd) return false;

    new_wnd->show();

    return true;
}

void StApp::startDialog( )
{
    if(!stDlg) stDlg = new StartDialog();
    stDlg->show();
}

//*************************************************
//* StartDialog                                   *
//*************************************************
StartDialog::StartDialog( ) : prjsLs(NULL), prjsBt(NULL)
{
    if(SYS->prjCustMode()) setWindowTitle(_("Qt-starter of OpenSCADA"));
    else if(SYS->prjNm().size()) setWindowTitle(QString(_("Project: %1")).arg(SYS->prjNm().c_str()));
    else setWindowTitle(_("OpenSCADA start"));

    QImage ico_t;
    if(SYS->prjNm().size())	ico_t.load(TUIS::icoGet(SYS->prjNm(),NULL,true).c_str());
    if(ico_t.isNull() && SYS->prjCustMode())	ico_t.load(TUIS::icoGet(SYS->id(),NULL,true).c_str());
    if(ico_t.isNull())		ico_t.load(":/images/oscada_qt.png");
    setWindowIcon(QPixmap::fromImage(ico_t));

    //Menu prepare
    // About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display program and OpenSCADA information."));
    //actAbout->setStatusTip(_("Press to display program and OpenSCADA information."));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    // About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information."));
    //actQtAbout->setStatusTip(_("Press to get the using Qt information."));
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // QTStarter manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    QAction *actManual = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(mod->modId().c_str()),this);
    actManual->setProperty("doc", "Modules/QTStarter|Modules/QTStarter");
    actManual->setShortcut(Qt::Key_F1);
    actManual->setWhatsThis(QString(_("The button for getting the using %1 manual.")).arg(mod->modId().c_str()));
    //actManual->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(mod->modId().c_str()));
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    // OpenSCADA manual index
    QAction *actManualSYS = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(PACKAGE_STRING),this);
    actManualSYS->setProperty("doc", "index|Documents");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using %1 manual.")).arg(PACKAGE_STRING));
    //actManualSYS->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(PACKAGE_STRING));
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    // What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    actWhatIs->setToolTip(_("Question about GUI elements"));
    actWhatIs->setWhatsThis(_("The button for requesting the information about user interface elements."));
    //actWhatIs->setStatusTip(_("Press for requesting about user interface elements"));
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Create menu "help"
    QMenu *help = menuBar()->addMenu(_("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addAction(actManual);
    help->addAction(actManualSYS);
    help->addSeparator();
    help->addAction(actWhatIs);

    //Append the logotype
    setCentralWidget(new QWidget(this));
    QVBoxLayout *wnd_lay = new QVBoxLayout(centralWidget());
    wnd_lay->setMargin(6);
    wnd_lay->setSpacing(4);

    QLabel *logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/images/logo.png"));
    if(logo->pixmap()) {
	logo->setAutoFillBackground(true);
	QPalette plt = logo->palette();
	plt.setBrush(QPalette::Window, logo->pixmap()->copy(logo->pixmap()->width()-1,0,1,logo->pixmap()->height()));
	logo->setPalette(plt);
    }
    wnd_lay->addWidget(logo, 0, 0);

    //Append the title and the list of Qt modules of OpenSCADA
    QFont labFont = font();
    labFont.setBold(true); labFont.setItalic(true);
    if(!SYS->prjCustMode() && SYS->prjNm().size()) {
	QLabel *lab = new QLabel(_("Qt modules of OpenSCADA"), this);
	lab->setFont(labFont);
	wnd_lay->addWidget(lab, 0, 0);
    }

    if(SYS->prjCustMode() || SYS->prjNm().size()) {
	vector<string> list;
	mod->owner().modList(list);
	for(unsigned iL = 0; iL < list.size(); iL++)
	    if(mod->owner().modAt(list[iL]).at().modInfo("SubType") == "Qt" &&
		mod->owner().modAt(list[iL]).at().modFuncPresent("QMainWindow *openWindow();"))
	{
	    QIcon icon;
	    if(mod->owner().modAt(list[iL]).at().modFuncPresent("QIcon icon();")) {
		QIcon (TModule::*iconGet)();
		mod->owner().modAt(list[iL]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
		icon = ((&mod->owner().modAt(list[iL]).at())->*iconGet)( );
	    }
	    else icon = QIcon(":/images/oscada_qt.png");

	    AutoHD<TModule> QtMod = mod->owner().modAt(list[iL]);
	    QPushButton *butt = new QPushButton(icon, QtMod.at().modName().c_str(), centralWidget());
	    butt->setObjectName(list[iL].c_str());
	    butt->setToolTip(QString(_("Call the Qt-based UI module '%1'")).arg(list[iL].c_str()));
	    butt->setWhatsThis(QString(_("Module: %1\n"
		"Name: %2\n"
		"Source: %3\n"
		"Version: %4\n"
		"Author: %5\n"
		"Description: %6\n"
		"License: %7")).arg(QtMod.at().modInfo("Module").c_str())
			      .arg(QtMod.at().modInfo("Name").c_str())
			      .arg(QtMod.at().modInfo("Source").c_str())
			      .arg(QtMod.at().modInfo("Version").c_str())
			      .arg(QtMod.at().modInfo("Author").c_str())
			      .arg(QtMod.at().modInfo("Description").c_str())
			      .arg(QtMod.at().modInfo("License").c_str()));
	    QObject::connect(butt, SIGNAL(clicked(bool)), mod->QtApp, SLOT(callQtModule()));
	    wnd_lay->addWidget(butt, 0, 0);
	}

	//Append the horizontal line and the title of projects of OpenSCADA
	if(!SYS->prjCustMode()) {
	    QFrame *gFrame = new QFrame(this);
	    gFrame->setFrameShape(QFrame::HLine);
	    gFrame->setFrameShadow(QFrame::Sunken);
	    wnd_lay->addWidget(gFrame, 0, 0);

	    QLabel *lab = new QLabel(_("Projects of OpenSCADA"), this);
	    lab->setFont(labFont);
	    wnd_lay->addWidget(lab, 0, 0);
	}
    }

    //Append the list of projects of OpenSCADA
    prjsLs = NULL;
    prjsBt = NULL;
    if(!SYS->prjCustMode()) {
	bool oscd_datadir_wr = (access(oscd_datadir_full,X_OK|W_OK) == 0);

	// Prepare the list widget for projects selection.
	prjsLs = new QListWidget(this);
	prjsLs->setToolTip(_("List of allowed to call/switch projects of OpenSCADA"));
	prjsLs->setWhatsThis(_("The list for call and switch to allowed projects of OpenSCADA."));
	prjsLs->addItem(_("<Creation-updating>"));
	prjsLs->item(prjsLs->count()-1)->setToolTip(_("New projects creation or updating of presented ones, like to desktop links"));
	string userDir = SYS->prjUserDir();
	if(!oscd_datadir_wr) {
	    prjsLs->addItem(_("=== User projects ==="));
	    prjsLs->item(prjsLs->count()-1)->setFlags(Qt::NoItemFlags);

	    DIR *IdDir = opendir(userDir.c_str());
	    if(IdDir) {
		struct stat file_stat;
		dirent	*scan_rez = NULL,
			*scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);

		while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez) {
		    if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0) continue;
		    //  Presenting of the project's folder
		    if(stat((userDir+"/"+scan_rez->d_name).c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFDIR)
			continue;
		    //  Presenting of the project's config file
		    string itNm = userDir + "/" + scan_rez->d_name + "/oscada.xml";
		    if(stat(itNm.c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) continue;
		    //  Item placing
		    string opt;
		    if(scan_rez->d_name == SYS->prjNm()) opt += string(opt.size()?", ":"") + _("current");
		    if(access((userDir+"/"+scan_rez->d_name+"/lock").c_str(),F_OK) == 0)
			opt += string(opt.size()?", ":"") + _("running");
		    prjsLs->addItem((string(scan_rez->d_name)+(opt.size()?" ("+opt+")":"")).c_str());
		    QListWidgetItem *tit = prjsLs->item(prjsLs->count()-1);
		    tit->setData(Qt::UserRole, scan_rez->d_name);
		    QImage ico;
		    if(ico.load((string(oscd_datadir_full "/icons/")+scan_rez->d_name+".png").c_str()) ||
			    ico.load((userDir+"/"+scan_rez->d_name+"/icons/"+scan_rez->d_name+".png").c_str()))
			tit->setIcon(QPixmap::fromImage(ico));
		}

		free(scan_dirent);
		closedir(IdDir);
	    }

	    prjsLs->addItem(_("=== Common projects ==="));
	    prjsLs->item(prjsLs->count()-1)->setFlags(Qt::NoItemFlags);
	}

	DIR *IdDir = opendir(oscd_datadir_full);
	if(IdDir) {
	    struct stat file_stat;
	    dirent	*scan_rez = NULL,
			*scan_dirent = (dirent*)malloc(offsetof(dirent,d_name) + NAME_MAX + 1);

	    while(readdir_r(IdDir,scan_dirent,&scan_rez) == 0 && scan_rez) {
		if(strcmp(scan_rez->d_name,"..") == 0 || strcmp(scan_rez->d_name,".") == 0) continue;
		//  Presenting of the project's folder
		if(stat((string(oscd_datadir_full)+"/"+scan_rez->d_name).c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFDIR)
		    continue;
		//  Presenting of the project's config file
		string itNm = string(sysconfdir_full) + "/oscada_" + scan_rez->d_name + ".xml";
		if(stat(itNm.c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) {
		    itNm = string(oscd_datadir_full) + "/" + scan_rez->d_name + "/oscada.xml";
		    if(stat(itNm.c_str(),&file_stat) != 0 || (file_stat.st_mode&S_IFMT) != S_IFREG) continue;
		}
		//  Item placing
		string opt;
		if(scan_rez->d_name == SYS->prjNm())	opt += string(opt.size()?", ":"") + _("current");
		if(access((string(oscd_datadir_full "/")+scan_rez->d_name+"/lock").c_str(),F_OK) == 0)
		    opt += string(opt.size()?", ":"") + _("running");
		prjsLs->addItem((string(scan_rez->d_name)+(opt.size()?" ("+opt+")":"")).c_str());
		QListWidgetItem *tit = prjsLs->item(prjsLs->count()-1);
		tit->setData(Qt::UserRole, scan_rez->d_name);
		QImage ico;
		if(ico.load((string(oscd_datadir_full "/icons/")+scan_rez->d_name+".png").c_str()) ||
			ico.load((string(oscd_datadir_full "/")+scan_rez->d_name+"/icons/"+scan_rez->d_name+".png").c_str()))
		    tit->setIcon(QPixmap::fromImage(ico));
	    }

	    free(scan_dirent);
	    closedir(IdDir);
	}

	// Mark current project's items in the list
	if(SYS->prjNm().size()) {
	    QList<QListWidgetItem *> sIt = prjsLs->findItems(SYS->prjNm().c_str(), Qt::MatchStartsWith);
	    for(int iIt = 0; iIt < sIt.length(); iIt++) {
		sIt[iIt]->setSelected(true);
		prjsLs->scrollToItem(sIt[iIt]);
	    }
	}
	// Connect
	connect(prjsLs, SIGNAL(itemSelectionChanged()), this, SLOT(projSelect()));
	connect(prjsLs, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(projSwitch()));

	wnd_lay->addWidget(prjsLs, 0, 0);

	prjsBt = new QPushButton(QIcon(":/images/ok.png"), SYS->prjNm().size()?_("Switch to the selected project"):_("Call the selected project"), this);
	prjsBt->setEnabled(false);
	prjsBt->setToolTip(SYS->prjNm().size()?_("Switch to the selected project"):_("Call the selected project"));
	prjsBt->setWhatsThis(SYS->prjNm().size()?_("The button for switch to the selected project."):_("The button for call the selected project."));
	QObject::connect(prjsBt, SIGNAL(clicked(bool)), this, SLOT(projSwitch()));
	wnd_lay->addWidget(prjsBt, 0, 0);
    }

    wnd_lay->addItem(new QSpacerItem(20,10,QSizePolicy::Minimum,QSizePolicy::Expanding));

    QFrame *gFrame = new QFrame(this);
    gFrame->setFrameShape(QFrame::HLine);
    gFrame->setFrameShadow(QFrame::Sunken);
    wnd_lay->addWidget(gFrame, 0, 0);

    QPushButton *butt = new QPushButton(QIcon(":/images/exit.png"),_("Exit the program"), this);
    butt->setObjectName("*exit*");
    butt->setToolTip(_("Exit the program"));
    butt->setWhatsThis(_("The button for exit the program."));
    QObject::connect(butt, SIGNAL(clicked(bool)), mod->QtApp, SLOT(callQtModule()));
    wnd_lay->addWidget(butt, 0, 0);
}

void StartDialog::showEvent( QShowEvent* )
{
    //Hide the projects apply button for too busy lists
    if(prjsLs && prjsBt)
	prjsBt->setVisible(prjsLs->height() > 9*QFontMetrics(prjsLs->font()).height());
}

void StartDialog::closeEvent( QCloseEvent *ce )
{
    if(!mod->QtApp->trayPresent()) {
	unsigned winCnt = 0;
	for(int iW = 0; iW < QApplication::topLevelWidgets().size(); iW++)
	    if(qobject_cast<QMainWindow*>(QApplication::topLevelWidgets()[iW]) && QApplication::topLevelWidgets()[iW]->isVisible())
		winCnt++;
	if(winCnt <= 1) SYS->stop();
    }
    ce->accept();
}

void StartDialog::about( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf, sizeof(buf), _(
	"%s v%s.\n%s\nAuthor: %s\nLicense: %s\n\n"
	"%s v%s.\n%s\nLicense: %s\nAuthor: %s\nWeb site: %s"),
	mod->modInfo("Name").c_str(), mod->modInfo("Version").c_str(), mod->modInfo("Description").c_str(),
	mod->modInfo("Author").c_str(), mod->modInfo("License").c_str(),
	PACKAGE_NAME, VERSION, _(PACKAGE_DESCR), PACKAGE_LICENSE, _(PACKAGE_AUTHOR), PACKAGE_SITE);

    QMessageBox::about(this, windowTitle(), buf);
}

void StartDialog::aboutQt( )	{ QMessageBox::aboutQt(this, mod->modInfo("Name").c_str()); }

void StartDialog::enterWhatsThis( )	{ QWhatsThis::enterWhatsThisMode(); }

void StartDialog::enterManual( )
{
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString());
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("No the manual '%1' found offline or online!")).arg(sender()->property("doc").toString()));
}

void StartDialog::projSelect( )
{
    if(!prjsLs || !prjsBt) return;

    QList<QListWidgetItem *> selPrj = prjsLs->selectedItems();
    prjsBt->setEnabled(selPrj.length() && (prjsLs->row(selPrj[0]) == 0 || selPrj[0]->data(Qt::UserRole).toString().toStdString() != SYS->prjNm()));
}

void StartDialog::projSwitch( )
{
    if(!prjsLs || !prjsBt)	return;

    QList<QListWidgetItem *> selPrj = prjsLs->selectedItems();
    if(!selPrj.length())	return;

    QString prjNm = selPrj[0]->data(Qt::UserRole).toString();
    bool toCreate = false;

    //Enter project name for new one creation or present one updating
    if(prjsLs->row(selPrj[0]) == 0) {
	bool ok = false;
	prjNm = QInputDialog::getText(this, _("New project or project to update"),
		_("Project name for new one creation or to update present one:"), QLineEdit::Normal, "NewProject", &ok);
	if(!ok)	return;
	if(!prjNm.length()) {
	    QMessageBox::warning(this, _("New project or project to update"), _("Empty name of the project is unavailable!"));
	    return;
	}
	toCreate = true;
    }

    //Switch to presented project and create new one before
    if(prjNm.toStdString() == SYS->prjNm())	return;
    if(SYS->prjNm().size() && QMessageBox::warning(this,_("Switch project"),
		    QString(_("Do you really want to change the current project \"%1\" to \"%2\"?")).arg(SYS->prjNm().c_str()).arg(prjNm),
			QMessageBox::Yes|QMessageBox::No,QMessageBox::No) != QMessageBox::Yes)	return;
    if((access(((oscd_datadir_full "/")+prjNm.toStdString()+"/lock").c_str(),F_OK) == 0 ||
	    access((SYS->prjUserDir()+"/"+prjNm.toStdString()+"/lock").c_str(),F_OK) == 0) &&
		QMessageBox::warning(this,SYS->prjNm().size()?_("Switch project"):_("Call project"),
		    QString(SYS->prjNm().size()?_("Project \"%1\" seems running now! You still want to switch the project?"):
						_("Project \"%1\" seems running now! You still want to call the project?")).arg(prjNm),
			QMessageBox::Yes|QMessageBox::No,QMessageBox::No) != QMessageBox::Yes)	return;
    if(!SYS->prjSwitch(prjNm.toStdString(),toCreate))
	QMessageBox::warning(this, SYS->prjNm().size()?_("Switch project"):_("Call project"), QString(_("Project \"%1\" seems wrong or broken!")).arg(prjNm));
}

//*************************************************
//* I18NTranslator                                *
//*************************************************
I18NTranslator::I18NTranslator( ) : QTranslator(0)
{

}

bool I18NTranslator::isEmpty( ) const	{ return false; }

#if QT_VERSION < 0x050000
QString I18NTranslator::translate( const char *context, const char *sourceText, const char *comment ) const
#else
QString I18NTranslator::translate( const char *context, const char *sourceText, const char *comment, int n ) const
#endif
{
    if(!sourceText) return "";
    QString trRes = _(sourceText);

    if(mess_lev() == TMess::Debug && trRes == sourceText)
	mess_debug(mod->nodePath().c_str(),_("No translated Qt message: '%s'"),sourceText);

    return trRes;
}


//OpenSCADA system module UI.QTStarter file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2005-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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
#include <QSplashScreen>
#include <QLocale>
#include <QMessageBox>
#include <QWhatsThis>
#include <QLabel>

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"QTStarter"
#define MOD_NAME	_("Qt GUI starter")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define MOD_VER		"1.7.0"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow Qt GUI starter. It is single for all Qt GUI modules!")
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
	if(n_mod == 0)	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
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
TUIMod::TUIMod( string name ) : TUI(MOD_ID),
    demon_mode(false), end_run(false), start_com(false), qtArgC(0), qtArgEnd(0)
{
    mod		= this;

    mName	= MOD_NAME;
    mType	= MOD_TYPE;
    mVers	= MOD_VER;
    mAuthor	= AUTHORS;
    mDescr	= DESCRIPTION;
    mLicense	= LICENSE;
    mSource	= name;

    //Qt massages not for compile but for indexing by gettext
#if 0
    char mess[][100] =
    {
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
}

TUIMod::~TUIMod( )
{
    if(run_st) modStop();
}

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect) {
	//Set Qt environments
	qtArgC = qtArgEnd = 0;
	if(SYS->argc) toQtArg(SYS->argv[0]);
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());	//codepage for Qt across QString recode!
#endif

	//Check command line for options no help and no daemon
	bool isHelp = false;
	string argCom, argVl;
	for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	    if(argCom == "h" || argCom == "help") isHelp = true;
	    else if(argCom == "demon") demon_mode = true;
		    // Qt bind options (debug)
	    else if(argCom == "sync" || argCom == "widgetcount" ||
		    // Qt bind options
		    argCom == "qws" || argCom == "style" || argCom == "stylesheet" || argCom == "session" ||
		    argCom == "reverse" || argCom == "graphicssystem" || argCom == "display" || argCom == "geometry")
		toQtArg(argCom.c_str(), argVl.c_str());

	//Start main Qt thread if no help and no daemon
	if(!(run_st || demon_mode || isHelp)) {
	    end_run = false;
	    SYS->taskCreate(nodePath('.',true), 0, Task, this);
	}
    }
}

void TUIMod::postDisable( int flag )
{
    if(run_st)
	try { SYS->taskDestroy(nodePath('.',true), &end_run); }
	catch(TError err){ mess_err(err.cat.c_str(),"%s",err.mess.c_str()); }
}

void TUIMod::load_( )
{
    mess_debug(nodePath().c_str(),_("Load module."));

    //Load parameters from command line
    string argCom, argVl;
    for(int argPos = 0; (argCom=SYS->getCmdOpt(argPos,&argVl)).size(); )
	if(argCom == "h" || argCom == "help")	fprintf(stdout,"%s",optDescr().c_str());

    //Load parameters from config-file
    start_mod = TBDS::genDBGet(nodePath()+"StartMod",start_mod);
}

void TUIMod::save_( )
{
    mess_debug(nodePath().c_str(),_("Save module."));

    TBDS::genDBSet(nodePath()+"StartMod",start_mod);
}

void TUIMod::modStart()
{
    mess_debug(nodePath().c_str(),_("Start module."));

    start_com = true;
}

void TUIMod::modStop()
{
    mess_debug(nodePath().c_str(),_("Stop module."));

    start_com = false;
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"----------- Qt debug commandline options ----------\n"
	"    --sync                 Switches to synchronous mode X11 for debugging.\n"
	"    --widgetcount          Prints debug message at the end about number of widgets\n"
	"                           left undestroyed and maximum number of widgets existed at\n"
	"                           the same time.\n"
	"----------- Qt commandline options ----------------\n"
	"    --qws                  With Qt for Embedded Linux makes this application the server.\n"
	"    --style=<nm>           Sets GUI style to <nm> (windows, platinum, plastique, ...).\n"
	"    --stylesheet=<path>    Sets styleSheet by <path> to file that contains.\n"
	"    --session=<nm>         Restores from an earlier session <nm>.\n"
	"    --reverse              Sets layout direction to Qt::RightToLeft.\n"
	"    --graphicssystem=<nm>  Sets the backend to be used for on-screen widgets and QPixmaps (raster, opengl).\n"
	"    --display=<nm>         Sets the X display (default is $DISPLAY).\n"
	"    --geometry=<geom>      Sets the client geometry of the first window that is shown.\n"
	"---------- Parameters of the module section '%s' in config-file ----------\n"
	"StartMod  <moduls>    Start modules list (sep - ';').\n\n"),
	MOD_TYPE,MOD_ID,nodePath().c_str());

    return buf;
}

void TUIMod::toQtArg( const char *nm, const char *arg )
{
    string plStr = nm;
    if(qtArgC) plStr.insert(0,"-");

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

void *TUIMod::Task( void * )
{
    vector<string> list;
    QImage ico_t;
    time_t st_time = time(NULL);
    vector<TMess::SRec> recs;

    //Init locale setLocale
    QLocale::setDefault(QLocale(Mess->lang().c_str()));

    //Qt application object init
    QApplication *QtApp = new QApplication(mod->qtArgC, (char**)&mod->qtArgV);
    QtApp->setApplicationName(PACKAGE_STRING);
    QtApp->setQuitOnLastWindowClosed(false);
    mod->run_st = true;

    //Create I18N translator
    I18NTranslator translator;
    QtApp->installTranslator(&translator);

    //Start splash create
    if(!ico_t.load(TUIS::icoGet(SYS->id()+"_splash",NULL,true).c_str())) ico_t.load(":/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(QPixmap::fromImage(ico_t));
    splash->show();
    QFont wFnt = splash->font();
    wFnt.setPixelSize(10);
    splash->setFont(wFnt);

    while(!mod->startCom() && !mod->endRun()) {
	SYS->archive().at().messGet(st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM);
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess += QString("\n%1: %2").arg(recs[i_m].categ.c_str()).arg(recs[i_m].mess.c_str());
	recs.clear();
	splash->showMessage(mess,Qt::AlignBottom|Qt::AlignLeft);
	QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }

    //Start external modules
    WinControl *winCntr = new WinControl( );

    int op_wnd = 0;
    mod->owner().modList(list);
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "Qt" &&
		mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
	{
	    // Search module into start list
	    int i_off = 0;
	    string s_el;
	    while((s_el=TSYS::strSepParse(mod->start_mod,0,';',&i_off)).size())
		if(s_el == list[i_l])	break;
	    if(!s_el.empty() || !i_off)
		if(winCntr->callQtModule(list[i_l])) op_wnd++;
	}

    delete splash;

    //Start call dialog
    if(QApplication::topLevelWidgets().isEmpty()) winCntr->startDialog( );

    QObject::connect(QtApp, SIGNAL(lastWindowClosed()), winCntr, SLOT(lastWinClose()));

    QtApp->exec();
    delete winCntr;

    //Stop splash create
    if(!ico_t.load(TUIS::icoGet(SYS->id()+"_splash_exit",NULL,true).c_str())) ico_t.load(":/images/splash.png");
    splash = new QSplashScreen(QPixmap::fromImage(ico_t));
    splash->show();
    splash->setFont(wFnt);

    st_time = time(NULL);
    while(!mod->endRun()) {
	SYS->archive().at().messGet( st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM );
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess += QString("\n%1: %2").arg(recs[i_m].categ.c_str()).arg(recs[i_m].mess.c_str());
	recs.clear();
	splash->showMessage(mess,Qt::AlignBottom|Qt::AlignLeft);
	QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }
    delete splash;

    //Qt application object free
    delete QtApp;

    mod->run_st = false;

    return NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //Get page info
    if(opt->name() == "info") {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("fld",opt,-1,"/prm/cfg/st_mod",_("Start Qt modules (sep - ';')"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/prm/cfg/lsQtMod");
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
	for(unsigned i_l = 0; i_l < list.size(); i_l++)
	    if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "Qt" &&
		    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
		opt->childAdd("el")->setText(list[i_l]);
    }
    else TUI::cntrCmdProc(opt);
}

//*************************************************
//* WinControl: Windows control                   *
//*************************************************
WinControl::WinControl( )
{
    tm = new QTimer(this);
    tm->setSingleShot(false);
    connect(tm, SIGNAL(timeout()), this, SLOT(checkForEnd()));
    tm->start(STD_WAIT_DELAY);
}

void WinControl::checkForEnd( )
{
    if(!mod->endRun() && mod->startCom()) return;
    tm->stop();
    QWidgetList wl = qApp->topLevelWidgets();
    for(int i_w = 0; i_w < wl.size(); i_w++) wl[i_w]->setProperty("forceClose",true);
    qApp->closeAllWindows();
}

void WinControl::callQtModule( )
{
    QObject *obj = (QObject *)sender();
    if(obj->objectName() == "*exit*")	SYS->stop();
    else {
	try{ callQtModule(obj->objectName().toStdString()); }
	catch(TError err) {  }
    }
}

void WinControl::lastWinClose( )
{
    if(!mod->startCom() || mod->endRun() || SYS->stopSignal())	qApp->quit();
    else startDialog();
}

bool WinControl::callQtModule( const string &nm )
{
    vector<string> list;

    AutoHD<TModule> QtMod = mod->owner().modAt(nm);
    QMainWindow *(TModule::*openWindow)( );
    QtMod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
    QMainWindow *new_wnd = ((&QtMod.at())->*openWindow)( );
    if(!new_wnd) return false;

    //Make Qt starter toolbar
    QToolBar *toolBar = NULL;
    QMenu *menu = NULL;
    if(!new_wnd->property("QTStarterToolDis").toBool()) {
	toolBar = new QToolBar("QTStarter",new_wnd);
	toolBar->setObjectName("QTStarterTool");
	new_wnd->addToolBar(Qt::TopToolBarArea,toolBar);
	toolBar->setMovable(true);
    }
    if(!new_wnd->property("QTStarterMenuDis").toBool() && !new_wnd->menuBar()->actions().empty())
	menu = new_wnd->menuBar()->addMenu("QTStarter");

    mod->owner().modList(list);
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "Qt" &&
	    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
    {
	AutoHD<TModule> QtMod = mod->owner().modAt(list[i_l]);

	QIcon icon;
	if(mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();"))
	{
	    QIcon(TModule::*iconGet)();
	    mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
	    icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
	}
	else icon = QIcon(":/images/oscada_qt.png");
	QAction *act_1 = new QAction(icon,QtMod.at().modName().c_str(),new_wnd);
	act_1->setObjectName(list[i_l].c_str());
	//act_1->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_1);
	act_1->setToolTip(QtMod.at().modName().c_str());
	act_1->setWhatsThis(QtMod.at().modInfo("Description").c_str());
	QObject::connect(act_1, SIGNAL(triggered()), this, SLOT(callQtModule()));

	if(toolBar) toolBar->addAction(act_1);
	if(menu) menu->addAction(act_1);
    }

    new_wnd->show();

    return true;
}

void WinControl::startDialog( )
{
    StartDialog *new_wnd = new StartDialog(this);
    new_wnd->show();
}

//*************************************************
//* StartDialog                                   *
//*************************************************
StartDialog::StartDialog( WinControl *wcntr )
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle(_("OpenSCADA system Qt-starter"));
    setWindowIcon(QIcon(":/images/oscada_qt.png"));

    //Menu prepare
    QImage ico_t;

    // About "System info"
    if(!ico_t.load(TUIS::icoGet("help",NULL,true).c_str())) ico_t.load(":/images/help.png");
    QAction *actAbout = new QAction(QPixmap::fromImage(ico_t),_("&About"),this);
    actAbout->setToolTip(_("Program and OpenSCADA information"));
    actAbout->setWhatsThis(_("The button for display program and OpenSCADA information"));
    actAbout->setStatusTip(_("Press to display program and OpenSCADA information"));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(about()));
    // About Qt
    QAction *actQtAbout = new QAction(_("About &Qt"),this);
    actQtAbout->setToolTip(_("Qt information"));
    actQtAbout->setWhatsThis(_("The button for getting the using Qt information"));
    actQtAbout->setStatusTip(_("Press to get the using Qt information."));
    connect(actQtAbout, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // QTStarter manual
    if(!ico_t.load(TUIS::icoGet("manual",NULL,true).c_str())) ico_t.load(":/images/manual.png");
    QAction *actManual = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(mod->modId().c_str()),this);
    actManual->setProperty("doc", "Modules/UI.QTStarter|QTStarter");
    actManual->setShortcut(Qt::Key_F1);
    actManual->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(mod->modId().c_str()));
    actManual->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(mod->modId().c_str()));
    connect(actManual, SIGNAL(triggered()), this, SLOT(enterManual()));
    // OpenSCADA manual index
    QAction *actManualSYS = new QAction(QPixmap::fromImage(ico_t),QString(_("%1 manual")).arg(PACKAGE_STRING),this);
    actManualSYS->setProperty("doc", "index|/");
    actManualSYS->setWhatsThis(QString(_("The button for getting the using %1 manual")).arg(PACKAGE_STRING));
    actManualSYS->setStatusTip(QString(_("Press to get the using %1 manual.")).arg(PACKAGE_STRING));
    connect(actManualSYS, SIGNAL(triggered()), this, SLOT(enterManual()));
    // What is
    if(!ico_t.load(TUIS::icoGet("contexthelp",NULL,true).c_str())) ico_t.load(":/images/contexthelp.png");
    QAction *actWhatIs = new QAction(QPixmap::fromImage(ico_t),_("What's &This"),this);
    actWhatIs->setShortcut(Qt::SHIFT+Qt::Key_F1);
    actWhatIs->setToolTip(_("Question about GUI elements"));
    actWhatIs->setWhatsThis(_("The button for requesting the information about user interface elements"));
    actWhatIs->setStatusTip(_("Press for requesting about user interface elements"));
    connect(actWhatIs, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

    // Create menu "help"
    QMenu *help = menuBar()->addMenu(_("&Help"));
    help->addAction(actAbout);
    help->addAction(actQtAbout);
    help->addAction(actManual);
    help->addAction(actManualSYS);
    help->addSeparator();
    help->addAction(actWhatIs);

    //Qt modules list prepare
    setCentralWidget(new QWidget(this));
    QVBoxLayout *wnd_lay = new QVBoxLayout(centralWidget());
    wnd_lay->setMargin(6);
    wnd_lay->setSpacing(4);

    QLabel *logo = new QLabel(this);
    logo->setPixmap(QPixmap(":/images/logo.png"));
    wnd_lay->addWidget(logo,0,0);

    vector<string> list;
    mod->owner().modList(list);
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "Qt" &&
	    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
    {
	QIcon icon;
	if(mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();")) {
	    QIcon (TModule::*iconGet)();
	    mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
	    icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
	}
	else icon = QIcon(":/images/oscada_qt.png");

	AutoHD<TModule> QtMod = mod->owner().modAt(list[i_l]);
	QPushButton *butt = new QPushButton(icon,QtMod.at().modName().c_str(),centralWidget());
	butt->setObjectName(list[i_l].c_str());
	butt->setToolTip(QString(_("Call the Qt-based UI module '%1'")).arg(list[i_l].c_str()));
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
	QObject::connect(butt, SIGNAL(clicked(bool)), wcntr, SLOT(callQtModule()));
	wnd_lay->addWidget(butt, 0, 0);
    }

    wnd_lay->addItem(new QSpacerItem(20,10,QSizePolicy::Minimum,QSizePolicy::Expanding));

    QFrame *gFrame = new QFrame(centralWidget());
    gFrame->setFrameShape(QFrame::HLine);
    gFrame->setFrameShadow(QFrame::Raised);
    wnd_lay->addWidget(gFrame,0,0);

    QPushButton *butt = new QPushButton(QIcon(":/images/exit.png"),_("Exit from system"), centralWidget());
    butt->setObjectName("*exit*");
    butt->setToolTip(_("Exit from the program"));
    butt->setWhatsThis(_("The button for exit from the program"));
    QObject::connect(butt, SIGNAL(clicked(bool)), wcntr, SLOT(callQtModule()));
    wnd_lay->addWidget(butt,0,0);
}

void StartDialog::closeEvent( QCloseEvent *ce )
{
    unsigned winCnt = 0;
    for(int i_w = 0; i_w < QApplication::topLevelWidgets().size(); i_w++)
	if(qobject_cast<QMainWindow*>(QApplication::topLevelWidgets()[i_w]) && QApplication::topLevelWidgets()[i_w]->isVisible())
	    winCnt++;

    if(winCnt <= 1) SYS->stop();
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
    string findDoc = TUIS::docGet(sender()->property("doc").toString().toStdString(), NULL, TUIS::GetExecCommand);
    if(findDoc.size())	system(findDoc.c_str());
    else QMessageBox::information(this, _("Manual"),
	QString(_("No the manual '%1' found offline or online!")).arg(sender()->property("doc").toString()));
}

//*************************************************
//* I18NTranslator                                *
//*************************************************
I18NTranslator::I18NTranslator( ) : QTranslator(0)
{

}

bool I18NTranslator::isEmpty( ) const	{ return false; }

QString I18NTranslator::translate( const char *context, const char *sourceText, const char *comment ) const
{
    if(!sourceText) return "";
    QString trRes = _(sourceText);

    if(mess_lev() == TMess::Debug && trRes == sourceText)
	mess_debug(mod->nodePath().c_str(),_("No translated Qt message: '%s'"),sourceText);

    return trRes;
}

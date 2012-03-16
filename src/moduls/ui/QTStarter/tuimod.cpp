
//OpenSCADA system module UI.QTStarter file: tuimod.cpp
/***************************************************************************
 *   Copyright (C) 2005-2010 by Roman Savochenko                           *
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

#include <tsys.h>
#include <tmess.h>
#include "tuimod.h"

//*************************************************
//* Modul info!                                   *
#define MOD_ID		"QTStarter"
#define MOD_NAME	_("QT GUI starter")
#define MOD_TYPE	SUI_ID
#define VER_TYPE	SUI_VER
#define MOD_VER		"1.6.2"
#define AUTHORS		_("Roman Savochenko")
#define DESCRIPTION	_("Allow QT GUI starter. It is single for all QT GUI modules!")
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
	if( n_mod==0 )	return TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE);
	return TModule::SAt("");
    }

#ifdef MOD_INCL
    TModule *ui_QTStarter_attach( const TModule::SAt &AtMod, const string &source )
#else
    TModule *attach( const TModule::SAt &AtMod, const string &source )
#endif
    {
	if( AtMod == TModule::SAt(MOD_ID,MOD_TYPE,VER_TYPE) )
	    return new QTStarter::TUIMod( source );
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

    //> Massages not for compile but for indexing by gettext
#if 0
    char mess[][100] =
    {
	_("&Yes"),_("&No"),_("&Cancel"),_("&OK"),_("Apply"),_("Close"),_("Back"),_("Forward"),_("Parent Directory"),
	_("Look in:"),_("Computer"),_("File"),_("Folder"),_("File &name:"),_("Open"),_("&Open"),_("Cancel"),_("Save"),_("&Save"),_("Date Modified"),_("All Files (*)"),
	_("Create New Folder"),_("List View"),_("Detail View"),_("Files of type:"),_("New Folder"),_("&New Folder"),_("Show &hidden files"),_("&Delete"),_("&Rename"),_("Remove"),
	_("&Undo"),_("&Redo"),_("Cu&t"),_("&Copy"),_("&Paste"),_("Delete"),_("Select All"),_("Insert Unicode control character"),
	_("%1 bytes"),_("%1 KB"),
	_("Are sure you want to delete '%1'?"),
	_("<p>This program uses Qt Open Source Edition version %1.</p><p>Qt Open Source Edition is intended for the development of Open Source applications. You need a commercial Qt license for development of proprietary (closed source) applications.</p><p>Please see <a href=\"http://www.trolltech.com/company/model/\">www.trolltech.com/company/model/</a> for an overview of Qt licensing.</p>"),
	_("<h3>About Qt</h3>%1<p>Qt is a C++ toolkit for cross-platform application development.</p><p>Qt provides single-source portability across MS&nbsp;Windows, Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix variants. Qt is also available for embedded devices as Qt for Embedded Linux and Qt for Windows CE.</p><p>Qt is a Nokia product. See <a href=\"http://www.trolltech.com/qt/\">www.trolltech.com/qt/</a> for more information.</p>"),
	_("Hu&e:"),_("&Sat:"),_("&Val:"),_("&Red:"),_("&Green:"),_("Bl&ue:"),_("A&lpha channel:"),_("&Basic colors"),_("&Custom colors"),_("&Add to Custom Colors"),_("Select color"),
	_("Form"),_("Printer"),_("&Name:"),_("P&roperties"),_("Location:"),_("Preview"),_("Type:"),_("Output &file:"),_("Print range"),_("Print all"),
	_("Pages from"),_("to"),_("Selection"),_("Output Settings"),_("Copies:"),_("Collate"),_("Reverse"),_("Copies"),_("Color Mode"),_("Color"),_("Grayscale"),
	_("Duplex Printing"),_("None"),_("Long side"),_("Short side"),_("Options"),_("&Options >>"),_("&Options <<"),_("&Print"),_("Print to File (PDF)"),_("Print to File (Postscript)"),
	_("Local file"),_("Write %1 file"),_("Paper"),_("Page size:"),_("Width:"),_("Height:"),_("Paper source:"),_("Orientation"),_("Portrait"),_("Landscape"),_("Reverse landscape"),
	_("Reverse portrait"),_("Margins"),_("top margin"),_("left margin"),_("right margin"),_("bottom margin"),_("Points (pt)"),_("Inches (in)"),
	_("Millimeters (mm)"),_("Centimeters (cm)"),_("Page"),_("Advanced"),
	_("Mon")
    };
#endif
}

TUIMod::~TUIMod()
{
    if( run_st ) modStop();
}

void TUIMod::postEnable( int flag )
{
    TModule::postEnable(flag);

    if(flag&TCntrNode::NodeConnect)
    {
	//> Set QT environments
	qtArgC = qtArgEnd = 0;
	if(SYS->argc) toQtArg(SYS->argv[0]);
	QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale () ); //codepage for QT across QString recode!

	//> Check command line for options no help and no daemon
	bool isHelp = false;
	int next_opt, option_index = 0;
	struct option long_opt[] =
	{
	    {"help",	0, NULL, 'h'},
	    {"demon",	0, NULL, 'd'},
	    //>QT bind options (debug)
	    {"sync",	0, NULL, 0},
	    {"widgetcount", 0, NULL, 0},
	    //>QT bind options
	    {"qws",	0, NULL, 0},
	    {"style",	1, NULL, 0},
	    {"stylesheet", 1, NULL, 0},
	    {"session",	1, NULL, 0},
	    {"reverse",	0, NULL, 0},
	    {"graphicssystem", 1, NULL, 0},
	    {"display",	1, NULL, 0},
	    {"geometry",1, NULL, 0},
	    {NULL,	0, NULL, 0}
	};

	optind=opterr=0;
	do
	{
	    next_opt = getopt_long(SYS->argc, (char * const *)SYS->argv, "h", long_opt, &option_index);
	    switch(next_opt)
	    {
		case 0	: toQtArg(long_opt[option_index].name, optarg);	break;
		case 'h': isHelp = true; break;
		case 'd': demon_mode = true; break;
		case -1 : break;
	    }
	} while(next_opt != -1);

	//> Start main QT thread if no help and no daemon
	if(!(run_st || demon_mode || isHelp))
	{
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
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Load module."));
#endif

    //> Load parameters from command line
    int next_opt;
    struct option long_opt[] =
    {
	{"help"    ,0,NULL,'h'},
	{NULL      ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,"h",long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,"%s",optDescr().c_str());	break;
	    case -1 : break;
	}
    } while(next_opt != -1);

    //> Load parameters from config-file
    start_mod = TBDS::genDBGet(nodePath()+"StartMod",start_mod);
}

void TUIMod::save_( )
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Save module."));
#endif

    TBDS::genDBSet(nodePath()+"StartMod",start_mod);
}

void TUIMod::modStart()
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Start module."));
#endif

    start_com = true;
}

void TUIMod::modStop()
{
#if OSC_DEBUG >= 1
    mess_debug(nodePath().c_str(),_("Stop module."));
#endif

    start_com = false;
}

string TUIMod::optDescr( )
{
    char buf[STR_BUF_LEN];

    snprintf(buf,sizeof(buf),_(
	"======================= The module <%s:%s> options =======================\n"
	"----------- QT debug commandline options ----------\n"
	"    --sync                 Switches to synchronous mode X11 for debugging.\n"
	"    --widgetcount          Prints debug message at the end about number of widgets\n"
	"                           left undestroyed and maximum number of widgets existed at\n"
	"                           the same time.\n"
	"----------- QT commandline options ----------------\n"
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
    //> Name process
    if(qtArgC >= (sizeof(qtArgV)/sizeof(char*)) || (qtArgEnd+plStr.size()+1) > sizeof(qtArgBuf)) return;
    strcpy(qtArgBuf+qtArgEnd, plStr.c_str());
    qtArgV[qtArgC++] = qtArgBuf+qtArgEnd;
    qtArgEnd += plStr.size()+1;

    //> Argument process
    if(arg)
    {
	plStr = arg;
	if(qtArgC >= (sizeof(qtArgV)/sizeof(char*)) || (qtArgEnd+plStr.size()+1) > sizeof(qtArgBuf)) return;
	strcpy(qtArgBuf+qtArgEnd, plStr.c_str());
	qtArgV[qtArgC++] = qtArgBuf+qtArgEnd;
	qtArgEnd += plStr.size()+1;
    }
}

void *TUIMod::Task( void * )
{
    vector<string> list;
    bool first_ent = true;
    QImage ico_t;
    time_t st_time = time(NULL);
    vector<TMess::SRec> recs;

    //> Init locale setLocale
    QLocale::setDefault(QLocale(Mess->lang().c_str()));

    //> QT application object init
    QApplication *QtApp = new QApplication(mod->qtArgC, (char**)&mod->qtArgV);
    QtApp->setApplicationName(PACKAGE_STRING);
    QtApp->setQuitOnLastWindowClosed(false);
    mod->run_st = true;

    //> Create I18N translator
    I18NTranslator translator;
    QtApp->installTranslator(&translator);

    //> Start splash create
    if(!ico_t.load(TUIS::icoPath(SYS->id()+"_splash").c_str()))	ico_t.load(":/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(QPixmap::fromImage(ico_t));
    splash->show();
    QFont wFnt = splash->font();
    wFnt.setPixelSize(10);
    splash->setFont(wFnt);

    while(!mod->startCom() && !mod->endRun())
    {
	SYS->archive().at().messGet(st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM);
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess += QString("\n%1: %2").arg(recs[i_m].categ.c_str()).arg(recs[i_m].mess.c_str());
	recs.clear();
	splash->showMessage(mess,Qt::AlignBottom|Qt::AlignLeft);
	QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }
    delete splash;

    //> Start external modules
    WinControl *winCntr = new WinControl( );

    int op_wnd = 0;
    mod->owner().modList(list);
    for(unsigned i_l = 0; i_l < list.size(); i_l++)
	if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
		mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
	{
	    //>> Search module into start list
	    int i_off = 0;
	    string s_el;
	    while((s_el=TSYS::strSepParse(mod->start_mod,0,';',&i_off)).size())
		if(s_el == list[i_l])	break;
	    if(!s_el.empty() || !i_off)
		if(winCntr->callQTModule(list[i_l])) op_wnd++;
	}

    //> Start call dialog
    if(QApplication::topLevelWidgets().isEmpty()) winCntr->startDialog( );

    QObject::connect(QtApp, SIGNAL(lastWindowClosed()), winCntr, SLOT(lastWinClose()));

    QtApp->exec();
    delete winCntr;

    //> Stop splash create
    if(!ico_t.load(TUIS::icoPath(SYS->id()+"_splash_exit").c_str()))	ico_t.load(":/images/splash.png");
    splash = new QSplashScreen(QPixmap::fromImage(ico_t));
    splash->show();
    splash->setFont(wFnt);

    st_time = time(NULL);
    while(!mod->endRun())
    {
	SYS->archive().at().messGet( st_time, time(NULL), recs, "", TMess::Debug, BUF_ARCH_NM );
	QString mess;
	for(int i_m = recs.size()-1; i_m >= 0 && i_m > ((int)recs.size()-10); i_m--)
	    mess+=QString("\n%1: %2").arg(recs[i_m].categ.c_str()).arg(recs[i_m].mess.c_str());
	recs.clear();
	splash->showMessage(mess,Qt::AlignBottom|Qt::AlignLeft);
	QtApp->processEvents();
	TSYS::sysSleep(0.5);
    }
    delete splash;

    //> QT application object free
    delete QtApp;
    first_ent = false;

    mod->run_st = false;

    return NULL;
}

void TUIMod::cntrCmdProc( XMLNode *opt )
{
    //> Get page info
    if(opt->name() == "info")
    {
	TUI::cntrCmdProc(opt);
	if(ctrMkNode("area",opt,1,"/prm/cfg",_("Module options")))
	    ctrMkNode("fld",opt,-1,"/prm/cfg/st_mod",_("Start QT modules (sep - ';')"),RWRWR_,"root",SUI_ID,3,"tp","str","dest","sel_ed","select","/prm/cfg/lsQTmod");
	ctrMkNode("fld",opt,-1,"/help/g_help",_("Options help"),R_R___,"root",SUI_ID,3,"tp","str","cols","90","rows","5");
	return;
    }

    //> Process command to page
    string a_path = opt->attr("path");
    if(a_path == "/prm/cfg/st_mod")
    {
	if(ctrChkNode(opt,"get",RWRWR_,"root",SUI_ID,SEC_RD))	opt->setText(startMod());
	if(ctrChkNode(opt,"set",RWRWR_,"root",SUI_ID,SEC_WR))	setStartMod(opt->text());
    }
    else if(a_path == "/prm/cfg/lsQTmod" && ctrChkNode(opt))
    {
	vector<string> list;
	mod->owner().modList(list);
	for(unsigned i_l = 0; i_l < list.size(); i_l++)
	    if(mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
		    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();"))
		opt->childAdd("el")->setText(list[i_l]);
    }
    else if(a_path == "/help/g_help" && ctrChkNode(opt,"get",R_R___,"root",SUI_ID))	opt->setText(optDescr());
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

void WinControl::callQTModule( )
{
    QObject *obj = (QObject *)sender();
    if( string("*exit*") == obj->objectName().toAscii().data() ) SYS->stop();
    else
    {
	try{ callQTModule(obj->objectName().toAscii().data()); }
	catch(TError err) {  }
    }
}

void WinControl::lastWinClose( )
{
    if( !mod->startCom() || mod->endRun() || SYS->stopSignal() )
	qApp->quit();
    else startDialog( );
}

bool WinControl::callQTModule( const string &nm )
{
    vector<string> list;

    AutoHD<TModule> qt_mod = mod->owner().modAt(nm);
    QMainWindow *(TModule::*openWindow)( );
    qt_mod.at().modFunc("QMainWindow *openWindow();",(void (TModule::**)()) &openWindow);
    QMainWindow *new_wnd = ((&qt_mod.at())->*openWindow)( );
    if( !new_wnd ) return false;

    //> Make QT starter toolbar
    QToolBar *toolBar = NULL;
    QMenu *menu = NULL;
    if( !new_wnd->property("QTStarterToolDis").toBool() )
    {
	toolBar = new QToolBar("QTStarter",new_wnd);
	toolBar->setObjectName("QTStarterTool");
	new_wnd->addToolBar(Qt::TopToolBarArea,toolBar);
	toolBar->setMovable(true);
    }
    if( !new_wnd->property("QTStarterMenuDis").toBool() && !new_wnd->menuBar()->actions().empty() )
	menu = new_wnd->menuBar()->addMenu("QTStarter");

    mod->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
	if( mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
	    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
	AutoHD<TModule> qt_mod = mod->owner().modAt(list[i_l]);

	QIcon icon;
	if( mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();") )
	{
	    QIcon(TModule::*iconGet)();
	    mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
	    icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
	}
	else icon = QIcon(":/images/oscada_qt.png");
	QAction *act_1 = new QAction(icon,qt_mod.at().modName().c_str(),new_wnd);
	act_1->setObjectName(list[i_l].c_str());
	//act_1->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_1);
	act_1->setToolTip(qt_mod.at().modName().c_str());
	act_1->setWhatsThis(qt_mod.at().modInfo("Description").c_str());
	QObject::connect(act_1, SIGNAL(triggered()), this, SLOT(callQTModule()));

	if( toolBar ) toolBar->addAction(act_1);
	if( menu ) menu->addAction(act_1);
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
    vector<string> list;

    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(_("OpenSCADA system QT-starter"));
    setWindowIcon(QIcon(":/images/oscada_qt.png"));

    setCentralWidget(new QWidget(this));
    QVBoxLayout *wnd_lay = new QVBoxLayout(centralWidget());
    wnd_lay->setMargin(6);
    wnd_lay->setSpacing(4);

    mod->owner().modList(list);
    for( unsigned i_l = 0; i_l < list.size(); i_l++ )
	if( mod->owner().modAt(list[i_l]).at().modInfo("SubType") == "QT" &&
	    mod->owner().modAt(list[i_l]).at().modFuncPresent("QMainWindow *openWindow();") )
    {
	QIcon icon;
	if( mod->owner().modAt(list[i_l]).at().modFuncPresent("QIcon icon();") )
	{
	    QIcon (TModule::*iconGet)();
	    mod->owner().modAt(list[i_l]).at().modFunc("QIcon icon();",(void (TModule::**)()) &iconGet);
	    icon = ((&mod->owner().modAt(list[i_l]).at())->*iconGet)( );
	}
	else icon = QIcon(":/images/oscada_qt.png");

	AutoHD<TModule> qt_mod = mod->owner().modAt(list[i_l]);
	QPushButton *butt = new QPushButton(icon,qt_mod.at().modName().c_str(),centralWidget());
	butt->setObjectName(list[i_l].c_str());
	QObject::connect(butt, SIGNAL(clicked(bool)), wcntr, SLOT(callQTModule()));
	wnd_lay->addWidget( butt, 0, 0 );
    }

    wnd_lay->addItem( new QSpacerItem( 20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *gFrame = new QFrame( centralWidget() );
    gFrame->setFrameShape(QFrame::HLine);
    gFrame->setFrameShadow(QFrame::Raised);
    wnd_lay->addWidget(gFrame,0,0);

    QPushButton *butt = new QPushButton(QIcon(":/images/exit.png"),_("Exit from system"), centralWidget());
    butt->setObjectName("*exit*");
    QObject::connect(butt, SIGNAL(clicked(bool)), wcntr, SLOT(callQTModule()));
    wnd_lay->addWidget( butt, 0, 0 );
}

void StartDialog::closeEvent( QCloseEvent* ce )
{
    if(QApplication::topLevelWidgets().size() <= 1) SYS->stop();
    ce->accept();
}

//*************************************************
//* I18NTranslator                                *
//*************************************************
I18NTranslator::I18NTranslator( ) : QTranslator(0)
{

}

bool I18NTranslator::isEmpty( ) const
{
    return false;
}

QString I18NTranslator::translate( const char *context, const char *sourceText, const char *comment ) const
{
    if( !sourceText ) return "";

#if OSC_DEBUG >= 3
    if( string(sourceText) == _(sourceText) )
	mess_debug(mod->nodePath().c_str(),_("No translated QT message: '%s'"),sourceText);
#endif

    return _(sourceText);
}

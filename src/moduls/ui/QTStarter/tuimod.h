
//OpenSCADA system module UI.QTStarter file: tuimod.h
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

#ifndef TUIMOD_H
#define TUIMOD_H

#include <QTranslator>
#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>
#include <QSessionManager>
#include <QSystemTrayIcon>

#include "tbds.h"
#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

class QTimer;
class QSplashScreen;
class QListWidget;
class QPushButton;
class QMenu;

using namespace OSCADA;

namespace QTStarter
{

//*************************************************
//* I18NTranslator                                *
//*************************************************
class I18NTranslator: public QTranslator
{
public:
    //Methods
    I18NTranslator( );

    bool isEmpty( ) const;

#if QT_VERSION < 0x050000
    QString translate( const char *context, const char *sourceText, const char *comment = 0 ) const;
#else
    QString translate( const char *context, const char *sourceText, const char *disambiguation = Q_NULLPTR, int n = -1 ) const;
#endif
};

//*************************************************
//* StartDialog                                   *
//*************************************************
class StartDialog: public QMainWindow
{
    Q_OBJECT
public:
    //Methods
    StartDialog( );

protected:
    //Methods
    void showEvent( QShowEvent* );
    void closeEvent( QCloseEvent* );

private:
    //Attributes
    QListWidget	*prjsLs;
    QPushButton	*prjsBt;

private slots:
    //Methods
    void about( );
    void aboutQt( );
    void enterWhatsThis( );
    void enterManual( );

    void projSelect( );
    void projSwitch( );
};

//*************************************************
//* StApp                                         *
//*************************************************
class StApp : public QApplication
{
    Q_OBJECT

public:
    //Methods
    StApp( int &argv, char **args );
    ~StApp( );

    bool trayPresent( )	{ return tray; }

    void createTray( );
    bool callQtModule( const string &nm );

    void saveState( QSessionManager &manager );

    int stExec( );
    void stClear( );

    //Attribute
    MtxString	origStl;

protected:
    //Methods
    void timerEvent( QTimerEvent *event );

private slots:
    void startDialog( );
    void callQtModule( );
    void makeStarterMenu( QWidget *mn = NULL );
    void lastWinClose( );
    void trayAct( QSystemTrayIcon::ActivationReason reason );

private:
    //Attributes
    bool	inExec;
    QMenu	*trayMenu;
    QSystemTrayIcon *tray;
    StartDialog	*stDlg;
    bool	initExec;
};

//*************************************************
//* TUIMod                                        *
//*************************************************
class TUIMod: public TUI
{
public:
    //Data
    enum SplashFlag { SPLSH_NULL = 0, SPLSH_START, SPLSH_STOP };

    //Methods
    TUIMod( string name );
    ~TUIMod( );

    // Module's info attributes
    string modInfo( const string &name );
    void   modInfo( vector<string> &list );

    bool endRun( )	{ return mEndRun; }
    bool startCom( )	{ return mStartCom; }
    string startMod( )	{ return mStartMod; }
    string style( bool mant = false );
    string palette( )	{ return mPalette; }
    string styleSheets( ) { return mStyleSheets; }
    bool closeToTray( )	{ return mCloseToTray; }

    void setStartMod( const string &vl )	{ mStartMod = vl; modif(); }
    void setStyle( const string &vl )		{ mStyle = vl; modif(); }
    void setPalette( const string &vl )		{ mPalette = vl; modif(); }
    void setStyleSheets( const string &vl )	{ mStyleSheets = vl; modif(); }
    void setCloseToTray( bool vl )		{ mCloseToTray = vl; modif(); }
    void modStart( );
    void modStop( );

    void	splashSet( SplashFlag flg = SPLSH_NULL );

public:
    //Attributes
    StApp	*QtApp;

protected:
    //Methods
    void load_( );
    void save_( );
    void cntrCmdProc( XMLNode *opt );		//Control interface command process
    void postEnable( int flag );
    void preDisable( int flag );
    void postDisable( int flag );

private:
#ifndef EN_QtMainThrd
    //Methods
    static void *Task( void * );
#endif

    //Methods
    string	optDescr( );
    void	toQtArg( const char *nm, const char *arg = NULL );

    //Attributes
    bool	hideMode, mEndRun, mStartCom, mCloseToTray;
    MtxString	mStartMod, mStyle, mPalette, mStyleSheets;

    TElem	elLF;

    // Command line options binding to Qt
    int		qtArgC, qtArgEnd;		//Arguments counter and end position
    char	*qtArgV[10];			//Argument's values
    char	qtArgBuf[1000];			//Arguments' strings buffer

    QSplashScreen *splash;
};

extern TUIMod *mod;
}

#endif //TUIMOD_H

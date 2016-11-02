
//OpenSCADA system module UI.QTStarter file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2005-2015 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <QObject>
#include <QTranslator>
#include <QMainWindow>
#include <QApplication>
#include <QCloseEvent>
#include <QSessionManager>

#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

class QTimer;

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
//* WinControl                                    *
//*************************************************
class WinControl: public QObject
{
    Q_OBJECT
public:
    //Methods
    WinControl( );
    ~WinControl( );

    bool callQtModule( const string &nm );
    void startDialog( );

private slots:
    //Methods
    void checkForEnd( );
    void callQtModule( );
    void lastWinClose( );

private:
    //Attributes
    QTimer	*tm;
};

//*************************************************
//* StartDialog                                   *
//*************************************************
class StartDialog: public QMainWindow
{
    Q_OBJECT
public:
    //Methods
    StartDialog( WinControl *wcntr );

protected:
    //Methods
    void closeEvent( QCloseEvent* );

private slots:
    //Methods
    void about( );
    void aboutQt( );
    void enterWhatsThis( );
    void enterManual( );
};

//*************************************************
//* StApp                                         *
//*************************************************
class StApp : public QApplication
{
    Q_OBJECT

    public:
	StApp( int &argv, char **args ) : QApplication(argv, args) { }

	void saveState( QSessionManager &manager )	{ manager.setRestartHint(QSessionManager::RestartNever); }
};

//*************************************************
//* TUIMod                                        *
//*************************************************
class TUIMod: public TUI
{
public:
    //Methods
    TUIMod( string name );
    ~TUIMod( );

    bool endRun( )	{ return mEndRun; }
    bool startCom( )	{ return mStartCom; }
    string startMod( )	{ return mStartMod; }

    void setStartMod( const string &vl )	{ mStartMod = vl; modif(); }

    void modStart( );
    void modStop( );

protected:
    //Methods
    void load_( );
    void save_( );
    void cntrCmdProc( XMLNode *opt );		//Control interface command process
    void postEnable( int flag );
    void postDisable( int flag );

private:
    //Methods
    static void *Task( void * );
    string	optDescr( );
    void	toQtArg( const char *nm, const char *arg = NULL );

    //Attributes
    bool	demonMode, mEndRun, mStartCom;
    string	mStartMod;

    // Command line options binding to Qt
    int		qtArgC, qtArgEnd;		//Arguments counter and end position
    char	*qtArgV[10];			//Argument's values
    char	qtArgBuf[1000];			//Arguments' strings buffer
};

extern TUIMod *mod;
}

#endif //TUIMOD_H

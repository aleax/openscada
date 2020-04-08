
//OpenSCADA module UI.QTCfg file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2004-2018 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <QMainWindow>

#include <tuis.h>
#include <telem.h>

#undef _
#define _(mess) mod->I18N(mess)

using namespace OSCADA;

namespace QTCFG
{

//*************************************************
//* TUIMod                                        *
//*************************************************
class ConfApp;

class TUIMod: public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error, Crit };

	//Methods
	TUIMod( string name );
	~TUIMod( );

	bool endRun( )		{ return mEndRun; }

	string tmConChk( )	{ return mTmConChk; }
	string startPath( )	{ return mStartPath; }
	string startUser( )	{ return mStartUser; }
	int toolTipLim( )	{ return mToolTipLim; }

	void setTmConChk( const string &vl );
	void setStartPath( const string &vl )	{ mStartPath = vl; modif(); }
	void setStartUser( const string &vl )	{ mStartUser = vl; modif(); }
	void setToolTipLim( int vl );

	void modStart( );
	void modStop( );

	// Module's info attributes
	void   modInfo( vector<string> &list );
	string modInfo( const string &name );

	// Windows registering
	void regWin( QMainWindow *win );
	void unregWin( QMainWindow *win );

	QIcon icon( );

	// Common
	void postMess( const string &cat, const string &mess, MessLev type = Info, QWidget *parent = 0 );
	string setHelp( const string &help, const string &addr = "", QWidget *w = NULL );

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );
	void save_( );

    private:
	//Methods
	void cntrCmdProc( XMLNode *opt );	//Control interface command process
	QMainWindow *openWindow( );

	string optDescr( );

	//Attributes
	vector<QMainWindow*> cfapp;		//Opened configurator opened window

	MtxString mTmConChk,			//Connection check timeout in seconds "{fail}:{good}"
		mStartUser,			//No password requested start user
		mStartPath;			//Start path
	int	mToolTipLim;			//ToolTip limit

	bool	mEndRun;			//End run command. Close all windows
};

extern TUIMod *mod;
}

#endif //TUIMOD_H

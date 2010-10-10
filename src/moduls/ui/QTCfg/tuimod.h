
//OpenSCADA system module UI.QTCfg file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2004-2010 by Roman Savochenko                           *
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

	bool endRun( )		{ return end_run; }

	string startPath( )	{ return start_path; }
	string startUser( )	{ return start_user; }

	void setStartPath( const string &vl )	{ start_path = vl; modif(); }
	void setStartUser( const string &vl )	{ start_user = vl; modif(); }

	void modStart( );
	void modStop( );

	//- Module info attributes -
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	//- Register window -
	void regWin( QMainWindow *win );
	void unregWin( QMainWindow *win );

	QIcon icon( );

	//- Put message -
	void postMess( const string &cat, const string &mess, MessLev type = Info, QWidget *parent = 0 );

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

	string	start_user;			//No password requested start user
	string	start_path;			//Start path

	bool	end_run;			//End run command. Close all windows
};

extern TUIMod *mod;
}

#endif //TUIMOD_H

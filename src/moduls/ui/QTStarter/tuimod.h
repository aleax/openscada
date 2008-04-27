
//OpenSCADA system module UI.QTStarter file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
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

#include <QObject>
#include <QApplication>

#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

class QTimer;

namespace QTStarter
{    

//*************************************************
//* WinControl                                    *
//*************************************************
class WinControl: public QObject
{
    Q_OBJECT
    public:
	//Methods
	WinControl( );
 
	bool callQTModule( const string &nm );
        void startDialog( );
	
    private slots:
	//Methods
	void checkForEnd( );
	void callQTModule( );
	void lastWinClose( );

    private:
	//Attributes
	QTimer 	*tm;
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
	
 	bool endRun( )		{ return end_run; }
	bool startCom( )	{ return start_com; }

	void modStart( );
	void modStop( );

    protected:
	//Methods
	void load_( );
	void save_( );	
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	void postEnable( int flag );
	void postDisable( int flag );	

    private:
	//Methods
        static void *Task( void * );	
	string optDescr( );	
	
	//Attributes
	bool	demon_mode, end_run, start_com;
        pthread_t pthr_tsk;
	string	start_mod;
	
	QApplication *QtApp;
};
    
extern TUIMod *mod;
}

#endif //TUIMOD_H

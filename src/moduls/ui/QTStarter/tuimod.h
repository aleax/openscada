
//OpenSCADA system module UI.QTStarter file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <qobject.h>
#include <tuis.h>

namespace QTStarter
{
class TUIMod: public QObject, public TUI
{
    Q_OBJECT
    public:
	TUIMod( string name );
	~TUIMod();

	void modLoad( );
	void modSave( );
	void modStart( );
	void modStop( );

	void postEnable( );
    
    private slots:
	void callQTModule( );	
	
    private:
        static void *Task(void *);
	void callQTModule( const string &nm );
	void startDialog( );
	
	string optDescr( );
	
	//================== Controll functions ========================
	void cntrCmd_( const string &a_path, XMLNode *opt, TCntrNode::Command cmd );
	
    private:
	bool	dem_mode;
	bool	end_run;
        pthread_t pthr_tsk;	
	string	start_mod;
};
    
extern TUIMod *mod;
}

#endif //TUIMOD_H


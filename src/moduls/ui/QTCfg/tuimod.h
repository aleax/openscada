
//OpenSCADA system module UI.QTCfg file: tuimod.h
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#include <tuis.h>
#include <qmainwindow.h>
#include <qpixmap.h>

namespace QTCFG
{

class ConfApp;
    
class TUIMod: public TUI
{
    public:
	TUIMod( string name );
	~TUIMod();

	void modStart();
	void modStop();

	void postEnable( );
	void modLoad( );
	    
        string modInfo( const string &name );
    	void   modInfo( vector<string> &list );
	
	void regWin( ConfApp *cf );
	void unregWin( ConfApp *cf );
    
    private:
	QMainWindow *openWindow();    
	QPixmap icon();
	
        string optDescr( );
	
    private:
        vector<ConfApp *> cfapp;
	
        pthread_t pthr_tsk;
};
    
extern TUIMod *mod;
}

#endif //TUIMOD_H


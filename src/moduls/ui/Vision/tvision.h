
//OpenSCADA system module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2006 by Roman Savochenko based on Vision of Evgen Zaichuk 2005
 *   rom_as@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#ifndef TVISION_H
#define TVISION_H

#include <QMainWindow>

#include <tuis.h>

//============ Modul info! =====================================================
#define MOD_ID      "Vision"
#define MOD_NAME    "Operation user interface (QT)"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "0.1.0"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Visual operation user interface."
#define LICENSE     "GPL"
//==============================================================================

namespace VISION
{
    
class TVision : public TUI
{
    public:
	//Methods
	TVision( string name );
	~TVision( );	

	void modStart();
	void modStop();

	void postEnable( );
	void modLoad( );
	
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	
	void regWin( QMainWindow *mwd );
	void unregWin( QMainWindow *mwd );
	    
    private:
	//Attributes
	QMainWindow *openWindow();
	
	vector<QMainWindow *> mn_winds;
};
    
extern TVision *mod;
}

#endif //TVISION_H


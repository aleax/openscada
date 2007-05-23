
//OpenSCADA system module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2007 by Roman Savochenko (rom_as@diyaorg.dp.ua)
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

#include "../VCAEngine/widget.h"
#include "../VCAEngine/libwidg.h"
#include "../VCAEngine/origwidg.h"
#include "../VCAEngine/vcaengine.h"

#undef _
#define _(mess) mod->I18N(mess)

namespace VISION
{

class WdgShape;
    
class TVision : public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error, Crit };
	
	//Methods
	TVision( string name );
	~TVision( );	
	
	void modStart();
	void modStop();

	void postEnable( int flag );
	void modLoad( );
	void modSave( );
	
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	
	void regWin( QMainWindow *mwd );
	void unregWin( QMainWindow *mwd );
	
	AutoHD<VCA::Engine> engine();
	
	WdgShape *getWdgShape( const string &id );
	
	QIcon icon();
	
	//- Put message -
        void postMess( const QString &cat, const QString &mess, 
		MessLev type = Info, QWidget *parent = NULL );
	
	//- Request to OpenSCADA control interface -
	int cntrIfCmd( XMLNode &node );

    private:
	//Methods
	string optDescr( );
	QMainWindow *openWindow();
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes	
	vector<QMainWindow *> mn_winds;	
	AutoHD<TUI>	engPnt;	
	string	start_user;
	
	vector<WdgShape *> shapesWdg;
};
    
extern TVision *mod;
}

#endif //TVISION_H


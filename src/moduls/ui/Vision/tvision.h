
//OpenSCADA system module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk                               
 *   evgen@diyaorg.dp.ua                                                     
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

#include <tuis.h>
#include <qmainwindow.h>

//============ Modul info! =====================================================
#define MOD_ID      "VISION"
#define MOD_NAME    "Operator interface system (QT)"
#define MOD_TYPE    "UI"
#define VER_TYPE    VER_UI
#define SUB_TYPE    "QT"
#define VERSION     "0.1.0"
#define AUTORS      "Evgen Zaichuk"
#define DESCRIPTION "Visual user interface."
#define LICENSE     "GPL"
//==============================================================================

namespace VISION
{
class TVisionDev;
class TVisionRun;
class TConfiguration;

//Класс подсистемы визуализации
class TVision: public TUI
{
    public:
	TVision( string name );
	~TVision();
	
	void callDevelopment();//вызов режима разработки (вызывается в TVisionRun)
	void callRuntime();//вызов режима выполнения (вызывается в TVisionDev)
	void closeRuntime();//закрытие режиме выполнения (вызывается в TVisionRun при его закрытии)

	void modStart(); //запуск модуля
	void modStop();  //останов

	void postEnable( ); //после подключения
	void modLoad( ); //загрузка модуля
	
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	    
    private:
	TVisionDev *development;
	TVisionRun *runtime;
	TConfiguration *cfg;
	
	QMainWindow *openWindow();
	
	
        pthread_t pthr_tsk;
};
    
extern TVision *mod;
}

#endif //TVISION_H


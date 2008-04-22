
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

#include <telem.h>
#include <tuis.h>

#undef _
#define _(mess) mod->I18N(mess)

namespace VISION
{

//*************************************************
//* QTCFG::TVision                                *
//*************************************************
class WdgShape;
    
class TVision : public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error, Crit };
	
	//Methods
	TVision( string name );
	~TVision( );
	
	bool endRun( )				{ return end_run; }
	string startUser( )			{ return start_user; }
	string runPrjs( )			{ return run_prjs; }
	string VCAStation( )			{ return vca_station; }
	int    runTimeUpdt( )			{ return run_tm_upd; }
	
	void setStartUser( const string &user )	{ start_user = user; }
	void setRunPrjs( const string &prj )	{ run_prjs = prj; }
	void setVCAStation( const string &stat ){ vca_station = stat; }
	void setRunTimeUpdt( int mod )		{ run_tm_upd = mod; }
	
	void modStart();
	void modStop();

	void postEnable( int flag );
	void modLoad( );
	void modSave( );
	
	string modInfo( const string &name );
	void   modInfo( vector<string> &list );
	
	void regWin( QMainWindow *mwd );
	void unregWin( QMainWindow *mwd );
	
	WdgShape *getWdgShape( const string &id );
	
	QIcon icon();
	
	//- Put message -
        void postMess( const QString &cat, const QString &mess, 
		MessLev type = Info, QWidget *parent = NULL );
	
	//- Request to OpenSCADA control interface -
	int cntrIfCmd( XMLNode &node, const string &user, const string &stat, bool glob = false );

	static QWidget *getFocusedWdg( QWidget *wcntr );

    private:
	//Methods
	string optDescr( );
	QMainWindow *openWindow();
	void cntrCmdProc( XMLNode *opt );       //Control interface command process
	
	//Attributes	
	vector<QMainWindow *> 	mn_winds;	
	string			start_user,	//No question start user
				run_prjs;	//Run projects list on the module start
	vector<WdgShape *>	shapesWdg;
	bool    		end_run;	//End run command. Close all windows
	
	string			vca_station;	//VCA station id ('.' - for local station)
	int 			run_tm_upd;	//RunTime update mode (0 - all widgets periodic adaptive update,\n
						//                     1 - update only changed widgets
};


    
extern TVision *mod;
}

#endif //TVISION_H

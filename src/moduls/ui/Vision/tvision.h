
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

class VCAHost
{
    public:
        //Methods
        VCAHost( const string &ist, const string &itransp, const string &iaddr, 
	    const string &iuser, const string &ipass) :
        	    stat(ist), transp(itransp), addr(iaddr), user(iuser), pass(ipass), 
		    ses_id(-1), link_ok(false) { }
							    
        //Attributes
	string  stat;		//External station
	string  st_nm;		//External station name
    	string  transp;         //Connect transport
        string  addr;           //External host address
        string  user;           //External host user
        string  pass;           //External host password
        int     ses_id;         //Session ID
        bool    link_ok;        //Link OK
};

class WdgShape;
    
class TVision : public TUI
{
    public:
	//Data
	enum MessLev	{ Info, Warning, Error, Crit };
	
	//Methods
	TVision( string name );
	~TVision( );
	
	bool endRun( )	{ return end_run; }
	string extTranspBD();
	string startUser( )	{ return start_user; }
	string runPrjs( )	{ return run_prjs; }
	string VCAStation( )	{ return vca_station; }
	
	void setStartUser( const string &user )	{ start_user = user; }
	void setRunPrjs( const string &prj )	{ run_prjs = prj; }
	void setVCAStation( const string &stat ){ vca_station = stat; }
	
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
	int cntrIfCmd( XMLNode &node, VCAHost &host, bool glob = false );

	TElem &elExt( )		{ return el_ext; }

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
	TElem   		el_ext;		//Extarnal host element
	
	string			vca_station;	//VCA station id ('.' - for local station)
};


    
extern TVision *mod;
}

#endif //TVISION_H


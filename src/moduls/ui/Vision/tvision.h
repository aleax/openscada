
//OpenSCADA system module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2008 by Roman Savochenko (rom_as@diyaorg.dp.ua)
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
	enum Alarm	{ Light = 0x01, Alarm = 0x02, Sound = 0x04 };

	//Methods
	TVision( string name );
	~TVision( );

	bool endRun( )				{ return end_run; }
	string startUser( )			{ return start_user; }
	string userPass( )			{ return user_pass; }
	string runPrjs( )			{ return run_prjs; }
	string VCAStation( )			{ return vca_station; }
	string playCom( )			{ return mPlayCom; }
	float cachePgLife( )			{ return mCachePgLife; }

	void setStartUser( const string &user )	{ start_user = user; modif(); }
	void setUserPass( const string &pass )	{ user_pass = pass; modif(); }
	void setRunPrjs( const string &prj )	{ run_prjs = prj; modif(); }
	void setVCAStation( const string &stat ){ vca_station = stat; modif(); }
	void setPlayCom( const string &com )	{ mPlayCom = com; modif(); }
	void setCachePgLife( float vl )		{ mCachePgLife = vl; modif(); }

	void modStart( );
	void modStop( );

	string modInfo( const string &name );
	void   modInfo( vector<string> &list );

	void regWin( QMainWindow *mwd );
	void unregWin( QMainWindow *mwd );

	WdgShape *getWdgShape( const string &id );

	QIcon icon( );

	//- Put message -
	void postMess( const QString &cat, const QString &mess, MessLev type = Info, QWidget *parent = NULL );

	static QWidget *getFocusedWdg( QWidget *wcntr );

	//- Request to OpenSCADA control interface -
	int cntrIfCmd( XMLNode &node, const string &user, const string &password, const string &VCAStat, bool glob = false );

	//Attributes
	vector<QMainWindow*>	mn_winds;

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );
	void save_( );

    private:
	//Methods
	string optDescr( );
	QMainWindow *openWindow();
	void cntrCmdProc( XMLNode *opt );       //Control interface command process


	string			start_user,	//No question start user
				user_pass,	//No quest user password
				run_prjs;	//Run projects list on the module start
	vector<WdgShape*>	shapesWdg;
	bool			end_run;	//End run command. Close all windows
	float			mCachePgLife;	//Cached pages lifetime

	string			vca_station;	//VCA station id ('.' - for local station)

	string			mPlayCom;	//Play command
};

extern TVision *mod;
}

#endif //TVISION_H


//OpenSCADA system module UI.Vision file: vis_run.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@diyaorg.dp.ua                                                  *
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

#ifndef VIS_RUN_H
#define VIS_RUN_H

#include <string>
#include <deque>
#include <map>

#include <QMainWindow>
#include <QLabel>

#include "tvision.h"

using std::string;
using std::deque;
using std::map;

namespace VISION
{

class UserStBar;
class RunPageView;
class RunWdgView;
class SndPlay;

class VisRun : public QMainWindow
{
    Q_OBJECT
    public:
	//Public methods
	VisRun( const string &prj_it, const string &open_user, const string &user_pass, const string &VCAstat, bool crSessForce = false );
	~VisRun( );

	string user( );
	string password( );
	string VCAStation( );
	int period( )		{ return m_period; }
	string workSess( )	{ return work_sess; }

	string srcProject( )	{ return src_prj; }
	float  xScale( )	{ return x_scale; }
	float  yScale( )	{ return y_scale; }
	unsigned reqTm( )	{ return reqtm; }

	void setXScale( float vl )	{ x_scale = vl; }
	void setYScale( float vl )	{ y_scale = vl; }

	void initSess( const string &prj_it, bool crSessForce = false ); //Init session for project's item path
	void callPage( const string &ses_it, XMLNode *upw = NULL );	//Call session page
	void fullUpdatePgs( );

	//- Cache commands -
	void pgCacheClear( );
	void pgCacheAdd( RunWdgView *wdg );
	RunWdgView *pgCacheGet( const string &id );

	//- Attributes commands -
	string wAttrGet( const string &path, const string &attr );
	bool wAttrSet( const string &path, const string &attr, const string &val );

	int cntrIfCmd( XMLNode &node, bool glob = false );

	void load( const string& item );

	//- Cache commands -
	string cacheResGet( const string &res );
	void cacheResSet( const string &res, const string &val );

	//- Alarms commands -
	unsigned alarmSt( )					{ return mAlrmSt; }
	char alarmTp( char tmpl, bool quitance = false )	{ return (mAlrmSt>>(quitance?16:8)) & tmpl; }
	int  alarmLev( )					{ return mAlrmSt & 0xFF; }
	void alarmSet( unsigned alarm );

    protected:
	//Protected methods
	void closeEvent( QCloseEvent* );	//Close runtime window event
	void resizeEvent( QResizeEvent * );	//Resize window event

    private slots:
	//Private slots
	void quitSt( );				//Full quit OpenSCADA

	void fullScreen( bool vl );		//Full screen toggle

	void userChanged( const QString &oldUser, const QString &oldPass );	//User changed

	void about( );				//About at programm
	void aboutQt( );			//About at QT library
	void enterWhatsThis( );			//What is GUI components
	void updatePage( );			//Update page data
	void endRunChk( );			//End run flag check

	void alarmAct( QAction *alrm );		//Alarm actions process

    private:
	//Data
	class CacheEl
	{
	    public:
		CacheEl( time_t itm, const string &ival ) : tm(itm), val(ival)	{ }
		CacheEl( )	{ }
		time_t	tm;
		string	val;
	};
	//Private attributes
	//- Menu root items -
	QMenu	*mn_file,			//Menu "File"
		*mn_alarm,			//Menu "Alarm"
		*mn_view,			//Menu "View"
		*mn_help;			//Menu "Help"

	//- Tool bars -
	QToolBar	*toolBarAlarm;		//Alarms toolbar

	//- Actions -
	QAction *actFullScr,			//Full screen action
	//-- Alarms actions --
		*actAlrmLev,			//Alarm level
		*actAlrmLight,			//Alarm by Light
		*actAlrmAlarm,			//Alarm by mono sound (PC speaker)
		*actAlrmSound;			//Alarm by sound or synthesis of speech

	//- Main components -
	QTimer		*endRunTimer, *updateTimer;
	bool		winClose;		//Close window flag
	UserStBar	*wUser;			//User status widget
	QLabel		*w_stat;		//VCA engine station
	string 		work_sess, src_prj;	//Work session and source project
	RunPageView	*master_pg;		//Master page of runtime session
	int 		m_period;		//Clock's period
	unsigned	w_prc_cnt;		//Process counter
	float		upd_tm;
	unsigned	reqtm;			//Requested time

	float		x_scale, y_scale;	//RunTime scaling

	//map<string,RunPageView*> pg_ls;		//Pages list
	deque<RunWdgView *>  cache_pg;		//Pages cache

	//- Alarm attributes -
	unsigned	mAlrmSt;		//Alarm status
	SndPlay		*alrmPlay;		//Alarm play widget
	bool		alrLevSet;		//Use for no quitance lamp blinking

	vector<string>	pgList;			//Pages list

	//- Resource cache -
	map<string,CacheEl>	mCacheRes;	//Resources cache
};

}

#endif //VIS_RUN


//OpenSCADA module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2006-2023 by Roman Savochenko (roman@oscada.org)
 *                 2005-2006 by Evgen Zaichuk
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

#include <tsys.h>
#include "../VCAEngine/types.h"

#undef _
#define _(mess) mod->I18N(mess).c_str()
#undef trS
#define trS(mess) mod->I18N(mess,mess_PreSave)

#define vrng(val,min,max) ((val) >= (min) && (val) <= (max))

#define POS_PREC_DIG	3
#define TM_ALRM_UPD	0.5

#define DEF_ExitLstRunPrjCls	true
#define DEF_DropCommonWdgStls	true
#define DEF_CachePgLife		1
#define DEF_CachePgSz		10
#define DEF_VCAstation		"."
#define DEF_RestoreTime		30


using namespace OSCADA;
using namespace VCA;

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

	bool endRun( )				{ return mEndRun; }
	string userStart( )			{ return mUserStart; }
	string userPass( )			{ return mUserPass; }
	string runPrjs( )			{ return mRunPrjs; }
	bool exitLstRunPrjCls( )		{ return mExitLstRunPrjCls; }
	bool dropCommonWdgStls( )		{ return mDropCommonWdgStls; }
	string VCAStation( )			{ return mVCAStation; }
	int restoreTime( )			{ return mRestTime; }
	double cachePgLife( )			{ return mCachePgLife; }
	int cachePgSz( )			{ return mCachePgSz; }
	string uiPropGet( const string &prop, const string &user = "root" );

	void setUserStart( const string &user )	{ mUserStart = user; modif(); }
	void setUserPass( const string &pass )	{ mUserPass = pass; modif(); }
	void setRunPrjs( const string &prj )	{ mRunPrjs = prj; modif(); }
	void setExitLstRunPrjCls( bool en )	{ mExitLstRunPrjCls = en; modif(); }
	void setDropCommonWdgStls( bool en )	{ mDropCommonWdgStls = en; modif(); }
	void setVCAStation( const string &stat ){ mVCAStation = stat; modif(); }
	void setRestoreTime( int vl )		{ mRestTime = vl; modif(); }
	void setCachePgLife( double vl )	{ mCachePgLife = vmax(0, vmin(1000,vl)); modif(); }
	void setCachePgSz( int vl )		{ mCachePgSz = vmax(0, vmin(100,vl)); modif(); }
	void uiPropSet( const string &prop, const string &vl, const string &user = "root" );

	void modStart( );
	void modStop( );

	void   modInfo( vector<string> &list );
	string modInfo( const string &name );

	void regWin( QMainWindow *mwd );
	void unregWin( QMainWindow *mwd );

	WdgShape *getWdgShape( const string &id );

	QIcon icon( );

	// Put message
	void postMess( const QString &cat, const QString &mess, MessLev type = Info, QWidget *parent = NULL );

	static QWidget *getFocusedWdg( QWidget *wcntr );

	// Request to OpenSCADA control interface
	int cntrIfCmd( XMLNode &node, const string &user, const string &password, const string &VCAStat, bool glob = false );

	static bool compareHosts( const TTransportS::ExtHost &v1, const TTransportS::ExtHost &v2 );

	//Attributes
	vector<QMainWindow*>	mnWinds;

    protected:
	//Methods
	void postEnable( int flag );
	void load_( );
	void save_( );

    private:
	//Methods
	string optDescr( );
	QMainWindow *openWindow( );
	void cntrCmdProc( XMLNode *opt );       //Control interface command process

	//Attributes
	MtxString	mVCAStation,		//VCA station id ('.' - for local station, '*' - for selection the station)
			mUserStart,		//No question start user
			mUserPass;		//No quest user password
	string		mRunPrjs;		//Run projects list on the module start
	vector<WdgShape*> shapesWdg;
	bool		mExitLstRunPrjCls,	//Exit program on last run project close
			mDropCommonWdgStls,	//Drops widget styles to the common one for some specific widgets in the runtime
			mEndRun;		//End run command. Close all windows
	int		mRestTime;		//Restore connection time, seconds
	double		mCachePgLife;		//Cached pages lifetime
	int		mCachePgSz;		//Cached pages number

	int		mScrnCnt;

	ResMtx		mnWindsRes;
};

extern TVision *mod;
}

#endif //TVISION_H

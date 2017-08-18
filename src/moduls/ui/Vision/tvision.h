
//OpenSCADA system module UI.VISION file: tvision.h
/***************************************************************************
 *   Copyright (C) 2005-2006 by Evgen Zaichuk
 *                 2006-2016 by Roman Savochenko (rom_as@oscada.org)
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
#include "tsys.h"
#include "../VCAEngine/types.h"

#undef _
#define _(mess) mod->I18N(mess)

#define vrng(val,min,max) ((val) >= (min) && (val) <= (max))

#define POS_PREC_DIG	3

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
	string VCAStation( )			{ return mVCAStation; }
	int restoreTime( )			{ return mRestTime; }
	float cachePgLife( )			{ return mCachePgLife; }
	string uiPropGet( const string &prop, const string &user = "root" );

	void setUserStart( const string &user )	{ mUserStart = user; modif(); }
	void setUserPass( const string &pass )	{ mUserPass = pass; modif(); }
	void setRunPrjs( const string &prj )	{ mRunPrjs = prj; modif(); }
	void setExitLstRunPrjCls( bool en )	{ mExitLstRunPrjCls = en; modif(); }
	void setVCAStation( const string &stat ){ mVCAStation = stat; modif(); }
	void setRestoreTime( int vl )		{ mRestTime = vl; modif(); }
	void setCachePgLife( float vl )		{ mCachePgLife = vmax(0,vmin(1000,vl)); modif(); }
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

	MtxString	mVCAStation,		//VCA station id ('.' - for local station)
			mUserStart,		//No question start user
			mUserPass;		//No quest user password
	string		mRunPrjs;		//Run projects list on the module start
	vector<WdgShape*> shapesWdg;
	bool		mExitLstRunPrjCls,	//Exit program on last run project close
			mEndRun;		//End run command. Close all windows
	int		mRestTime;		//Restore connection time, seconds
	float		mCachePgLife;		//Cached pages lifetime

	int		mScrnCnt;
};

extern TVision *mod;
}

#endif //TVISION_H


//OpenSCADA system module UI.Vision file: vis_run_widgs.h
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

#ifndef VIS_RUN_WIDGS_H
#define VIS_RUN_WIDGS_H

#include <QString>
#include <QThread>

#include "vis_widgs.h"

namespace VISION
{
    class VisRun;
    //*********************************************
    //* Shape widget view runtime mode            *
    //*********************************************
    class RunWdgView: public WdgView
    {
	Q_OBJECT

	public:
	    //- Public methods -
	    RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	    ~RunWdgView( );

	    string user( );
	    VisRun *mainWin( );

	    string pgGrp( );
	    string pgOpenSrc( );

	    bool   permCntr( )	{ return mPermCntr; }
	    bool   permView( )	{ return mPermView; }
	    unsigned reqTm( )	{ return reqtm; }

	    void setPgOpenSrc( const string &vl );
	    void setPermCntr( bool vl )	{ mPermCntr = vl; }
	    void setPermView( bool vl )	{ mPermView = vl; }
	    unsigned setReqTm( unsigned vl )	{ reqtm = vl; }

	    WdgView *newWdgItem( const string &iwid );
	    void attrLoad( QMap<QString, QString> &attrs );

	    void update( bool full, const string &wpath = "" );
	    bool attrSet( const string &attr, const string &val, int uiPrmPos = 0 );

	protected:
	    //- Protected methods -
	    bool event( QEvent * event );
	    int cntrIfCmd( XMLNode &node, bool glob = false );
	    void childsUpdate( bool newLoad = true );
	    void orderUpdate( );

	private:
	    //- Attributes -
	    unsigned	reqtm;		//Request values time
	    bool	mPermCntr;	//Control widget's permition
	    bool	mPermView;	//View widget's permition
    };

    //*********************************************
    //* Shape page view runtime mode              *
    //*********************************************
    class RunPageView: public RunWdgView
    {
	Q_OBJECT

	public:
	    //- Public methods -
	    RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	    ~RunPageView( );

	    float  xScale( bool full = false );
	    float  yScale( bool full = false );

	    RunPageView *findOpenPage( const string &pg );
	    bool callPage( const string &pg_it, const string &pgGrp, const string &pgSrc );

	    RunPageView *parent( );
	    RunPageView *pgOpen( const string &pg );

	    //- Public attributes -
	    float wx_scale, wy_scale;

	protected:
	    //- Protected methods -
	    void closeEvent ( QCloseEvent *event );
    };

    //*********************************************
    //* Play sound thread for RunTime session     *
    //*********************************************
    class SndPlay: public QThread
    {
	public:
	    //- Public methods -
	    SndPlay( QObject * parent = 0 );

	    unsigned time( )	{ return mTm; }
	    string widget( )	{ return mWdg; }
	    string playData( )	{ return mPlayData; }

	    void setTime( unsigned vl )	{ mTm = vl; }
	    void setWidget( string vl )	{ mWdg = vl; }
	    void setData( string vl )	{ mPlayData = vl; }

	    VisRun *mainWin( );

	protected:
	    //- Protected methods -
	    void run( );

	private:
	    //- Private attributes -
	    unsigned	mTm;		//Request last sound time
	    string	mWdg;		//Play widget
	    string	mPlayData;	//Play data
    };
}

#endif //VIS_RUN_WIDGS

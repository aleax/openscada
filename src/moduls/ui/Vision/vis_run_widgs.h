
//OpenSCADA system module UI.Vision file: vis_run_widgs.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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
	    //Public methods
	    RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	    ~RunWdgView( );

	    string name( );
	    string user( );
	    VisRun *mainWin( );

	    string pgGrp( );
	    string pgOpenSrc( );

	    bool permCntr( )	{ return mPermCntr; }
	    bool permView( )	{ return mPermView; }

	    void setPgOpenSrc( const string &vl );
	    void setPermCntr( bool vl )	{ mPermCntr = vl; }
	    void setPermView( bool vl )	{ mPermView = vl; }

	    WdgView *newWdgItem( const string &iwid );
	    void update( bool full, XMLNode *aBr = NULL, bool FullTree = false );
	    bool attrSet( const string &attr, const string &val, int uiPrmPos = 0 );

	    void shapeList( const string &snm, vector<string> &ls );
	    RunWdgView *findOpenWidget( const string &wdg );

	    string resGet( const string &res );

	    bool isVisible( QPoint pos );

	protected:
	    //Protected methods
	    bool event( QEvent * event );
	    int cntrIfCmd( XMLNode &node, bool glob = false );
	    void orderUpdate( );

	private:
	    //Attributes
	    char	mPermCntr :1;	//Control widget's permission
	    char	mPermView :1;	//View widget's permission
    };

    //*********************************************
    //* Shape page view runtime mode              *
    //*********************************************
    class RunPageView: public RunWdgView
    {
	Q_OBJECT

	public:
	    //> Public methods
	    RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	    ~RunPageView( );

	    unsigned reqTm( )	{ return reqtm; }
	    float  xScale( bool full = false );
	    float  yScale( bool full = false );

	    void setReqTm( unsigned vl )	{ reqtm = vl; }

	    RunPageView *findOpenPage( const string &pg );
	    bool callPage( const string &pg_it, const string &pgGrp, const string &pgSrc );

	    RunPageView *parent( );

	    //> Public attributes
	    float wx_scale, wy_scale;

	protected:
	    //> Protected methods
	    void closeEvent ( QCloseEvent *event );

	private:
	    //> Private attributes
	    unsigned	reqtm;		//Request values time
    };

    //*********************************************
    //* Play sound thread for RunTime session     *
    //*********************************************
    class SndPlay: public QThread
    {
	public:
	    //> Public methods
	    SndPlay( QObject * parent = 0 );

	    unsigned time( )	{ return mTm; }
	    string widget( )	{ return mWdg; }
	    string playData( )	{ return mPlayData; }

	    void setTime( unsigned vl )	{ mTm = vl; }
	    void setWidget( string vl )	{ mWdg = vl; }
	    void setData( string vl )	{ mPlayData = vl; }

	    VisRun *mainWin( );

	protected:
	    //> Protected methods
	    void run( );

	private:
	    //> Private attributes
	    unsigned	mTm;		//Request last sound time
	    string	mWdg;		//Play widget
	    string	mPlayData;	//Play data
    };

    //*********************************************
    //* Status bar styles                         *
    //*********************************************
    class StylesStBar : public QLabel
    {
	Q_OBJECT

	public:
	    StylesStBar( int styleId, QWidget *parent = 0 );

	    int style( )		{ return mStyle; }

	    void setStyle( int istl, const string &nm = "" );

	    bool styleSel( );

	    VisRun *mainWin( );

	signals:
	    void styleChanged( );

	protected:
	    bool event( QEvent *event );

	private:
	    int	mStyle;
    };
}

#endif //VIS_RUN_WIDGS

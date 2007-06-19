
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

#include "vis_widgs.h"

namespace VISION
{
    class VisRun;
    //****************************************
    //* Shape widget view runtime mode       *
    //****************************************
    class RunWdgView: public WdgView
    {
	Q_OBJECT
    
        public:
    	    //- Public methods -
	    RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent = 0 );
    	    ~RunWdgView( );
	    
	    string user( );
	    VisRun *mainWin( );
	    
	    string pgGrp( );
	    string pgOpenSrc( );
	    
	    void setPgOpenSrc( const string &vl );
	    
	    WdgView *newWdgItem( const string &iwid );
	    void load( const string& item, bool update = false );
	
	protected:
	    //- Protected methods -
	    bool event( QEvent * event );
    };
    
    //****************************************
    //* Shape page view runtime mode         *
    //****************************************    
    class RunPageView: public RunWdgView
    {
	Q_OBJECT
    
        public:
    	    //- Public methods -
	    RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent = 0 );
    	    ~RunPageView( );
	    
	    bool findOpenPage( const string &pg );
	    bool callPage( const string &pg_it, const string &pgGrp, const string &pgSrc );
	    
	    RunPageView *parent( );
	    
	    RunPageView *pgOpen( const string &pg );
	    
	protected:
    	    //- Protected methods -
	    void closeEvent ( QCloseEvent *event );
    };
}

#endif //VIS_RUN_WIDGS

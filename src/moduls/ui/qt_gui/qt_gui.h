/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
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

#ifndef QT_GUI_H
#define QT_GUI_H

//#include <qapplication.h>
#include <qmainwindow.h>

#include <tuis.h>

namespace QT_GUI
{
    class TUIMod: public TUI
    {
	public:
	    TUIMod( string name );
	    ~TUIMod();

	    void start();
	    void stop();

	    void mod_connect(  );
	    void mod_CheckCommandLine( );
	public:
    
	private:
	    string opt_descr( );
	    string mod_info( const string &name );
	    void   mod_info( vector<string> &list );

	    static void *Task(void *);
	private:
	    pthread_t pthr_tsk;
    };

    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( );
	    ~ConfApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void about();
	    void aboutQt();
	    void conf();
	    void w_place();
    };
    
    class AWPApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    AWPApp( );
	    ~AWPApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void about();
	    void aboutQt();
	    void conf();
	    void w_place();
    };
    /*
    class NewWidget : public QWidget
    {
	public:
    	    NewWidget( QWidget *parent=0, const char *name=0 );
	    ~NewWidget();
    };
    */
}
#endif //QT_GUI_H


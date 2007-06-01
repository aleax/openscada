
//OpenSCADA system module UI.QTCfg file: qtcfg.h
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#ifndef QTCFG_H
#define QTCFG_H

#include <string>
#include <vector>

#include <QWidget>
#include <QMainWindow>

#include <tcntrnode.h>
#include <xml.h>

using std::string;
using std::vector;

class QTreeWidget;
class QTreeWidgetItem;
class QListWidgetItem;
class QDateTime;
class QDateTimeEdit;
class QPushButton;
class QLineEdit;
class QHBoxLayout;
class QLabel;
class QTabWidget;

namespace QTCFG
{
    //Config application
    class UserStBar;
    class TUIMod;

    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( string open_user );
	    ~ConfApp( );

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void quitSt();
	    
            void pageUp();	    
            void pagePrev();
            void pageNext();
	    void pageRefresh();
            void pageCyclRefrStart();
            void pageCyclRefrStop();				    

            void userSel();
	    
	    void about();
	    void aboutQt();
	    void enterWhatsThis();

	    //QListView	    
	    void selectItem( );				//Processing of select item signal
	    void viewChild( QTreeWidgetItem * i );	//Processing of view item signal 
	    void onItem( QTreeWidgetItem * i );		//View item path
	    void ctrTreePopup( );
	    
	    //QTabWidget
	    void tabSelect( QWidget *wdg );		//Change curent widget

	    //Self widget's slots
	    void checkBoxStChange( int stat ); 		//QCheckBox	    
	    void buttonClicked( );			//Button
	    //void editReturnPress( );			//QLineEdit
	    void combBoxActivate( const QString& );	//QComboBox
	    void listBoxGo( QListWidgetItem* );		//QListBox go for banch	    
	    void listBoxPopup();			//QListBox popup menu
	    void tablePopup( const QPoint &pos );	//QTable popup menu
	    void tableSet( int row, int col );		//QTable set
	    void dataTimeChange( const QDateTime & );	//Change data-time
	    void editChange( const QString& );		//Change Edit (LineEdit and TextEdit)
	    void applyButton( );			//Apply button
	    void cancelButton( );			//Cancel button
	    void imgPopup( const QPoint &pos );		//Image popup
	    
	private:	    
	    //Page display
	    void pageDisplay( const string &path );
	    
	    //View ListItem with recursive processing of the ControllArea
	    void viewChildRecArea( QTreeWidgetItem *i, int level );
	    
	    //Update structure and put service labels
	    bool upStruct(XMLNode &w_nd, const XMLNode &n_nd);
	    
	    //Select ListItem with recursive processing of the ControllArea	    
	    void selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget = NULL, bool refr = false );
	    void basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool refr, bool comm = false );

	    //Controll system requests
	    void initHosts();
	    int cntrIfCmd( XMLNode &node );
	    
	private:
            QTimer	*autoUpdTimer;
	    
	    QTreeWidget	*CtrTree;
	    QLabel	*titleIco;
	    QLabel	*titleLab;
	    QTabWidget  *tabs;
	    UserStBar	*w_user;
	    
	    QAction 	*actUp;
	    QAction 	*actPrev;
	    QAction 	*actNext;	    
	    QAction 	*actUser;	    
	    QAction 	*actStartUpd;
	    QAction 	*actStopUpd;

	    XMLNode	pg_info;
	    XMLNode 	*root;    
	    string	sel_path;

	    int		que_sz;
	    vector<string>	prev;
	    vector<string>	next;
	    
	    int 	hd_res;
	    bool	block_tabs;
	    
	    bool 	tbl_init;
    };    
}

#endif //QTCFG_H


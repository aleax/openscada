
//OpenSCADA system module UI.QTCfg file: qtcfg.h
/***************************************************************************
 *   Copyright (C) 2004-20014 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#include <resalloc.h>
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

using namespace OSCADA;

namespace QTCFG
{

//************************************************
//* ConfApp                                      *
//************************************************
class UserStBar;
class TUIMod;

class ConfApp: public QMainWindow
{
    Q_OBJECT

    public:
	//Methods
	ConfApp( string open_user );
	~ConfApp( );

    protected:
	//Methods
	void closeEvent( QCloseEvent* );

    private slots:
	//Slots
	void quitSt( );
	bool exitModifChk( );

	void pageUp( );
	void pagePrev( );
	void pageNext( );
	void itDBLoad( );
	void itDBSave( );
	void itAdd( );
	void itDel( const string &it = "" );
	void itCut( );
	void itCopy( );
	void itPaste( );
	void pageRefresh( bool tm = false );
	void pageCyclRefrStart( );
	void pageCyclRefrStop( );

	void stMessChanged( const QString &mess );
	void stHistCall( );

	void userSel( );

	void about( );
	void aboutQt( );
	void enterWhatsThis( );

	void editToolUpdate( );				//Edit tools visible update
	void endRunChk( );				//End run flag check

	// QListView
	void selectItem( );				//Processing of select item signal
	void viewChild( QTreeWidgetItem * i );		//Processing of view item signal
	void onItem( QTreeWidgetItem * i );		//View item path
	void ctrTreePopup( );
	void treeUpdate( );				//Update expanded content of tree
	void treeSearch( );

	// QTabWidget
	void tabSelect( int idx );			//Change current tab

	// Self widget's slots
	void checkBoxStChange( int stat );		//QCheckBox
	void buttonClicked( );				//Button
	void combBoxActivate( const QString& );		//QComboBox
	void listBoxGo( QListWidgetItem* );		//QListBox go for banch
	void listBoxPopup();				//QListBox popup menu
	void tablePopup( const QPoint &pos );		//QTable popup menu
	void tableSet( int row, int col );		//QTable set
	void editChange( const QString& );		//Change Edit (LineEdit and TextEdit)
	void applyButton( );				//Apply button
	void cancelButton( );				//Cancel button
	void imgPopup( const QPoint &pos );		//Image popup

    private:
	//Methods
	// Page display
	void selectPage( const string &path );
	void pageDisplay( const string &path );

	// View ListItem with recursive processing of the ControllArea
	void viewChildRecArea( QTreeWidgetItem *i, bool upTree = false );

	// Update structure and put service labels
	bool upStruct( XMLNode &w_nd, const XMLNode &n_nd );

	// Select ListItem with recursive processing of the ControllArea
	void selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget = NULL );
	void basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm = false );

	// Controll system requests
	void initHosts();
	int cntrIfCmd( XMLNode &node );

	//Attributes
	QTimer		*endRunTimer, *autoUpdTimer;

	QTreeWidget	*CtrTree;
	QLabel		*titleIco;
	QLabel		*titleLab;
	QLabel		*mStModify;
	QTabWidget	*tabs;
	UserStBar	*w_user;

	QAction		*actUp, *actPrev, *actNext,
			*actUser,
			*actStartUpd, *actStopUpd,
			*actDBLoad, *actDBSave,
			*actItAdd, *actItDel,
			*actItCut, *actItCopy, *actItPaste;

	XMLNode		pg_info, genReqs;
	XMLNode 	*root;
	string		sel_path,
			copy_buf;       //Copy buffer

	int		que_sz;
	vector<string>	prev;
	vector<string>	next;

	vector<string>	stMess;

	bool		tbl_init;
    };
}

#endif //QTCFG_H

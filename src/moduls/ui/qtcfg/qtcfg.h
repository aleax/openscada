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

#ifndef QTCFG_H
#define QTCFG_H

#include <qmainwindow.h>

class QGridLayout;
class QListView;
class QListViewItem;
class QFrame;
class QGridLayout;
class QLabel;
class QTabWidget;

#define CTR_INFO	0
#define CTR_GET		1
#define CTR_SET		2
#define CTR_CMD		3

namespace QTCFG
{
    //******************************************************************
    //** ConfApp 
    //******************************************************************
    class TUIMod;
    
    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( TUIMod *owner );
	    ~ConfApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
	    void about();
	    void aboutQt();
	    //Processing of select item signal
	    void selectItem( QListViewItem * i );
	    //Processing of view item signal
	    void viewChild( QListViewItem * i );

	    //Self widget's slots
	    void checkBoxStChange( int stat ); 		//QCheckBox	    
	    void buttonClicked( );			//Button
	    void editReturnPress( );			//QLineEdit
	    void combBoxActivate( const QString& );	//QComboBox
	private:	    
	    //Prepare path for the tree branch
	    string getItemPath( QListViewItem * i );
	    //View ListItem with recursive processing of the ControllArea	    
	    void viewChildRecArea( const string &path, const XMLNode &node, const string &a_path, QListViewItem * i, int level );
	    
	    //Select ListItem with recursive processing of the ControllArea	    
	    void selectChildRecArea( const string &path, const XMLNode &node, const string &a_path, const XMLNode &root, QWidget *widget = NULL );

	    //Controll system requests
	    void ctrCmd( const string &path, XMLNode &node, int cmd, TContr *cntr = NULL, int level = 0 );
	    
	    //Code/Encode xpath
	    string xpathCode( const string &path );
	    string xpathEncode( const string &path );

	    //Put message
	    void postMess( const string &mess, int type );
	    //Check access to node
	    bool chkAccess( const XMLNode &fld, string user, char mode );
	private:
	    QListView	*CtrTree;
	    QLabel	*titleLab;
	    QTabWidget  *tabs;

	    XMLNode 	node;    
	    string	sel_path;
	    string	user;
	    
	    TUIMod	*own;
    };    
}

#endif //QTCFG_H


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
#include <qtooltip.h>
#include <qlistview.h>

class QGridLayout;
class QListView;
class QListViewItem;
class QFrame;
class QGridLayout;
class QLabel;
class QTabWidget;
class QListBoxItem;
class QAction;

#define CTR_INFO	0
#define CTR_GET		1
#define CTR_SET		2
#define CTR_CMD		3

namespace QTCFG
{
    //*****************************************************************
    //******************* ListViewToolTip *****************************
    //*****************************************************************
    class ListViewToolTip : public QToolTip
    {
	public:
    	    ListViewToolTip( QListView* parent ) : 
		QToolTip( parent->viewport() ), listView( parent ){ }
	protected:
	    void maybeTip( const QPoint& p );
	private:
	    QListView* listView;
    };			       

    //******************************************************************
    //** ConfApp 
    //******************************************************************
    class TUIMod;
    
    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( TUIMod *owner, ConfApp *parent );
	    ~ConfApp();

	protected:
	    void closeEvent( QCloseEvent* );

	private slots:
            void new_w();
            void up_page();
            void prew_page();
            void next_page();
            void sel_user();
	    
	    void about();
	    void aboutQt();

	    //QListView	    
	    void selectItem( QListViewItem * i );	//Processing of select item signal 	    
	    void viewChild( QListViewItem * i );	//Processing of view item signal 
	    void onItem( QListViewItem * i );		//View item path
	    
	    //QTabWidget
	    void tabSelect( const QString &wdg );	//Change curent widget

	    //Self widget's slots
	    void checkBoxStChange( int stat ); 		//QCheckBox	    
	    void buttonClicked( );			//Button
	    void editReturnPress( );			//QLineEdit
	    void combBoxActivate( const QString& );	//QComboBox
	    void listBoxPopup( QListBoxItem* );		//QListBox popup menu
	    void listBoxGo( QListBoxItem* );		//QListBox go for banch	    
	    
	private:	    
	    //Page display
	    void pageDisplay( const string &path );
	    //Prepare path for the tree branch
	    string getItemPath( QListViewItem * i );
	    //View ListItem with recursive processing of the ControllArea	    
	    //  Return found groups
	    //  i == NULL if check groups number
	    int viewChildRecArea( const string &path, const XMLNode &node, const string &a_path, QListViewItem * i, int level = 0, int grp = 0 );
	    
	    //Select ListItem with recursive processing of the ControllArea	    
	    void selectChildRecArea( const string &path, const XMLNode &node, const string &a_path, const XMLNode &root, QWidget *widget = NULL );

	    //Controll system requests
	    void ctrCmd( const string &path, XMLNode &node, int cmd, TContr *cntr = NULL, int level = 0 );
	    
	    //Code/Encode xpath
	    string xpathCode( const string &path, bool text );
	    string xpathEncode( const string &path, bool text );

	    //Put message
	    void postMess( const string &mess, int type = 1 );
	    //Check access to node
	    bool chkAccess( const XMLNode &fld, string user, char mode );

	    //Del child. self delete if close window
	    void childClose( ConfApp *child );
	private:
	    QListView	*CtrTree;
	    QLabel	*titleLab;
	    QTabWidget  *tabs;
	    
	    QAction 	*actUp;
	    QAction 	*actPrev;
	    QAction 	*actNext;	    
	    QAction 	*actUser;	    

	    XMLNode 	node;    
	    string	sel_path;
	    string	user;

	    int		que_sz;
	    vector<string>	prev;
	    vector<string>	next;
	    
	    TUIMod	*own;
	    ConfApp	*m_parent;
	    vector<ConfApp *>	childs;
	    
	    bool	p_active;
    };    
}

#endif //QTCFG_H


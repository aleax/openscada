
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

#include <qmainwindow.h>
#include <qtooltip.h>
#include <qlistview.h>

#include <tcntrnode.h>
#include <xml.h>

using std::string;
using std::vector;

class QGridLayout;
class QListView;
class QListViewItem;
class QFrame;
class QGridLayout;
class QLabel;
class QTabWidget;
class QListBoxItem;
class QAction;
class QTimer;
class QDateTime;
class QDateTimeEdit;
class QPushButton;
class QLineEdit;
class QDialog;

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
        
    //*****************************************************************
    //************************ LineEdit *******************************
    //*****************************************************************
    class LineEdit : public QWidget
    {
	Q_OBJECT
	
	public:
	    LineEdit( QWidget *parent, const char * name = 0, bool prev_dis = false );

	    void setText(const QString &);
	    QString text() const;

	    QLineEdit	*edit()	{ return ed_fld; }

	signals:
	    void apply( );
	    void cancel( );
	    void textChanged(const QString&);
	    
	protected:
	    bool event( QEvent * e );

	private slots:
	    void changed( const QString& );
	    void applySlot( );
	    
	private:
	    QHBoxLayout *box;
	    QLineEdit	*ed_fld;
	    QPushButton	*bt_fld;
    };
    
    //*****************************************************************
    //************************ DateTimeEdit ***************************
    //*****************************************************************
    class DateTimeEdit : public QWidget
    {
	Q_OBJECT
	
	public:
	    DateTimeEdit( QWidget *parent, const char * name = 0, bool prev_dis = false );

	    void setDateTime(const QDateTime & dt);
	    QDateTime dateTime() const;

	    QDateTimeEdit *dtEdit() { return ed_fld; }

	signals:
            void apply( );
	    void cancel( );
            void valueChanged(const QDateTime&);	    	
	    
	protected:
	    bool event( QEvent * e );	    
		
	private slots:
	    void changed( const QDateTime & );
	    void applySlot( );
	    
	private:
	    QHBoxLayout *box;
	    QDateTimeEdit *ed_fld;
	    QPushButton	*bt_fld;
    };

    //******************************************************************
    //** ConfApp 
    //******************************************************************
    class TUIMod;
    
    class ConfApp: public QMainWindow
    {
        Q_OBJECT

	public:
	    ConfApp( );
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

	    //QListView	    
	    void selectItem( QListViewItem * i );	//Processing of select item signal 	    
	    void viewChild( QListViewItem * i );	//Processing of view item signal 
	    void onItem( QListViewItem * i );		//View item path
	    
	    //QTabWidget
	    void tabSelect( QWidget *wdg );	//Change curent widget

	    //Self widget's slots
	    void checkBoxStChange( int stat ); 		//QCheckBox	    
	    void buttonClicked( );			//Button
	    //void editReturnPress( );			//QLineEdit
	    void combBoxActivate( const QString& );	//QComboBox
	    void listBoxGo( QListBoxItem* );		//QListBox go for banch	    
	    void listBoxPopup( QListBoxItem* );		//QListBox popup menu
	    void tablePopup(int row, int col, const QPoint &pos );	//QTable popup menu
	    void tableSet( int row, int col );		//QTable set
	    void dataTimeChange( const QDateTime & );	//Change data-time
	    void editChange( );				//Change QTextEdit
	    void editLineChange( const QString& );	//Change QLineEdit
	    void applyButton( );			//Apply button
	    void cancelButton( );			//Cancel button
	    
	private:	    
	    //Page display
	    void pageDisplay( const string &path );
	    //Prepare path for the tree branch
	    string getItemPath( QListViewItem * i );
	    //View ListItem with recursive processing of the ControllArea	    
	    void viewChildRecArea( const string &path, QListViewItem *i, int level );
	    
	    //Update structure and put service labels
	    bool upStruct(XMLNode &w_nd, const XMLNode &n_nd);
	    
	    //Select ListItem with recursive processing of the ControllArea	    
	    void selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget = NULL, bool refr = false );
	    void basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool refr, bool comm = false );

	    //Controll system requests
	    int cntrIfCmd( XMLNode &node );
	    
	    //Put message
	    void postMess( const string &cat, const string &mess, int type = 1 );

	    //Adress convertors
	    string addr2str( void *addr );
	    void *str2addr( const string &str );
	    
	private:
            QTimer	*autoUpdTimer;
	    
	    QListView	*CtrTree;
	    QLabel	*titleIco;
	    QLabel	*titleLab;
	    QTabWidget  *tabs;
	    QPushButton *w_user;
	    
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
    };    
}

#endif //QTCFG_H



//OpenSCADA module UI.QTCfg file: qtcfg.h
/***************************************************************************
 *   Copyright (C) 2004-2021 by Roman Savochenko, <roman@oscada.org>       *
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

#include <QWidget>
#include <QMainWindow>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QProgressDialog>

#include <string>
#include <vector>

#include <tmess.h>
#include <tsys.h>
#include <tsecurity.h>
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
class QSplitter;

using namespace OSCADA;

namespace QTCFG
{

//***********************************************
// SCADAHost - Host thread's control object     *
class SCADAHost: public QThread
{
    Q_OBJECT

public:
    //Methods
    SCADAHost( const QString &id, const QString &user, bool iIsRemote, QObject *parent = 0 );
    ~SCADAHost( );

    void terminate( );

    void userSet( const QString &user );
    void sendSIGALRM( );

    // To the thread request function, return ready status (true).
    // First-init request will cause short waiting at the condition variable and next only the ready status return
    bool reqDo( XMLNode &node, bool &done );
    // Only checking to done for <node>.
    bool reqBusy( );

    //Attributes
    bool	isRemote;
    time_t	reqTmMax;

signals:
    void setSt( const QString &hid, int lnkOK, const QImage &image, const QStringList &brs = QStringList(), const QString &toolTip = QString() );

protected:
    void run( );

private:
    //Methods
    int cntrIfCmd( XMLNode &node, const QString &user );

    //Attributes
    ResMtx	mtx;
    CondVar	cond;

    QString	id, user;
    bool	lnkOK, endRun, reqDone;
    time_t	tm;

    XMLNode	*req;
    bool	*done;
    pthread_t	pid;		//Thread id
};

//************************************************
//* ConfApp                                      *
//************************************************
class UserStBar;
class TUIMod;

class ConfApp: public QMainWindow
{
    friend class CfgTable;

    Q_OBJECT

public:
    //Methods
    ConfApp( string open_user );
    ~ConfApp( );

    //Atributes
    bool	winClose;			//Closing window flag

signals:
    void makeStarterMenu( QWidget *mn );

protected:
    //Methods
    void closeEvent( QCloseEvent* );		//Close window event
    void resizeEvent( QResizeEvent * );		//Resize window event

private slots:
    //Slots
    void quitSt( );
    bool exitModifChk( );
    void hostStSet( const QString &hid, int lnkOK, const QImage &image, const QStringList &brs, const QString &toolTip );

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
    void pageRefresh( int tm = 0 );
    void pageCyclRefrStart( );
    void pageCyclRefrStop( );

    void stMessChanged( const QString &mess );
    void stHistCall( );

    void userSel( );

    void about( );
    void aboutQt( );
    void enterManual( );
    void enterWhatsThis( );

    void editToolUpdate( );			//Edit tools visible update
    void endRunChk( );				//End run flag check

    // [cur >= 0] for create the progress; [cur < 0] for close-disable the progress bar.
    // [max >= 0] for the progressbar maximum limit set
    void reqPrgrsSet( int cur = -1, const QString &lab = "", int max = -1 );

    // QListView
    void selectItem( );				//Processing of select item signal
    void viewChild( QTreeWidgetItem * i );	//Processing of view item signal
    void onItem( QTreeWidgetItem * i );		//View item path
    void ctrTreePopup( );
    void treeUpdate( );				//Update expanded content of tree
    void treeSearch( );

    // QTabWidget
    void tabSelect( int idx );			//Change current tab

    // Self widget's slots
    void checkBoxStChange( int stat );		//QCheckBox
    void buttonClicked( );			//Button
    void combBoxActivate( const QString& );	//QComboBox
    void listBoxGo( QListWidgetItem* );		//QListBox go for banch
    void listBoxPopup();			//QListBox popup menu
    void tablePopup( const QPoint &pos );	//QTable popup menu
    void tableSet( int row, int col );		//QTable set
    void editChange( const QString& );		//Change Edit (LineEdit and TextEdit)
    void applyButton( );			//Apply button
    void cancelButton( );			//Cancel button
    void imgPopup( const QPoint &pos );		//Image popup

private:
    //Methods
    // Page display
    void selectPage( const string &path, int tm = 0 );
    void pageDisplay( const string path );

    // View ListItem with recursive processing of the ControllArea
    void viewChildRecArea( QTreeWidgetItem *i, bool upTree = false );
    QTreeWidgetItem *getExpandTreeWIt( const string &path );

    // Update structure and put service labels
    bool upStruct( XMLNode &w_nd, const XMLNode &n_nd );

    // Select ListItem with recursive processing of the ControllArea
    void selectChildRecArea( const XMLNode &node, const string &a_path, QWidget *widget = NULL );
    void basicFields( XMLNode &t_s, const string &a_path, QWidget *widget, bool wr, QHBoxLayout **l_hbox, int &l_pos, bool comm = false );

    // Control requests
    void initHosts( );
    int cntrIfCmd( XMLNode &node );
    int cntrIfCmdHosts( XMLNode &node );

    string getPrintVal( const string &vl );

    //Attributes
    QTimer	*endRunTimer, *autoUpdTimer, *reqPrgrsTimer;

    QTreeWidget	*CtrTree;
    QSplitter	*splitter;
    QLabel	*titleIco,
		*titleLab,
		*mStModify;
    QTabWidget	*tabs;
    UserStBar	*wUser;
    QProgressDialog *reqPrgrs;

    QAction	*actUp, *actPrev, *actNext,
		*actUpdate, *actStartUpd, *actStopUpd,
		*actDBLoad, *actDBLoadF, *actDBSave, *actDBSaveF,
		*actItAdd, *actItDel,
		*actItCut, *actItCopy, *actItPaste,
		*actManualPage;

    XMLNode	pgInfo, genReqs;
    XMLNode	*root;
    string	selPath,
		copyBuf;	//Copy buffer

    int		queSz;
    vector<string> prev;
    vector<string> next;

    vector<string> stMess;

    int		inHostReq;
    map<string, SCADAHost*> hosts;

    bool	tblInit, pgDisplay;

    static int	winCntr;
};

}

#endif //QTCFG_H

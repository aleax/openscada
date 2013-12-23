
//OpenSCADA system module UI.Vision file: vis_devel.h
/***************************************************************************
 *   Copyright (C) 2006-2013 by Roman Savochenko                           *
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

#ifndef VIS_DEVEL_H
#define VIS_DEVEL_H

#include <string>
#include <vector>

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>

#include "tvision.h"

using std::string;
using std::vector;

class QMdiArea;
class QSignalMapper;
class QActionGroup;

namespace VISION
{

class UserStBar;
class WScaleStBar;
class WMdfStBar;
class ProjTree;
class WdgTree;
class InspAttrDock;
class InspLnkDock;
class LibProjProp;
class VisItProp;
class ShapeElFigure;

class VisDevelop : public QMainWindow
{
    Q_OBJECT

    public:
	//Public methods
	VisDevelop( const string &open_user, const string &user_pass, const string &VCAstat );
	~VisDevelop( );

	string user( );
	string password( );
	string VCAStation( );
	bool   wdgScale( );
	double wdgVisScale( );
	const string &workWdg()	{ return work_wdg; }

	void setVCAStation( const string& st );
	void setWdgScale( bool val );
	void setWdgVisScale(double val );

	int cntrIfCmd( XMLNode &node, bool glob = false );

	QString getFileName(const QString &caption, const QString &dir, const QString &filter, QFileDialog::AcceptMode mode = QFileDialog::AcceptOpen);

    signals:
	//Public signals
	void modifiedItem( const string& );

    public slots:
	//Public slots
	void selectItem( const string &item, bool force = false );	//Update enabled actions state
	void editToolUpdate( );						//Edit tools visible update
	void modifyToolUpdate( const string& );
	bool exitModifChk( );

	void quitSt( );		//Full quit OpenSCADA

	void about( );		//About at program
	void aboutQt( );	//About at QT library
	void enterWhatsThis( );	//What is GUI components
	void updateMenuWindow();//Generate menu "Windows"

	void itDBLoad( );	//Load item data from DB
	void itDBSave( );	//Save item data to DB

	void prjRun( );		//Run project execution
	void prjNew( );		//New project create
	void libNew( );		//New widgets library creating
	void visualItAdd( QAction*, const QPointF &pnt = QPointF(), const string &wid = "",
	    const string &wnm = "", const string &own = "", bool chNoWr = false );	//Add visual item (widget or page)
	void visualItDel( const string &itms = "", bool chNoWr = false );		//Delete selected visual items
	void visualItProp( );	//Visual item (widget, library, project or page) properties
	void visualItEdit( );	//Visual item graphical edit
	void visualItClear( const string &el_wa = "" );		//Visual item changes clear
	void visualItCut( );	//Visual item cut
	void visualItCopy( );	//Visual item copy
	void visualItPaste( const string &wsrc = "", const string &wdst = "",
	    const string &wnm = "", bool chNoWr = false );	//Visual item paste

    public:
	//Public attributes
	//> Actions
	//>> VCA actions of items
	QAction *actDBLoad,		//Load item from DB
		*actDBSave,		//Save item to DB
		*actPrjRun,		//Run project execution from selected project item
		*actPrjNew,		//New project create
		*actLibNew,		//New widgets library create
		*actVisItAdd, 		//Add visual item to library, container widget, project or page
		*actVisItDel,		//Delete visual item (library, widget, project or page)
		*actVisItProp,		//Visual item (library, widget, project or page) properties
		*actVisItEdit,		//Graphical edit of visual item (widget or page)
		*actVisItClear,		//Clear of visual item changes (widget or page)
		*actVisItCut,		//Cut visual item
		*actVisItCopy,		//Copy visual item
		*actVisItPaste,		//Paste visual item
		*actVisItUnDo,		//UnDo visual item changes
		*actVisItReDo,		//ReDo visual item changes
	//>> Widget's ordering actions
		*actFullScr,		//Full screen action
		*actLevUp,		//Up widget level
		*actLevDown,		//Down widget level
		*actLevRise,		//Rise widget level
		*actLevLower,		//Lower widget level
		*actAlignLeft,		//Align left
		*actAlignVCenter,	//Align vertical center
		*actAlignRight,		//Align right
		*actAlignTop,		//Align top
		*actAlignHCenter,	//Align horizontal center
		*actAlignBottom,	//Align bottom
	//>> Elementar figure actions
		*actElFigLine,		//Create line
		*actElFigArc,		//Create arc
		*actElFigBesie,		//Create Besie curve
		*actElFigCheckAct,	//Enable holds
		*actElFigCursorAct,	//Unset cursor
	//>> Window manipulation actions
		*actWinClose,		//Close window
		*actWinCloseAll,	//Close all windows
		*actWinTile,		//Tile windows
		*actWinCascade,		//Cascade windows
		*actWinNext,		//Select next window
		*actWinPrevious;	//Select previous window
	QActionGroup	*actGrpWdgAdd;	//Add widgets action group

	//> Toolbars
        QToolBar *visItToolBar;         //Main visual items toolbar
	QToolBar *wdgToolView;		//Widget's view functions
	QToolBar *elFigTool;		//Elementar figure base widget tools
	vector<QToolBar*> lb_toolbar;	//Library toolbars

	//> Dock widgets
	WdgTree		*wdgTree;	//Widgets tree
	ProjTree	*prjTree;	//Progects tree
	InspAttrDock	*attrInsp;	//Docked attributes inspector
	InspLnkDock	*lnkInsp;	//Docked links inspector

	//> Work space
	QMdiArea	*work_space;	//MDI widgets workspace

	//> Menu root items
	QMenu	*mn_file,		//Menu "File"
		*mn_edit,		//Menu "Edit"
		*mn_proj,		//Menu "Project"
		*mn_widg,		//Menu "Widget"
		*mn_widg_fnc,		//Submenu "View functions"
		*mn_window,		//Menu "Window"
		*mn_view,		//Menu "View"
		*mn_help;		//Menu "Help"
	vector<QMenu*>	lb_menu;	//Library menus

	QFileDialog	*fileDlg;

    protected:
	//Protected methods
	void closeEvent( QCloseEvent* );
	QMenu *createPopupMenu( );

    private slots:
	//Private slots
	void applyWorkWdg( );	//Timeouted apply work widget
	void endRunChk( );	//End run flag check
	void setToolIconSize( );
	void setActiveSubWindow(QWidget *);

	void fullScreen( bool vl );	//Full screen toggle

    private:
	//Private attributes
	//> Main components
	bool		winClose;
	UserStBar	*mWUser;	//User status widget
	QLabel		*mWStat;	//VCA engine station
	QLabel		*mWVisScale;	//Current widget visual scaling
	WMdfStBar	*mStModify;	//VCA station modified
	WScaleStBar	*w_scale;	//Scale trigger
	QTimer		*endRunTimer, *work_wdgTimer;
	string		work_wdg, work_wdg_new,	//Work widget
			copy_buf;	//Copy buffer
	QSignalMapper	*wMapper;	//Internal window mapper

	//> Main dialogs
	LibProjProp	*prjLibPropDlg;	//Widget's library and project properties dialog
	VisItProp	*visItPropDlg;	//Visual item properties properties dialog
};

}

#endif //VIS_DEVEL

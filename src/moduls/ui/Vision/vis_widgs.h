
//OpenSCADA system module UI.Vision file: vis_widgs.h
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

#ifndef VIS_WIDGS_H
#define VIS_WIDGS_H

#include <string>
#include <vector>

#include <QLabel>
#include <QDialog>
#include <QMap>
#include <QVariant>

//#include "../VCAEngine/widget.h"

using std::string;
using std::vector;

class QMainWindow;
class QComboBox;
class QLineEdit;

namespace VISION
{
    //*********************************************
    //* Id and name input dialog                  *
    //*********************************************
    class InputDlg : public QDialog
    {
	public:
	    InputDlg( QWidget *parent, const QIcon &icon, const QString &mess, 
		    const QString &ndlg = "Vision dialog", bool with_id = false, bool with_nm = true );

	    QString id();
	    QString name();

	    void setId( const QString &val );
	    void setName( const QString &val );

	private:
	    QLineEdit 	*m_id, *m_name;
    };    

    //*********************************************
    //* User select dialog                        *
    //*********************************************
    class DlgUser : public QDialog
    {
	Q_OBJECT
    
	public:
	    //Data
	    enum Results { SelCancel, SelOK, SelErr };

	    //Methods
	    DlgUser( QWidget *parent = 0 );

	    QString user();
	    QString password();

	private slots:
	    void finish( int result );

	private:
	    QComboBox 	*users;
	    QLineEdit 	*passwd;
    };
    
    //*********************************************
    //* Status bar user widget                    *
    //*********************************************
    class UserStBar : public QLabel
    {
	Q_OBJECT
    
	public:
	    UserStBar( const QString &iuser, QWidget * parent = 0 );

	    QString user();
	    void setUser( const QString &val );

	    bool userSel( );

	signals:
            void userChanged();

	protected:
	    bool event( QEvent *event );

	private:
	    QString	user_txt;
    };    
    
    //****************************************
    //* Shape widget view                    *
    //****************************************
    class WdgShape;
    
    class WdgView: public QWidget
    {
	Q_OBJECT
    
    	//- Main part -
	//-------------
        public:
    	    //- Public methods -
	    WdgView( const string &iwid, int ilevel, bool devMod, 
		    QMainWindow *mainWind, QWidget* parent = 0 );
	    ~WdgView( );

	    string id( )    		{ return idWidget; }
	    bool   develMode( )		{ return mode_dev; }
	    int    wLevel( )		{ return w_level; }
	    QMap<QString, QString>  &dataReq()  { return reqdata; }
	    QMap<QString, QVariant> &dataCache(){ return cache_data; }
	    QMainWindow *mainWin( )	{ return main_win; }
	    int	   z( )			{ return z_coord;  }
	    string user( );

	    void   setZ( int val )	{ z_coord = val; }

	public slots:
	    void loadData( const string& item, bool update = false );
	    void saveData( const string& item );
	    
	private:
	    //- Private methods -
	    bool event( QEvent * event );
	    bool eventFilter( QObject *object, QEvent *event );	    
	    
	    //- Private attributes -
	    int 		w_level;	//Widget level
	    int			z_coord;	//Z coordinate
	    bool		mode_dev;	//Develop mode
	    string		idWidget;	//Full widget identifier
	    WdgShape		*shape;		//Link to root widget shape
	    QMap<QString, QVariant> cache_data;	//Internal data cache
	    QMainWindow		*main_win;	//Main window
	    time_t		reqtm;		//Request values time
	    QMap<QString, QString> reqdata;	//Request values data	    

    	
	//- Developing mode part -
	//------------------------
        public:
	    //- Public methods -
	    bool select( );				//Select widget state
	    string selectChilds( int *cnt = NULL );	//Get selected include widgets list
	    bool edit( );				//Edit mode state
	    bool moveHold( );				//Mouse move hold state
	    bool holdChild( );				//Hold child widget in time of moving and resizing

	    void setSelect( bool vl, bool childs = true );
	    void setEdit( bool vl );
	    void setMoveHold( bool vl );
	    void setHoldChild( bool vl );
	    
	signals:
	    void selected( const string& item );	//Change selection signal

	public slots:
	    void wdgViewTool( QAction* );   		//View order and align of included widgets operated

	private:
	    //- Private methods -
	    bool grepAnchor( const QPoint &apnt, const QPoint &cpnt );	
	    void upMouseCursors( const QPoint &pnt );
	    
	    //- Private data -
	    class SizePntWdg : public QWidget
	    {
		public:
		    SizePntWdg( QWidget* parent = 0 );
		    
		    void setSelArea( const QRect &geom, bool edit = false );
		    void paintEvent( QPaintEvent * event );
		
		private:
		    bool m_edit;
	    };
	    //- Private attributes -
	    bool 	leftTop;	//Left top anchors
	    QPoint 	hold_pnt;	//Hold move point
	    SizePntWdg	*pnt_view;	//Select size point view
    };
}

#endif //VIS_WIDGS_H


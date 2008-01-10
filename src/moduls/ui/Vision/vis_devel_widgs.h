
//OpenSCADA system module UI.Vision file: vis_devel_widgs.h
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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

#ifndef VIS_DEVEL_WIDGS_H
#define VIS_DEVEL_WIDGS_H

#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QDockWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QItemDelegate>

#include "vis_widgs.h"

using std::string;
using std::vector;

class QTreeWidgetItem;
class QTreeWidget;
class QDrag;

namespace VISION
{

//****************************************
//* Inspector of attributes model        *
//**************************************** 
class VisDevelop;

class ModInspAttr: public QAbstractTableModel
{
    Q_OBJECT

    public:
        //Public data
 	//* Item of the inspector of attributes model   *
	class Item
	{
	    public:
		//Public data
		enum Type { WdgGrp, Wdg, AttrGrp, Attr };
		enum Flag 
		{ 
		    Select = 0x01, 
		    FullText = 0x08,
		    Active = 0x0100
		};

		//Public attributes
                Item( const string &iid, Type tp, Item *parent = NULL );
		~Item();

                string id( )        	{ return idItem; }
                string name( );
                Type   type( )		{ return typeItem; }
		bool   edited()		{ return edit_access; }
		int    flag()		{ return flag_item; }
                QVariant data( );
                QVariant dataEdit( );

                void setName( const string &nit )       { nameItem = nit; }
		void setEdited( bool ied )		{ edit_access = ied; }
		void setFlag( int iflg )		{ flag_item = iflg; }
                void setData( const QVariant &idt )     { dataItem = idt; }
                void setDataEdit( const QVariant &idt ) { dataEditItem = idt; }		

                void clean( );

                Item *child(int row) const;
                int  childGet( const string &id ) const;
		int  childCount() const;
                int  childInsert( const string &id, int row, Type tp );
                void childDel( int row );
		
		Item *parent()	{ return parentItem; }
		
	    private:
                string  idItem, nameItem;
                Type    typeItem;
                QVariant dataItem, dataEditItem;
		bool	edit_access;
		int	flag_item;

		QList<Item*> childItems;
		Item *parentItem;
	};                

	//Public methods
	ModInspAttr( const string &wdg, VisDevelop *mainWind );
	~ModInspAttr( );
	
	string user( );
	
	void setWdg( const string &iwdg );
	
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        QModelIndex parent(const QModelIndex &index) const;
	
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        bool setData ( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );
	
	VisDevelop *mainWin( )     { return main_win; }

    signals:
	void modified( const string &idwdg );

    private:
        //Private methods
        void wdgAttrUpdate( const QModelIndex &mod_it );// Item *it);

	//Private attributes
	string cur_wdg;
	Item *rootItem;
	VisDevelop *main_win;
};

//****************************************
//* Inspector of attributes widget       *
//****************************************
class InspAttr: public QTreeView
{
    Q_OBJECT
    
    public:
	//Public methods
	InspAttr( QWidget * parent, VisDevelop *mainWind );
	~InspAttr( );
		
	bool hasFocus( );
		
	void setWdg( const string &iwdg );

    signals:	
	void modified( const string &idwdg );
	
    private:
	//Private data
	//* Attributes item delegate    *
	//*******************************
	class ItemDelegate: public QItemDelegate
	{
	    public:
		//Public methods
	        ItemDelegate(QObject *parent = 0);

	        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	        void setEditorData(QWidget *editor, const QModelIndex &index) const;
    		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	    private:		
		//Private methods			
	        bool eventFilter(QObject *object, QEvent *event);
	};
	
	//Private methods
	bool event( QEvent *event );
	
	//Private attributes
	ModInspAttr modelData;
};

//****************************************
//* Inspector of attributes dock widget  *
//**************************************** 
class VisDevelop; 

class InspAttrDock: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	InspAttrDock( VisDevelop * parent );
	~InspAttrDock( );
	
	VisDevelop *owner( );
	
	bool hasFocus( );

    signals:
	void modified( const string &idwdg );
	
    public slots:
	void setWdg( const string &iwdg );	

    private:
        //Private attributes
        InspAttr *ainsp_w;
};

//****************************************
//* Inspector of links widget            *
//****************************************
class InspLnk: public QTreeWidget
{
    Q_OBJECT
    
    public:
	//Public methods
	InspLnk( QWidget * parent, VisDevelop *mainWind );
	~InspLnk( );
	
	string user( );
	
	void setWdg( const string &iwdg );

	VisDevelop *mainWin( )     { return main_win; }
	
    public slots:
	void changeLnk( QTreeWidgetItem*, int );
	
    private:
	//Private data
	//* Link item delegate    *
	//*************************
	class ItemDelegate: public QItemDelegate
	{
	    public:
		//Public methods
	        ItemDelegate(InspLnk *parent = 0);		

	        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	        void setEditorData(QWidget *editor, const QModelIndex &index) const;
    		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		
		InspLnk *owner( ) const;
	};
	//Private attributes
	bool show_init;
	string it_wdg;
	VisDevelop *main_win;
};
 
//****************************************
//* Inspector of links dock widget       *
//**************************************** 
class InspLnkDock: public QDockWidget
{    
    Q_OBJECT

    public:
	//Public methods
	InspLnkDock( VisDevelop * parent );
	~InspLnkDock( );
	
	VisDevelop *owner( );

    public slots:
	void setWdg( const string &iwdg );	

    private:
	//Private attributes
	InspLnk *ainsp_w;
};
 
//****************************************
//* Widget's libraries tree              *
//****************************************
class WdgTree: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	WdgTree( VisDevelop *parent = 0 );
	~WdgTree();
	
	VisDevelop *owner( );
	
	bool hasFocus( );

    signals:
        void selectItem( const string &vca_it );

    public slots:	
	void updateTree( const string &vca_it = "" );
	
    protected:
	//Protecten methods
	bool eventFilter( QObject *target, QEvent *event );

    private slots:
	//Private slots
	void ctrTreePopup( );
	void selectItem( );
	
    private:
	//Private attributes
	QTreeWidget *treeW;
	QPoint      dragStartPos;
};
 
//****************************************
//* Project's tree                       *
//**************************************** 
class ProjTree: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	ProjTree( VisDevelop * parent = 0 );
	~ProjTree();
	
	bool hasFocus( );
	
	VisDevelop *owner( );	

    signals:
        void selectItem( const string &idwdg );

    public slots:	
	void updateTree( const string &vca_it = "", QTreeWidgetItem *it = NULL );
	
    protected:
	//Protecten methods
	bool eventFilter( QObject *target, QEvent *event );

    private slots:
	//Private slots
	void ctrTreePopup( );
	void selectItem( );	
	
    private:
	//Private attributes
	QTreeWidget *treeW;	
};

//****************************************
//* Shape widget view development mode   *
//****************************************	
class DevelWdgView: public WdgView
{
    Q_OBJECT
		
    public:
	//Data
	enum DevelFlgs
	{
	    makeScale 	= 0x01,	//Make visual item scaling
	    wdgEdit   	= 0x02,	//Widget under edition
	    wdgSelect 	= 0x04,	//Widget is selected
	    moveHold  	= 0x08,	//Mouse move hold state
	    holdChild 	= 0x10,	//Hold child widget in time of moving and resizing
	    leftTop   	= 0x20,	//Left top anchors
	    holdSelRect = 0x40	//Hold for select rect
	};
    
	//Public methods
        DevelWdgView( const string &iwid, int ilevel, VisDevelop *mainWind, QWidget* parent = 0 );
	~DevelWdgView( );
	
	string user( );
	VisDevelop *mainWin( );
	
        bool select( )		{ return m_flgs&wdgSelect; }	//Select widget state
        string selectChilds( int *cnt = NULL, vector<DevelWdgView*> *wdgs = NULL );     	//Get selected include widgets list
        bool edit( )		{ return m_flgs&wdgEdit; }     	//Edit mode state

        void setSelect( bool vl, bool childs = true );
        void setEdit( bool vl );
	void wdgsMoveResize( const QPointF &dP );
	
	WdgView *newWdgItem( const string &iwid );
						
    signals:
        void selected( const string& item );        //Change selection signal
	void apply( const string & );
    
    public slots:
        void wdgViewTool( QAction* );               //View order and align of included widgets operated
	void saveGeom( const string& item );
	void wdgPopup( );
	void makeIcon( );
	void editEnter( );
	void editExit( );
	
    protected:
        //Protected methods
	bool event( QEvent * event );
	int cntrIfCmd( XMLNode &node, bool glob = false );
    
    private:
	//Private data
        class SizePntWdg : public QWidget
        {
            public:
		//Data
		enum WView { SizeDots, EditBorder, SelectBorder };
		//Methods
                SizePntWdg( QWidget* parent = 0 );
                
		void setSelArea( const QRect &geom, WView view = SizeDots );
                void paintEvent( QPaintEvent * event );
            
	    private:
		//Attributes
		WView 	view;
        };
	//Private methods
        bool grepAnchor( const QPointF &apnt, const QPoint &cpnt );
        void upMouseCursors( const QPoint &pnt );
	
        //Private attributes
	char		m_flgs;		//Developmen flags
	QPoint 		holdPnt;	//Hold move point
        SizePntWdg 	*pntView;	//Point view
	DevelWdgView	*editWdg;
        QPoint 		dragStartPos;
};

}

#endif //VIS_DEVEL_WIDGS

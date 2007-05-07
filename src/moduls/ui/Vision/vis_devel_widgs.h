
//OpenSCADA system module UI.Vision file: vis_devel_widgs.h
/***************************************************************************
 *   Copyright (C) 2004-2007 by Roman Savochenko                           *
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

#ifndef VIS_DEVEL_WIDGS_H
#define VIS_DEVEL_WIDGS_H

#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QDockWidget>
#include <QTreeView>
#include <QTreeWidget>
#include <QItemDelegate>

using std::string;
using std::vector;

class QTreeWidgetItem;
class QTreeWidget;

namespace VISION
{

//****************************************
//* Inspector of attributes model        *
//**************************************** 
class ModInspAttr: public QAbstractTableModel
{
    Q_OBJECT

    public:    
	//Public methods
	ModInspAttr( const string &iwdg = "" );
	~ModInspAttr( );
	
	void setWdg( const string &iwdg );
	
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        QModelIndex parent(const QModelIndex &index) const;
	
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;
	int columnCount( const QModelIndex &parent = QModelIndex() ) const;
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        bool setData ( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );

    signals:
	void modified( const string &idwdg );

    private:
	//Private data
	//* Item of the inspector of attributes model   *
	class Item
	{
	    public:
		//Public data
		enum Type { WdgGrp, Wdg, AttrGrp, Attr };

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
    
        //Private methods
        void wdgAttrUpdate(Item *it);

	//Private attributes
	Item *rootItem;
};

//****************************************
//* Inspector of attributes widget       *
//****************************************
class InspAttr: public QTreeView
{
    Q_OBJECT
    
    public:
	//Public methods
	InspAttr( QWidget * parent = 0 );
	~InspAttr( );
	
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
class InspAttrDock: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	InspAttrDock( QWidget * parent = 0 );
	~InspAttrDock( );

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
	InspLnk( QWidget * parent = 0 );
	~InspLnk( );
	
	void setWdg( const string &iwdg );
	
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
	        ItemDelegate(QObject *parent = 0);

	        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	        void setEditorData(QWidget *editor, const QModelIndex &index) const;
    		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	};
	//Private attributes
	bool show_init;
	string it_wdg;
};
 
//****************************************
//* Inspector of links dock widget       *
//**************************************** 
class InspLnkDock: public QDockWidget
{    
    Q_OBJECT

    public:
	//Public methods
	InspLnkDock( QWidget * parent = 0 );
	~InspLnkDock( );	

    public slots:
	void setWdg( const string &iwdg );	

    private:
	//Private attributes
	InspLnk *ainsp_w;
};
 
//****************************************
//* Widget's libraries tree              *
//****************************************
class VisDevelop;

class WdgTree: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	WdgTree( VisDevelop *parent = 0 );
	~WdgTree();
	
	VisDevelop *owner( );	

    signals:
        void selectItem( const string &vca_it );

    public slots:	
	void updateTree( const string &vca_it = "" );
	void pressItem( QTreeWidgetItem* );
	
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
//* Project's tree                       *
//**************************************** 
class ProjTree: public QDockWidget
{
    Q_OBJECT

    public:
	//Public methods
	ProjTree( VisDevelop * parent = 0 );
	~ProjTree();
	
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

}

#endif //VIS_DEVEL_WIDGS

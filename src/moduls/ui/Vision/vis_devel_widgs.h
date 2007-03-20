
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
#include <QDialog>
#include <QTreeView>
#include <QItemDelegate>

using std::string;
using std::vector;

class QTreeWidgetItem;
class QTreeWidget;
class QLabel;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QTextEdit;
class QDialogButtonBox;
class QTabWidget;

namespace VISION
{

//****************************************
//* Inspector of attributes model        *
//**************************************** 
class ModInspAttr: public QAbstractTableModel
{
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

                string id( )        { return idItem; }
                string name( );
                Type type( )        { return typeItem; }
                QVariant data( );
                QVariant dataEdit( );

                void setName( const string &nit )       { nameItem = nit; }
                void setData( const QVariant &idt )     { dataItem = idt; }
                void setDataEdit( const QVariant &idt ) { dataEditItem = idt; }

                void clean( );

                Item *child(int row) const;
                int childGet( const string &id ) const;
		int childCount() const;
                int childInsert( const string &id, int row, Type tp );
                void childDel( int row );
		
		Item *parent()	{ return parentItem; }
		
	    private:
                string  idItem, nameItem;
                Type    typeItem;
                QVariant dataItem, dataEditItem;

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
    public:
	//Public methods
	InspAttr( QWidget * parent = 0 );
	~InspAttr( );
	
	void setWdg( const string &iwdg );
	
    private:
	//Private data
	//* Attributes item delegate    *
	class ItemDelegate: public QItemDelegate
	{
	    public:
		//Public methods
	        ItemDelegate(QObject *parent = 0);

	        //void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	        void setEditorData(QWidget *editor, const QModelIndex &index) const;
    		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	        //void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &indeex) const;

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
    public:
	//Public methods
	InspAttrDock( QWidget * parent = 0 );
	~InspAttrDock( );

        void setWdg( const string &iwdg );

    private:
        //Private attributes
        InspAttr *ainsp_w;
};
 
//****************************************
//* Inspector of links dock widget       *
//**************************************** 
class InspLnk: public QDockWidget
{
    public:
	//Public methods
	InspLnk( QWidget * parent = 0 );
	~InspLnk( );
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
	
	void updateLibs();	

	VisDevelop *owner();
	
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
    public:
	//Public methods
	ProjTree( VisDevelop * parent = 0 );
	~ProjTree();
};

//****************************************
//* Widget's library properties dialog   *
//**************************************** 
class WdgLibProp: public QDialog
{
    Q_OBJECT

    public:
	//Public methods
	WdgLibProp( VisDevelop *parent = 0 );
	~WdgLibProp( );

	string user();
	string grp();	
	short permit();
	
	void  setPermit( short vl );
	
	void showDlg( const string &ilb );

	VisDevelop *owner();

    private slots:
	//Private slots
	void selectIco( );
	void isModify( );
	void pressApply( );
	void pressCancel( );
	void pressClose( );
	void selectPermission( );
	void selectUser(const QString &val);
	
    private:
	//Private attributes
	QPushButton *wlb_ico;	//Icon
	QCheckBox *wlb_enable;	//Enabled stat
        QLineEdit *wlb_db;	//DB
	QComboBox *wlb_user,	//User 
		  *wlb_grp,	//Group
		  *wlb_accuser,	//User access
		  *wlb_accgrp,	//Group access
		  *wlb_accother;//Other access
	QLabel    *wlb_id;	//Id
	QLineEdit *wlb_name;	//Name
	QTextEdit *wlb_descr;	//Description
	QDialogButtonBox *butbox;	//Buttons
	
	bool	  is_modif, ico_modif;
	string	  ed_lib;
};

//****************************************
//* Widget properties dialog             *
//**************************************** 
class ModInspAttr;

class WdgProp: public QDialog
{
    Q_OBJECT

    public:
	//Public methods
	WdgProp( VisDevelop *parent = 0 );
	~WdgProp( );

	string user();
	string grp();	
	short permit();
	
	void  setPermit( short vl );
	
	void showDlg( const string &ilb );

	VisDevelop *owner();

    private slots:
	//Private slots
	void selectIco( );
	void selectParent(const QString &val);
	void isModify( );
	void pressApply( );
	void pressCancel( );
	void pressClose( );
	void selectPermission( );
	void selectUser(const QString &val);
	
    private:
	//Private attributes
	QTabWidget  *wdg_tabs;	//Tabs
	QPushButton *wdg_ico;	//Icon
	QCheckBox *wdg_enable;	//Enabled stat
	QComboBox *wdg_parent;	//Parent widget
	QComboBox *wdg_user,	//User 
		  *wdg_grp,	//Group
		  *wdg_accuser,	//User access
		  *wdg_accgrp,	//Group access
		  *wdg_accother;//Other access
	QLabel    *wdg_id;	//Id
	QLineEdit *wdg_name;	//Name
	QTextEdit *wdg_descr;	//Description	
	InspAttr  *wdg_attr;	//Attributes inspector
	
	QDialogButtonBox *butbox;	//Buttons
	
	bool	  is_modif, ico_modif;
	string	  ed_lib;
};


}

#endif //VIS_DEVEL_WIDGS

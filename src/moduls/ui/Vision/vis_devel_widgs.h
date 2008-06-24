
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
		    Select	= 0x01,
		    FullText	= 0x08,
		    Active	= 0x0100,
		    Image	= 0x0200,
		    Color	= 0x0400,
		    Font	= 0x0800,
		    DataTime	= 0x0200
		};

		//Public attributes
		Item( const string &iid, Type tp, Item *parent = NULL );
		~Item( );

		string	id( )		{ return idItem; }
		string	name( );
		Type	type( )		{ return typeItem; }
		bool	edited()	{ return edit_access; }
		int	flag()		{ return flag_item; }
		QVariant data( );
		QVariant dataEdit( );

		void setName( const string &nit )	{ nameItem = nit; }
		void setEdited( bool ied )		{ edit_access = ied; }
		void setFlag( int iflg )		{ flag_item = iflg; }
		void setData( const QVariant &idt )	{ dataItem = idt; }
		void setDataEdit( const QVariant &idt )	{ dataEditItem = idt; }

		void clean( );

		Item *child(int row) const;
		int  childGet( const string &id ) const;
		int  childCount() const;
		int  childInsert( const string &id, int row, Type tp );
		void childDel( int row );

		Item *parent()		{ return parentItem; }

	    private:
		string	idItem, nameItem;
		Type	typeItem;
		QVariant	dataItem, dataEditItem;
		bool	edit_access;
		int	flag_item;

		QList<Item*>	childItems;
		Item	*parentItem;
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

	VisDevelop *mainWin( )		{ return main_win; }

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
	//Private methods
	bool event( QEvent *event );

	//Private attributes
	bool show_init;
	string it_wdg;
	VisDevelop *main_win;
};

//*************************
//* Link item delegate    *
//*************************
class LinkItemDelegate: public QItemDelegate
{
    Q_OBJECT

    public:
	//Public methods
	LinkItemDelegate(InspLnk *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	InspLnk *owner( ) const;

    //public slots:
    //	void selItem( int val );

    private:
	bool initVal;
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
        void selectItem( const string &vca_it, bool force = false );

    public slots:
	void updateTree( const string &vca_it = "" );

    protected:
	//Protecten methods
	bool eventFilter( QObject *target, QEvent *event );

    private slots:
	//Private slots
	void ctrTreePopup( );
	void dblClick( );
	void selectItem( bool force = false );

    private:
	//Private attributes
	QTreeWidget	*treeW;
	QPoint		dragStartPos;
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
        void selectItem( const string &idwdg, bool force = false );

    public slots:
	void updateTree( const string &vca_it = "", QTreeWidgetItem *it = NULL );

    protected:
	//Protecten methods
	bool eventFilter( QObject *target, QEvent *event );

    private slots:
	//Private slots
	void ctrTreePopup( );
	void dblClick( );
	void selectItem( bool force = false );

    private:
	//Private attributes
	QTreeWidget *treeW;
};

//**********************************************************************************************
//* Text edit line widget with detail dialog edit button. Support: Font and Color edit dialogs.*
//**********************************************************************************************
class LineEditProp : public QWidget
{
    Q_OBJECT

    public:
	//- Data -
	enum DType { Font, Color };

	//- Methods -
	LineEditProp( QWidget *parent, DType tp = Font );

	DType type( )       { return m_tp; }
	QString value( );

	void setType( DType tp )	{ m_tp = tp; }
	void setValue( const QString& );

    private slots:
	void callDlg( );

    private:
	DType		m_tp;
	QLineEdit	*ed_fld;
};

//*********************************************
//* Status bar scale indicator                *
//*********************************************
class WScaleStBar : public QLabel
{
    Q_OBJECT

    public:
	WScaleStBar( QWidget *parent = 0 );

	bool scale()			{ return isScale; }
	void setScale( bool val );

    protected:
	void mousePressEvent( QMouseEvent * event );

    private:
	bool isScale;
};

//*************************************************
//* Size points widget                            *
//*************************************************
class SizePntWdg : public QWidget
{
    Q_OBJECT

    public:
	//Data
	enum WView { Hide, SizeDots, EditBorder, SelectBorder };
	//Methods
	SizePntWdg( QWidget* parent = 0 );

	QPointF posF( )             { return w_pos; }
	QSizeF  sizeF( )            { return w_size; }
	QRectF geometryF( )         { return QRectF(w_pos,w_size); }

	void setSelArea( const QRectF &geom, WView view = SizeDots );
	bool event( QEvent *event );

    public slots:
	void apply( );

    private:
	//Attributes
	WView	view;
	QPointF	w_pos;		//Widget position into real;
	QSizeF	w_size;		//Widget size into real;
};

//*************************************************
//* Shape widget view development mode            *
//*************************************************
class DevelWdgView: public WdgView
{
    Q_OBJECT

    public:
	//Data
	enum DevelFlgs
	{
	    makeScale	= 0x01,		//Make visual item scaling
	    wdgEdit	= 0x02,		//Widget under edition
	    wdgSelect	= 0x04,		//Widget is selected
	    moveHold	= 0x08,		//Mouse move hold state
	    holdChild	= 0x10,		//Hold child widget in time of moving and resizing
	    leftTop	= 0x20,		//Left top anchors
	    holdSelRect	= 0x40,		//Hold for select rect
	    moveHoldMove= 0x80,		//Mouse move on hold
	    hideChilds  = 0x100		//Hide childs on move
	};

	//Public methods
	DevelWdgView( const string &iwid, int ilevel, VisDevelop *mainWind, QWidget* parent = 0 );
	~DevelWdgView( );

	string user( );
	VisDevelop *mainWin( );

	short int flags( )	{ return m_flgs; }
	bool select( )		{ return m_flgs&wdgSelect; }	//Select widget state
	string selectChilds( int *cnt = NULL, vector<DevelWdgView*> *wdgs = NULL );	//Get selected include widgets list
	bool edit( )		{ return m_flgs&wdgEdit; }	//Edit mode state

	void setSelect( bool vl, bool childs = true );
	void setEdit( bool vl );
	void wdgsMoveResize( const QPointF &dP );

	WdgView *newWdgItem( const string &iwid );

	void load( const string& item, bool load = true, bool init = true );

    signals:
	void selected( const string& item );		//Change selection signal
	void apply( const string & );

    public slots:
	void wdgViewTool( QAction* );			//View order and align of included widgets operated
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
	//Private methods
	bool grepAnchor( const QPointF &apnt, const QPoint &cpnt );
	void upMouseCursors( const QPoint &pnt );

	//Private attributes
	short int	m_flgs;		//Developmen flags
	QPoint		holdPnt;	//Hold move point
	SizePntWdg	*pntView;	//Point view
	DevelWdgView	*editWdg;
	QPoint		dragStartPos;
};

}

#endif //VIS_DEVEL_WIDGS


//OpenSCADA system module UI.Vision file: vis_devel_dlgs.h
/***************************************************************************
 *   Copyright (C) 2007-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef VIS_DEVEL_DLGS_H
#define VIS_DEVEL_DLGS_H

#include <string>
#include <vector>

#include <QDialog>
#include <QItemDelegate>
#include <QCloseEvent>

#include "vis_devel_widgs.h"

using std::string;
using std::vector;

class QLabel;
class QComboBox;
class QCheckBox;
class QDialogButtonBox;
class QTabWidget;
class QTableWidget;

namespace VISION
{

//****************************************************
//* Widget's library and project properties dialog   *
//****************************************************
class VisDevelop;
class LineEdit;
class TextEdit;

class LibProjProp: public QDialog
{
    Q_OBJECT

    public:
	//Public methods
	LibProjProp( VisDevelop *parent = 0 );
	~LibProjProp( );

	void showDlg( const string &iit, bool reload = false );

	VisDevelop *owner();

    protected:
	//Protected methods
	void closeEvent( QCloseEvent* );
	void showEvent( QShowEvent * event );

    signals:
	void apply(const string &);

    private slots:
	//Private slots
	void selectIco( );
	void isModify( QObject *snd = NULL );

	void addMimeData( );
	void delMimeData( );
	void loadMimeData( );
	void unloadMimeData( );
	void mimeDataChange( int, int );

	void delStlItem( );
	void stlTableChange( int, int );

	void tabChanged( int itb );

    private:
	//Private attributes
	QTabWidget	*wdg_tabs;	//Tabs
	QPushButton	*obj_ico;	//Icon
	QCheckBox	*obj_enable,	//Enabled stat
			*prj_keepAspRt;	//Keep master page aspect ratio
	LineEdit	*obj_db;	//DB
	QComboBox	*obj_user,	//User
			*obj_grp,	//Group
			*obj_accuser,	//User access
			*obj_accgrp,	//Group access
			*obj_accother,	//Other access
			*prj_runw;	//Project's run window mode
	QLabel		*obj_id,	//Id
			*obj_tmstmp;	//TimeStamp
	LineEdit	*obj_name,	//Name
			*prj_ctm;	//Calc time of project
	TextEdit	*obj_descr;	//Description

	QTableWidget	*mimeDataTable;
	QPushButton	*buttDataAdd,
			*buttDataDel,
			*buttDataLoad,
			*buttDataUnload;

	QComboBox	*stl_select;	//Style select
	LineEdit	*stl_name;	//Name
	QTableWidget	*stl_table;	//Style's iems
	QPushButton	*buttStlTableDel,
			*buttStlDel;

	QDialogButtonBox *butbox;	//Buttons

	bool		show_init, is_modif, ico_modif;
	string		ed_it;
};

//****************************************
//* Visual item properties dialog        *
//****************************************
class VisItProp : public QDialog
{
    Q_OBJECT

    public:
	//Public methods
	VisItProp( VisDevelop *parent = 0 );
	~VisItProp( );

	void showDlg( const string &iit, bool reload = false );

	VisDevelop *owner();

    signals:
	void apply( const string& );

    protected:
	//Protected methods
	void closeEvent( QCloseEvent* );
	void showEvent( QShowEvent * event );

    private slots:
	//Private slots
	void selectIco( );
	void selectParent( );
	void isModify( QObject *snd = NULL );

	void addAttr( );
	void delAttr( );
	void changeAttr(QTreeWidgetItem *it, int col);

	void tabChanged( int itb );

    private:
	//Private data
	//* Attributes item delegate    *
	class ItemDelegate: public QItemDelegate
	{
	    public:
		//Public methods
		ItemDelegate(QObject *parent = 0);

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	};
	//Private attributes
	QTabWidget	*wdg_tabs;	//Tabs
	QLabel		*obj_id,	//Id
			*obj_root,	//Root
			*obj_path,	//Path
			*obj_tmstmp;	//TimeStamp
	QPushButton	*obj_ico;	//Icon
	QCheckBox	*obj_enable;	//Enabled stat
	QComboBox	*obj_parent,	//Parent widget
	 		*obj_user,	//User
			*obj_grp,	//Group
			*obj_accuser,	//User access
			*obj_accgrp,	//Group access
			*obj_accother,	//Other access
			*pg_tp;		//Page: Page type

	LineEdit	*obj_name;	//Name
	TextEdit	*obj_descr;	//Description

	InspAttr	*obj_attr;	//Attributes inspector
	InspLnk		*obj_lnk;	//Links inspector

	LineEdit	*proc_per;	//Procedure calculate period
	QComboBox	*proc_lang;	//Widget's procedure name
	TextEdit	*proc_text;	//Widget's procedure program text

	QTreeWidget	*obj_attr_cfg;	//Attribute configuration widget
	QPushButton	*buttAttrAdd,	//Add new attribute button
			*buttAttrDel;	//Delete attribute record

	QDialogButtonBox *butbox;	//Buttons

	bool		show_init, is_modif, ico_modif, lib_wdg;
	string		ed_it;
};

}

#endif //VIS_DEVEL_DLGS

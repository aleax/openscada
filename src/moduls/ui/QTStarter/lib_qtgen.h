
//OpenSCADA module UI.QTStarter file: lib_qtgen.h
/***************************************************************************
 *   Copyright (C) 2021-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#ifndef LIB_QTGEN_H
#define LIB_QTGEN_H

#include <QItemDelegate>
#include <QSyntaxHighlighter>

#include <xml.h>

#define LIM_TEXT_DEF	300

using namespace OSCADA;

namespace OSCADA_QT
{

extern int icoSize( float mult = 1 );

extern QColor colorAdjToBack( const QColor &clr, const QColor &backClr );

extern QFont getFont( const string &val, float fsc = 1, bool pixSize = true, const QFont &defFnt = QFont() );
extern QColor getColor( const string &val );

//*************************************************
//* TableDelegate: Combobox table delegate        *
//*************************************************
class TableDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
	//Data
	enum AddRoles { SelectRole = Qt::UserRole+10, TextLimRole, OneLineString };

	//Public attributes
	TableDelegate( QObject *parent = 0 );

	QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void setEditorData( QWidget *editor, const QModelIndex &index ) const;
	void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

	void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:
	//Private attributes
	bool eventFilter( QObject *object, QEvent *event );
};

//*************************************************
//* SnthHgl: Syntax highlighter                   *
//*************************************************
class SnthHgl : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
	//Methods
	SnthHgl( QTextDocument *parent = 0 );

	XMLNode &snthHgl( )	{ return rules; }
	void setSnthHgl( const XMLNode &nd );

	static bool checkInSnthHgl( const QString &text, XMLNode &nd );

	//Attributes
	bool isBuiltInSH;

    protected:
	void highlightBlock( const QString &text );
	void rule( XMLNode *irl, const QString &text, int off = 0, char lev = 0 );

	XMLNode rules;
};

}

#endif //LIB_QTGEN_H

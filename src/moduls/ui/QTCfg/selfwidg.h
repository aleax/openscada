
//OpenSCADA system module UI.QTCfg file: selfwidg.h
/***************************************************************************
 *   Copyright (C) 2004-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#ifndef SELFWIDG_H
#define SELFWIDG_H

#include <string>
#include <vector>

#include <QLabel>
#include <QWidget>
#include <QImage>
#include <QDialog>
#include <QDateTimeEdit>
#include <QItemDelegate>
#include <QDialogButtonBox>
#include <QSyntaxHighlighter>
#include <QTableWidget>

#include "tuimod.h"

using std::string;
using std::vector;


class QPaintEvent;
class QComboBox;
class QLineEdit;
class QHBoxLayout;
class QTextEdit;
class QDialogButtonBox;
class QGridLayout;

namespace QTCFG
{

//*********************************************************************************************
//* Universal edit line widget. Contain support of: QLineEdit, QSpinBox, QDoubleSpinBox,      *
//* QTimeEdit, QDateEdit and QDateTimeEdit.                                                   *
//*********************************************************************************************
class LineEdit : public QWidget
{
    Q_OBJECT

    public:
	//> Data
	enum LType { Text, Integer, Real, Time, Date, DateTime, Combo };

	//> Methods
	LineEdit( QWidget *parent, LType tp = Text, bool prev_dis = false );

	LType type( )		{ return m_tp; }
	QString value( );
	bool isEdited( );

	void setType( LType tp );
	void setValue( const QString& );
	void setCfg( const QString& );

	QWidget *workWdg( )	{ return ed_fld; }
	void viewApplyBt( bool view );

    signals:
	void apply( );
	void cancel( );
	void valChanged( const QString& );

    protected:
	bool event( QEvent * e );

    private slots:
	void changed( );
	void applySlot( );

    private:
	LType		m_tp;
	QString		m_val;
	bool		mPrev;
	QWidget		*ed_fld;
	QPushButton	*bt_fld;
};

//*************************************************
//* SyntxHighl: Syntax highlighter                *
//*************************************************
class SyntxHighl : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
	SyntxHighl(QTextDocument *parent = 0);

	void setSnthHgl(XMLNode nd);

    protected:
	void highlightBlock(const QString &text);
	void rule(XMLNode *irl, const QString &text, int off = 0, char lev = 0);

	XMLNode rules;
};

//*************************************************
//* TextEdit: Text edit widget                    *
//*************************************************
class TextEdit : public QWidget
{
    Q_OBJECT

    public:
	//Methods
	TextEdit(QWidget *parent, const char * name = 0, bool prev_dis = false);

	QString text( );
	bool hasFocus( ) const;
	bool isChanged( );

	void setText( const QString& );
	void setSnthHgl( XMLNode nd );

	QTextEdit *edit( )	{ return ed_fld; }

    signals:
	//Signals
	void apply( );
	void cancel( );
	void textChanged( const QString& );

    protected:
	bool event( QEvent *e );

    private slots:
	//Private slots
	void changed( );
	void btApply( );
	void btCancel( );
	void curPosChange( );
	void ctrTreePopup( );
	void find( );

    private:
	//Private attributes
	bool		isInit;
	QAction		*actFind, *actFindNext;
	QTextEdit	*ed_fld;
	SyntxHighl	*snt_hgl;
	QDialogButtonBox *but_box;
	QPoint		holdPnt;
};

//************************************************
//* ImgView: Image view widget                   *
//************************************************
class ImgView : public QWidget
{
    Q_OBJECT
    public:
	//Methods
	ImgView( QWidget * parent = 0, Qt::WindowFlags f = 0, int ih_sz = 0, int iv_sz = 0 );
	~ImgView( );

	QImage &image( )	{ return m_img; }

	bool setImage( const string &imgdata );

    protected:
	//Methods
	void paintEvent( QPaintEvent * );

    private:
	//Attributes
	QImage m_img;
	int   h_sz, v_sz;
};

//************************************************
//* CfgTable: Table view widget                  *
//************************************************
class CfgTable : public QTableWidget
{
    Q_OBJECT
    public:
	//Methods
	CfgTable( QWidget *parent = 0 );

    public slots:
	//Public slots
	void resizeRowsToContentsLim( );

    protected:
	//Methods
	bool event( QEvent *e );

    private:
	//Private attributes
	QPoint	holdPnt;
};

//************************************************
//* InputDlg: Id and name input dialog           *
//************************************************
class InputDlg : public QDialog
{
    public:
	//Methods
	InputDlg( QWidget *parent, const QIcon &icon, const QString &mess,
	    const QString &ndlg = _("QTCfg dialog"), int with_id = 0, int with_nm = 200,
	    QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok|QDialogButtonBox::Cancel );

	QString id( );
	QString name( );
	QString mess( );

	void setId( const QString &val );
	void setName( const QString &val );
	void setMess( const QString &val );

	//Attributes
	QGridLayout	*ed_lay;

    protected:
	void showEvent( QShowEvent * event );

    protected:
	//Attributes
	QLabel		*inpLab, *mIdLab, *mNameLab;
	QLineEdit	*mId, *mName;
};

//*****************************************************
//* ReqIdNameDlg: Request node identifier and/or name *
//*****************************************************
class ReqIdNameDlg : public InputDlg
{
    Q_OBJECT

    public:
	//Methods
	ReqIdNameDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg = _("Node id and/or name select") );

	string target( );

	void setTargets( const vector<string> &tgs );

    protected:
	//Attributes
	QLabel		*itTpLab;
	QComboBox	*itTp;

    private slots:
	//Private slots
	void selectItTp( int it );
};

//************************************************
//* DlgUser: User select dialog                  *
//************************************************
class DlgUser : public QDialog
{
    Q_OBJECT

    public:
	//Data
	enum Results { SelCancel, SelOK, SelErr };

	//Methods
	DlgUser( QWidget * parent = 0 );

	QString user( );
	QString password( );

    protected:
        void showEvent( QShowEvent * event );

    private slots:
	//Private slots
	void finish( int result );

    private:
	//Attributes
	QComboBox	*users;
	QLineEdit	*passwd;
};

//************************************************
//* UserStBar: Status bar user widget            *
//************************************************
class UserStBar : public QLabel
{
    Q_OBJECT

    public:
	//Methods
	UserStBar( const QString &iuser, QWidget * parent = 0 );

	QString user( );
	bool userSel( );

	void setUser( const QString &val );

    signals:
	//Signals
	void userChanged( );

    protected:
	//Methods
	bool event( QEvent *event );

    private:
	//Attributes
	QString	user_txt;
};

//*************************************************
//* TableDelegate: Combobox table delegate        *
//*************************************************
class TableDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
	//> Public attributes
	TableDelegate( QObject *parent = 0 );

	//QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void setEditorData( QWidget *editor, const QModelIndex &index ) const;
	void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

	void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:
	//> Private attributes
	bool eventFilter( QObject *object, QEvent *event );
};

}

#endif //SELFWIDG_H

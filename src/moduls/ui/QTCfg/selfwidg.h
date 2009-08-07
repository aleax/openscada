
//OpenSCADA system module UI.QTCfg file: selfwidg.h
/***************************************************************************
 *   Copyright (C) 2004-2008 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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
	//- Data -
	enum LType { Text, Integer, Real, Time, Date, DateTime, Combo };

	//- Methods -
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
//* TextEdit: Text edit widget                    *
//*************************************************
class TextEdit : public QWidget
{
    Q_OBJECT

    public:
	//Methods
	TextEdit( QWidget *parent, const char * name = 0, bool prev_dis = false );

	QString text( );
	bool hasFocus( ) const;
	bool isChanged( );

	void setText( const QString & );

	QTextEdit *edit( )	{ return ed_fld; }

    signals:
	//Signals
	void apply( );
	void cancel( );
	void textChanged( const QString& );

    private slots:
	//Private slots
	void changed( );
	void btApply( );
	void btCancel( );

    private:
	//Private attributes
	bool	isInit;
	QTextEdit	*ed_fld;
	QDialogButtonBox *but_box;
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
//* InputDlg: Id and name input dialog           *
//************************************************
class InputDlg : public QDialog
{
    public:
	//Methods
	InputDlg( QWidget *parent, const QIcon &icon, const QString &mess,
		const QString &ndlg = "QTCfg dialog", int with_id = 0, int with_nm = 200 );

	QString id( );
	QString name( );
	QString mess( );

	void setId( const QString &val );
	void setName( const QString &val );
	void setMess( const QString &val );

	//Attributes
	QGridLayout	*ed_lay;

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
	ReqIdNameDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg = "Node id and/or name select" );

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
	QString     user_txt;
};

}

//*************************************************
//* TableDelegate: Combobox table delegate        *
//*************************************************
class TableDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
	//- Public attributes -
	TableDelegate( QObject *parent = 0 );

	void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void setEditorData( QWidget *editor, const QModelIndex &index ) const;
	void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

	void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:
	//- Private attributes -
	bool eventFilter( QObject *object, QEvent *event );
};

#endif //SELFWIDG_H


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

#include <xml.h>

using std::string;
using std::vector;

class QMainWindow;
class QComboBox;
class QLineEdit;
class QTextEdit;
class QDialogButtonBox;
class QFontComboBox;
class QSpinBox;
class QCheckBox;

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

	    QString id( );
	    QString name( );
	    QString mess( );

	    void setId( const QString &val );
	    void setName( const QString &val );
	    void setMess( const QString &val );

	private:
	    QLabel	*inp_lab;
	    QLineEdit	*m_id, *m_name;
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
	    DlgUser( const QString &iuser, const QString &ipass, const QString &iVCAstat, QWidget *parent = 0 );

	    QString user();
	    QString password();

	private slots:
	    void finish( int result );

	private:
	    QComboBox	*users;
	    QLineEdit	*passwd;
	    QString	VCAstat;
    };

    //*********************************************
    //* Font select dialog                        *
    //*********************************************
    class FontDlg : public QDialog
    {
	Q_OBJECT

	public:
	    FontDlg( QWidget *parent, const QString &ifnt );

	    QString font( );

	    void setFont( const QString &fnt );

	private slots:
	    void cfgChange();

	private:
	    QFontComboBox	*fntSel;
	    QSpinBox		*spBox;
	    QCheckBox		*chBold,
				*chItalic,
				*chStrike,
				*chUnder;
	    QLineEdit		*sampleText;
    };

    //*********************************************
    //* Status bar user widget                    *
    //*********************************************
    class UserStBar : public QLabel
    {
	Q_OBJECT

	public:
	    UserStBar( const QString &iuser, const QString &ipass, const QString &iVCAstat, QWidget *parent = 0 );

	    QString user( )		{ return user_txt; }
	    QString pass( )		{ return user_pass; }
	    QString VCAStation( )	{ return VCA_stat; }

	    void setUser( const QString &val );
	    void setPass( const QString &val )		{ user_pass = val; }
	    void setVCAStation( const QString &val )	{ VCA_stat = val.isEmpty() ? "." : val; }

	    bool userSel( );

	signals:
	    void userChanged( const QString &oldUser, const QString &oldPass );

	protected:
	    bool event( QEvent *event );

	private:
	    QString	user_txt, user_pass, VCA_stat;
    };

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
	    ~LineEdit( );

	    LType type( )	{ return m_tp; }
	    QString value( );
	    bool isEdited( );

	    void setType( LType tp );
	    void setValue( const QString& );
	    void setCfg( const QString& );

	    QWidget *workWdg( )	{ return ed_fld; }

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
	    LType	m_tp;
	    QString	m_val;
	    QWidget	*ed_fld;
	    QPushButton	*bt_fld;
    };

    //*********************************************
    //* Text edit widget                          *
    //*********************************************
    class TextEdit : public QWidget
    {
	Q_OBJECT

	public:
	    TextEdit( QWidget *parent, bool prev_dis = false );

	    QString text( );
	    void setText( const QString & );

	    QTextEdit *workWdg( )	{ return ed_fld; }

	signals:
	    void apply( );
	    void cancel( );
	    void textChanged( const QString& );

	private slots:
	    void changed( );
	    void applySlot( );
	    void cancelSlot( );

	private:
	    bool		isInit;
	    QString		m_text;
	    QTextEdit		*ed_fld;
	    QDialogButtonBox	*but_box;
    };

    //****************************************
    //* Shape widget view                    *
    //****************************************
    class WdgShape;

    class WdgView: public QWidget
    {
	Q_OBJECT

	public:
	    //- Public methods -
	    WdgView( const string &iwid, int ilevel, QMainWindow *mainWind, QWidget* parent = 0, Qt::WindowFlags f = 0 );
	    ~WdgView( );

	    string	id( )		{ return idWidget; }
	    int		wLevel( )	{ return w_level; }
	    string	root( );
	    QMainWindow	*mainWin( )	{ return main_win; }
	    QPointF	posF( )		{ return w_pos; }
	    QSizeF	sizeF( )	{ return w_size; }
	    QRectF	geometryF( )	{ return QRectF(w_pos,w_size); }
	    virtual float xScale( bool full = false );
	    virtual float yScale( bool full = false );
	    int		z( )		{ return z_coord;  }
	    virtual string user( )	{ return ""; }
	    bool	allAttrLoad( )	{ return all_attr_load; }

	    void	moveF( const QPointF &pos );
	    void	resizeF( const QSizeF &size );
	    void	setAllAttrLoad( bool vl )	{ all_attr_load = vl; }

	    virtual bool attrSet( const string &attr, const string &val, int uiPrmPos = 0 );
	    virtual string resGet( const string &res );

	    virtual int cntrIfCmd( XMLNode &node, bool glob = false )	{ };

            //Attributes
	    void	*shpData;	//Shape data link

	public slots:
	    virtual WdgView *newWdgItem( const string &iwid );
	    virtual void load( const string& item, bool load = true, bool init = true );

	protected:
	    //- Protected methods -
	    virtual void childsUpdate( bool newLoad = true );
	    void shapeUpdate( );
	    virtual void orderUpdate( );

	    bool event( QEvent * event );
	    bool eventFilter( QObject *object, QEvent *event );

	    //- Protected attributes -
	    bool		all_attr_load;	//All attributes load
	    int			w_level;	//Widget level
	    QPointF		w_pos;		//Widget position into real;
	    QSizeF		w_size;		//Widget size into real;
	    float		x_scale, 	//Widget x scale
				y_scale;	//	 y scale
	    int			z_coord;	//Z coordinate
	    string		idWidget,	//Full widget identifier
				m_root;		//Root widget identifier
	    WdgShape		*shape;		//Link to root widget shape
	    QMainWindow		*main_win;	//Main window
    };
}

#endif //VIS_WIDGS_H

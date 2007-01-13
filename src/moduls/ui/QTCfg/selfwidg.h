
//OpenSCADA system module UI.QTCfg file: selfwidg.h
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#ifndef SELFWIDG_H
#define SELFWIDG_H

#include <string>
#include <vector>

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

namespace QTCFG
{
    //Line edit widget
    class LineEdit : public QWidget
    {
	Q_OBJECT
	
	public:
	    LineEdit( QWidget *parent, bool prev_dis = false );

	    void setText(const QString &);
	    QString text() const;

	    QLineEdit	*edit()	{ return ed_fld; }

	signals:
	    void apply( );
	    void cancel( );
	    void textChanged(const QString&);
	    
	protected:
	    bool event( QEvent * e );

	private slots:
	    void changed( const QString& );
	    void applySlot( );
	    
	private:
	    QLineEdit	*ed_fld;
	    QPushButton	*bt_fld;
    }; 
    
    //Date and time edit widget
    class DateTimeEdit : public QWidget
    {
	Q_OBJECT
	
	public:
	    DateTimeEdit( QWidget *parent, bool prev_dis = false );

	    void setDateTime(const QDateTime & dt);
	    QDateTime dateTime() const;

	    QDateTimeEdit *dtEdit() { return ed_fld; }

	signals:
            void apply( );
	    void cancel( );
            void valueChanged(const QDateTime&);	    	
	    
	protected:
	    bool event( QEvent * e );	    
		
	private slots:
	    void changed( const QDateTime & );
	    void applySlot( );
	    
	private:
	    QDateTimeEdit *ed_fld;
	    QPushButton	*bt_fld;
    };
    
    //Text edit widget
    class TextEdit : public QWidget
    {
	Q_OBJECT
	
	public:
	    TextEdit( QWidget *parent, const char * name = 0, bool prev_dis = false );

	    QString text();	  
	    void setText(const QString &);  

	    QTextEdit	*edit()	{ return ed_fld; }

	signals:
	    void apply( );
	    void cancel( );
 	    void textChanged(const QString&);
	    
	private slots:
	    void changed();
	    
	private:
	    bool	isInit;
	    QTextEdit	*ed_fld;
	    QPushButton	*bt_apply, *bt_cancel;
    };     
    
    //Image view widget
    class ImgView : public QWidget
    {
    	Q_OBJECT
    	public:
	    ImgView( QWidget * parent = 0, Qt::WindowFlags f = 0, int ih_sz = 0, int iv_sz = 0 );
	    ~ImgView( );
	    
	    QImage &image()	{ return m_img; }
	    bool setImage( const string &imgdata );

	protected:
    	    void paintEvent( QPaintEvent * );
	
	private:
	    QImage m_img;
	    int	   h_sz, v_sz;
    };

    //Id and name input dialog
    class InputDlg : public QDialog
    {
	public:
	    InputDlg( bool with_id = false );
	    
	    QString id();
	    QString name();

	    void id(const QString &val);
	    void name(const QString &val);	    

	private:
	    QLineEdit 	*m_id, *m_name;
    };    

    //User select dialog
    class DlgUser : public QDialog
    {
	public:
	    DlgUser( );

	    QString user();
	    QString password();

	    void user( vector<string> &lst );

	private:
	    QComboBox 	*users;
	    QLineEdit 	*passwd;
    };
}

    //Combobox table delegate
    class ComboBoxDelegate : public QItemDelegate
    {
        Q_OBJECT
	
	public:
	    ComboBoxDelegate(QObject *parent = 0);
	    
	    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    
	    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
					  
    	    void setEditorData(QWidget *editor, const QModelIndex &index) const;
            void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
									
	    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	    
	    bool eventFilter(QObject *object, QEvent *event);
    };

#endif //SELFWIDG_H


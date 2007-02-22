
//OpenSCADA system module UI.Vision file: vis_widgs.h
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
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

#ifndef VIS_WIDGS_H
#define VIS_WIDGS_H

#include <string>
#include <vector>

#include <QLabel>
#include <QDialog>

using std::string;
using std::vector;

class QComboBox;
class QLineEdit;    

namespace VISION
{
    //*********************************************
    //* Id and name input dialog                  *
    //*********************************************
    class InputDlg : public QDialog
    {
	public:
	    InputDlg( const QIcon & icon, const QString &mess, 
		    const QString &ndlg = "Vision dialog", bool with_id = false, bool with_nm = true );
	    
	    QString id();
	    QString name();

	    void id( const QString &val );
	    void name( const QString &val );

	private:
	    QLineEdit 	*m_id, *m_name;
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
	    DlgUser( );

	    QString user();
	    QString password();
	    
	private slots:
	    void finish( int result );

	private:
	    QComboBox 	*users;
	    QLineEdit 	*passwd;
    };
    
    //*********************************************
    //* Status bar user widget                    *
    //*********************************************
    class UserStBar : public QLabel
    {
	Q_OBJECT
    
	public:
	    UserStBar( const QString &iuser, QWidget * parent = 0 );

	    QString user();
	    void user( const QString &val );
	    
	    bool userSel( );
	    
	signals:
            void userChanged();
	
	protected:
	    bool event( QEvent *event );
	    
	private:
	    QString	user_txt;
    };    
}

#endif //VIS_WIDGS_H


/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
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

#ifndef DLGUSER_H
#define DLGUSER_H

#include <qdialog.h>

class QComboBox;
class QLineEdit;

namespace QTCFG
{
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

#endif //DLGUSER_H


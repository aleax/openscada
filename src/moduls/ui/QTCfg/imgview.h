
//OpenSCADA system module UI.QTCfg file: imgview.h
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

#ifndef IMGVIEW_H
#define IMGVIEW_H

#include <string>

#include <qwidget.h>
#include <qimage.h>

using std::string;

namespace QTCFG
{    
    class ImgView : public QWidget
    {
    	Q_OBJECT
    	public:
	    ImgView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
	    ~ImgView( );

	    bool setImage( const string &imgdata );
	    void draw( );

	protected:
    	    void paintEvent( QPaintEvent * );
	
	private:
	    QImage m_img;
    };

}

#endif //IMGVIEW_H


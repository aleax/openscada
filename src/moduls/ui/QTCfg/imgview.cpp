
//OpenSCADA system module UI.QTCfg file: imgview.cpp
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

#include <qpainter.h>

#include "imgview.h"

using namespace QTCFG;

ImgView::ImgView( QWidget * parent, const char * name, WFlags f ) :
    QWidget(parent,name,f)
{
    
}

ImgView::~ImgView( )
{

}

bool ImgView::setImage( const string &imgdata )
{
    bool rez = m_img.loadFromData((const uchar*)imgdata.c_str(),imgdata.size());
    if(rez) 
    {
	setMinimumSize(m_img.width(),m_img.height());
	//resize(QSize(m_img.width(),m_img.height()));
	draw();
    }
    
    return rez;
}

void ImgView::paintEvent( QPaintEvent * )
{
    draw( );
}

void ImgView::draw( )
{
    QPainter pnt( this );
    pnt.setWindow( 0, 0, m_img.width(), m_img.height() );
    pnt.drawRect( 0, 0, m_img.width(), m_img.height() );
    pnt.drawImage(QPoint(0,0),m_img);
}


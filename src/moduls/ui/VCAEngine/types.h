
//OpenSCADA system module UI.VCAEngine file: types.h
/***************************************************************************
 *   Copyright (C) 2013 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
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

#ifndef TYPES_H
#define TYPES_H

namespace VCA
{
    //Attributes preset indexes
    enum AttrID
    {
	//> Generic
	A_COM_LOAD	= -1,
	A_ROOT		= 1,
	A_EN		= 5,
	A_ACTIVE	= 6,
	A_GEOM_X	= 7,
	A_GEOM_Y	= 8,
	A_GEOM_W	= 9,
	A_GEOM_H	= 10,
	A_GEOM_Z	= 11,
	A_GEOM_MARGIN	= 12,
	A_GEOM_X_SC	= 13,
	A_GEOM_Y_SC	= 14,
	A_TIP_TOOL	= 15,
	A_TIP_STATUS	= 16,
	A_CTX_MENU	= 17,

	//> Primitive "ElFigure" specific
	//...
	//> Primitive "FormEl" specific
	A_FormEl_TYPE	= 20,
	A_FormEl_VALUE	= 21,
    };

    //FormEl preset indexes
    enum FormEl_Views	{ F_LINE_ED = 0, F_TEXT_ED, F_CHECK_BOX, F_BUTTON, F_COMBO, F_LIST, F_SLIDER, F_SCROLL_BAR, F_TREE };
    enum FormEl_LineViews { FL_TEXT = 0, FL_COMBO, FL_INTEGER, FL_REAL, FL_TIME, FL_DATE, FL_DATE_TM };

}

#endif //TYPES_H


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
	A_COM_FOCUS	= -2,
	A_COM_LOAD	= -1,
	A_ROOT		= 1,
	A_PG_OPEN_SRC	= 3,
	A_PG_GRP	= 4,
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
	A_ElFigLineW	= 20,
	A_ElFigLineClr	= 21,
	A_ElFigLineStl	= 22,
	A_ElFigBordW	= 23,
	A_ElFigBordClr	= 24,
	A_ElFigFillClr	= 25,
	A_ElFigFillImg	= 26,
	A_ElFigOrient	= 28,
	A_ElFigElLst	= 27,
	A_ElFigIts	= 30,
	A_ElFigItsSz	= 6,
	A_ElFigItPntX	= 0,
	A_ElFigItPntY	= 1,
	A_ElFigItW	= 2,
	A_ElFigItClr	= 3,
	A_ElFigItImg	= 4,
	A_ElFigItStl	= 5,

	//> Primitive "FormEl" specific
	A_FormElType	= 20,
	A_FormElValue	= 21,
	A_FormElMixP1	= 22,	//LINE_ED:view; TEXT_ED:wordWrap; BUTTON:img; {COMBO,LIST,TREE}:items; {SLIDER,SCROLL_BAR}:cfg
	A_FormElMixP2	= 23,	//LINE_ED:cfg; BUTTON:color
	A_FormElMixP3	= 24,	//{LINE_ED,TEXT_ED}:confirm; BUTTON:mode
	A_FormElFont	= 25,
	A_FormElName	= 26,
	A_FormElMixP4	= 27,	//BUTTON:colorText

	//> Primitive "Text" specific
	A_TextBackClr	= 20,
	A_TextBackImg	= 21,
	A_TextBordWidth	= 22,
	A_TextBordColor	= 23,
	A_TextBordStyle	= 24,
	A_TextFont	= 25,
	A_TextColor	= 26,
	A_TextOrient	= 27,
	A_TextWordWrap	= 28,
	A_TextAlignment	= 29,
	A_TextText	= 30,
	A_TextNumbArg	= 40,

	//> Primitive "Media" specific
	A_MediaBackClr	= 20,
	A_MediaBackImg	= 21,
	A_MediaBordWdth	= 22,
	A_MediaBordClr	= 23,
	A_MediaBordStl	= 24,
	A_MediaSrc	= 25,
	A_MediaFit	= 26,
	A_MediaType	= 27,
	A_MediaAreas	= 28,
	A_MediaSpeedPlay= 29,
	A_MediaRoll	= 30,
	A_MediaPause	= 31,
	A_MediaSize	= 32,
	A_MediaSeek	= 33,
	A_MediaVolume	= 34,
	A_MediaArs	= 40,
	A_MediaArsSz	= 3,
	A_MediaArShape	= 0,
	A_MediaArCoord	= 1,
	A_MediaArTitle	= 2,

	//> Primitive "Box" specific
	A_BoxBackClr	= 20,
	A_BoxBackImg	= 21,
	A_BoxBordWdth	= 22,
	A_BoxBordClr	= 23,
	A_BoxBordStl	= 24,
    };

    //ElFigure preset indexes
    enum ElFig_LineSt	{ EF_SOLID = 0, EF_DASH, EF_DOT };

    //FormEl preset indexes
    enum FormEl_Views	{ F_LINE_ED = 0, F_TEXT_ED, F_CHECK_BOX, F_BUTTON, F_COMBO, F_LIST, F_SLIDER, F_SCROLL_BAR, F_TREE, F_TABLE };
    enum FormEl_LineViews { FL_TEXT = 0, FL_COMBO, FL_INTEGER, FL_REAL, FL_TIME, FL_DATE, FL_DATE_TM };
    enum FormEl_BtModes { FBT_STD = 0, FBT_CHECK, FBT_MENU, FBT_LOAD, FBT_SAVE };

    //Media preset indexes
    enum Media_Types	{ FM_IMG = 0, FM_ANIM, FM_FULL_VIDEO };
    enum Media_ArShapes	{ FM_RECT = 0, FM_POLY, FM_CIRCLE };

    //Box preset indexes
    enum Box_BordStls	{ FBRD_NONE = 0, FBRD_DOT, FBRD_DASH, FBRD_SOL, FBRD_DBL, FBRD_GROOVE, FBRD_RIDGE, FBRD_INSET, FBRD_OUTSET };
}

#endif //TYPES_H

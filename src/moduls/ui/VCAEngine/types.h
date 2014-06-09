
//OpenSCADA system module UI.VCAEngine file: types.h
/***************************************************************************
 *   Copyright (C) 2014 by Roman Savochenko, <rom_as@oscada.org>           *
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
	// Generic
	A_PG_NAME	= -4,
	A_PERM		= -3,
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

	//  BackGround
	A_BackColor	= 20,
	A_BackImg	= 21,
	//  Border
	A_BordWidth	= 22,
	A_BordColor	= 23,
	A_BordStyle	= 24,

	// Primitive "ElFigure" specific
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

	// Primitive "FormEl" specific
	A_FormElType	= 20,
	A_FormElValue	= 21,
	A_FormElMixP1	= 22,	//LINE_ED:view; TEXT_ED:wordWrap; BUTTON:img; {COMBO,LIST,TREE}:items; {SLIDER,SCROLL_BAR}:cfg
	A_FormElMixP2	= 23,	//LINE_ED:cfg; BUTTON:color
	A_FormElMixP3	= 24,	//{LINE_ED,TEXT_ED}:confirm; BUTTON:mode
	A_FormElFont	= 25,
	A_FormElName	= 26,
	A_FormElMixP4	= 27,	//BUTTON:colorText

	// Primitive "Text" specific
	A_TextFont	= 25,
	A_TextColor	= 26,
	A_TextOrient	= 27,
	A_TextWordWrap	= 28,
	A_TextAlignment	= 29,
	A_TextText	= 30,
	A_TextNumbArg	= 40,
	A_TextArs	= 50,
	A_TextArsSz	= 10,
	A_TextArsVal	= 0,
	A_TextArsTp	= 1,
	A_TextArsCfg	= 2,

	// Primitive "Media" specific
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

	// Primitive "Diagram" specific
	A_DiagramTrcPer		= 25,
	A_DiagramType		= 26,
	A_DiagramTSek		= 27,
	A_DiagramTUSek		= 28,
	A_DiagramTSize		= 29,
	A_DiagramCurSek		= 30,
	A_DiagramCurUSek	= 31,
	A_DiagramCurColor	= 32,
	A_DiagramSclColor	= 33,
	A_DiagramSclHor		= 34,
	A_DiagramSclVer		= 35,
	A_DiagramSclMarkColor	= 36,
	A_DiagramSclMarkFont	= 37,
	A_DiagramValArch	= 38,
	A_DiagramParNum		= 39,
	A_DiagramSclVerScl	= 40,
	A_DiagramSclVerSclOff	= 41,
	A_DiagramValsForPix	= 42,
	A_DiagramSclHorPer	= 43,
	A_DiagramTrs	= 50,
	A_DiagramTrsSz	= 10,
	A_DiagramTrAddr = 0,
	A_DiagramTrBordL= 1,
	A_DiagramTrBordU= 2,
	A_DiagramTrClr	= 3,
	A_DiagramTrVal	= 4,
	A_DiagramTrScl	= 5,
	A_DiagramTrWdth = 6,
	A_DiagramTrProp	= 7,

	// Primitive "Protocol" specific
	A_ProtFont	= 22,
	A_ProtHeadVis	= 23,
	A_ProtTime	= 24,
	A_ProtTSize	= 25,
	A_ProtTrcPer	= 26,
	A_ProtArch	= 27,
	A_ProtTmpl	= 28,
	A_ProtLev	= 29,
	A_ProtViewOrd	= 30,
	A_ProtCol	= 31,
	A_ProtItProp	= 32,
	A_ProtProps	= 40,
	A_ProtPropsSz	= 5,
	A_ProtPropLev	= 0,
	A_ProtPropTmpl	= 1,
	A_ProtPropFnt	= 2,
	A_ProtPropClr	= 3,

	// Primitive "Document" specific
	A_DocStyle	= 20,
	A_DocTmpl	= 21,
	A_DocDoc	= 22,
	A_DocTime	= 23,
	A_DocBTime	= 24,
	A_DocN		= 25,
	A_DocFont	= 26,
	A_DocProcess	= 27,
	A_DocCalcPrmRez		= 0,
	A_DocCalcPrmTime	= 1,
	A_DocCalcPrmBTime	= 2,
	A_DocCalcPrmLTime	= 3,
	A_DocCalcPrmRTime	= 4,
	A_DocCalcPrmRTimeU	= 5,
	A_DocCalcPrmRPer	= 6,
	A_DocCalcPrmMTime	= 7,
	A_DocCalcPrmMTimeU	= 8,
	A_DocCalcPrmMLev	= 9,
	A_DocCalcPrmMCat	= 10,
	A_DocCalcPrmMVal	= 11,

	// Primitive "Box" specific
    };

    //Generic preset indexes
    enum BordStls	{ FBRD_NONE = 0, FBRD_DOT, FBRD_DASH, FBRD_SOL, FBRD_DBL, FBRD_GROOVE, FBRD_RIDGE, FBRD_INSET, FBRD_OUTSET };

    //ElFigure preset indexes
    enum ElFig_LineSt	{ EF_SOLID = 0, EF_DASH, EF_DOT };

    //FormEl preset indexes
    enum FormEl_Views	{ F_LINE_ED = 0, F_TEXT_ED, F_CHECK_BOX, F_BUTTON, F_COMBO, F_LIST, F_SLIDER, F_SCROLL_BAR, F_TREE, F_TABLE };
    enum FormEl_LineViews { FL_TEXT = 0, FL_COMBO, FL_INTEGER, FL_REAL, FL_TIME, FL_DATE, FL_DATE_TM };
    enum FormEl_BtModes { FBT_STD = 0, FBT_CHECK, FBT_MENU, FBT_LOAD, FBT_SAVE };

    //Text preset indexes
    enum Text_ArsTp	{ FT_INT = 0, FT_REAL, FT_STR };
    enum Text_Align	{ FT_TOP_LEFT = 0, FT_TOP_RIGHT, FT_TOP_CENTER, FT_TOP_JUST,
			  FT_BT_LEFT, FT_BT_RIGHT, FT_BT_CENTER, FT_BT_JUST,
			  FT_CNTR_LEFT, FT_CNTR_RIGHT, FT_CNTR, FT_CNTR_JUST };

    //Media preset indexes
    enum Media_Types	{ FM_IMG = 0, FM_ANIM, FM_FULL_VIDEO };
    enum Media_ArShapes	{ FM_RECT = 0, FM_POLY, FM_CIRCLE };

    //Diagram preset indexes
    enum Diagram_Types	{ FD_TRND = 0, FD_SPECTR };
    enum Diagram_Scale	{ FD_NO = 0, FD_GRD, FD_MARKS, FD_GRD_MARKS, FD_LOG, FD_GRD_LOG, FD_MARKS_LOG, FD_GRD_MARKS_LOG };

    enum Prot_ViewOrd	{ FP_ON_TM = 0, FP_ON_LEV, FP_ON_CAT, FP_ON_MESS, FP_ON_TM_REV, FP_ON_LEV_REV, FP_ON_CAT_REV, FP_ON_MESS_REV };
}

#endif //TYPES_H

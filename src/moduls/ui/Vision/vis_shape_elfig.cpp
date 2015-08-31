
//OpenSCADA system module UI.VISION file: vis_shape_elfig.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Yashina Kseniya <ksu@oscada.org>
 *		   2007-2012 by Lysenko Maxim <mlisenko@oscada.org>
 *		   2012-2015 by Roman Savochenko <rom_as@oscada.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include <QToolBar>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QStatusBar>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QColorDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QFont>
#include <QCheckBox>
#include <QPushButton>
//#include <QPolygonF>

#include <tsys.h>

#include "tvision.h"
#include "vis_devel.h"
#include "vis_widgs.h"
#include "vis_devel_widgs.h"
#include "vis_run_widgs.h"
#include "vis_shape_elfig.h"


using namespace VISION;

ShapeElFigure::ShapeElFigure( ) :
    WdgShape("ElFigure"), itemInMotion(NULL), count_Shapes(0), fill_index(-1), index_del(-1), rect_num(-1), dyn_num(0), status_hold(true),
    flag_up(false), flag_down(false), flag_left(false), flag_right(false), flag_ctrl(false), flag_m(false), flag_hold_arc(false), flag_A(false),
    flag_copy(false), flag_check_pnt_inund(false), flag_check_point(false), flag_rect(false), flag_arc_rect_3_4(false), flag_first_move(false),
    flag_move(false), flag_hold_move(false), flag_inund_break(false), fTransl(true), flag_angle_temp(false),flag_geom (false),
    flag_rect_items (false), flag_def_stat(false), flag_dyn_save(true), fl_status_move(false), fl_orto_move(false), fl_orto_disable(false),
    count_holds(0), count_rects(0), rect_num_arc(-1), current_ss(-1), current_se(-1), current_ee(-1), current_es(-1), geomH(0), geomW(0), rect_dyn(-1)
{
    newPath.addEllipse(QRect(0,0,0,0));
}

void ShapeElFigure::init( WdgView *w )
{
    w->shpData = new ElFigDt(w);
}

void ShapeElFigure::destroy( WdgView *w )
{
    rectItems.clear();

    delete (ElFigDt*)w->shpData;
}

bool ShapeElFigure::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    ElFigDt	*elFD = (ElFigDt*)w->shpData;
    RunWdgView	*runW = qobject_cast<RunWdgView*>(w);
    bool	rel_list= false;				//change signal
    bool	up	= false;
    bool	paint	= false;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems, shapeItems_temp = shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems, inundItems_temp = inundItems;
    PntMap	&pnts	= elFD->shapePnts, &pnts_temp = elFD->shapePnts_temp;
    WidthMap	&widths	= elFD->shapeWidths, &widths_temp = elFD->shapeWidths_temp;
    ColorMap	&colors	= elFD->shapeColors, &colors_temp = elFD->shapeColors_temp;
    ImageMap	&images	= elFD->shapeImages, &images_temp = elFD->shapeImages_temp;
    StyleMap	&styles	= elFD->shapeStyles, &styles_temp = elFD->shapeStyles_temp;

    QPointF	CtrlMotionPos_4;
    string	backimg;
    QImage	img;
    rect_num = -1;
    switch(uiPrmPos) {
	case A_COM_LOAD: rel_list = true;	break;
	case A_EN:
	    if(!runW)	break;
	    elFD->en = (bool)s2i(val);
	    w->setVisible(elFD->en && runW->permView());
	    break;
	case A_ACTIVE:
	    if(!runW)	break;
	    elFD->active = (bool)s2i(val);
	    w->setFocusPolicy((elFD->active && runW->permCntr()) ? Qt::TabFocus : Qt::NoFocus);
	    break;
	case A_GEOM_W:
	    if(geomW == s2i(val))	break;
	    geomW = s2i(val);
	    flag_geom = rel_list = true;
	    break;
	case A_GEOM_H:
	    if(geomH == s2i(val))	break;
	    geomH = s2i(val);
	    flag_geom = rel_list = true;
	    break;
	case A_GEOM_MARGIN: elFD->geomMargin = s2i(val); up = true;	break;
	case A_GEOM_X_SC: rel_list = true;	break;
	case A_GEOM_Y_SC: rel_list = true;	break;
	case A_ElFigLineW: widths[SpI_DefLine] = s2r(val); rel_list = true;	break;
	case A_ElFigLineClr: colors[SpI_DefLine] = getColor(val); rel_list = true;	break;
	case A_ElFigLineStl:
	    switch(s2i(val)) {
		case EF_SOLID:	styles[SpI_DefLine] = Qt::SolidLine;	break;
		case EF_DASH:	styles[SpI_DefLine] = Qt::DashLine;	break;
		case EF_DOT:	styles[SpI_DefLine] = Qt::DotLine;	break;
	    }
	    rel_list = true;
	    break;
	case A_ElFigBordW: widths[SpI_DefBord] = s2r(val); rel_list = true;	break;
	case A_ElFigBordClr: colors[SpI_DefBord] = getColor(val); rel_list = true;	break;
	case A_ElFigFillClr: colors[SpI_DefFill] = getColor(val); rel_list = true;	break;
	case A_ElFigFillImg:
	    backimg = w->resGet(val);
	    images[SpI_DefFillImg] = (!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size())) ? val : "";
	    rel_list = true;
	    break;
	case A_ElFigOrient: elFD->orient = s2r(val); rel_list = true;	break;
	case A_ElFigMirror: elFD->mirror = s2r(val); rel_list = true;	break;
	case A_ElFigElLst: elFD->elLst = val.c_str(); rel_list = true;	break;
	default:
	    if(uiPrmPos >= A_ElFigIts) {
		int pnt  = (uiPrmPos-A_ElFigIts)/A_ElFigItsSz;
		int patr = (uiPrmPos-A_ElFigIts)%A_ElFigItsSz;
		QPointF pnt_ = pnts[pnt];
		switch(patr) {
		    case A_ElFigItPntX:	pnt_.setX(vmax(XY_MIN,vmin(XY_MAX,s2r(val)))); pnts[pnt] = pnt_;	break;
		    case A_ElFigItPntY:	pnt_.setY(vmax(XY_MIN,vmin(XY_MAX,s2r(val)))); pnts[pnt] = pnt_;	break;
		    case A_ElFigItW:	widths[pnt] = s2r(val);		break;
		    case A_ElFigItClr:	colors[pnt] = getColor(val);	break;
		    case A_ElFigItImg:
			backimg = w->resGet(val);
			if(!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size())) images[pnt] = val;
			else images[pnt] = "";
			break;
		    case A_ElFigItStl:
			switch(s2i(val)) {
			    case EF_SOLID: styles[pnt] = Qt::SolidLine;	break;
			    case EF_DASH:  styles[pnt] = Qt::DashLine;	break;
			    case EF_DOT:   styles[pnt] = Qt::DotLine;	break;
			}
			break;
		}
		rel_list = true;
	    }
    }
    if(rel_list && !w->allAttrLoad()) {
	//Deleting the pairs in the map with the key <= SpI_StatIts
	for(PntMap::iterator pi = pnts.begin(); pi != pnts.end(); )
	    if(pi->first <= SpI_StatIts) pnts.erase(pi++); else ++pi;
	for(ColorMap::iterator pi = colors.begin(); pi != colors.end(); )
	    if(pi->first <= SpI_StatIts) colors.erase(pi++); else ++pi;
	for(ImageMap::iterator pi = images.begin(); pi != images.end(); )
	    if(pi->first <= SpI_StatIts) images.erase(pi++); else ++pi;
	for(StyleMap::iterator pi = styles.begin(); pi != styles.end(); )
	    if(pi->first <= SpI_StatIts) styles.erase(pi++); else ++pi;
	int map_index = SpI_StatIts, w_index = SpI_StatIts, c_index = SpI_StatIts, i_index = SpI_StatIts, s_index = SpI_StatIts;
	flag_ctrl = flag_A = flag_copy = false;
	index_array.clear();
	count_Shapes = 0;
	itemInMotion = 0;
	shapeItems.clear();
	inundItems.clear();

	//Parse last attributes list and make point list
	string sel;
	QVector<int> p;
	int width, bord_width, color, img, bord_color, style;
	map<int, bool> pntsServ;
	for(int off = 0, el_off = 0; (sel=TSYS::strSepParse(elFD->elLst,0,'\n',&off)).size(); el_off = 0) {
	    string el = TSYS::strSepParse(sel, 0, ':', &el_off), el_s;
	    ShapeType elTp;
	    int nPnts = 0, servPnts;
	    if(el == "line")		{ elTp = ShT_Line; nPnts = 2; servPnts = 2; }
	    else if(el == "arc")	{ elTp = ShT_Arc; nPnts = 5; servPnts = 2; }
	    else if(el == "bezier")	{ elTp = ShT_Bezier; nPnts = 4; servPnts = 2; }
	    else if(el == "fill")	{ elTp = ShT_Fill; nPnts = -1; servPnts = -1; }
	    else continue;

	    // Reading and setting attributes for the current line
	    //  Points
	    float x_s, y_s;
	    int w_s;
	    bool pnts_ok = true;
	    p.clear();
	    for(int i_p = 0, off_last = 0; pnts_ok && (nPnts < 0 || i_p < nPnts); i_p++) {
		el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		if(sscanf(el_s.c_str(),"(%f|%f)",&x_s,&y_s) == 2) {
		    bool fl = false;
		    //  Detecting whether there is a point with same coordinates in the map.
		    for(PntMap::reverse_iterator pi = pnts.rbegin(); (servPnts < 0 || i_p < servPnts) && !fl && pi != pnts.rend(); pi++)
			if(pi->first <= SpI_StatIts && !pntsServ[pi->first] &&
			    fabs(rRnd(x_s,POS_PREC_DIG)-rRnd(pi->second.x(),POS_PREC_DIG)) < 0.01 &&
			    fabs(rRnd(y_s,POS_PREC_DIG)-rRnd(pi->second.y(),POS_PREC_DIG)) < 0.01)
			{ p.push_back(pi->first); fl = true; }
		    if(!fl) {
			p.push_back(map_index--);
			pnts[p[i_p]] = QPointF(x_s, y_s);
			if(i_p >= servPnts) pntsServ[p[i_p]] = true;
		    }
		}
		else if(sscanf(el_s.c_str(),"%d",&w_s) == 1) p.push_back(w_s);
		else { pnts_ok = false; el_off = off_last; }
		off_last = el_off;
	    }
	    if(!pnts_ok && nPnts > 0) continue;

	    //  Other properties
	    switch(elTp) {
		case ShT_Line: case ShT_Arc: case ShT_Bezier:
		    //   Line width
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"w%d",&w_s) == 1) width = w_s;
		    else if(sscanf(el_s.c_str(),"%d",&w_s) == 1) widths[(width=w_index--)] = (fabs(w_s) < 1) ? 0 : vmin(1000, vmax(1,w_s));
		    else width = SpI_DefLine;

		    //   Line color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) color = w_s;
		    else if(el_s.size()) colors[(color=c_index--)] = getColor(el_s);
		    else color = SpI_DefLine;

		    //   Border width
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"w%d",&w_s) == 1) bord_width = w_s;
		    else if(sscanf(el_s.c_str(),"%d",&w_s) == 1) widths[(bord_width=w_index--)] = (fabs(w_s) < 1) ? 0 : vmin(1000,vmax(1,w_s));
		    else bord_width = SpI_DefBord;

		    //   Border color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) bord_color = w_s;
		    else if(el_s.size()) colors[(bord_color=c_index--)] = getColor(el_s);
		    else bord_color = SpI_DefBord;

		    //   Line style
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"s%d",&w_s) == 1) style = w_s;
		    else if(sscanf(el_s.c_str(),"%d",&w_s) == 1 && (w_s == EF_SOLID || w_s == EF_DASH || w_s == EF_DOT))
			styles[(style=s_index--)] = (Qt::PenStyle)(w_s+1);
		    else style = SpI_DefLine;

		    p.resize(5);

		    //   Prebuild the current arc.
		    if(elTp == ShT_Arc)
			CtrlMotionPos_4 = getArcStartEnd(pnts[p[0]], pnts[p[1]], pnts[p[2]], pnts[p[3]], pnts[p[4]]);

		    //   Building the path of the line and adding it to container
		    shapeItems.push_back(ShapeItem(newPath,newPath,p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,
					    color,bord_color,style,width,bord_width,elTp,angle_temp));
		    break;
		case ShT_Fill:
		    //   Fill color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) color = w_s;
		    else if(el_s.size()) colors[(color=c_index--)] = getColor(el_s);
		    else color = SpI_DefFill;

		    //   Fill image
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"i%d",&w_s) == 1) img = w_s;
		    else if(!w->resGet(TSYS::strDecode(el_s)).empty()) images[(img=i_index--)] = TSYS::strDecode(el_s);
		    else img = SpI_DefFillImg;

		    //   Make elements
		    if(p.size() > 1) inundItems.push_back(inundationItem(newPath,p,color,img));
		    break;
	    }
	}
	for(int i = 0; i < shapeItems_temp.size(); i++) {
	    elFD->dropPoint(shapeItems_temp[i].n1, shapeItems.size()+1);
	    elFD->dropPoint(shapeItems_temp[i].n2, shapeItems.size()+1);
	    elFD->dropPoint(shapeItems_temp[i].n3, shapeItems.size()+1);
	    elFD->dropPoint(shapeItems_temp[i].n4, shapeItems.size()+1);
	    elFD->dropPoint(shapeItems_temp[i].n5, shapeItems.size()+1);
	}
	up = true;
    }
    if(up && !w->allAttrLoad()) {
	//Repainting all shapes by calling moveItemTo to each shape
	QVector<int> inundation_fig_num;
	bool flag_push_back;
	for(WidthMap::iterator pi = widths.begin(); pi != widths.end(); ) {
	    bool unDel = false;
	    for(int i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->first == shapeItems[i].width || pi->first == shapeItems[i].borderWidth);
	    if(pi->first > 0 && !unDel) widths.erase(pi++); else ++pi;
	}
	for(ColorMap::iterator pi = colors.begin(); pi != colors.end(); ) {
	    bool unDel = false;
	    for(int i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->first == shapeItems[i].lineColor || pi->first == shapeItems[i].borderColor);
	    for(int i = 0; !unDel && i < inundItems.size(); i++)
		unDel = pi->first > 0 && pi->first == inundItems[i].brush;
	    if(pi->first > 0 && !unDel) colors.erase(pi++); else ++pi;
	}
	for(ImageMap::iterator pi = images.begin(); pi != images.end(); ) {
	    bool unDel = false;
	    for(int i = 0; !unDel && i < inundItems.size(); i++)
		unDel = pi->first > 0 && (pi->first == inundItems[i].brushImg);
	    if(pi->first > 0 && !unDel) images.erase(pi++); else ++pi;
	}
	for(StyleMap::iterator pi = styles.begin(); pi != styles.end(); ) {
	    bool unDel = false;
	    for(int i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->first == shapeItems[i].style);
	    if(pi->first > 0 && !unDel) styles.erase(pi++); else ++pi;
	}

	QVector<int> items_array, fill_number;
	bool fill_build = false, flag_all = false;
	QVector<inundationItem> in_build;

	for(int i = 0; i < shapeItems.size(); i++) items_array.push_back(i);
	initShapeItems(QPointF(), items_array, w);
	//Detecting if there is a necessity to rebuild inundationItem
	if(shapeItems.size() != shapeItems_temp.size() || (flag_geom && fabs(elFD->orient) >= 0.01)) fill_build = true;
	// Detecting if the shapeItems have changed
	else for(int i = 0; !fill_build && i < shapeItems.size(); i++)
	    fill_build = shapeItems[i].path != shapeItems_temp[i].path || shapeItems[i].pathSimple != shapeItems_temp[i].pathSimple ||
		rRnd(shapeItems[i].ctrlPos4.x(),POS_PREC_DIG,true) != rRnd(shapeItems_temp[i].ctrlPos4.x(),POS_PREC_DIG,true) ||
		rRnd(shapeItems[i].ctrlPos4.y(),POS_PREC_DIG,true) != rRnd(shapeItems_temp[i].ctrlPos4.y(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n1].x(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n1].x(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n1].y(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n1].y(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n2].x(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n2].x(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n2].y(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n2].y(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n3].x(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n3].x(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n3].y(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n3].y(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n4].x(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n4].x(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n4].y(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n4].y(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n5].x(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n5].x(),POS_PREC_DIG,true) ||
		rRnd(pnts[shapeItems[i].n5].y(),POS_PREC_DIG,true) != rRnd(pnts_temp[shapeItems_temp[i].n5].y(),POS_PREC_DIG,true) ||
		colors[shapeItems[i].lineColor].rgba() != colors_temp[shapeItems_temp[i].lineColor].rgba() ||
		colors[shapeItems[i].borderColor].rgba() != colors_temp[shapeItems_temp[i].borderColor].rgba() ||
		styles[shapeItems[i].style] != styles_temp[shapeItems_temp[i].style] ||
		widths[shapeItems[i].width] != widths_temp[shapeItems_temp[i].width] ||
		widths[shapeItems[i].borderWidth] != widths_temp[shapeItems_temp[i].borderWidth] ||
		shapeItems[i].type != shapeItems_temp[i].type ||
		rRnd(shapeItems[i].angle_temp,POS_PREC_DIG,true) != rRnd(shapeItems_temp[i].angle_temp,POS_PREC_DIG,true);
	paint = fill_build;
	// Detecting the difference beetwen inundItems
	if(inundItems.size() != inundItems_temp.size()) { flag_all = paint = true; in_build = inundItems; }
	else for(int i_f = 0; i_f < inundItems.size(); i_f++) {
	    if(inundItems[i_f].n != inundItems_temp[i_f].n || fill_build) {
		in_build.push_back(inundItems[i_f]);
		fill_number.push_back(i_f);
		paint = true;
		continue;
	    }
	    if(colors[inundItems[i_f].brush].rgba() != colors_temp[inundItems_temp[i_f].brush].rgba() ||
		      inundItems_temp[i_f].brush != inundItems[i_f].brush)
	    {
		inundItems_temp[i_f].brush = inundItems[i_f].brush;
		paint = true;
	    }
	    if(images[inundItems[i_f].brushImg] != images_temp[inundItems_temp[i_f].brushImg] ||
		inundItems[i_f].brushImg != inundItems_temp[i_f].brushImg)
	    {
		inundItems_temp[i_f].brushImg = inundItems[i_f].brushImg;
		paint = true;
	    }
	}
	inundItems.clear();
	inundItems = in_build;
	//Building fills
	if(shapeItems.size())
	    for(int i = 0; i < inundItems.size(); i++) {
		if(inundItems[i].n.size() > 2) {
		    for(int k = 0; k < inundItems[i].n.size()-1; k++)
			for(int j = 0; j < shapeItems.size(); j++)
			    if((shapeItems[j].n1 == inundItems[i].n[k] && shapeItems[j].n2 == inundItems[i].n[k+1]) ||
				(shapeItems[j].n1 == inundItems[i].n[k+1] && shapeItems[j].n2 == inundItems[i].n[k]))
			    {
				flag_push_back = true;
				for(int p = 0; p < inundation_fig_num.size(); p++)
				    if((shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
					(shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1))
				    {
					flag_push_back = false;
					if(((shapeItems[inundation_fig_num[p]].type == ShT_Arc && shapeItems[j].type == ShT_Line) ||
						(shapeItems[inundation_fig_num[p]].type == ShT_Bezier && shapeItems[j].type == ShT_Line) ||
						(shapeItems[inundation_fig_num[p]].type == ShT_Arc && shapeItems[j].type == ShT_Bezier)) && inundation_fig_num[p] != j)
					    inundation_fig_num[p] = j;
				    }
				if(flag_push_back) inundation_fig_num.push_back(j);
			    }
		    for(int j = 0; j < shapeItems.size(); j++)
			if((shapeItems[j].n1 == inundItems[i].n[inundItems[i].n.size()-1] && shapeItems[j].n2 == inundItems[i].n[0]) ||
			    (shapeItems[j].n1 == inundItems[i].n[0] && shapeItems[j].n2 == inundItems[i].n[inundItems[i].n.size()-1]))
			{
			    flag_push_back = true;
			    for(int p = 0; p < inundation_fig_num.size(); p++)
				if((shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
				    (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1))
				{
				    flag_push_back = false;
				    if(((shapeItems[inundation_fig_num[p]].type == ShT_Arc && shapeItems[j].type == ShT_Line) ||
					    (shapeItems[inundation_fig_num[p]].type == ShT_Bezier && shapeItems[j].type == ShT_Line) ||
					    (shapeItems[inundation_fig_num[p]].type == ShT_Arc && shapeItems[j].type == ShT_Bezier)) && inundation_fig_num[p] != j)
					inundation_fig_num[p] = j;
				}
				if(flag_push_back) inundation_fig_num.push_back(j);
			}
		    QPainterPath temp_path = createInundationPath(inundation_fig_num, pnts, w);
		    inundation_fig_num = inundationSort(temp_path, inundation_fig_num, w);
		    inundItems[i].path = createInundationPath(inundation_fig_num, pnts, w);
		    inundItems[i].n = inundation_fig_num;
		    if(inundation_fig_num.size() > inundItems[inundItems.size()-1].n.size())
			inundItems[inundItems.size()-1].path = newPath;
		}
		if(inundItems[i].n.size() == 2) {
		    for(int j = 0; j < shapeItems.size(); j++) {
			if(((shapeItems[j].n1 == inundItems[i].n[0]) && (shapeItems[j].n2 == inundItems[i].n[1])) ||
				((shapeItems[j].n1 == inundItems[i].n[1]) && (shapeItems[j].n2 == inundItems[i].n[0])))
			    inundation_fig_num.push_back(j);
			if(inundation_fig_num.size() == 2) break;
		    }
		    inundItems[i].path = createInundationPath(inundation_fig_num, pnts, w);
		    inundItems[i].n = inundation_fig_num;
		}
		inundation_fig_num.clear();
	    }
	if(inundItems_temp.size() && shapeItems.size() && !flag_all) {
	    for(int k = 0; k < fill_number.size(); k++) inundItems_temp.replace(fill_number[k], inundItems[k]);
	    inundItems.clear();
	    inundItems = inundItems_temp;
	}
	itemInMotion = 0;
	index = -1;
	rectItems.clear();
	flag_ctrl = false;
    }
    if(up && !w->allAttrLoad() && (paint || !shapeItems_temp.size() || !inundItems_temp.size() || flag_geom)) {
	paintImage(w);
	if(uiPrmPos != -1) w->update();
	flag_geom = false;
	pnts_temp = elFD->shapePnts;
	widths_temp = elFD->shapeWidths;
	colors_temp = elFD->shapeColors;
	images_temp = elFD->shapeImages;
	styles_temp = elFD->shapeStyles;
    }

    return up;
}

void ShapeElFigure::shapeSave( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems;
    PntMap   &pnts = elFD->shapePnts;
    WidthMap &widths = elFD->shapeWidths;
    ColorMap &colors = elFD->shapeColors;
    ImageMap &images = elFD->shapeImages;
    StyleMap &styles = elFD->shapeStyles;
    string elList;
    AttrValS stAttrs;

    //Building attributes for all el_figures and fills for el_figures
    for(int i_s = 0; i_s < shapeItems.size(); i_s++) {
	int nPnts = 0;
	switch(shapeItems[i_s].type) {
	    case ShT_Line:   elList += "line:"; nPnts = 2;	break;
	    case ShT_Arc:    elList += "arc:"; nPnts = 5;	break;
	    case ShT_Bezier: elList += "bezier:"; nPnts = 4;	break;
	    default: break;
	}
	if(!nPnts) continue;

	// Points process
	for(int iP = 0; iP < nPnts; iP++) {
	    short tP = 0;
	    switch(iP) {
		case 0: tP = shapeItems[i_s].n1;	break;
		case 1: tP = shapeItems[i_s].n2;	break;
		case 2: tP = shapeItems[i_s].n3;	break;
		case 3: tP = shapeItems[i_s].n4;	break;
		case 4: tP = shapeItems[i_s].n5;	break;
	    }
	    if(tP > 0) elList += i2s(tP);
	    else if(tP <= SpI_StatIts) elList += "(" + r2s(rRnd(pnts[tP].x(),POS_PREC_DIG)) + "|" + r2s(rRnd(pnts[tP].y(),POS_PREC_DIG)) +")";
	    elList += ":";
	}

	// Line width
	if(shapeItems[i_s].width <= SpI_StatIts && (int)rRnd(widths[shapeItems[i_s].width]) != (int)rRnd(widths[SpI_DefLine]))
	    elList += i2s(rRnd(widths[shapeItems[i_s].width]));
	else if(shapeItems[i_s].width > 0) elList += "w" + i2s(shapeItems[i_s].width);
	elList += ":";

	// Line color
	if(shapeItems[i_s].lineColor <= SpI_StatIts && colors[shapeItems[i_s].lineColor].rgba() != colors[SpI_DefLine].rgba())
	    elList += colors[shapeItems[i_s].lineColor].name().toStdString() +
		((colors[shapeItems[i_s].lineColor].alpha()<255)?("-"+i2s(colors[shapeItems[i_s].lineColor].alpha())):string(""));
	else if(shapeItems[i_s].lineColor > 0) elList += "c" + i2s(shapeItems[i_s].lineColor);
	elList += ":";

	// Border width
	if(shapeItems[i_s].borderWidth <= SpI_StatIts && (int)rRnd(widths[shapeItems[i_s].borderWidth]) != (int)rRnd(widths[SpI_DefBord]))
	    elList += i2s((int)rRnd(widths[shapeItems[i_s].borderWidth]));
	else if(shapeItems[i_s].borderWidth > 0) elList += "w" + i2s(shapeItems[i_s].borderWidth);
	elList += ":";

	// Border color
	if(shapeItems[i_s].borderColor <= SpI_StatIts && colors[shapeItems[i_s].borderColor].rgba() != colors[SpI_DefLine].rgba())
	    elList += colors[shapeItems[i_s].borderColor].name().toStdString() +
		((colors[shapeItems[i_s].borderColor].alpha()<255)?("-"+i2s(colors[shapeItems[i_s].borderColor].alpha())):string(""));
	else if(shapeItems[i_s].borderColor > 0) elList += "c" + i2s(shapeItems[i_s].borderColor);
	elList += ":";

	// Line style
	if(shapeItems[i_s].style <= SpI_StatIts && styles[shapeItems[i_s].style] != styles[SpI_DefLine])
	    elList += i2s(styles[shapeItems[i_s].style]-1);
	else if(shapeItems[i_s].style > 0) elList += "s" + i2s(shapeItems[i_s].style);
	elList += "\n";
    }

    //!!!! Rethink for the code roll from here !!!!
    //For fills
    for(int i = 0; i < inundItems.size(); i++) {
	if(!inundItems[i].n.size()) continue;
	bool flag_n1 = false, flag_n2 = false;
	QVector<int> temp;
	elList += "fill:";

	// Points process
	for(int k = 0; k < inundItems[i].n.size()-1; k++) {
	    if(inundItems[i].n.size() > 2) {
		if(k == 0) {
		    if((shapeItems[inundItems[i].n[k]].n1 == shapeItems[inundItems[i].n[k+1]].n1) ||
			(shapeItems[inundItems[i].n[k]].n1 == shapeItems[inundItems[i].n[k+1]].n2))
		    {
			if(shapeItems[inundItems[i].n[k]].n2 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n2) + ":";
			else if(shapeItems[inundItems[i].n[k]].n2 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].y(),POS_PREC_DIG)) + "):";
			if(shapeItems[inundItems[i].n[k]].n1 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n1) + ":";
			else if(shapeItems[inundItems[i].n[k]].n1 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].y(),POS_PREC_DIG)) + "):";
		    }
		    else {
			if(shapeItems[inundItems[i].n[k]].n1 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n1) + ":";
			else if(shapeItems[inundItems[i].n[k]].n1 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].y(),POS_PREC_DIG)) + "):";
			if(shapeItems[inundItems[i].n[k]].n2 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n2) + ":";
			else if(shapeItems[inundItems[i].n[k]].n2 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].y(),POS_PREC_DIG)) + "):";
		    }
		    temp.push_back(shapeItems[inundItems[i].n[k]].n1);
		    temp.push_back(shapeItems[inundItems[i].n[k]].n2);
		}
		else {
		    for(int p = 0; p < temp.size(); p++) {
			if(shapeItems[inundItems[i].n[k]].n1 == temp[p]) { flag_n1 = true; flag_n2 = false; break; }
			else { flag_n2 = true; flag_n1 = false; }
		    }
		    if(flag_n1 && temp.size() < inundItems[i].n.size()) {
			if(shapeItems[inundItems[i].n[k]].n2 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n2) + ":";
			else if(shapeItems[inundItems[i].n[k]].n2 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n2].y(),POS_PREC_DIG)) + "):";

			temp.push_back(shapeItems[inundItems[i].n[k]].n2);
		    }
		    if(flag_n2 && temp.size() < inundItems[i].n.size()) {
			if(shapeItems[inundItems[i].n[k]].n1 > 0) elList += i2s(shapeItems[inundItems[i].n[k]].n1) + ":";
			else if(shapeItems[inundItems[i].n[k]].n1 <= SpI_StatIts)
			    elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].x(),POS_PREC_DIG)) + "|"
					  + r2s(rRnd(pnts[shapeItems[inundItems[i].n[k]].n1].y(),POS_PREC_DIG)) + "):";
			temp.push_back(shapeItems[inundItems[i].n[k]].n1);
		    }
		}
	    }
	}
	if(inundItems[i].n.size() <= 2) {
	    if(shapeItems[inundItems[i].n[0]].n1 > 0) elList += i2s(shapeItems[inundItems[i].n[0]].n1) + ":";
	    else if(shapeItems[inundItems[i].n[0]].n1 <= SpI_StatIts)
		elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[0]].n1].x(),POS_PREC_DIG)) + "|"
			      + r2s(rRnd(pnts[shapeItems[inundItems[i].n[0]].n1].y(),POS_PREC_DIG)) + "):";
	    if(shapeItems[inundItems[i].n[0]].n2 > 0) elList += i2s(shapeItems[inundItems[i].n[0]].n2) + ":";
	    else if(shapeItems[inundItems[i].n[0]].n2 <= SpI_StatIts)
		elList += "(" + r2s(rRnd(pnts[shapeItems[inundItems[i].n[0]].n2].x(),POS_PREC_DIG)) + "|"
			      + r2s(rRnd(pnts[shapeItems[inundItems[i].n[0]].n2].y(),POS_PREC_DIG)) + "):";
	}

	// Brush-color process
	if(inundItems[i].brush <= SpI_StatIts || inundItems[i].brush == SpI_DefFill) {
	    if(colors[inundItems[i].brush].alpha() < 255) {
		if(colors[inundItems[i].brush].rgba() == colors[SpI_DefFill].rgba()) elList += ":";
		else elList += colors[inundItems[i].brush].name().toStdString() + "-" +
				      i2s(colors[inundItems[i].brush].alpha()) + ":";
	    }
	    else elList += ((colors[inundItems[i].brush].name() == colors[SpI_DefFill].name()) ? "" :
				      colors[inundItems[i].brush].name().toStdString()) + ":";

	}
	else if(inundItems[i].brush > 0) elList += "c" + i2s(inundItems[i].brush) + ":";

	// Brush-image process
	if(inundItems[i].brushImg <= SpI_StatIts || inundItems[i].brushImg == SpI_DefFillImg)
	    elList += ((images[inundItems[i].brushImg] == images[SpI_DefFillImg]) ? string("") :
		    images[inundItems[i].brushImg].c_str()) + "\n";
	else if(inundItems[i].brushImg > 0) elList += "i" + i2s(inundItems[i].brushImg) + "\n";
    }
    XMLNode chCtx("attr");
    chCtx.setAttr("name", _("Elementary figure"));
    if(devW && elList != elFD->elLst) {
	chCtx.setAttr("id", "elLst")->setAttr("noMerge", "1")->setAttr("prev", elFD->elLst)->setText(elList);
	devW->chLoadCtx(chCtx, "", "elLst");
    }
    stAttrs.push_back(std::make_pair("elLst",elList));
    elFD->elLst = elList;
    //devW->chRecord(*XMLNode("attr").setAttr("id","elLst")->setAttr("noMerge","1")->setAttr("prev",elFD->elLst)->setText(elList));

    //Write dynamic shapes points to the data model
    for(PntMap::iterator pi = pnts.begin(); pi != pnts.end(); pi++) {
	if(pi->first <= 0) continue;
	PntMap::iterator pt = elFD->shapePnts_temp.find(pi->first);
	QPointF pnt_next = QPointF(rRnd(pi->second.x(),POS_PREC_DIG), rRnd(pi->second.y(),POS_PREC_DIG));
	if(pt != elFD->shapePnts_temp.end()) {
	    QPointF pnt_prev = QPointF(rRnd(pt->second.x(),POS_PREC_DIG), rRnd(pt->second.y(),POS_PREC_DIG));
	    if(pnt_prev != pnt_next) {
		chCtx.childAdd("attr")->setAttr("id", "p"+i2s(pi->first)+"x")->setAttr("prev", r2s(pnt_prev.x(),POS_PREC_DIG))->setText(r2s(pnt_next.x(),POS_PREC_DIG));
		chCtx.childAdd("attr")->setAttr("id", "p"+i2s(pi->first)+"y")->setAttr("prev", r2s(pnt_prev.y(),POS_PREC_DIG))->setText(r2s(pnt_next.y(),POS_PREC_DIG));
	    }
	}
	else {
	    chCtx.childAdd("attr")->setAttr("id", "p"+i2s(pi->first)+"x")->setAttr("prev", "0")->setText(r2s(pnt_next.x(),POS_PREC_DIG));
	    chCtx.childAdd("attr")->setAttr("id", "p"+i2s(pi->first)+"y")->setAttr("prev", "0")->setText(r2s(pnt_next.y(),POS_PREC_DIG));
	}
	stAttrs.push_back(std::make_pair("p"+i2s(pi->first)+"x",r2s(pnt_next.x())));
	stAttrs.push_back(std::make_pair("p"+i2s(pi->first)+"y",r2s(pnt_next.y())));
    }
    elFD->shapePnts_temp = elFD->shapePnts;

    //Write dynamic shapes widths to the data model
    for(WidthMap::iterator pi = widths.begin(); pi != widths.end(); pi++) {
	if(pi->first <= 0) continue;
	WidthMap::iterator pt = elFD->shapeWidths_temp.find(pi->first);
	float wdt_next = rRnd(pi->second, POS_PREC_DIG);

	if(pt != elFD->shapeWidths_temp.end()) {
	    float wdt_prev = rRnd(pt->second, POS_PREC_DIG);
	    if(wdt_prev != wdt_next)
		chCtx.childAdd("attr")->setAttr("id", "w"+i2s(pi->first))->setAttr("prev", r2s(wdt_prev,POS_PREC_DIG))->setText(r2s(wdt_next,POS_PREC_DIG));
	}
	else
	    chCtx.childAdd("attr")->setAttr("id", "w"+i2s(pi->first))->setAttr("prev", "0")->setText(r2s(wdt_next,POS_PREC_DIG));
	    stAttrs.push_back(std::make_pair("w"+i2s(pi->first),r2s(rRnd(pi->second,POS_PREC_DIG))));
    }
    elFD->shapeWidths_temp = elFD->shapeWidths;

    //Write dynamic shapes colors to the data model
    for(ColorMap::iterator pi = colors.begin(); pi != colors.end(); pi++) {
	if(pi->first <= 0) continue;
	ColorMap::iterator pt = elFD->shapeColors_temp.find(pi->first);
	QColor clr_next = pi->second;

	if(pt != elFD->shapeColors_temp.end()) {
	    QColor clr_prev = pt->second;
	    if(clr_prev != clr_next) {
		if(clr_next.alpha() < 255)
		    chCtx.childAdd("attr")->setAttr("id", "c"+i2s(pi->first))->
			setAttr("prev", clr_prev.name().toStdString() + "-" + i2s(clr_prev.alpha()))->
			setText(clr_next.name().toStdString() + "-" + i2s(clr_next.alpha()));
		else chCtx.childAdd("attr")->setAttr("id", "c"+i2s(pi->first))->
			setAttr("prev", clr_prev.name().toStdString())->setText(clr_next.name().toStdString());
	    }
	}
	else {
	    if(clr_next.alpha() < 255)
		chCtx.childAdd("attr")->setAttr("id", "c"+i2s(pi->first))->
		    setAttr("prev", "black")->setText(clr_next.name().toStdString() + "-" + i2s(clr_next.alpha()));
		else chCtx.childAdd("attr")->setAttr("id", "c"+i2s(pi->first))->setAttr("prev", "black")->setText(clr_next.name().toStdString());
	    }
	    if(pi->second.alpha() < 255)
		stAttrs.push_back(std::make_pair("c"+i2s(pi->first),pi->second.name().toStdString()+"-"+i2s(pi->second.alpha())));
	    else stAttrs.push_back(std::make_pair("c"+i2s(pi->first),pi->second.name().toStdString()));
    }
    elFD->shapeColors_temp = elFD->shapeColors;

    //Write dynamic fills images to the data model
    for(ImageMap::iterator pi = images.begin(); pi != images.end(); pi++) {
	if(pi->first <= 0) continue;
	ImageMap::iterator pt = elFD->shapeImages_temp.find(pi->first);
	string img_next = pi->second;

	if(pt != elFD->shapeImages_temp.end()) {
	    string img_prev = pt->second;
	    if(img_prev != img_next)
		chCtx.childAdd("attr")->setAttr("id", "i"+i2s(pi->first))->setAttr("prev", img_prev)->setText(img_next);
	}
	else chCtx.childAdd("attr")->setAttr("id", "i"+i2s(pi->first))->setAttr("prev", "")->setText(img_next);
	stAttrs.push_back(std::make_pair("i"+i2s(pi->first),pi->second.c_str()));
    }
    elFD->shapeImages_temp = elFD->shapeImages;

    //Write dynamic shapes styles to the data model
    for(StyleMap::iterator pi = styles.begin(); pi != styles.end(); pi++) {
	if(pi->first <= 0) continue;
	StyleMap::iterator pt = elFD->shapeStyles_temp.find(pi->first);
	Qt::PenStyle stl_next = pi->second;

	if(pt != elFD->shapeStyles_temp.end()) {
	    Qt::PenStyle stl_prev = pt->second;
	    if(stl_prev != stl_next)
		chCtx.childAdd("attr")->setAttr("id", "s"+i2s(pi->first))->setAttr("prev", i2s(stl_prev-1))->setText(i2s(stl_next-1));
	}
	else chCtx.childAdd("attr")->setAttr("id", "s"+i2s(pi->first))->setAttr("prev", "0")->setText(i2s(stl_next-1));
	stAttrs.push_back(std::make_pair("s"+i2s(pi->first),i2s(pi->second-1)));
    }

    if(stAttrs.size()) w->attrsSet(stAttrs);
    elFD->shapeStyles_temp = elFD->shapeStyles;
    if(devW && (chCtx.attr("id").size() || chCtx.childSize())) devW->chRecord(chCtx);
    devW->setSelect(true, false);
}

//!!!! Continue for the code cleun up from here !!!!
void ShapeElFigure::initShapeItems( const QPointF &pos, QVector<int> &items_array, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;
    for(int i = 0; i < items_array.size(); i++)
	if(shapeItems[items_array[i]].type == 2)
	{
	    //> Detecting if the start or end point of this arc matches the start or end point of the other arc and, if so, add this point to the map again
	    for(int j = 0; j < shapeItems.size(); j++)
	    {
		if((shapeItems[j].n1 == shapeItems[items_array[i]].n1 || shapeItems[j].n1 == shapeItems[items_array[i]].n2) &&
			items_array[i] != j && shapeItems[j].type == ShT_Arc)
		    shapeItems[j].n1 = elFD->appendPoint(pnts[shapeItems[j].n1], true);
		if((shapeItems[j].n2 == shapeItems[items_array[i]].n1 || shapeItems[j].n2 == shapeItems[items_array[i]].n2) &&
			items_array[i] != j && shapeItems[j].type == ShT_Arc)
		    shapeItems[j].n2 = elFD->appendPoint(pnts[shapeItems[j].n2], true);
	    }
	    count_moveItemTo	= 1;
	    count_Shapes	= 1;
	    flag_ctrl_move	= false;
	    flag_ctrl		= true;
	    offset		= QPointF( );
	    index		= items_array[i];
	    itemInMotion	= &shapeItems[index];
	    moveItemTo(pos, w);
	}
    for( int i=0; i < items_array.size(); i++ )
	if( shapeItems[items_array[i]].type != 2 )
	{
	    count_moveItemTo = 1;
	    count_Shapes     = 1;
	    flag_ctrl_move   = false;
	    flag_ctrl	= true;
	    offset = QPointF();
	    index	    = items_array[i];
	    itemInMotion = &shapeItems[index];
	    moveItemTo(pos, w);
	}
}

QPointF ShapeElFigure::getArcStartEnd( QPointF pBeg, QPointF pEnd, QPointF pCntr1, QPointF pCntr2, QPointF pCntr3 )
{
    double a = length(pCntr3, pCntr1),
	   //b = length(pCntr2, pCntr1),
	   ang = angle(QLineF(pCntr1,pCntr3), QLineF(pCntr1,QPointF(pCntr1.x()+10,pCntr1.y())));
    if(!a) return QPointF(0, 0);
    if(pCntr3.y() >= pCntr1.y()) ang = 360 - ang;

    //pCntr2 = QPointF(pCntr1.x()+rotate(arc(0.25,a,b),ang).x(), pCntr1.y()-rotate(arc(0.25,a,b),ang).y());
    //pCntr3 = QPointF(pCntr1.x()+rotate(arc(0,a,b),ang).x(), pCntr1.y()-rotate(arc(0,a,b),ang).y());
    pBeg = unRotate(pBeg, ang, pCntr1);
    if(pBeg.x() >= a)	pBeg = QPointF(a, (pBeg.y()/pBeg.x())*a);
    if(pBeg.x() < -a)	pBeg = QPointF(-a, (pBeg.y()/pBeg.x())*(-a));
    double t_start = acos(pBeg.x()/a)/(2*M_PI);
    if(pBeg.y() > 0)	t_start = 1 - t_start;

    pEnd = unRotate(pEnd, ang, pCntr1);
    if(pEnd.x() < -a)	pEnd = QPointF(-a, (pEnd.y()/pEnd.x())*(-a));
    if(pEnd.x() >= a)	pEnd = QPointF(a, (pEnd.y()/pEnd.x())*(a));
    double t_end = acos(pEnd.x()/a)/(2*M_PI);
    if(pEnd.y() > 0)	t_end = 1 - t_end;
    if(t_start > t_end)	t_end += 1;
    if((t_end-1) > t_start)	t_end -= 1;
    if(fabs(t_start-t_end) < ARC_STEP) t_end += 1;
    if(t_end > t_start && t_start >= 1 && t_end > 1) { t_start -= 1; t_end -= 1; }

    return QPointF(t_start, t_end);
}

void ShapeElFigure::editEnter( DevelWdgView *w )
{
    itemInMotion = NULL;
    index = -1; status = /*fMoveHoldMove =*/ false;

    w->mainWin()->elFigTool->setVisible(true);

    //> Self-shape tools
    connect(w->mainWin()->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    //>> Init actions' address
    for(int i_a = 0; i_a < w->mainWin()->elFigTool->actions().size(); i_a++)
    {
	w->mainWin()->elFigTool->actions().at(i_a)->setEnabled(true);
	w->mainWin()->elFigTool->actions().at(i_a)->setProperty("wdgAddr", TSYS::addr2str(w).c_str());
    }
    //> Main tools (copy)
    connect(w->mainWin()->visItToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    w->mainWin()->actVisItCopy->setProperty("wdgAddr", TSYS::addr2str(w).c_str());
    w->mainWin()->actVisItPaste->setProperty("wdgAddr", TSYS::addr2str(w).c_str());
    w->mainWin()->actVisItCut->setVisible(false);
    w->mainWin()->actVisItCopy->setEnabled(false);
    w->mainWin()->actVisItPaste->setEnabled(false);
    //> Figures level tools
    connect(w->mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    w->mainWin()->actLevRise->setProperty("wdgAddr", TSYS::addr2str(w).c_str());
    w->mainWin()->actLevLower->setProperty("wdgAddr", TSYS::addr2str(w).c_str());
    w->mainWin()->actLevRise->setEnabled(false);
    w->mainWin()->actLevLower->setEnabled(false);
    status_hold = true;
}

void ShapeElFigure::editExit( DevelWdgView *w )
{
    disconnect(w->mainWin()->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    w->mainWin()->elFigTool->setVisible(false);
    //> Clear action's address
    for(int i_a = 0; i_a < w->mainWin()->elFigTool->actions().size(); i_a++)
    {
	w->mainWin()->elFigTool->actions().at(i_a)->setProperty("wdgAddr", "");
	w->mainWin()->elFigTool->actions().at(i_a)->setEnabled(false);
    }
    w->mainWin()->actElFigLine->setChecked(false);
    w->mainWin()->actElFigArc->setChecked(false);
    w->mainWin()->actElFigBesie->setChecked(false);
    w->mainWin()->actElFigCheckAct->setChecked(true);
    w->mainWin()->actElFigCursorAct->setChecked(true);

    disconnect(w->mainWin()->visItToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    w->mainWin()->actVisItCopy->setProperty("wdgAddr", "");
    w->mainWin()->actVisItPaste->setProperty("wdgAddr", "");
    w->mainWin()->actVisItCut->setVisible(true);
    disconnect(w->mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)));
    w->mainWin()->actLevRise->setProperty("wdgAddr", "");
    w->mainWin()->actLevLower->setProperty("wdgAddr", "");

    w->mainWin()->actVisItCut->setVisible(true);
    w->mainWin()->actVisItCopy->setEnabled(true);
    w->mainWin()->actVisItPaste->setEnabled(false);

    shapeSave(w);
    w->unsetCursor();
    status = false;
    flag_ctrl = flag_A = flag_copy = flag_up = flag_down = flag_left = flag_right =  false;
    itemInMotion = 0;
    count_Shapes = 0;
    index_array.clear();
    if(rectItems.size())
    {
	rectItems.clear();
	paintImage(w);
    }
}

void ShapeElFigure::wdgPopup( WdgView *w, QMenu &menu )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    if(devW)
    {
	int actDyn = 0;
	ElFigDt *elFD = (ElFigDt*)w->shpData;
	QImage ico_propDlg;
	if(!ico_propDlg.load(TUIS::icoGet("edit",NULL,true).c_str())) ico_propDlg.load(":/images/edit.png");
	dyn_num = 0;

	PntMap *pnts = &elFD->shapePnts;
	WidthMap *widths = &elFD->shapeWidths;
	ColorMap *colors = &elFD->shapeColors;
	ImageMap *images = &elFD->shapeImages;
	StyleMap *styles = &elFD->shapeStyles;

	QVector<ShapeItem> &shapeItems = elFD->shapeItems;
	QVector<inundationItem> &inundItems = elFD->inundItems;
	for(int i_a = 0; i_a < devW->mainWin()->elFigTool->actions().size(); i_a++)
	    menu.addAction(devW->mainWin()->elFigTool->actions().at(i_a));
	menu.addSeparator();
	menu.addAction(devW->mainWin()->actVisItUnDo);
	menu.addAction(devW->mainWin()->actVisItReDo);
	menu.addSeparator();
	menu.addAction(devW->mainWin()->actVisItCopy);
	menu.addAction(devW->mainWin()->actVisItPaste);
	menu.addSeparator();
	menu.addAction(devW->mainWin()->actLevRise);
	menu.addAction(devW->mainWin()->actLevLower);
	menu.addSeparator();
	switch(rect_num)
	{
	    case 0:
		if( shapeItems[index].n1 <= -10 ) actDyn = 1;
		else if( shapeItems[index].n1 > 0 ) actDyn = 2;
		break;
	    case 1:
		if( shapeItems[index].n2 <= -10 ) actDyn = 1;
		else if( shapeItems[index].n2 > 0 ) actDyn = 2;
		break;
	    case 2:
		if( shapeItems[index].n3 <= -10 ) actDyn = 1;
		else if( shapeItems[index].n3 > 0 ) actDyn = 2;
		break;
	    case 3:
		if( shapeItems[index].n4 <= -10 ) actDyn = 1;
		else if( shapeItems[index].n4 > 0 ) actDyn = 2;
		break;
	    case 4:
		if( shapeItems[index].n5 <= -10 ) actDyn = 1;
		else if( shapeItems[index].n5 > 0 ) actDyn = 2;
		break;
	}
	if( actDyn == 1 )
	{
	    QAction *actDynamicPoint = new QAction( _("Make this point dynamic"), w->mainWin() );
	    actDynamicPoint->setObjectName("d_point");
	    actDynamicPoint->setStatusTip(_("Press to make this point dynamic"));
	    connect( actDynamicPoint, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
	    menu.addAction(actDynamicPoint);
	    menu.addSeparator();
	}
	else if( actDyn == 2 )
	{
	    QAction *actDynamicPoint = new QAction( _("Make this point static"), w->mainWin() );
	    actDynamicPoint->setObjectName("s_point");
	    actDynamicPoint->setStatusTip(_("Press to make this point static"));
	    connect( actDynamicPoint, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
	    menu.addAction(actDynamicPoint);
	    menu.addSeparator();
	}
	else if( index != -1 && actDyn == 0 && rect_num == -1 )
	{
	    if( shapeItems[index].width <= -5 )
	    {
		QAction *actDynamicWidth = new QAction( _("Make line width dynamic"), w->mainWin() );
		actDynamicWidth->setObjectName("d_width");
		actDynamicWidth->setStatusTip(_("Press to make line width dynamic"));
		connect( actDynamicWidth, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicWidth);
	    }
	    else if( shapeItems[index].width > 0 )
	    {
		QAction *actDynamicWidth = new QAction( _("Make line width static"), w->mainWin() );
		actDynamicWidth->setObjectName("s_width");
		actDynamicWidth->setStatusTip(_("Press to make line width static"));
		connect( actDynamicWidth, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicWidth);
	    }
	    if( shapeItems[index].lineColor <= -5 )
	    {
		QAction *actDynamicColor = new QAction( _("Make line color dynamic"), w->mainWin() );
		actDynamicColor->setObjectName("d_color");
		actDynamicColor->setStatusTip(_("Press to make line color dynamic"));
		connect( actDynamicColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicColor);
	    }
	    else if( shapeItems[index].lineColor > 0 )
	    {
		QAction *actDynamicColor = new QAction( _("Make line color static"), w->mainWin() );
		actDynamicColor->setObjectName("s_color");
		actDynamicColor->setStatusTip(_("Press to make line color static"));
		connect( actDynamicColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicColor);
	    }
	    if( (*widths)[shapeItems[index].borderWidth] > 0.01 )
	    {
		if( shapeItems[index].borderWidth <= -6 )
		{
		    QAction *actDynamicBorderWidth = new QAction( _("Make border width dynamic"), w->mainWin() );
		    actDynamicBorderWidth->setObjectName("d_border_width");
		    actDynamicBorderWidth->setStatusTip(_("Press to make border width dynamic"));
		    connect( actDynamicBorderWidth, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		    menu.addAction(actDynamicBorderWidth);
		}
		else if( shapeItems[index].borderWidth > 0 )
		{
		    QAction *actDynamicBorderWidth = new QAction( _("Make border width static"), w->mainWin() );
		    actDynamicBorderWidth->setObjectName("s_border_width");
		    actDynamicBorderWidth->setStatusTip(_("Press to make border width static"));
		    connect( actDynamicBorderWidth, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		    menu.addAction(actDynamicBorderWidth);
		}
		if( shapeItems[index].borderColor <= -6 )
		{
		    QAction *actDynamicBorderColor = new QAction( _("Make border color dynamic"), w->mainWin() );
		    actDynamicBorderColor->setObjectName("d_border_color");
		    actDynamicBorderColor->setStatusTip(_("Press to make border color dynamic"));
		    connect( actDynamicBorderColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		    menu.addAction(actDynamicBorderColor);
		}
		else if( shapeItems[index].borderColor > 0 )
		{
		    QAction *actDynamicBorderColor = new QAction( _("Make border color static"), w->mainWin() );
		    actDynamicBorderColor->setObjectName("s_border_color");
		    actDynamicBorderColor->setStatusTip(_("Press to make border color static"));
		    connect( actDynamicBorderColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		    menu.addAction(actDynamicBorderColor);
		}
	    }
	    if( shapeItems[index].style <= -5 )
	    {
		QAction *actDynamicStyle = new QAction( _("Make line style dynamic"), w->mainWin() );
		actDynamicStyle->setObjectName("d_style");
		actDynamicStyle->setStatusTip(_("Press to make line style dynamic"));
		connect( actDynamicStyle, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicStyle);
	    }
	    else if( shapeItems[index].style > 0 )
	    {
		QAction *actDynamicStyle = new QAction( _("Make line style static"), w->mainWin() );
		actDynamicStyle->setObjectName("s_style");
		actDynamicStyle->setStatusTip(_("Press to make line style static"));
		connect( actDynamicStyle, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actDynamicStyle);
	    }
	    QAction *actShowProperties = new QAction( QPixmap::fromImage(ico_propDlg), _("Show the figure's properties"), w->mainWin() );
	    switch( shapeItems[index].type )
	    {
		case 1:	actShowProperties->setObjectName("Line"); break;
		case 2:	actShowProperties->setObjectName("Arc"); break;
		case 3: actShowProperties->setObjectName("Bezier curve"); break;
	    }
	    fill_index = -1;
	    actShowProperties->setStatusTip(_("Press to show the properties dialog"));
	    connect(actShowProperties, SIGNAL(triggered()), elFD, SLOT(properties()));
	    menu.addAction(actShowProperties);
	    flag_rect_items = (bool)rectItems.size();
	    menu.addSeparator();
	}
	else if( index == -1 && (int)pop_pos.x() != -1 && (int)pop_pos.y() != -1 )
	{
	    bool flg = false;
	    for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
		if(pi->first > 0 )
		{
		    flg = true;
		    break;
		}
		else for( WidthMap::reverse_iterator pi = widths->rbegin(); pi != widths->rend(); pi++ )
		if(pi->first > 0 )
		{
		    flg = true;
		    break;
		}
		else for( ColorMap::reverse_iterator pi = colors->rbegin(); pi != colors->rend(); pi++ )
		if(pi->first > 0 )
		{
		    flg = true;
		    break;
		}
		else for( ImageMap::reverse_iterator pi = images->rbegin(); pi != images->rend(); pi++ )
		if(pi->first > 0 )
		{
		    flg = true;
		    break;
		}
		else for( StyleMap::reverse_iterator pi = styles->rbegin(); pi != styles->rend(); pi++ )
		if(pi->first > 0 )
		{
		    flg = true;
		    break;
		}

	    fill_index = -1;
	    for( int i = inundItems.size()-1; i >= 0; i-- )
		if( inundItems[i].path.contains(pop_pos) )
		{
		    QAction *actDeleteFill = new QAction( _("Delete the current fill"), w->mainWin() );
		    actDeleteFill->setObjectName("delete_fill");
		    actDeleteFill->setStatusTip(_("Press to delete the current fill"));
		    connect( actDeleteFill, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		    menu.addAction(actDeleteFill);

		    if( inundItems[i].brush <= -7 )
		    {
			QAction *actDynamicFillColor = new QAction( _("Make fill color dynamic"), w->mainWin() );
			actDynamicFillColor->setObjectName("d_fill_color");
			actDynamicFillColor->setStatusTip(_("Press to make fill color dynamic"));
			connect( actDynamicFillColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
			menu.addAction(actDynamicFillColor);
		    }
		    else if( inundItems[i].brush > 0 )
		    {
			QAction *actDynamicFillColor = new QAction( _("Make fill color static"), w->mainWin() );
			actDynamicFillColor->setObjectName("s_fill_color");
			actDynamicFillColor->setStatusTip(_("Press to make fill color static"));
			connect( actDynamicFillColor, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
			menu.addAction(actDynamicFillColor);
		    }
		    if( inundItems[i].brushImg <= -5 )
		    {
			QAction *actDynamicFillImage = new QAction( _("Make fill image dynamic"), w->mainWin() );
			actDynamicFillImage->setObjectName("d_fill_image");
			actDynamicFillImage->setStatusTip(_("Press to make fill image dynamic"));
			connect( actDynamicFillImage, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
			menu.addAction(actDynamicFillImage);
		    }
		    else if( inundItems[i].brushImg > 0 )
		    {
			QAction *actDynamicFillImage = new QAction( _("Make fill image static"), w->mainWin() );
			actDynamicFillImage->setObjectName("s_fill_image");
			actDynamicFillImage->setStatusTip(_("Press to make fill image static"));
			connect( actDynamicFillImage, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
			menu.addAction(actDynamicFillImage);
		    }

		    fill_index = i;
		    QAction *actShowFillProperties = new QAction( QPixmap::fromImage(ico_propDlg), _("Show the fill's properties"), w->mainWin() );
		    actShowFillProperties->setObjectName("Fill");
		    actShowFillProperties->setStatusTip(_("Press to show the fill's properties dialog"));
		    connect( actShowFillProperties, SIGNAL(triggered()), elFD, SLOT(properties()) );
		    menu.addAction(actShowFillProperties);

		    break;
		}
	    if( flg )
	    {
		QAction *actStatic = new QAction( _("Make all values of the widget the static ones"), w->mainWin() );
		actStatic->setObjectName("static");
		actStatic->setStatusTip(_("Press to make all values of the widget static ones"));
		connect( actStatic, SIGNAL(triggered()), elFD, SLOT(dynamic()) );
		menu.addAction(actStatic);
		menu.addSeparator();

	    }
	}
	pop_pos = QPointF(-1,-1);
    }
}

void ShapeElFigure::toolAct( QAction *act )
{
    DevelWdgView *w = dynamic_cast<DevelWdgView*>((WdgView*)TSYS::str2addr(act->property("wdgAddr").toString().toStdString()));
    if(!w) return;
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems;
    PntMap &pnts 	= elFD->shapePnts;
    WidthMap &widths	= elFD->shapeWidths;
    StyleMap &styles	= elFD->shapeStyles;
    ColorMap &clrs	= elFD->shapeColors;
    ImageMap &imgs	= elFD->shapeImages;

    if(act->objectName() == "cursor" || act->objectName() == "line" || act->objectName() == "arc" || act->objectName() == "besier")
	for(int i_a = 0; i_a < w->mainWin()->elFigTool->actions().size(); i_a++)
	    if(w->mainWin()->elFigTool->actions().at(i_a)->objectName() != "hold")
		w->mainWin()->elFigTool->actions().at(i_a)->setChecked(false);
    bool fl_figure = false;
    if(act->objectName() == "line")		{ shapeType = 1; fl_figure = true; }
    else if(act->objectName() == "arc")		{ shapeType = 2; fl_figure = true; }
    else if(act->objectName() == "besier")	{ shapeType = 3; fl_figure = true; }
    else if(act->objectName() == "hold")
    {
	status_hold = act->isChecked();
	if(flag_A)
	{
	    flag_ctrl = flag_A = flag_copy = false;
	    flag_check_pnt_inund = false;
	    itemInMotion = 0;
	    count_Shapes = 0;
	}
	index_array.clear();
	index_array_copy.clear();
	rectItems.clear();
	paintImage(w);
	w->repaint();
    }
    else if(act->objectName() == "cursor")
    {
	act->setChecked(true);
	w->unsetCursor();
	status = false;
	paintImage(w);
	w->update();
    }
    else if(act->objectName() == "editcopy")
    {
	if(copy_index.size()) copy_index.clear();
	if(flag_A) copy_index = index_array_copy_flag_A.size() ? index_array_copy_flag_A : index_array;
	else if(index_array.size() && !status_hold && index_array[0] != -1) copy_index = index_array;
	else if(status_hold && index_array_copy.size())	copy_index = index_array_copy;
	else if(index_temp != -1) copy_index.push_back(index_temp);
	index_array_copy_flag_A.clear();
	w->mainWin()->actVisItPaste->setEnabled(true);
    }
    else if(act->objectName() == "editpaste")
    {
	QVector<int> inund_figs;
	map<int, QVector<int> > inund_map;
	if(index_array.size()) index_array.clear();
	bool f_present = true;
	for(int i_p = 0; f_present && i_p < copy_index.size(); i_p++)
	    if(copy_index[i_p] != -1)
	    {
		bool f_pr = false;
		for(int i_s = 0; !f_pr && i_s < shapeItems.size(); i_s++)
		    if(copy_index[i_p] == i_s)	f_pr = true;
		if(!f_pr) f_present = false;
	    }
	if(f_present)
	    for(int i = 0; i < copy_index.size(); i++)
	    {
		if(copy_index[i] == -1) continue;
		shapeItems.push_back(shapeItems[copy_index[i]]);
		index_array.push_back(shapeItems.size()-1);
		ShapeItem &newIt = shapeItems[shapeItems.size()-1];
		if(!vrng(newIt.n1,-9,0))	newIt.n1 = elFD->appendPoint(pnts[newIt.n1], (newIt.n1 <= -10));
		if(!vrng(newIt.n2,-9,0))	newIt.n2 = elFD->appendPoint(pnts[newIt.n2], (newIt.n2 <= -10));
		if(!vrng(newIt.n3,-9,0))	newIt.n3 = elFD->appendPoint(pnts[newIt.n3], (newIt.n3 <= -10));
		if(!vrng(newIt.n4,-9,0))	newIt.n4 = elFD->appendPoint(pnts[newIt.n4], (newIt.n4 <= -10));
		if(!vrng(newIt.n5,-9,0))	newIt.n5 = elFD->appendPoint(pnts[newIt.n5], (newIt.n5 <= -10));
		if(!vrng(newIt.width,-9,0))	newIt.width = elFD->appendWidth(widths[newIt.width], (newIt.width <= -10));
		if(!vrng(newIt.borderWidth,-9,0)) newIt.borderWidth = elFD->appendWidth(widths[newIt.borderWidth], (newIt.borderWidth <= -10));
		if(!vrng(newIt.style,-9,0))	newIt.style = elFD->appendStyle(styles[newIt.style], (newIt.style <= -10));
		if(!vrng(newIt.lineColor,-9,0))	newIt.lineColor = elFD->appendColor(clrs[newIt.lineColor], (newIt.lineColor <= -10));
		if(!vrng(newIt.borderColor,-9,0)) newIt.borderColor = elFD->appendColor(clrs[newIt.borderColor], (newIt.borderColor <= -10));
	    }
	else w->mainWin()->actVisItCopy->setEnabled(false);
	if(index_array.size())
	{
	    for(int i = 0; i < index_array.size(); i++)
		for(int h_p = 0; h_p < index_array.size(); h_p++)
		{
		    ShapeItem &iP = shapeItems[index_array[i]], &hP = shapeItems[index_array[h_p]];
		    if(h_p == i || (iP.type == ShT_Arc && hP.type == ShT_Arc)) continue;

		    if(fabs(pnts[iP.n1].x()-pnts[hP.n1].x()) < 0.01 && fabs(pnts[iP.n1].y()-pnts[hP.n1].y()) < 0.01)
		    { elFD->dropPoint(hP.n1, index_array[h_p]); hP.n1 = iP.n1; }
		    if(fabs(pnts[iP.n1].x()-pnts[hP.n2].x()) < 0.01 && fabs(pnts[iP.n1].y()-pnts[hP.n2].y()) < 0.01)
		    { elFD->dropPoint(hP.n2, index_array[h_p]); hP.n2 = iP.n1; }
		    if(fabs(pnts[iP.n2].x()-pnts[hP.n2].x()) < 0.01 && fabs(pnts[iP.n2].y()-pnts[hP.n2].y()) < 0.01)
		    { elFD->dropPoint(hP.n2, index_array[h_p]); hP.n2 = iP.n2; }
		}
	    for(int i = 0; i < inundItems.size(); i++)
	    {
		for(int j = 0; j < inundItems[i].n.size(); j++)
		    for(int k = 0; k < copy_index.size(); k++)
			if(inundItems[i].n[j] == copy_index[k])
		    	    inund_figs.push_back(index_array[k]);
		if(inund_figs.size() == inundItems[i].n.size())	inund_map[i] = inund_figs;
		inund_figs.clear();
	    }
	    for(map<int, QVector<int> >::iterator pi = inund_map.begin(); pi != inund_map.end(); pi++)
	    {
		inundItems.push_back(inundationItem(createInundationPath(pi->second,pnts,w),pi->second,
			inundItems[pi->first].brush,inundItems[pi->first].brushImg));
		inundationItem &newIt = inundItems[inundItems.size()-1];
		if(!vrng(newIt.brush,SpI_StatIts+1,0)) newIt.brush = elFD->appendColor(clrs[newIt.brush],(newIt.brush <= SpI_StatIts));
		if(!vrng(newIt.brushImg,SpI_StatIts+1,0)) newIt.brushImg = elFD->appendImage(imgs[newIt.brushImg],(newIt.brushImg <= SpI_StatIts));
	    }
	    flag_copy = flag_A = true;
	    if(rect_array.size())
	    {
		flag_rect = false;
		rect_array.clear();
	    }
	    rect_num = -1;
	    flag_ctrl_move = true;
	    count_Shapes = index_array.size();
	    moveAll(QPointF(), w);
	    shapeSave(w);
	    paintImage(w);
	    w->repaint();
	}
	copy_index.clear();
	w->mainWin()->actVisItPaste->setEnabled(false);
    }
    else if(act->objectName() == "level_rise")
    {
	int index_array_inund;
	if(index_array.size() && !status_hold && index_array[0] != -1)
	{
	    //> Sorting the fills
	    vector<int> rise_fill;	//container of the fills to be rised
	    int fig_col;		//quantity of the figures matched with the each fill's shapes
	    for(int j = 0; j < inundItems.size(); j++)
	    {
		fig_col = 0;
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    for(int i = 0; i < index_array.size(); i++)
			if(inundItems[j].n[p] == index_array[i]) fig_col++;
		if(fig_col == inundItems[j].n.size())	// addition the fill to the container
		    rise_fill.push_back(j);
	    }
	    //>> Replacing the fills
	    for(unsigned i = 0; i < rise_fill.size(); i++) inundItems.push_back(inundItems[rise_fill[i]]);
	    for(unsigned i = 0, off = 0; i < rise_fill.size(); i++, off++) inundItems.remove(rise_fill[i]-off);

	    //> Sorting the figures
	    ShapeItem item_temp;
	    for(int i = 0; i < index_array.size(); i++)
		if(index_array[i] != -1)
		{
		    index_array_inund = index_array[i];
		    item_temp = shapeItems[index_array[i]];
		    for(int j = index_array[i]; j < (shapeItems.size()-1); j++)
		    {
			shapeItems[j] = shapeItems[j+1];
			for(int k = 0; k < index_array.size(); k++)
			    if(j == index_array[k] && index_array[k] != -1)
				index_array[k] = index_array[k] - 1;
		    }
		    shapeItems[shapeItems.size()-1] = item_temp;
		    for(int j = 0; j < inundItems.size(); j++)
			for(int p = 0; p < inundItems[j].n.size(); p++)
			    if(inundItems[j].n[p] == index_array_inund)
				inundItems[j].n[p] = shapeItems.size()-1;
			    else if(inundItems[j].n[p] > index_array_inund)
				inundItems[j].n[p]--;
		}
	}
	else if(status_hold && (index_array_copy.size() || (index_array_copy.size() == 0 && (index_array.size() && index_array[0] != -1))))
	{
	    if(index_array_copy.size() == 0 && index_array.size() && index_array[0] != -1) index_array_copy = index_array;

	    //> Sorting the fills
	    vector<int> rise_fill;
	    int fig_col;
	    for(int j = 0; j < inundItems.size(); j++)
	    {
		fig_col = 0;
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    for(int i = 0; i < index_array_copy.size(); i++)
			if(inundItems[j].n[p] == index_array_copy[i]) fig_col++;
		if(fig_col == inundItems[j].n.size()) rise_fill.push_back(j);
	    }
	    for(unsigned i = 0; i < rise_fill.size(); i++) inundItems.push_back(inundItems[rise_fill[i]]);
	    for(unsigned i = 0, off = 0; i < rise_fill.size(); i++, off++) inundItems.remove(rise_fill[i] - off);

	    //> Sorting the figures
	    ShapeItem item_temp;
	    for(int i = 0; i < index_array_copy.size(); i++)
		if(index_array_copy[i] != -1)
		{
		    index_array_inund = index_array_copy[i];
		    item_temp = shapeItems[index_array_copy[i]];
		    for(int j = index_array_copy[i]; j < (shapeItems.size()-1); j++)
		    {
			shapeItems[j] = shapeItems[j+1];
			for(int k = 0; k < index_array_copy.size(); k++)
			    if(j == index_array_copy[k] && index_array_copy[k] != -1)
				index_array_copy[k] = index_array_copy[k] - 1;
		    }
		    shapeItems[shapeItems.size()-1] = item_temp;
		    for(int j = 0; j < inundItems.size(); j++)
			for(int p = 0; p < inundItems[j].n.size(); p++)
			    if(inundItems[j].n[p] == index_array_inund)		inundItems[j].n[p] = shapeItems.size()-1;
			    else if(inundItems[j].n[p] > index_array_inund)	inundItems[j].n[p]--;
		}
	}
	else if(index_temp != -1)
	{
	    shapeItems.push_back(shapeItems[index_temp]);
	    shapeItems.remove(index_temp);
	    for(int j = 0; j < inundItems.size(); j++)
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    if(inundItems[j].n[p] == index_temp)	inundItems[j].n[p] = shapeItems.size()-1;
		    else if(inundItems[j].n[p] > index_temp)	inundItems[j].n[p]--;
	}
	shapeSave(w);
	itemInMotion = 0;
	rectItems.clear();
	index_array.clear();
	index_array_copy.clear();
	index = index_temp = -1;
	flag_ctrl = flag_A = flag_copy = false;
	w->mainWin()->actLevRise->setEnabled(false);
	w->mainWin()->actLevLower->setEnabled(false);
	paintImage(w);
	w->repaint();
    }
    else if(act->objectName() == "level_lower")
    {
	int index_array_inund;
	if(index_array.size() && !status_hold && index_array[0] != -1)
	{
	    //> Sorting the fills
	    vector<int> low_fill;
	    int fig_col;
	    for(int j = 0; j < inundItems.size(); j++)
	    {
		fig_col = 0;
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    for(int i = 0; i < index_array.size(); i++)
			if(inundItems[j].n[p] == index_array[i]) fig_col++;
		if(fig_col == inundItems[j].n.size()) low_fill.push_back(j);
	    }
	    for(int i = low_fill.size()-1, off = 0; i > -1; i--, off++)   inundItems.prepend(inundItems[low_fill[i] + off]);
	    for(unsigned i = 0, off = 0; i < low_fill.size(); i++, off++) inundItems.remove(low_fill[i] + low_fill.size() - off);

	    //> Sorting the figures
	    ShapeItem item_temp;
	    for(int i = 0; i < index_array.size(); i++)
		if(index_array[i] != -1)
		{
		    index_array_inund = index_array[i];
		    item_temp = shapeItems[index_array[i]];
		    for(int j = index_array[i]; j > 0; j--)
		    {
			shapeItems[j] = shapeItems[j-1];
			for(int k = 0; k < index_array.size(); k++)
			    if(j == index_array[k] && index_array[k] != -1)
				index_array[k] = index_array[k] + 1;
		    }
		    shapeItems[0] = item_temp;
		    for(int j = 0; j < inundItems.size(); j++)
			for(int p = 0; p < inundItems[j].n.size(); p++)
			    if(inundItems[j].n[p] == index_array_inund)		inundItems[j].n[p] = 0;
			    else if(inundItems[j].n[p] < index_array_inund)	inundItems[j].n[p]++;
		}
	}
	else if(status_hold && (index_array_copy.size() || (index_array_copy.size() == 0 && (index_array.size() && index_array[0] != -1))))
	{
	    if(index_array_copy.size() == 0 && index_array.size() && index_array[0] != -1) index_array_copy = index_array;

	    //> Sorting the fills
	    vector<int> low_fill;
	    int fig_col;
	    for(int j = 0; j < inundItems.size(); j++)
	    {
		fig_col = 0;
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    for(int i = 0; i < index_array_copy.size(); i++)
			if(inundItems[j].n[p] == index_array_copy[i]) fig_col++;
		if(fig_col == inundItems[j].n.size()) low_fill.push_back(j);
	    }
	    for(int i = low_fill.size()-1, off = 0; i > -1; i--, off++)   inundItems.prepend(inundItems[low_fill[i] + off]);
	    for(unsigned i = 0, off = 0; i < low_fill.size(); i++, off++) inundItems.remove(low_fill[i] + low_fill.size() - off);

	    //> Sorting the figures
	    ShapeItem item_temp;
	    for(int i = 0; i < index_array_copy.size(); i++)
		if(index_array_copy[i] != -1)
		{
		    index_array_inund = index_array_copy[i];
		    item_temp = shapeItems[index_array_copy[i]];
		    for(int j = index_array_copy[i]; j > 0; j--)
		    {
			shapeItems[j] = shapeItems[j-1];
			for(int k = 0; k < index_array_copy.size(); k++)
			    if(j == index_array_copy[k] && index_array_copy[k] != -1)
				index_array_copy[k] = index_array_copy[k] + 1;
		    }
		    shapeItems[0] = item_temp;
		    for(int j = 0; j < inundItems.size(); j++)
			for(int p = 0; p < inundItems[j].n.size(); p++)
			    if(inundItems[j].n[p] == index_array_inund)		inundItems[j].n[p] = 0;
			    else if(inundItems[j].n[p] < index_array_inund)	inundItems[j].n[p]++;
		}
	}
	else if(index_temp != -1)
	{
	    shapeItems.push_front(shapeItems[index_temp]);
	    shapeItems.remove(index_temp + 1);
	    for(int j = 0; j < inundItems.size(); j++)
		for(int p = 0; p < inundItems[j].n.size(); p++)
		    if(inundItems[j].n[p] == index_temp)	inundItems[j].n[p] = 0;
		    else if(inundItems[j].n[p] < index_temp)	inundItems[j].n[p]++;
	}
	shapeSave(w);
	itemInMotion = 0;
	rectItems.clear();
	index_array.clear();
	index_array_copy.clear();
	index = index_temp = -1;
	flag_ctrl = flag_A = flag_copy = false;
	w->mainWin()->actLevRise->setEnabled(false);
	w->mainWin()->actLevLower->setEnabled(false);
	paintImage(w);
	w->repaint();
    }
    if(fl_figure)
    {
	act->setChecked(true);
	if(flag_A)
	{
	    flag_ctrl = flag_A = flag_copy = false;
	    flag_check_pnt_inund = false;
	    index_array.clear();
	    rectItems.clear();
	    itemInMotion = 0;
	    count_Shapes = 0;
	}
	else
	{
	    rectItems.clear();
	    itemInMotion = 0;
	}
	status = true;
	paintImage(w);
	w->repaint();
    }
}

bool ShapeElFigure::event( WdgView *w, QEvent *event )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView	*runW = qobject_cast<RunWdgView*>(w);
    PntMap	&pnts = elFD->shapePnts;
    WidthMap	&widths = elFD->shapeWidths;
    double	scaleW = vmin(w->xScale(true), w->yScale(true));

    switch(event->type())
    {
	case QEvent::Paint:
	{
	    QPainter pnt_v(w);
	    pnt_v.drawPixmap(QPoint(), (dashedRect.isValid() && devW && devW->edit()) ? rect_img : elFD->pictObj);
	    pnt_v.end();
	    return true;
	}
	case QEvent::MouseButtonPress:
	{
	    QMouseEvent *ev = static_cast<QMouseEvent*>(event);
	    if(runW && elFD->active && runW->permCntr())
	    {
		string sev;
		for(int i = 0; i < inundItems.size(); i++)
		    if(inundItems[i].path.contains(w->mapFromGlobal(w->cursor().pos())))
			sev = "ws_Fig"+i2s(i);
		if(!sev.empty())
		{
		    AttrValS attrs;
		    if(!runW->hasFocus())		runW->setFocus(Qt::MouseFocusReason);
		    if(ev->buttons()&Qt::LeftButton)	attrs.push_back(std::make_pair("event",sev+"Left\nws_FigLeft"));
		    if(ev->buttons()&Qt::RightButton)	attrs.push_back(std::make_pair("event",sev+"Right\nws_FigRight"));
		    if(ev->buttons()&Qt::MidButton)	attrs.push_back(std::make_pair("event",sev+"Midle\nws_FigMiddle"));
		    w->attrsSet(attrs);
		    return false;
		}
	    }
	    else if(devW)
	    {
		mousePress_pos = ev->pos();
		if(flag_down || flag_up || flag_left || flag_right) break;
		if(ev->button() == Qt::LeftButton && !status)
		{
		    //> initialization for holds
		    current_ss = current_se = current_es = current_ee = -1;
		    //> getting the index of the figure
		    if((index=itemAt(ev->pos(),w)) == -1)
		    {
			devW->mainWin()->actVisItCopy->setEnabled(false);
			devW->mainWin()->actLevRise->setEnabled(false);
			devW->mainWin()->actLevLower->setEnabled(false);
		    }
		    itemInMotion = NULL;
		    index_temp = index_del = index;
		    previousPosition_all = previousPosition = ev->pos();
		    count_holds = 0;
		    //> getting figures or rect number for moveItemTo
		    if(index != -1)
		    {
			devW->mainWin()->actVisItCopy->setEnabled(true);
			devW->mainWin()->actVisItPaste->setEnabled(false);
			devW->mainWin()->actLevRise->setEnabled(true);
			devW->mainWin()->actLevLower->setEnabled(true);
			if(!flag_copy)
			{
			    itemInMotion = &shapeItems[index];
			    //>> Check for append to selection list by Ctrl hold and Ctrl+A
			    if((flag_ctrl && !status_hold && index_array.size()) || (flag_ctrl && flag_A))
			    {
				bool fn = false;
				for(int i = 0; !fn && i < count_Shapes; i++)
				    if(index_array[i] == index) fn = true;
				if(!fn)
				{
				    index_array[count_Shapes] = index;
				    count_Shapes += 1;
				    itemInMotion = &shapeItems[index];
				    flag_ctrl_move = true;
				    offset = QPointF();
				    moveItemTo(previousPosition, w);
				}
				flag_check_pnt_inund = true;
				paintImage(w);
				w->repaint();
			    }
			}
			else
			{
			    flag_check_pnt_inund = true;
			    if(index_array.size() && index_array[0] != -1)
				itemInMotion = &shapeItems[index_array[0]];
			}
			if(status_hold && !flag_A)
			{
			    holds(w);
			    if(count_holds)
			    {
				if(rect_num != -1)
				{
				    int rect_num_temp = rect_num;
				    rect_num = realRectNum(rect_num, w);
				    itemInMotion = &shapeItems[index];
				    if((rect_num == 2 || rect_num == 3) && shapeItems[index].type == ShT_Bezier) flag_rect = false;
				    if(rect_num == 0 || rect_num == 1) rectNum0_1(rect_num_temp, w);
				    if((rect_num == 3 || rect_num == 4) && shapeItems[index].type == ShT_Arc)
				    {
					Prev_pos_1 = scaleRotate(pnts[shapeItems[index].n1], w);
					Prev_pos_2 = scaleRotate(pnts[shapeItems[index].n2], w);
					rectNum3_4(w);
				    }
				}
				else
				{
				    count_moveItemTo = 0;
				    num_vector.clear();
				    offset = QPointF();
				    for(int i = 0; i <= count_holds; i++)
					if(index_array[i] != -1)
					{
					    count_moveItemTo += 1;
					    flag_ctrl_move = false;
					    flag_ctrl = true;
					    itemInMotion = &shapeItems[index_array[i]];
					    index = index_array[i];
					    moveItemTo(ev->pos(), w);
					}
				    paintImage(w);
				    w->repaint();
				}
			    }
			}
			//> Appending points for the figure if they were conected to the other figure
			if(count_holds == 0 && !flag_ctrl && !status_hold) flag_check_point = true;
		    }
		    else
		    {
			if(flag_A)
			{
			    flag_ctrl = flag_A = flag_copy = false;
			    flag_check_pnt_inund = false;
			    index_array.clear();
			    itemInMotion = 0;
			    count_Shapes = 0;
			}
			if(!(QApplication::keyboardModifiers()&Qt::ControlModifier) && rectItems.size())
			{
			    rectItems.clear();
			    paintImage(w);
			    w->repaint();
			}
		    }
		}
		if(ev->button() == Qt::RightButton && !status)
		{
		    pop_pos = ev->pos();
		    index = itemAt(ev->pos(), w);
		    count_holds = 0;
		    if(status_hold && !flag_A && index != -1)
		    {
			holds(w);
			if(count_holds && rect_num != -1)
			{
			    int rect_num_temp = rect_num;
			    rect_num = realRectNum(rect_num, w);
			    itemInMotion = &shapeItems[index];
			    if((rect_num == 2 || rect_num == 3) && shapeItems[index].type == ShT_Bezier)	flag_rect=false;
			    if(rect_num == 0 || rect_num == 1) rectNum0_1(rect_num_temp, w);
			    if((rect_num == 3 || rect_num == 4) && shapeItems[index].type == ShT_Arc)
			    {
				Prev_pos_1 = scaleRotate(pnts[shapeItems[index].n1], w);
				Prev_pos_2 = scaleRotate(pnts[shapeItems[index].n2], w);
				rectNum3_4(w);
			    }
			}
		    }
		    rect_dyn = rect_num;
		}
		//> getting start point for drawing and draw the figure
		if((ev->button() == Qt::LeftButton) && status)
		{
		    double ang;
		    QPainterPath circlePath, bigPath;
		    switch(shapeType)
		    {
			case ShT_Line:
			{
			    StartLine = EndLine = ev->pos();
			    ang = 360 - angle(QLineF(StartLine,EndLine), QLineF(StartLine,QPointF(StartLine.x()+10,StartLine.y())));
			    if(widths[SpI_DefBord] > 0)
			    {
				circlePath = painterPath(widths[SpI_DefLine]*scaleW, widths[SpI_DefBord], 1, ang, StartLine, EndLine);
				shapeItems.push_back(ShapeItem(circlePath,newPath,-1,-1,-1,-1,-1,QPointF(),-5,-6,-5,-5,-6,1,angle_temp));
			    }
			    else if(fabs(widths[SpI_DefBord]) < 0.01)
			    {
				circlePath = painterPathSimple(1, ang, StartLine, EndLine);
				QPainterPath bigPath = painterPath(vmax(1,widths[SpI_DefLine])+1, widths[SpI_DefBord], 1, ang, StartLine, EndLine);
				shapeItems.push_back(ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(),-5,-6,-5,-5,-6,1,angle_temp));
			    }
			    StartLine = unScaleRotate(StartLine, w);
			    EndLine = unScaleRotate(EndLine, w);
			    shapeItems[shapeItems.size()-1].n1 = elFD->appendPoint(StartLine, true);
			    shapeItems[shapeItems.size()-1].n2 = elFD->appendPoint(EndLine, true);
			    rect_num = 1;
			    index = shapeItems.size()-1;
			    itemInMotion = &shapeItems[index];
			    fl_status_move = true;
			    previousPosition_all = ev->pos();
			    break;
			}
			case ShT_Arc:
			{
			    QPointF CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4;
			    double a, b;
			    StartLine = EndLine = ev->pos();
			    CtrlPos_1 = QPointF( StartLine.x()+(EndLine.x()-StartLine.x())/2, StartLine.y()+(EndLine.y()-StartLine.y())/2 );
			    a = length(EndLine,StartLine)/2;
			    b = a+50;
			    ang = angle(QLineF(CtrlPos_1,StartLine), QLineF(CtrlPos_1,QPointF(CtrlPos_1.x()+10, CtrlPos_1.y())));
			    CtrlPos_2 = QPointF( CtrlPos_1.x()+rotate(arc(0.25,a,b),ang).x(), CtrlPos_1.y()-rotate(arc(0.25,a,b),ang).y() );
			    CtrlPos_3 = StartLine;
			    CtrlPos_4 = QPointF(0,0.5);

			    if(widths[-6] > 0)
			    {
				circlePath = painterPath(widths[-5]*scaleW, widths[-6], 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2,  CtrlPos_3, CtrlPos_4);
				shapeItems.push_back(ShapeItem(circlePath,newPath,-1,-1,-1,-1,-1,CtrlPos_4,-5,-6,-5,-5,-6,2,ang));
			    }
			    else if(fabs(widths[-6]-0) < 0.01)
			    {
				QPainterPath bigPath = painterPath(vmax(1,widths[-5])+1, widths[-6], 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4);
				circlePath = painterPathSimple(2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4);
				shapeItems.push_back(ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,CtrlPos_4,-5,-6,-5,-5,-6,2,ang));
			    }
			    StartLine = unScaleRotate(StartLine, w);
			    EndLine = unScaleRotate(EndLine, w);
			    CtrlPos_1 = unScaleRotate(CtrlPos_1, w);
			    CtrlPos_2 = unScaleRotate(CtrlPos_2, w);
			    CtrlPos_3 = unScaleRotate(CtrlPos_3, w);
			    shapeItems[shapeItems.size()-1].n1 = elFD->appendPoint(StartLine, true);
			    shapeItems[shapeItems.size()-1].n2 = elFD->appendPoint(EndLine, true);
			    shapeItems[shapeItems.size()-1].n3 = elFD->appendPoint(CtrlPos_1, true);
			    shapeItems[shapeItems.size()-1].n4 = elFD->appendPoint(CtrlPos_2, true);
			    shapeItems[shapeItems.size()-1].n5 = elFD->appendPoint(CtrlPos_3, true);
			    rect_num = 4;
			    index = shapeItems.size()-1;
			    itemInMotion = &shapeItems[index];
			    fl_status_move = true;
			    previousPosition_all = ev->pos();
			    break;
			}
			case ShT_Bezier:
			{
			    QPointF CtrlPos_1, CtrlPos_2, EndLine_temp;
			    StartLine = EndLine = ev->pos();
			    CtrlPos_1 = CtrlPos_2 = QPointF( 0, 0 );
			    ang = angle(QLineF(StartLine,EndLine), QLineF(StartLine,QPointF(StartLine.x()+10,StartLine.y())));
			    CtrlPos_1 = QPointF( StartLine.x()+rotate(CtrlPos_1,ang).x(), StartLine.y()-rotate(CtrlPos_1,ang).y() );
			    CtrlPos_2 = QPointF( StartLine.x()+rotate(CtrlPos_2,ang).x(), StartLine.y()-rotate(CtrlPos_2,ang).y() );
			    if(widths[-6] > 0)
			    {
				circlePath = painterPath(widths[-5]*scaleW, widths[-6], 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2);
				shapeItems.push_back(ShapeItem(circlePath,newPath,-1,-1,-1,-1,-1,QPointF(),-5,-6,-5,-5,-6,3,angle_temp));
			    }
			    else if(fabs(widths[-6]-0) < 0.01)
			    {
				bigPath = painterPath(vmax(1,widths[-5])+1, widths[-6], 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2);
				circlePath = painterPathSimple(3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2);
				shapeItems.push_back(ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(),-5,-6,-5,-5,-6,3,angle_temp));
			    }
			    StartLine = unScaleRotate(StartLine, w);
			    EndLine = unScaleRotate(EndLine, w);
			    CtrlPos_1 = unScaleRotate(CtrlPos_1, w);
			    CtrlPos_2 = unScaleRotate(CtrlPos_2, w);

			    shapeItems[shapeItems.size()-1].n1 = elFD->appendPoint(StartLine, true);
			    shapeItems[shapeItems.size()-1].n2 = elFD->appendPoint(EndLine, true);
			    shapeItems[shapeItems.size()-1].n3 = elFD->appendPoint(CtrlPos_1, true);
			    shapeItems[shapeItems.size()-1].n4 = elFD->appendPoint(CtrlPos_2, true);
			    rect_num = 1;
			    index = shapeItems.size()-1;
			    itemInMotion = &shapeItems[index];
			    fl_status_move = true;
			    previousPosition_all = ev->pos();
			    break;
			}
		    }
		}
		//> repainting figures by mouse click
		if( (ev->button() == Qt::LeftButton) && (itemInMotion || rect_num != -1) && !status && !flag_ctrl && count_holds == 0 )
		{
		    count_Shapes = 1;
		    count_moveItemTo = 1;
		    offset = QPointF();
		    moveItemTo(ev->pos(), w);
		    paintImage(w);

		    w->repaint();
		}
		return true;
	    }
	    break;
	}
	case QEvent::MouseButtonDblClick:
	{
	    bool fl_brk;
	    QMouseEvent *ev = static_cast<QMouseEvent*>(event);

	    if(runW && elFD->active && runW->permCntr())
	    {
		string sev;
		for(int i = 0; i < inundItems.size(); i++)
		    if(inundItems[i].path.contains(ev->pos()))
			sev = "ws_Fig"+i2s(i);
		if(!sev.empty())
		{
		    if(!runW->hasFocus()) runW->setFocus(Qt::MouseFocusReason);
		    AttrValS attrs;
		    attrs.push_back(std::make_pair("event",sev+"DblClick\nws_FigDblClick"));
		    w->attrsSet(attrs);
		    return false;
		}
	    }
	    else if( devW )
	    {
		bool fl_shapeSave = false;
		if( !flag_down && !flag_up && !flag_left && !flag_right )
		{
		    index = itemAt(ev->pos(), w);
		    //> getting fill by double click
		    if( ev->button() == Qt::LeftButton && shapeItems.size() && index == -1 )
		    {
			flag_angle_temp = true;
			QBrush fill_img_brush;
			if(!inundation1_2(ev->pos(),-1,w))
			{
			    if( buildMatrix(w) != 2 )
			    {
				fl_brk = false;
				inundation(ev->pos(), buildMatrix(w), w);
				if( inundationPath != newPath )
				{
				    //for( int i=0; i < inundItems.size(); i++ )
				    for( int i = inundItems.size()-1; i >= 0; i-- )
				    {
					if( inundItems[i].path.contains(ev->pos()) )
					{
					    inundItems.remove(i);
					    fl_brk = true;
					    fl_shapeSave = true;
					    paintImage(w);
					    w->repaint();
					    break;
					}
				    }
				    if( !fl_brk && status_hold )
				    {
					inundItems.push_back(inundationItem(inundationPath,inundation_vector,SpI_DefFill,SpI_DefFillImg));
					fl_shapeSave = true;
					paintImage(w);
					w->repaint();
				    }
				}
			    }
			    inundation_vector.clear();
			    vect.clear();
			    map_matrix.clear();
			}
			else
			{
			    fl_shapeSave = true;
			    paintImage(w);
			    w->repaint();
			}
			flag_angle_temp = false;
		    }
		}
		if ( fl_shapeSave ) shapeSave(w);
		return true;
	    }
    	    break;
	}
	case QEvent::MouseButtonRelease:
	{
	    QMouseEvent *ev = static_cast<QMouseEvent*>(event);
	    if( devW )
	    {
		bool fl_dashedRect = false;
		//fMoveHoldMove = false;
		if( !flag_down && !flag_up && !flag_left && !flag_right )
		{
		    if( flag_move && ev->button() == Qt::LeftButton && !(QApplication::keyboardModifiers()&Qt::ControlModifier) )
		    {
			flag_move = false;
			dashedRectPath = newPath;
			dashedRectPath.addRect( dashedRect );
			index_array.clear();
			for( int i = 0; i < shapeItems.size(); i++ )
			    if( dashedRectPath.contains( shapeItems[i].path ) )
				index_array.push_back( i );
			dashedRect = QRect(QPoint(0,0), QPoint(0,0));
			dashedRect.setLeft(1);
			dashedRect.setRight(0);
			dashedRect.setTop(1);
			dashedRect.setBottom(0);
			if( index_array.size() )
			{
			    flag_copy = flag_A = true;
			    index_array_copy.clear();
			    flag_ctrl_move = true;
			    count_Shapes = index_array.size();
			    offset = QPointF();
			    moveAll(QPointF(), w);
			    paintImage(w);
			    flag_check_pnt_inund = true;
			}
			((VisDevelop *)w->mainWin())->actLevRise->setEnabled(true);
			((VisDevelop *)w->mainWin())->actLevLower->setEnabled(true);
			((VisDevelop *)w->mainWin())->actVisItCopy->setEnabled(true);
			fl_dashedRect = true;
			w->repaint();
		    }
		    if(index >= 0 && index < shapeItems.size() && ev->button() == Qt::LeftButton && itemInMotion)
		    {
			bool paint_im = false;
			flag_inund_break = false;
			itemInMotion = &shapeItems[index];
			index_temp = index;
			QVector<int> init_array;
			//> connecting the figures

			if( status_hold )
			{
			    if( current_ss != -1 )
			    {
				if( itemInMotion->type == 2 )
				{
				    for( int i=0; i < shapeItems.size(); i++ )
				    {
					if( shapeItems[current_ss].n1==shapeItems[i].n1 && i!=index )
					{
					    shapeItems[i].n1 = shapeItems[index].n1;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
					if( shapeItems[current_ss].n1==shapeItems[i].n2 && i!=index )
					{
					    shapeItems[i].n2 = shapeItems[index].n1;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
				    }
				}
				else if( !(itemInMotion->type==3 && shapeItems[current_ss].n1==itemInMotion->n2) )
				    {
					elFD->dropPoint(itemInMotion->n1, index);
					itemInMotion->n1 = shapeItems[current_ss].n1;
					if( !init_array.contains(index) ) init_array.push_back(index);
				    }
			    }
			    if( current_se != -1 )
			    {
				if( itemInMotion->type == 2 )
				{
				    for( int i=0; i < shapeItems.size(); i++ )
				    {
					if( shapeItems[current_se].n1==shapeItems[i].n1 && i!=index )
					{
					    shapeItems[i].n1 = shapeItems[index].n2;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
					if( shapeItems[current_se].n1==shapeItems[i].n2 && i!=index )
					{
					    shapeItems[i].n2 = shapeItems[index].n2;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
				    }
				}
				else if( !(itemInMotion->type==3 && shapeItems[current_se].n1==itemInMotion->n1) )
				{
				    elFD->dropPoint(itemInMotion->n2, index);
				    itemInMotion->n2 = shapeItems[current_se].n1;
				    if( !init_array.contains(index) ) init_array.push_back(index);
				}
			    }
			    if( current_es != -1 )
			    {
				if( itemInMotion->type == 2 )
				{
				    for( int i=0; i < shapeItems.size(); i++ )
				    {
					if( shapeItems[current_es].n2==shapeItems[i].n1 && i!=index )
					{
					    shapeItems[i].n1 = shapeItems[index].n1;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
					if( shapeItems[current_es].n2==shapeItems[i].n2 && i!=index )
					{
					    shapeItems[i].n2 = shapeItems[index].n1;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
				    }
				}
				else if( !(itemInMotion->type==3 && shapeItems[current_es].n2==itemInMotion->n2) )
				{
				    elFD->dropPoint(itemInMotion->n1, index);
				    itemInMotion->n1 = shapeItems[current_es].n2;
				    if( !init_array.contains(index) ) init_array.push_back(index);
				}
			    }
			    if( current_ee != -1 )
			    {
				if( itemInMotion->type == 2 )
				{
				    for(int i=0; i < shapeItems.size(); i++ )
				    {
					if( shapeItems[current_ee].n2==shapeItems[i].n1 && i!=index )
					{
					    shapeItems[i].n1 = shapeItems[index].n2;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
					if( shapeItems[current_ee].n2==shapeItems[i].n2 && i!=index )
					{
					    shapeItems[i].n2 = shapeItems[index].n2;
					    if( !init_array.contains(i) ) init_array.push_back(i);
					}
				    }
				}
				else if( !(itemInMotion->type==3 && shapeItems[current_ee].n2==itemInMotion->n1) )
				{
				    elFD->dropPoint(itemInMotion->n2, index);
				    itemInMotion->n2 = shapeItems[current_ee].n2;
				    if( !init_array.contains(index) ) init_array.push_back(index);
				}
			    }
			    if( current_ss != -1 || current_se != -1 || current_es != -1 || current_ee != -1 )
			    {
				QVector<int> inund_Rebuild;
				//>> Detecting if there is a necessity to rebuild the fill's path and if it is so push_back the fill to the array
				for( int i = 0; i < inundItems.size(); i++ )
				    for( int p = 0; p < inundItems[i].n.size(); p++ )
					for( int z = 0; z < init_array.size(); z++ )
					    if( (inundItems[i].n[p] == init_array[z]) && !inund_Rebuild.contains(i) )
						inund_Rebuild.push_back(i);
				fl_orto_disable = true;
				initShapeItems( ev->pos(), init_array, w );
				fl_orto_disable = false;
				for( int i = 0; i < inund_Rebuild.size(); i++ )
				    inundItems[inund_Rebuild[i]].path = createInundationPath(inundItems[inund_Rebuild[i]].n, pnts, w);
				index = index_temp; itemInMotion = &shapeItems[index];
				rectItems.clear(); init_array.clear();
			    }
			}
			if( itemInMotion->type != 2 && (!flag_ctrl && status_hold)  )//> if simple figure and status_hold
			{
			    count_Shapes = 1;
			    count_moveItemTo = 1;
			    offset = QPointF();
			    moveItemTo(ev->pos(), w);
			    //w->repaint();
			}
			bool ell_present = false; //> check for an allipse presence -
			if( ellipse_draw_startPath != newPath )
			{
			    ellipse_draw_startPath = newPath;
			    ell_present = true;
			}
			if( ellipse_draw_endPath != newPath )
			{
			    ellipse_draw_endPath = newPath;
			    ell_present = true;

			}
			//> calling moveItemTo for figures, connected with the arc, if there was moving 3 or 4 rects of the arc -
			if( count_holds && (flag_arc_rect_3_4 || (flag_rect && shapeItems[index_array[0]].type==2)))
			{
			    count_moveItemTo = 0;
			    flag_ctrl_move = false;
			    flag_ctrl = true;
			    flag_hold_arc = true;
			    offset = QPointF();
			    moveAll(ev->pos(), w);
			}
			//> Finish of the drawing the figure
			if(status && !ell_present)
			{
			    EndLine = unScaleRotate(ev->pos(), w);
			    //>> Remove the figure if its Start point is equal to its End point
			    if((EndLine == StartLine) && shapeItems.size() > 0 && itemInMotion)
			    {
				elFD->dropPoint(shapeItems[shapeItems.size()-1].n1, shapeItems.size()-1);
				elFD->dropPoint(shapeItems[shapeItems.size()-1].n2, shapeItems.size()-1);
				elFD->dropPoint(shapeItems[shapeItems.size()-1].n3, shapeItems.size()-1);
				elFD->dropPoint(shapeItems[shapeItems.size()-1].n4, shapeItems.size()-1);
				elFD->dropPoint(shapeItems[shapeItems.size()-1].n5, shapeItems.size()-1);
				shapeItems.remove(shapeItems.size()-1);
			    }
			    fl_status_move = false;
			    rectItems.clear();
			    paint_im = true;
			    index = -1;
			}
			bool flag_save = false;
			if( (mousePress_pos != ev->pos()) && !fl_dashedRect ) { flag_save = true; shapeSave(w); }
			itemInMotion = 0;
			flag_check_point = false;
			flag_m = false;
			if( (paint_im || ell_present) && flag_save ){ paintImage(w); w->repaint(); }
			if( status_hold && !flag_A )
			{
			    count_moveItemTo = 0;
			    flag_ctrl = flag_hold_move = false;
			    count_Shapes = 0;
			    count_holds = 0;
			    if( index_array.size() )
			    {
				index_array_copy = index_array;
				index_array.clear();
			    }
			    if( count_rects && !flag_arc_rect_3_4 && flag_rect && rect_array.size() ) rect_array.clear();
			    if( count_rects && flag_arc_rect_3_4 && arc_rect_array.size() && fig_rect_array.size() )
			    {
				arc_rect_array.clear();
				fig_rect_array.clear();
			    }
			    flag_rect = false;
			    count_rects = 0;
			    flag_hold_arc = false;
			    rect_num_arc = -1;
			    flag_arc_rect_3_4 = false;
			}
		    }
		    if( ev->button()==Qt::RightButton && !status )
		    {
			if( status_hold && !flag_A )
			{
			    count_moveItemTo = 0;
			    flag_ctrl = flag_hold_move = false;
			    count_Shapes = 0;
			    count_holds = 0;
			    if( index_array.size() )
			    {
				index_array_copy = index_array;
				index_array.clear();
			    }
			    if( count_rects && !flag_arc_rect_3_4 && flag_rect && rect_array.size() ) rect_array.clear();
			    if( count_rects && flag_arc_rect_3_4 && arc_rect_array.size() && fig_rect_array.size() )
			    {
				arc_rect_array.clear();
				fig_rect_array.clear();
			    }
			    flag_rect = false;
			    count_rects = 0;
			    flag_hold_arc = false;
			    rect_num_arc = -1;
			    flag_arc_rect_3_4 = false;
			}
		    }
		}
		return true;
	    }
	    break;
	}
	case QEvent::MouseMove:
	{
	    int fl;
	    int temp = -1;
	    bool flag_break_move,
		 flag_arc_inund = false;
	    QMouseEvent *ev = static_cast<QMouseEvent*>(event);
	    if(devW)
	    {
		Mouse_pos = ev->pos();

		/*if(!fMoveHoldMove && ev->buttons()&Qt::LeftButton &&
			(Mouse_pos-mousePress_pos).manhattanLength() >= QApplication::startDragDistance())
		    fMoveHoldMove = true;*/

		//> Move item, often second point, at append
		if(/*(fMoveHoldMove || status) &&*/ (ev->buttons()&Qt::LeftButton) && itemInMotion && (!status || fl_status_move))
		{
		    flag_m = true;
		    if(count_holds)
		    {
			count_Shapes = count_holds + 1;
			flag_hold_move = flag_ctrl = true;
		    }
		    //> calling moveItemTo for all connected figures
		    if(flag_ctrl && count_Shapes && ((rect_num == -1 && rect_num_arc == -1) || flag_rect || flag_arc_rect_3_4))
		    {
			if(flag_check_pnt_inund)
			{
			    checkPoint_checkInundation(w);
			    flag_check_pnt_inund = false;
			}
			offset = ev->pos()-previousPosition_all;
			count_moveItemTo = 0;
			if(flag_rect || flag_arc_rect_3_4)	count_Shapes = count_rects;
			flag_hold_arc = (index_array.size() && index_array[0] >= 0 && index_array[0] < shapeItems.size() && shapeItems[index_array[0]].type == ShT_Arc);
			moveAll(ev->pos(), w);
			paintImage(w);
			w->repaint();
		    }
		    //> moving the rect, which belongs only for one figure
		    else
		    {
			if(count_holds)
			{
			    offset = ev->pos()-previousPosition_all;
			    flag_ctrl = true;
			    flag_rect = false;
			    count_moveItemTo = 1;
			    flag_ctrl_move = false;
			    count_Shapes = 1;
			    itemInMotion = &shapeItems[index];
			    if(rect_num == 2 && itemInMotion->type == 2) offset = QPointF();
			    if(itemInMotion->type == ShT_Arc && rect_num != 2) rect_num = rect_num_arc;
			    num_vector.clear();
			    moveItemTo(ev->pos(), w);
			    for(int i = 0; i < inundItems.size(); i++)
				for(int j = 0; j < inundItems[i].n.size(); j++)
				{
				    flag_break_move = false;
				    if(inundItems[i].n[j] == index)
				    {
					inundationPath = createInundationPath(inundItems[i].n, pnts, w);
					inundItems[i].path = inundationPath;
					flag_break_move = true;
					break;
				    }
				    if(flag_break_move) break;
				}
			    paintImage(w);
			    w->repaint();
			}
			else if(!flag_ctrl && index >= 0 && index < shapeItems.size())
			{
			    count_Shapes = 1;
			    count_moveItemTo = 1;
			    offset = ev->pos()-previousPosition_all;
			    ShapeItem &newIt = shapeItems[index];
			    itemInMotion = &shapeItems[index];

			    //> Circle contour gap
			    if(newIt.type == ShT_Arc && !vrng(newIt.n2,-9,0) && newIt.n2 == newIt.n1)
				newIt.n2 = elFD->appendPoint(pnts[newIt.n2], (newIt.n2<=-10));

			    if(flag_check_point || !flag_first_move)
			    {
				for(int i = 0; i < shapeItems.size(); i++)
				{
				    if(i == index) continue;
				    ShapeItem &curIt = shapeItems[i];
				    if(newIt.type == ShT_Arc && !vrng(newIt.n5,-9,0) &&
					    (newIt.n5 == curIt.n1 || newIt.n5 == curIt.n2 || newIt.n5 == curIt.n3 || newIt.n5 == curIt.n4 || newIt.n5 == curIt.n5))
					newIt.n5 = elFD->appendPoint(pnts[newIt.n5], (newIt.n5<=-10));
				    if(newIt.type == ShT_Arc || newIt.type == ShT_Bezier)
				    {
					if(!vrng(newIt.n4,-9,0) && (newIt.n4 == curIt.n1 || newIt.n4 == curIt.n2 || newIt.n4 == curIt.n3 ||
						newIt.n4 == curIt.n4 || newIt.n4 == curIt.n5))
					    newIt.n4 = elFD->appendPoint(pnts[newIt.n4], (newIt.n4<=-10));
					if(!vrng(newIt.n3,-9,0) && (newIt.n3 == curIt.n1 || newIt.n3 == curIt.n2 || newIt.n3 == curIt.n3 ||
						newIt.n3 == curIt.n4 || newIt.n3 == curIt.n5))
					    newIt.n3 = elFD->appendPoint(pnts[newIt.n3], (newIt.n3<=-10));
				    }
				    if(!vrng(newIt.n1,-9,0) && (newIt.n1 == curIt.n1 || newIt.n1 == curIt.n2 || newIt.n1 == curIt.n3 ||
					    newIt.n1 == curIt.n4 || newIt.n1 == curIt.n5))
					newIt.n1 = elFD->appendPoint(pnts[newIt.n1], (newIt.n1<=-10));
				    if(!vrng(newIt.n2,-9,0) && (newIt.n2 == curIt.n1 || newIt.n2 == curIt.n2 || newIt.n2 == curIt.n3 ||
					    newIt.n2 == curIt.n4 || newIt.n2 == curIt.n5))
					newIt.n2 = elFD->appendPoint(pnts[newIt.n2], (newIt.n2 <=-10));
				}
				flag_check_point = false;
			    }

			    moveItemTo(ev->pos(), w);
			    //> deleting fill's
			    if(inundItems.size() && newIt.type == ShT_Arc)
				for(int i = 0; i < inundItems.size(); i++)
				{
				    if(inundItems[i].n.size() == 1 && inundItems[i].n[0] == index)
				    {
					if( rect_num == 0 || rect_num == 1 )
					{
					    inundItems.remove(i);
					    shapeSave(w);
					    flag_arc_inund = true;
					}
					else
					{
					    inundItems[i].path = createInundationPath( inundItems[i].n, pnts, w );
					    flag_arc_inund = true;
					}
				    }
				}
			    else if( inundItems.size() && !flag_inund_break && !flag_arc_inund )
			    {
				QVector<int> ind_array; ind_array.push_back( index );
				removeFill(ind_array, 1, w);
				flag_inund_break = true;
			    }
			    flag_arc_inund = false;
			    paintImage(w);
			    w->repaint();
			}
			if(rect_num >= 0 && rect_num < rectItems.size()) temp = realRectNum(rect_num, w);
			//- if the figure or it's rect is not connected to other one -
			if(index >= 0 && index < shapeItems.size() && status_hold && (rect_num == -1|| ((temp == 0 || temp == 1) && !flag_rect)))
			{
			    current_se = current_ss = current_ee = current_es = -1;
			    ellipse_draw_startPath = newPath;
			    ellipse_draw_endPath = newPath;
			    itemInMotion = &shapeItems[index];
			    //- drawing the ellipse for connecting points -
			    for( int i=0; i <= shapeItems.size()-1; i++ )
			    {
				ellipse_startPath = newPath;
				ellipse_endPath = newPath;
				if( i != index && !flag_A )
				{
				    ellipse_startPath.addEllipse( scaleRotate(pnts[shapeItems[i].n1], w).x()-8,
								  scaleRotate(pnts[shapeItems[i].n1], w).y()-8, 16, 16 );
				    if( ellipse_startPath.contains( scaleRotate(pnts[shapeItems[index].n1], w) ) )
				    {
					if( temp == 0 || rect_num == -1 )
					{
					    if( itemInMotion->type == 2 && shapeItems[i].type == 2 ) break;
					    ellipse_draw_startPath.addEllipse( scaleRotate(pnts[shapeItems[i].n1], w).x()-8,
									       scaleRotate(pnts[shapeItems[i].n1], w).y()-8, 16, 16 );
					    current_ss = i;
					}
				    }
				    if( ellipse_startPath.contains(scaleRotate(pnts[shapeItems[index].n2], w)) )
				    {
					if( temp == 1 || rect_num == -1 )
					{
					    if( itemInMotion->type == 2 && shapeItems[i].type == 2 ) break;
					    ellipse_draw_startPath.addEllipse( scaleRotate(pnts[shapeItems[i].n1], w).x()-8,
									       scaleRotate(pnts[shapeItems[i].n1], w).y()-8, 16, 16 );
					    current_se = i;
					}
				    }
				    ellipse_endPath.addEllipse( scaleRotate(pnts[shapeItems[i].n2], w).x()-8,
								scaleRotate(pnts[shapeItems[i].n2], w).y()-8, 16, 16 );
				    if( ellipse_endPath.contains( scaleRotate(pnts[shapeItems[index].n2], w) ) )
				    {
					if( temp == 1 || rect_num == -1 )
					{
					    if( itemInMotion->type == 2 && shapeItems[i].type == 2 ) break;
					    ellipse_draw_endPath.addEllipse( scaleRotate(pnts[shapeItems[i].n2], w).x()-8,
									     scaleRotate(pnts[shapeItems[i].n2], w).y()-8, 16, 16 );
					    current_ee = i;
					}
				    }
				    if( ellipse_endPath.contains( scaleRotate(pnts[shapeItems[index].n1], w) ) )
				    {
					if( temp == 0 || rect_num == -1 )
					{
					    if( itemInMotion->type == 2 && shapeItems[i].type == 2 ) break;
					    ellipse_draw_endPath.addEllipse( scaleRotate(pnts[shapeItems[i].n2], w).x()-8,
									     scaleRotate(pnts[shapeItems[i].n2], w).y()-8, 16, 16 );
					    current_es = i;
					}
				    }
				}
			    }
			}
		    }
		    flag_first_move = true;
		}
		//> Group selection by rect
		else if(/*fMoveHoldMove &&*/ (ev->buttons()&Qt::LeftButton) && !itemInMotion && !status && index_temp == -1 && !(QApplication::keyboardModifiers()&Qt::ControlModifier))
		{
		    if( !flag_move )
		    {
			stPointDashedRect = ev->pos();
			dashedRect = QRect( stPointDashedRect, stPointDashedRect );
			flag_move = true;
		    }
		    else dashedRect = QRect( stPointDashedRect, ev->pos() ).normalized();
		    rect_img = QPixmap(elFD->pictObj);
		    QPainter pnt_rect( &rect_img );
		    int margin = elFD->geomMargin;
		    QRect draw_area = w->rect().adjusted(0,0,-2*margin,-2*margin);
		    pnt_rect.setWindow(draw_area);
		    pnt_rect.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));
		    pnt_rect.setBrush( Qt::NoBrush );
		    pnt_rect.setPen( Qt::white );
		    pnt_rect.drawRect( dashedRect );
		    pnt_rect.setBrush( Qt::NoBrush );
		    pnt_rect.setPen( Qt::black );
		    pnt_rect.setPen( Qt::DashLine );
		    pnt_rect.drawRect( dashedRect );
		    pnt_rect.end();

		    w->repaint();
		}
		//> flag_first_move clean and all other
		else
		{
		    if(flag_down || flag_left || flag_right || flag_up) break;
		    if(flag_first_move && !flag_A)
		    {
			shapeSave(w);
			flag_inund_break = false;
			offset = QPointF();
			if( flag_ctrl && status_hold )
			{
			    count_moveItemTo = 0;
			    flag_hold_move = false;
			    if( flag_rect )
			    {
				offset = QPointF();
				rect_array.clear();
			    }
			    if( flag_arc_rect_3_4 )
			    {
				offset = QPointF();
				arc_rect_array.clear();
				fig_rect_array.clear();
			    }
			    count_Shapes = 0;
			    count_holds  = 0;
			    count_rects  = 0;
			    rect_num_arc = -1;
			    flag_arc_rect_3_4 = flag_rect = flag_ctrl = false;
			    index_array.clear();
			}
			flag_first_move = false;
		    }
		    if(!flag_m)
		    {
			fl = itemAt(ev->pos(), w);
			if(fl != -1 && rect_num == -1 && !status)	w->setCursor(Qt::SizeAllCursor);
			else if(!status && rect_num != -1)		w->setCursor(Qt::SizeHorCursor);
			else if(status)
			{
			    if(w->cursor().shape() != Qt::CrossCursor)	w->setCursor(Qt::CrossCursor);
			}
			else if(w->cursor().shape() != Qt::ArrowCursor)	w->unsetCursor();
		    }
		}
	    }
	    return true;
	}
	case QEvent::KeyPress:
	{
	    QKeyEvent *ev = static_cast<QKeyEvent*>(event);
	    if(!devW)	break;
	    if(ev->key() == Qt::Key_Shift) { fl_orto_move = true; return true; }
	    if(flag_m)  break;
	    switch(ev->key())
	    {
		case Qt::Key_Control:	//Select All
		    if(flag_A) index_array_copy_flag_A = index_array;
		    if(status_hold || flag_copy) break;
		    flag_ctrl = true;
		    index_array.clear();
		    for(int i = 0; i < shapeItems.size(); i++)
			index_array.push_back(-1);
		    count_Shapes = 0;
		    return true;
		case Qt::Key_Delete:	//Delete selected
		{
		    if(index_del == -1 && !flag_A)	break;
		    bool flag_arc_inund = false;
		    bool flag_in_break, flag_ar_break;
		    QVector<int> drop_inunds;
		    if(flag_A || (index_array_copy.size() && index_array_copy[0] != -1) || (index_array.size() && index_array[0] != -1))
		    {
			flag_ar_break = false;
			for(int i = 0; !flag_ar_break && i < index_array.size(); i++)
			    if(index_array[i] == -1) flag_ar_break = true;
			if(index_array.size() == shapeItems.size() && !flag_ar_break)
			{
			    pnts.clear();
			    inundItems.clear();
			    shapeItems.clear();
			    rectItems.clear();
			    shapeSave(w);
			    flag_copy = flag_A = flag_ctrl = false;
			    index_array.clear();
			    itemInMotion = NULL;
			    count_Shapes = 0;
			    paintImage(w);
			    w->repaint();
			}
			else
			{
			    if(index_array_copy.size())
				if(index_array_copy[0] != -1 && status_hold && index_array.size() == 0)
				    index_array = index_array_copy;//!!!!
			    for(int i = 0; i < inundItems.size(); i++)
				for(int j = 0; j < inundItems[i].n.size(); j++)
				{
				    flag_in_break = false;
				    for(int p = 0; p < index_array.size(); p++)
				    {
					if(index_array[p] != -1 && inundItems[i].n[j] == index_array[p])
					{
					    drop_inunds.push_back(i);
					    flag_in_break = true;
					    break;
					}
				    }
				    if(flag_in_break) break;
				}
			    for(int i = 0; i < drop_inunds.size(); i++)
			    {
				inundItems.remove(drop_inunds[i]);
				for(int j = i+1; j < drop_inunds.size(); j++)
				    if(drop_inunds[j] > drop_inunds[i])
					drop_inunds[j]--;
			    }
			    for(int p = 0; p < index_array.size(); p++)
			    {
				if(index_array[p] != -1)
				{
				    elFD->dropPoint(shapeItems[index_array[p]].n1, index_array[p]);
				    elFD->dropPoint(shapeItems[index_array[p]].n2, index_array[p]);
				    elFD->dropPoint(shapeItems[index_array[p]].n3, index_array[p]);
				    elFD->dropPoint(shapeItems[index_array[p]].n4, index_array[p]);
				    elFD->dropPoint(shapeItems[index_array[p]].n5, index_array[p]);
				    shapeItems.remove(index_array[p]);
				    for(int j = 0; j < inundItems.size(); j++)
					for(int k = 0; k < inundItems[j].n.size(); k++)
					    if(inundItems[j].n[k] > index_array[p])	inundItems[j].n[k]--;
				    for(int i = p+1; i < index_array.size(); i++)
					if(index_array[i] > index_array[p]) index_array[i]--;
				}
			    }
			    rectItems.clear();
			    shapeSave(w);
			    flag_ctrl = flag_A = flag_copy = false;
			    index_array.clear();
			    index_array_copy.clear();
			    drop_inunds.clear();
			    itemInMotion = 0;
			    count_Shapes = 0;
			    paintImage(w);
			    w->repaint();
			}
		    }
		    else
		    {
			elFD->dropPoint(shapeItems[index_del].n1, index_del);
			elFD->dropPoint(shapeItems[index_del].n2, index_del);
			elFD->dropPoint(shapeItems[index_del].n3, index_del);
			elFD->dropPoint(shapeItems[index_del].n4, index_del);
			elFD->dropPoint(shapeItems[index_del].n5, index_del);

			//> deleting fill if deleted figure was in the fill's path
			for(int i = 0; i < inundItems.size(); i++)
			    if(shapeItems[index_del].type == 2 && inundItems[i].n.size() == 1 &&
				inundItems[i].n[0] == index_del)
			    {
				inundItems.remove(i);
				flag_arc_inund = true;
			    }
			QVector<int> ind_array;
			ind_array.push_back(index_del);
			if(!flag_arc_inund) removeFill(ind_array, 1, w);
			for(int i = 0; i < inundItems.size(); i++)
			    for(int j = 0; j < inundItems[i].n.size(); j++)
				if(inundItems[i].n[j] > index_del)	inundItems[i].n[j] -= 1;
			shapeItems.remove(index_del);
			rectItems.clear();
			shapeSave(w);
			paintImage(w);
			w->repaint();
		    }
		    w->unsetCursor();
		    index_del = -1;
		    flag_A = false;
		    return true;
		}
		case Qt::Key_A:		//Select All
		{
		    if(status || !(QApplication::keyboardModifiers()&Qt::ControlModifier))	break;
		    flag_A = true;
		    flag_ctrl_move = true;
		    if(index_array.size()) index_array.clear();
		    for(int i = 0; i < shapeItems.size(); i++)
			index_array.push_back(i);
		    count_Shapes = shapeItems.size();
		    offset = QPointF();
		    moveAll(QPointF(), w);
		    if(index_array.size())
		    {
			((VisDevelop *)w->mainWin())->actVisItCopy->setEnabled(true);
			flag_check_pnt_inund = true;
		    }
		    paintImage(w);

		    w->repaint();
		    return true;
		}
		case Qt::Key_Up:
		    flag_up = true;
		    offset = QPointF(0, (QApplication::keyboardModifiers()&Qt::ShiftModifier)?-1:-5);
		case Qt::Key_Down:
		    if(!flag_up)
		    {
			flag_down = true;
			offset = QPointF(0, (QApplication::keyboardModifiers()&Qt::ShiftModifier)?1:5);
		    }
		case Qt::Key_Left:
		    if(!(flag_up || flag_down))
		    {
			flag_left = true;
			offset = QPointF((QApplication::keyboardModifiers()&Qt::ShiftModifier)?-1:-5, 0);
		    }
		case Qt::Key_Right:
		    if(!(flag_up || flag_down || flag_left))
		    {
			flag_right = true;
			offset = QPointF((QApplication::keyboardModifiers()&Qt::ShiftModifier)?1:5, 0);
		    }
		    if(!flag_A && (index_temp == -1 || index < 0 || index >= shapeItems.size()))
		    { flag_up = flag_down = flag_left = flag_right = false; break; }
		    if(w->cursor().shape() != Qt::ArrowCursor) QCursor::setPos((QCursor::pos()+offset).toPoint());
		    moveUpDown(w);
		    paintImage(w);
		    w->repaint();
		    return true;
	    }
	    break;
	}
	case QEvent::KeyRelease:
	{
	    QKeyEvent *ev = static_cast<QKeyEvent*>(event);
	    if(!devW) break;
	    switch(ev->key())
	    {
		case Qt::Key_Shift:	fl_orto_move = false; return true;
		case Qt::Key_Control:
		    if(status_hold || flag_A) break;
		    flag_ctrl = false;
		    if(count_Shapes)
		    {
			count_Shapes = 0;
			rectItems.clear();
			index_array.clear();
			flag_m = false;
			w->unsetCursor();
			paintImage(w);
			itemInMotion = 0;
			index_temp = -1;
			devW->mainWin()->actVisItCopy->setEnabled(false);
			w->repaint();
		    }
		    return true;
		case Qt::Key_Up: case Qt::Key_Down: case Qt::Key_Left: case Qt::Key_Right:
		    flag_up = flag_down = flag_left = flag_right = false;
		    itemInMotion = NULL;
		    flag_first_move = true;
		    return true;
	    }
	    break;
	}
	default: break;
    }

    return false;
}

//**********************************************************
//* Moving/forming the figure by properties;		   *
//*  count_moveItemTo - moving items count;		   *
//*  count_Shapes -
//*  flag_ctrl_move -
//*  flag_ctrl -
//*  offset - moving offset;				   *
//*  index -
//*  itemInMotion - moving items;			   *
//*  rect_num - moving item's rect number (PntNull-whole)  *
//*  t_start, t_end - stored global, need for review!      *
//**********************************************************
void ShapeElFigure::moveItemTo( const QPointF &pos, WdgView *w )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap   &pnts = elFD->shapePnts;
    WidthMap &widths = elFD->shapeWidths;
    double scaleW = vmin(w->xScale(true), w->yScale(true));

    ShapeItem temp_shape;
    double a = 0, b = 0, ang = 0, ang_t = 0;
    itemInMotion->ang_t = 0;
    QPointF StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4;
    int MotionNum_1 = itemInMotion->n1, MotionNum_2 = itemInMotion->n2,
	MotionNum_3 = itemInMotion->n3, MotionNum_4 = itemInMotion->n4, MotionNum_5 = itemInMotion->n5,
	MotionLineColor = itemInMotion->lineColor,
	MotionBorderColor = itemInMotion->borderColor,
	MotionStyle = itemInMotion->style,
	MotionWidth = itemInMotion->width,
	MotionBorderWidth = itemInMotion->borderWidth;
    shapeType = itemInMotion->type;
    QPointF EndMotionPos_temp, CtrlMotionPos_1_temp, CtrlMotionPos_2_temp;
    bool flag_MotionNum_1 = false, flag_MotionNum_2 = false,
	 flag_MotionNum_3 = false, flag_MotionNum_4 = false, flag_MotionNum_5 = false,
	 fl_orto = false;
    for(int i = 0; i < num_vector.size(); i++) {
	if(num_vector[i] == MotionNum_1) flag_MotionNum_1 = true;
	if(num_vector[i] == MotionNum_2) flag_MotionNum_2 = true;
	if(num_vector[i] == MotionNum_3) flag_MotionNum_3 = true;
	if(num_vector[i] == MotionNum_4) flag_MotionNum_4 = true;
	if(num_vector[i] == MotionNum_5) flag_MotionNum_5 = true;
    }

    //Moving the whole figure
    if(rect_num == PntNull) {
	if((status_hold && count_holds > 1) || (flag_ctrl && count_Shapes > 1)) {
	    StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w);
	    if(!flag_MotionNum_1) { StartMotionPos += offset; num_vector.append(MotionNum_1); }
	    EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
	    if(!flag_MotionNum_2) { EndMotionPos += offset; num_vector.append(MotionNum_2); }
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    if(!flag_MotionNum_3) { CtrlMotionPos_1 += offset; num_vector.append(MotionNum_3); }
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    if(!flag_MotionNum_4) { CtrlMotionPos_2 += offset; num_vector.append(MotionNum_4); }
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w);
	    if(!flag_MotionNum_5) { CtrlMotionPos_3 += offset; num_vector.append(MotionNum_5); }
	}
	else {
	    StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w) + offset;
	    EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w) + offset;
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w) + offset;
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w) + offset;
	}
	if(shapeType == ShT_Arc) {
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w) + offset;
	    CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	    a = length(CtrlMotionPos_3, CtrlMotionPos_1); b = length(CtrlMotionPos_2, CtrlMotionPos_1);
	    ang = angle(QLineF(CtrlMotionPos_1,CtrlMotionPos_3), QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10, CtrlMotionPos_1.y())));
	    if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	    angle_temp = ang;
	    t_start = CtrlMotionPos_4.x();
	    t_end = CtrlMotionPos_4.y();
	    StartMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y());
	    EndMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y());
	}
    }
    //Moving the start point of the figure
    if(rect_num == PntStart) {
	w->mainWin()->statusBar()->showMessage(QString(_("Point coordinates(x,y): (%1, %2)")).
		arg(rRnd(pnts[itemInMotion->n1].x(),POS_PREC_DIG)).arg(rRnd(pnts[itemInMotion->n1].y(),POS_PREC_DIG)), 10000);
	EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);

	if(shapeType == ShT_Arc) {
	    StartMotionPos = Mouse_pos;
	    if(flag_up || flag_down || flag_right || flag_left) StartMotionPos = pnts[itemInMotion->n1]+offset;
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w);
	    CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	    a = length(CtrlMotionPos_3, CtrlMotionPos_1); b = length(CtrlMotionPos_2, CtrlMotionPos_1);
	    t_end = CtrlMotionPos_4.y();
	    ang = angle(QLineF(CtrlMotionPos_1,CtrlMotionPos_3), QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y())));
	    if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	    angle_temp = ang;
	    StartMotionPos = unRotate(StartMotionPos, ang, CtrlMotionPos_1);
	    if(StartMotionPos.x() >= a)	StartMotionPos = QPointF(a, (StartMotionPos.y()/StartMotionPos.x())*a);
	    if(StartMotionPos.x() < -a)	StartMotionPos = QPointF(-a, (StartMotionPos.y()/StartMotionPos.x())*(-a));
	    t_start = a ? acos(StartMotionPos.x()/a)/(2*M_PI) : 0;
	    if(StartMotionPos.y() > 0)	t_start = 1 - t_start;
	    if(t_start < 0)		t_start = 1 + t_start;
	    if(t_start > t_end)		t_end += 1;
	    if((t_end-1) > t_start)	t_end -= 1;
	    if(t_start == t_end) 	t_end += 1;
	    if(t_end > t_start && t_start >= 1 && t_end > 1)	{ t_start -= 1; t_end -= 1; }
	    StartMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y());
	    EndMotionPos = QPointF(CtrlMotionPos_1.x() + rotate(arc(t_end,a,b),ang).x(), CtrlMotionPos_1.y() - rotate(arc(t_end,a,b),ang).y());
	}
	else {
	    if(!flag_hold_arc && !flag_arc_rect_3_4) {		// if the figure is not connected to the arc
		StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w);
		if(status_hold && count_holds && flag_rect) {
		    if(!flag_MotionNum_1) { StartMotionPos += offset; num_vector.append(MotionNum_1); }
		}
		else StartMotionPos += offset;
		CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    }
	    if(flag_hold_arc || flag_arc_rect_3_4) {	// if the figure is connected to the arc
		if(arc_rect == 0) {
		    StartMotionPos = scaleRotate(pnts[shapeItems[index_array[0]].n1], w);
		    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
		}
		if(arc_rect == 1) {
		    StartMotionPos = scaleRotate(pnts[shapeItems[index_array[0]].n2], w);
		    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
		}
	    }
	}
    }
    //Moving the end point of the figure
    if(rect_num == PntEnd) {
	w->mainWin()->statusBar()->showMessage(QString(_("Point coordinates(x,y): (%1, %2)")).
		arg(rRnd(pnts[itemInMotion->n2].x(),POS_PREC_DIG)).arg(rRnd(pnts[itemInMotion->n2].y(),POS_PREC_DIG)), 10000);
	StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w);

	if(shapeType == ShT_Arc) {
	    EndMotionPos = Mouse_pos;
	    if(flag_up || flag_down || flag_right || flag_left) {
		EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
		EndMotionPos += offset;
	    }
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w);
	    CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	    b = length(CtrlMotionPos_2, CtrlMotionPos_1); a = length(CtrlMotionPos_3, CtrlMotionPos_1);
	    ang = angle(QLineF(CtrlMotionPos_1,CtrlMotionPos_3), QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y())));
	    if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	    angle_temp = ang;
	    EndMotionPos = unRotate(EndMotionPos, ang, CtrlMotionPos_1);
	    if(EndMotionPos.x() <= -a)	{ EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(-a));	EndMotionPos.setX(-a); }
	    if(EndMotionPos.x() > a)	{ EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(a));	EndMotionPos.setX(a); }
	    t_end = a ? acos(EndMotionPos.x()/a)/(2*M_PI) : 0;
	    if(EndMotionPos.y() > 0)	t_end = 1 - t_end;
	    if(t_start > t_end)		t_end += 1;
	    if((t_end-1) > t_start)	t_end -= 1;
	    if(t_start == t_end)	t_end += 1;
	    if((t_end > t_start) && t_start >= 1 && t_end > 1) { t_start -= 1; t_end -= 1; }
	    EndMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y());
	}
	else {
	    if(!flag_hold_arc && !flag_arc_rect_3_4) {
		EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
		if(status_hold && count_holds && flag_rect) {
		    if(!flag_MotionNum_2) { EndMotionPos += offset; num_vector.append(MotionNum_2); }
		}
		else {
		    EndMotionPos += offset;
		    if((QApplication::keyboardModifiers()&Qt::ShiftModifier) && status && !fl_orto_disable) {
			fl_orto = true;
			if(fl_orto_move) { CtrlMotionPos_4 = EndMotionPos; fl_orto_move = false; }
			else CtrlMotionPos_4 = itemInMotion->ctrlPos4 + offset;
			if(length(StartMotionPos,QPointF(CtrlMotionPos_4.x(),StartMotionPos.y())) >=
				length(StartMotionPos,QPointF(StartMotionPos.x(),CtrlMotionPos_4.y())))
			    EndMotionPos.setY(StartMotionPos.y());
			else EndMotionPos.setX(StartMotionPos.x());
		    }
		}
		if(fl_status_move) {
		    double ang_bezier = 0;
		    QLineF ln1, ln2;
		    CtrlMotionPos_1 = QPointF(length(EndMotionPos,StartMotionPos)/3, 0);
		    CtrlMotionPos_2 = QPointF(2*length(EndMotionPos,StartMotionPos)/3, 0);
		    ln2 = QLineF(StartMotionPos, QPointF(StartMotionPos.x()+10,StartMotionPos.y()));
		    ln1 = QLineF(StartMotionPos, EndMotionPos);
		    ang_bezier = angle(ln1, ln2);
		    if(StartMotionPos.y() <= EndMotionPos.y())	ang_bezier = 360 - ang_bezier;
		    CtrlMotionPos_1 = QPointF(StartMotionPos.x()+rotate(CtrlMotionPos_1,ang_bezier).x(), StartMotionPos.y()-rotate(CtrlMotionPos_1,ang_bezier).y());
		    CtrlMotionPos_2 = QPointF(StartMotionPos.x()+rotate(CtrlMotionPos_2,ang_bezier).x(), StartMotionPos.y()-rotate(CtrlMotionPos_2,ang_bezier).y());
		}
		else {
		    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
		}
	    }
	    if(flag_hold_arc || flag_arc_rect_3_4) {
		if(arc_rect == 0) {
		    EndMotionPos = scaleRotate(pnts[shapeItems[index_array[0]].n1], w);
		    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
		}
		if(arc_rect == 1) {
		    EndMotionPos = scaleRotate(pnts[shapeItems[index_array[0]].n2], w);
		    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
		    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
		}
	    }
	}
    }
    //Moving the first control point of the figure
    if(rect_num == PntCntr1) {
	w->mainWin()->statusBar()->showMessage(QString(_("Point coordinates(x,y): (%1, %2)")).
		arg(rRnd(pnts[itemInMotion->n3].x(),POS_PREC_DIG)).arg(rRnd(pnts[itemInMotion->n3].y(),POS_PREC_DIG)), 10000);
	StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w);
	EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
	if(shapeType == ShT_Arc) {
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w);
	    CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	    b = length(CtrlMotionPos_2, CtrlMotionPos_1); a = length(CtrlMotionPos_3, CtrlMotionPos_1);
	    ang = angle(QLineF(CtrlMotionPos_1, CtrlMotionPos_3), QLineF(CtrlMotionPos_1, QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y())));
	    if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	    angle_temp = ang;
	    t_start = CtrlMotionPos_4.x();
	    t_end = CtrlMotionPos_4.y();
	}
	else {
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	    CtrlMotionPos_1 += offset;
	}
    }
    //Moving the second control point of the figure
    if(rect_num == PntCntr2) {
	w->mainWin()->statusBar()->showMessage(QString(_("Point coordinates(x,y): (%1, %2)")).
		arg(rRnd(pnts[itemInMotion->n4].x(),POS_PREC_DIG)).arg(rRnd(pnts[itemInMotion->n4].y()),POS_PREC_DIG), 10000);
	StartMotionPos = scaleRotate(pnts[itemInMotion->n1], w);
	EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
	if(shapeType == ShT_Arc) {
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w) + offset;
	    CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w);
	    CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	    EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
	    a = length(CtrlMotionPos_3, CtrlMotionPos_1); b = length(CtrlMotionPos_2, CtrlMotionPos_1);
	    t_start = CtrlMotionPos_4.x(); t_end = CtrlMotionPos_4.y();
	    ang = angle(QLineF(CtrlMotionPos_1,CtrlMotionPos_3), QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y())));
	    if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	    angle_temp = ang;
	    CtrlMotionPos_3 = unRotate(CtrlMotionPos_3, ang, CtrlMotionPos_1);
	    CtrlMotionPos_2 = unRotate(CtrlMotionPos_2, ang, CtrlMotionPos_1);
	    ang_t = angle(QLineF(QPointF(), CtrlMotionPos_2), QLineF(QPointF(), QPointF(-100,0))) - 90;
	    ang = ang + ang_t;
	    StartMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y());
	    EndMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y());
	}
	else {
	    CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	    CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w) + offset;
	}
    }
    //Moving the third control point of the figure
    if(rect_num == PntCntr3) {
	w->mainWin()->statusBar()->showMessage(QString(_("Point coordinates(x,y): (%1, %2)")).
		arg(rRnd(pnts[itemInMotion->n5].x(),POS_PREC_DIG)).arg(rRnd(pnts[itemInMotion->n5].y(),POS_PREC_DIG)), 10000);
	CtrlMotionPos_1 = scaleRotate(pnts[itemInMotion->n3], w);
	CtrlMotionPos_2 = scaleRotate(pnts[itemInMotion->n4], w);
	CtrlMotionPos_3 = scaleRotate(pnts[itemInMotion->n5], w) + offset;
	CtrlMotionPos_4 = itemInMotion->ctrlPos4;
	EndMotionPos = scaleRotate(pnts[itemInMotion->n2], w);
	a = length(CtrlMotionPos_3, CtrlMotionPos_1); b = length(CtrlMotionPos_2, CtrlMotionPos_1);
	t_start = CtrlMotionPos_4.x(); t_end = CtrlMotionPos_4.y();
	ang = angle(QLineF(CtrlMotionPos_1,CtrlMotionPos_3), QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+100,CtrlMotionPos_1.y())));
	if(CtrlMotionPos_3.y() > CtrlMotionPos_1.y()) ang = 360 - ang;
	angle_temp = ang;
	StartMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y());
	EndMotionPos = QPointF(CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y());
    }
    shapeItems.remove(index);
    if(!flag_ctrl || (!flag_ctrl_move && count_Shapes == (count_moveItemTo+count_Shapes-1))) rectItems.clear();

    //Building the line
    if(shapeType == ShT_Line) {
	ang = angle(QLineF(StartMotionPos, EndMotionPos), QLineF(StartMotionPos, QPointF(StartMotionPos.x()+10,StartMotionPos.y())));
	if(StartMotionPos.y() <= EndMotionPos.y()) ang = 360 - ang;
	QPointF orto_pnt = fl_orto ? CtrlMotionPos_4 : QPointF();
	shapeItems.append(ShapeItem(painterPath(widths[MotionWidth]*scaleW,widths[MotionBorderWidth],1,ang,StartMotionPos,EndMotionPos),
			            painterPathSimple(1,ang,StartMotionPos,EndMotionPos),
				    MotionNum_1,MotionNum_2,-1,-1,-1,orto_pnt,MotionLineColor,MotionBorderColor,MotionStyle,MotionWidth,MotionBorderWidth,1,angle_temp));
	if(devW && devW->edit()) {
	    rectItems.append(RectItem(MotionNum_1)); rectItems.last().path.addRect(QRectF(StartMotionPos,QSize(6,6)).translated(-3,-3));
	    rectItems.append(RectItem(MotionNum_2)); rectItems.last().path.addRect(QRectF(EndMotionPos,QSize(6,6)).translated(-3,-3));
	}
	StartMotionPos = unScaleRotate(StartMotionPos, w);
	EndMotionPos = unScaleRotate(EndMotionPos, w);
	pnts[MotionNum_1] = StartMotionPos;
	pnts[MotionNum_2] = EndMotionPos;
    }
    //Building the arc
    if(shapeType == ShT_Arc) {
	CtrlMotionPos_2 = QPointF(CtrlMotionPos_1.x()+rotate(arc(0.25,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(0.25,a,b),ang).y());
	CtrlMotionPos_4 = QPointF(t_start, t_end);
	CtrlMotionPos_3 = QPointF(CtrlMotionPos_1.x()+rotate(arc(0,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(0,a,b),ang).y());
	shapeItems.append(ShapeItem(painterPath(widths[MotionWidth]*scaleW,widths[MotionBorderWidth],2,ang,StartMotionPos,EndMotionPos,CtrlMotionPos_1,CtrlMotionPos_2,CtrlMotionPos_3,CtrlMotionPos_4),
				    painterPathSimple(ShT_Arc,ang,StartMotionPos,EndMotionPos,CtrlMotionPos_1,CtrlMotionPos_2,CtrlMotionPos_3,CtrlMotionPos_4),
				    MotionNum_1,MotionNum_2,MotionNum_3,MotionNum_4,MotionNum_5,CtrlMotionPos_4,MotionLineColor,MotionBorderColor,MotionStyle,MotionWidth,MotionBorderWidth,2,angle_temp,ang_t));
	if(devW && devW->edit()) {
	    rectItems.append(RectItem(MotionNum_1));
	    rectItems.last().path.addRect(QRectF(StartMotionPos,QSize(6,6)).translated(-3,-3));

	    rectItems.append(RectItem(MotionNum_2));
	    rectItems.last().path.addRect(QRectF(EndMotionPos,QSize(6,6)).translated(-3,-3));

	    rectItems.append(RectItem(MotionNum_3,QPainterPath(),QBrush(QColor(50,50,50,128),Qt::SolidPattern)));
	    rectItems.last().path.addRect(QRectF(CtrlMotionPos_1,QSize(6,6)).translated(-3,-3));

	    rectItems.append(RectItem(MotionNum_4,QPainterPath(),QBrush(QColor(127,127,127,128),Qt::SolidPattern)));
	    int xSh = ((sqrt(pow((CtrlMotionPos_2-StartMotionPos).x(),2)-pow((CtrlMotionPos_2-StartMotionPos).y(),2)) < 5) ||
		(sqrt(pow((CtrlMotionPos_2-EndMotionPos).x(),2)-pow((CtrlMotionPos_2-EndMotionPos).y(),2)) < 5)) ? 10*w->xScale(true) : 0;
	    rectItems.last().path.addRect(QRectF(CtrlMotionPos_2,QSize(6,6)).translated(-3-xSh,-3));

	    rectItems.append(RectItem(MotionNum_5,QPainterPath(),QBrush(QColor(127,127,127,128),Qt::SolidPattern)));
	    xSh = ((sqrt(pow((CtrlMotionPos_3-StartMotionPos).x(),2)-pow((CtrlMotionPos_3-StartMotionPos).y(),2)) < 5) ||
		    (sqrt(pow((CtrlMotionPos_3-EndMotionPos).x(),2)-pow((CtrlMotionPos_3-EndMotionPos).y(),2)) < 5)) ? 10*w->xScale(true) : 0;
	    rectItems.last().path.addRect(QRectF(CtrlMotionPos_3,QSize(6,6)).translated(-3-xSh,-3));
	}

	StartMotionPos = unScaleRotate(StartMotionPos, w);
	EndMotionPos = unScaleRotate(EndMotionPos, w);
	CtrlMotionPos_1 = unScaleRotate(CtrlMotionPos_1, w);
	CtrlMotionPos_2 = unScaleRotate(CtrlMotionPos_2, w);
	CtrlMotionPos_3 = unScaleRotate(CtrlMotionPos_3, w);
	pnts[MotionNum_1] = StartMotionPos;
	pnts[MotionNum_2] = EndMotionPos;
	pnts[MotionNum_3] = CtrlMotionPos_1;
	pnts[MotionNum_4] = CtrlMotionPos_2;
	pnts[MotionNum_5] = CtrlMotionPos_3;
    }
    //Building the bezier curve
    if(shapeType == ShT_Bezier) {
	ang = angle(QLineF(StartMotionPos,EndMotionPos), QLineF(StartMotionPos,QPointF(StartMotionPos.x()+10,StartMotionPos.y())));
	if(StartMotionPos.y() <= EndMotionPos.y()) ang = 360 - ang;
	shapeItems.append(ShapeItem(painterPath(widths[MotionWidth]*scaleW,widths[MotionBorderWidth],3,ang,StartMotionPos,EndMotionPos,CtrlMotionPos_1,CtrlMotionPos_2),
			            painterPathSimple(3,ang,StartMotionPos,EndMotionPos,CtrlMotionPos_1,CtrlMotionPos_2),
				    MotionNum_1,MotionNum_2,MotionNum_3,MotionNum_4,-1,CtrlMotionPos_4,MotionLineColor,MotionBorderColor,MotionStyle,MotionWidth,MotionBorderWidth,3,angle_temp));

	if(devW && devW->edit()) {
	    rectItems.append(RectItem(MotionNum_1)); rectItems.last().path.addRect(QRectF(StartMotionPos,QSize(6,6)).translated(-3,-3));
	    rectItems.append(RectItem(MotionNum_2)); rectItems.last().path.addRect(QRectF(EndMotionPos,QSize(6,6)).translated(-3,-3));
	    rectItems.append(RectItem(MotionNum_3,QPainterPath(),QBrush(QColor(127,127,127,128),Qt::SolidPattern)));
	    rectItems.last().path.addRect(QRectF(CtrlMotionPos_1,QSize(6,6)).translated(-3,-3));
	    rectItems.append(RectItem(MotionNum_4,QPainterPath(),QBrush(QColor(127,127,127,128),Qt::SolidPattern)));
	    rectItems.last().path.addRect(QRectF(CtrlMotionPos_2,QSize(6,6)).translated(-3,-3));
	}
	StartMotionPos = unScaleRotate(StartMotionPos, w);
	EndMotionPos = unScaleRotate(EndMotionPos, w);
	CtrlMotionPos_1 = unScaleRotate(CtrlMotionPos_1, w);
	CtrlMotionPos_2 = unScaleRotate(CtrlMotionPos_2, w);
	pnts[MotionNum_1] = StartMotionPos;
	pnts[MotionNum_2] = EndMotionPos;
	pnts[MotionNum_3] = CtrlMotionPos_1;
	pnts[MotionNum_4] = CtrlMotionPos_2;
    }

    for(int i = (shapeItems.size()-1); i > index; i--) {
	temp_shape = shapeItems[i-1];
	shapeItems[i-1] = shapeItems[i];
	shapeItems[i] = temp_shape;
    }
    if(count_moveItemTo == count_Shapes) previousPosition_all = pos;
}

//**********************************************************
//* Detecting connected figures				   *
//**********************************************************
bool ShapeElFigure::holds( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    int num, index_hold;
    bool flag_equal;
    index_array.clear();
    for(int i = 0; i < shapeItems.size(); i++) index_array.push_back(-1);
    index_array[0] = index;
    num = 0;
    do {
	index_hold = index_array[num];
	for(int i = 0; i < shapeItems.size(); i++)
	    if(i != index_hold && ((shapeItems[index_hold].n1 == shapeItems[i].n1) ||
			(shapeItems[index_hold].n2 == shapeItems[i].n2) ||
			(shapeItems[index_hold].n1 == shapeItems[i].n2) ||
			(shapeItems[index_hold].n2 == shapeItems[i].n1)) &&
			ellipse_draw_startPath == newPath && ellipse_draw_endPath == newPath )
	    {
		flag_equal = false;
		for(int j = 0; j <= count_holds; j++)
		    if(index_array[j] == i) flag_equal = 1;
		    if(flag_equal == 0) { count_holds++; index_array[count_holds] = i; }
	    }
	num++;
    }
    while(num != (count_holds+1));

    return (count_holds > 0);
}

//**********************************************************
//* Detecting the figures(figure), or their points	   *
//* to move by key(up, down, left, rught) events	   *
//**********************************************************
void ShapeElFigure::moveUpDown( WdgView *w )
{
    int rect_num_temp;
    count_moveItemTo = 0;
    bool flag_break_move;
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;
    QVector<inundationItem> &inundItems = elFD->inundItems;

    if(flag_ctrl && count_Shapes)
    {
	if(!flag_first_move && flag_check_pnt_inund)
	{
	    checkPoint_checkInundation(w);
	    flag_check_pnt_inund = false;
	}
	moveAll(QPointF(), w);
    }
    else
    {
	if(!flag_first_move)
	{
	    count_holds = 0;
	    flag_rect = false;
	    if(status_hold)	holds(w);			//Calling "holds" to detect is the figure connected with other or not.
	    if(count_holds)					//If the Ctrl is pressed and there are connected figures
	    {
		count_Shapes = count_holds + 1;			//Getting the number of figures to move
		if(rect_num != -1)				//If there is rect under the mouse pointer
		{
		    rect_num_temp = rect_num;
		    rect_num = realRectNum(rect_num, w);	//Detecting the number of the rect for moveItemTo
		    if((rect_num == 2 || rect_num == 3) && shapeItems[index].type == ShT_Bezier)	flag_rect = false;
		    if(rect_num == 0 || rect_num == 1)	rectNum0_1(rect_num_temp, w);
		    if((rect_num == 3 || rect_num == 4) && shapeItems[index].type == ShT_Arc) rectNum3_4(w);
		}
	    }
	    // if there is the rect number of figures to move becomes equal to the number of figures, connected with this rect
	    if(flag_rect || flag_arc_rect_3_4)	count_Shapes = count_rects;
	}
	if(flag_rect || flag_arc_rect_3_4 || (rect_num == -1 && count_holds))
	    moveAll(QPointF(), w);
    }
    if((!flag_ctrl || (!flag_rect && rect_num != -1)) && !flag_arc_rect_3_4 && !flag_copy)// if there is simple figure or one of its rects
    {
	if(index != -1)
	{
	    ShapeItem &newIt = shapeItems[index];
	    itemInMotion = &shapeItems[index];
	    num_vector.clear();
	    flag_ctrl = true;
	    flag_ctrl_move = false;
	    count_moveItemTo = 1;
	    count_Shapes = 1;

	    //> Circle contour gap
	    if(newIt.type == ShT_Arc && !vrng(newIt.n2,-9,0) && newIt.n2 == newIt.n1)
                newIt.n2 = elFD->appendPoint(pnts[newIt.n2], (newIt.n2<=-10));

	    for(int i = 0; /*!status_hold &&*/ !flag_first_move && i < shapeItems.size(); i++)
	    {
		if(i == index) continue;
		ShapeItem &curIt = shapeItems[i];
		if(newIt.type == ShT_Arc && !vrng(newIt.n5,-9,0) &&
			(newIt.n5 == curIt.n1 || newIt.n5 == curIt.n2 || newIt.n5 == curIt.n3 || newIt.n5 == curIt.n4 || newIt.n5 == curIt.n5))
		    newIt.n5 = elFD->appendPoint(pnts[newIt.n5], (newIt.n5<=-10));
		if(newIt.type == ShT_Arc || newIt.type == ShT_Bezier)
		{
		    if(!vrng(newIt.n4,-9,0) && (newIt.n4 == curIt.n1 || newIt.n4 == curIt.n2 || newIt.n4 == curIt.n3 || newIt.n4 == curIt.n4 || newIt.n4 == curIt.n5))
			newIt.n4 = elFD->appendPoint(pnts[newIt.n4], (newIt.n4<=-10));
		    if(!vrng(newIt.n3,-9,0) && (newIt.n3 == curIt.n1 || newIt.n3 == curIt.n2 || newIt.n3 == curIt.n3 || newIt.n3 == curIt.n4 || newIt.n3 == curIt.n5))
			newIt.n3 = elFD->appendPoint(pnts[newIt.n3], (newIt.n3<=-10));
		}
		if(!vrng(newIt.n1,-9,0) && (newIt.n1 == curIt.n1 || newIt.n1 == curIt.n2 || newIt.n1 == curIt.n3 || newIt.n1 == curIt.n4 || newIt.n1 == curIt.n5))
		    newIt.n1 = elFD->appendPoint(pnts[newIt.n1], (newIt.n1<=-10));
		if(!vrng(newIt.n2,-9,0) && (newIt.n2 == curIt.n1 || newIt.n2 == curIt.n2 || newIt.n2 == curIt.n3 || newIt.n2 == curIt.n4 || newIt.n2 == curIt.n5))
		    newIt.n2 = elFD->appendPoint(pnts[newIt.n2], (newIt.n2<=-10));
	    }

	    moveItemTo(pnts[newIt.n1], w);
	    if(inundItems.size())
	    {
		bool flag_single_arc = false;
		for(int p = 0; newIt.type == ShT_Arc && p < inundItems.size(); p++)
		    if(inundItems[p].n.size() == 1 && inundItems[p].n[0] == index)
			flag_single_arc = true;
		if((newIt.type == ShT_Arc && (rect_num == 3 || rect_num == 4 || rect_num == -1) && (flag_single_arc || status_hold)) ||
			(newIt.type == ShT_Bezier && (rect_num == 2 || rect_num == 3) && status_hold))
		    for(int i = 0; i < inundItems.size(); i++)
		    {
			flag_break_move = false;
			for(int j = 0; j < inundItems[i].n.size(); j++)
			{
			    if(inundItems[i].n[j] != index) continue;
			    inundationPath = createInundationPath(inundItems[i].n, pnts, w);
			    inundItems[i].path = inundationPath;
			    flag_break_move = true;
			    break;
			}
			if(flag_single_arc && flag_break_move) break;
		    }
		else if(!flag_inund_break /*&& !flag_ctrl && !flag_A && !(rect_num == 3 || rect_num == 4)*/ )
		{
		    QVector<int> ind_array;
		    ind_array.push_back(index);
		    removeFill(ind_array, 1, w);
		    flag_inund_break = true;
		}
	    }
	    flag_ctrl = false;
	}
	else itemInMotion = NULL;
    }
}

int ShapeElFigure::realRectNum( int rect_num_old, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    int rect_num_new = 0;
    //Detecting the correct index of the figure
    for(int i = 0; i <= shapeItems.size()-1; i++)
	switch(shapeItems[i].type) {
	    case ShT_Line:
		if((rectItems[rect_num].num == shapeItems[i].n1) || (rectItems[rect_num].num == shapeItems[i].n2))
		    index = i;
		break;
	    case ShT_Arc:
		if((rectItems[rect_num].num == shapeItems[i].n1) || (rectItems[rect_num].num == shapeItems[i].n2) ||
			(rectItems[rect_num].num == shapeItems[i].n3) || (rectItems[rect_num].num == shapeItems[i].n4) ||
			(rectItems[rect_num].num == shapeItems[i].n5))
		    index = i;
	    break;
	    case ShT_Bezier:
		if((rectItems[rect_num].num == shapeItems[i].n1) || (rectItems[rect_num].num == shapeItems[i].n2) ||
			(rectItems[rect_num].num == shapeItems[i].n3) || (rectItems[rect_num].num == shapeItems[i].n4))
		    index = i;
		break;
	}

    //Detecting the number of the rect for moveItemTo
    switch(shapeItems[index].type) {
	case ShT_Line:
	    if(rectItems[rect_num_old].num == shapeItems[index].n1)	rect_num_new = 0;
	    if(rectItems[rect_num_old].num == shapeItems[index].n2)	rect_num_new = 1;
	    break;
	case ShT_Arc:
	    if(rectItems[rect_num_old].num == shapeItems[index].n1)	rect_num_new = 0;
	    if(rectItems[rect_num_old].num == shapeItems[index].n2)	rect_num_new = 1;
	    if(rectItems[rect_num_old].num == shapeItems[index].n3)	rect_num_new = 2;
	    if(rectItems[rect_num_old].num == shapeItems[index].n4)	rect_num_new = 3;
	    if(rectItems[rect_num_old].num == shapeItems[index].n5)	rect_num_new = 4;
	    break;
	case ShT_Bezier:
	    if(rectItems[rect_num_old].num == shapeItems[index].n1)	rect_num_new = 0;
	    if(rectItems[rect_num_old].num == shapeItems[index].n2)	rect_num_new = 1;
	    if(rectItems[rect_num_old].num == shapeItems[index].n3)	rect_num_new = 2;
	    if(rectItems[rect_num_old].num == shapeItems[index].n4)	rect_num_new = 3;
	    break;
    }

    return rect_num_new;
}

void ShapeElFigure::rectNum0_1( int rect_num_temp, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;

    flag_rect = true;
    count_rects = 0;
    QVector<int> index_array_temp;

    //> detecting the common points for all connected figures
    for(int i = 0; i <= count_holds; i++)
    {
        if(pnts[rectItems[rect_num_temp].num] == pnts[shapeItems[index_array[i]].n1])
        {
            index_array_temp.push_back(index_array[i]);
            rect_array.push_back(0);
            count_rects++;
        }
        if(pnts[rectItems[rect_num_temp].num] == pnts[shapeItems[index_array[i]].n2])
        {
            index_array_temp.push_back(index_array[i]);
            rect_array.push_back(1);
            count_rects++;
        }
    }
    index_array.clear();
    for(int i = 0; i < count_rects; i++)	index_array.push_back(index_array_temp[i]);

    //> if there is an arc in "index_array" we put it on the first place in it.
    int num_arc = -1;
    for(int i = 0; i < count_rects; i++)
	if(shapeItems[index_array[i]].type == ShT_Arc)
	{
	    flag_hold_arc = true;
	    num_arc = i;
	}
    if(num_arc != -1)
    {
	int index_0 = index_array[0];
	int rect_0 = rect_array[0];
	index_array[0] = index_array[num_arc];
	index_array[num_arc] = index_0;
	rect_array[0] = rect_array[num_arc];
	rect_array[num_arc] = rect_0;
    }
    if(count_rects == 1)
    {
	flag_rect = false;
	if(shapeItems[index_array[0]].type == ShT_Arc)
	{
	    rect_num_arc = rect_num;
	    flag_hold_arc = false;
	}
    }
}

void ShapeElFigure::rectNum3_4( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    flag_arc_rect_3_4 = true;
    QVector<int> index_array_temp;
    for(int i = 0; i < count_holds + 5; i++)
    {
	fig_rect_array.push_back(0);
	arc_rect_array.push_back(0);
	index_array_temp.push_back(-1);
    }
    flag_rect = false;
    index_array_temp[0] = index;
    if(rect_num == 3)	arc_rect_array[0] = fig_rect_array[0] = 3;
    if(rect_num == 4)	arc_rect_array[0] = fig_rect_array[0] = 4;
    count_rects = 1;
    for(int i = 0; i <= count_holds; i++)
    {
	if(index_array[i] != index)
	{
	    if(shapeItems[index].n1 == shapeItems[index_array[i]].n1)
	    {
		index_array_temp[count_rects] = index_array[i];
		arc_rect_array[count_rects] = 0;
		fig_rect_array[count_rects] = 0;
		count_rects++;
	    }
	    if(shapeItems[index].n1 == shapeItems[index_array[i]].n2)
	    {
		index_array_temp[count_rects] = index_array[i];
		arc_rect_array[count_rects] = 0;
		fig_rect_array[count_rects] = 1;
		count_rects++;
	    }
	    if(shapeItems[index].n2 == shapeItems[index_array[i]].n1)
	    {
		index_array_temp[count_rects] = index_array[i];
		arc_rect_array[count_rects] = 1;
		fig_rect_array[count_rects] = 0;
		count_rects++;
	    }
	    if(shapeItems[index].n2 == shapeItems[index_array[i]].n2)
	    {
		index_array_temp[count_rects] = index_array[i];
		arc_rect_array[count_rects] = 1;
		fig_rect_array[count_rects] = 1;
		count_rects++;
	    }
	}
    }
    index_array.clear();
    for(int i = 0; i < count_rects; i++) index_array.push_back(-1);
    for(int i = 0; i < count_rects; i++) index_array[i] = index_array_temp[i];
    index_array_temp.clear();
}

//**********************************************************
//* Moving all connected figures			   *
//**********************************************************
void ShapeElFigure::moveAll( const QPointF &pos, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    PntMap &pnts = elFD->shapePnts;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    ImageMap &images = elFD->shapeImages;
    QVector<inundationItem> &inundItems = elFD->inundItems;

    num_vector.clear();
    bool flag_break;
    for( int i = 0; i < count_Shapes; i++ )
    {
	count_moveItemTo += 1;
	flag_ctrl_move = false;
	flag_ctrl = true;
	itemInMotion = &shapeItems[index_array[i]];
	if( flag_rect )
	{
	    rect_num = rect_array[i];
	    arc_rect = rect_array[0];
	}
	if( flag_arc_rect_3_4 )
	{
	    if( i == 0 && !flag_down && !flag_up && !flag_right && !flag_left ) offset = pos - previousPosition_all;
	    if( i > 0 )
	    {
		if( arc_rect_array[i] == 0 )
		    offset = scaleRotate(pnts[shapeItems[index_array[0]].n1], w) - Prev_pos_1;
		else
		    offset = scaleRotate(pnts[shapeItems[index_array[0]].n2], w) - Prev_pos_2;
	    }
	    rect_num = fig_rect_array[i];
	    arc_rect = arc_rect_array[i];
	}
	index = index_array[i];
	moveItemTo(pos, w);
	if( i == 0 && flag_arc_rect_3_4 )
	{
	    Prev_pos_1 = scaleRotate(pnts[shapeItems[index_array[0]].n1], w);
	    Prev_pos_2 = scaleRotate(pnts[shapeItems[index_array[0]].n2], w);
	}
    }
    if( inundItems.size() )
    {
	for( int i = 0; i < inundItems.size(); i++ )
	    for(int j = 0; j < inundItems[i].n.size(); j++ )
	    {
		flag_break = false;
		for(int k = 0; k < index_array.size(); k++ )
		    if( inundItems[i].n[j] == index_array[k] )
		    {
			bool fl_buildPath = true;
			if( !images[inundItems[i].brushImg].empty() )
			{
			    QImage img;
			    string backimg = w->resGet(images[inundItems[i].brushImg]);
			    img.loadFromData((const uchar*)backimg.data(), backimg.size());
			    if( !img.isNull() ) fl_buildPath = false;
			}
			if( fl_buildPath )
			{
			    inundationPath = createInundationPath(inundItems[i].n, pnts, w);
			    inundItems[i].path = inundationPath;
			}
			flag_break = true;
			break;
		    }
		    if( flag_break ) break;
	    }
    }
}

void ShapeElFigure::removeFill( QVector<int> ind_array, int count, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<inundationItem> &inundItems = elFD->inundItems;

    QVector<int> rem_inund;
    bool fl_in_brk;
    bool flag_ravno;
    for( int i = 0; i < count; i++ )
	for( int j = 0; j < inundItems.size(); j++ )
	    for( int k = 0; k < inundItems[j].n.size(); k++ )
	    {
		fl_in_brk = false;
		if( ind_array[i] == inundItems[j].n[k] )
		{
		    if( rem_inund.size() )
		    {
			flag_ravno = false;
			for( int p = 0; p < rem_inund.size(); p++ )
			    if( j == rem_inund[p] ){ flag_ravno = true; break; }
			if( !flag_ravno )
			{
			    rem_inund.push_back(j);
			    fl_in_brk = true;
			    break;
			}
		    }
		    else { rem_inund.push_back(j); break; }
		}
		if( fl_in_brk ) break;
	    }
    bool flag_exist;
    int tmp_inund;
    QVector<int> real_rem_inund;
    //-- Removing the fill(inundation) which has one or more figures that are not moving --
    for( int j = 0; j < rem_inund.size(); j++ )
    {
	tmp_inund = 0;
	for( int p = 0; p < real_rem_inund.size(); p++ )
	    if( rem_inund[j] > real_rem_inund[p] ) tmp_inund++;

	for( int k = 0; k < inundItems[rem_inund[j]-tmp_inund].n.size(); k++ )
	{
	    flag_exist = false;
	    for( int i = 0; i < count; i++ )
	    {
		if( inundItems[rem_inund[j]-tmp_inund].n[k] == ind_array[i] )
		{
		    flag_exist = true;
		    break;
		}
	    }
	    if( flag_exist == false )
	    {
		inundItems.remove(rem_inund[j]-tmp_inund);
		real_rem_inund.push_back( rem_inund[j] );
		break;
	    }
	}
    }
    rem_inund.clear();
    real_rem_inund.clear();
}

//**********************************************************
//* Checking if appending of the point and deleting of	   *
//*  the fill(inundation) is needed when several(choosen   *
//*  with one of the methods) figures are moving	   *
//**********************************************************
void ShapeElFigure::checkPoint_checkInundation( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;

    QPointF Temp;
    bool flag_in_n;
    int figuresCount;
    if( index_array.size() > count_Shapes ) figuresCount = count_Shapes;
    else figuresCount = index_array.size();
    for( int i = 0; i < figuresCount; i++ )
	for( int j = 0; j < shapeItems.size(); j++ )
	{
	    //-- Append the first point of the figure(n1) if it is needed --
	    if( shapeItems[index_array[i]].n1 == shapeItems[j].n1 || shapeItems[index_array[i]].n1 == shapeItems[j].n2 )
	    {
		flag_in_n = false;
		for( int k = 0; k < figuresCount; k++ )
		    if( j == index_array[k] )
		    {
			flag_in_n = true;
			break;
		    }
		if( !flag_in_n )
		{
		    Temp = pnts[shapeItems[index_array[i]].n1];
		    if( shapeItems[index_array[i]].n1 > 0 )
			shapeItems[index_array[i]].n1 = elFD->appendPoint(Temp, false);
		    else if( shapeItems[index_array[i]].n1 <= -10 )
			shapeItems[index_array[i]].n1 = elFD->appendPoint(Temp, true);
		}
	    }
	    //-- Append the second point of the figure(n2) if it is needed --
	    if( shapeItems[index_array[i]].n2 == shapeItems[j].n2 || shapeItems[index_array[i]].n2 == shapeItems[j].n1 )
	    {
		flag_in_n = false;
		for( int k = 0; k < figuresCount; k++ )
		    if( j == index_array[k] )
		    {
			flag_in_n = true;
			break;
		    }
		if( !flag_in_n )
		{
		    Temp = pnts[shapeItems[index_array[i]].n2];
		    if( shapeItems[index_array[i]].n2 > 0 )
			shapeItems[index_array[i]].n2 = elFD->appendPoint(Temp, false);
		    else if( shapeItems[index_array[i]].n2 <= -10 )
			shapeItems[index_array[i]].n2 = elFD->appendPoint(Temp, true);
		}
	    }
	}
    //-- Detecting the all inundations(fills) in which the mooving figures are present --
    removeFill( index_array, figuresCount, w );
}

//**********************************************************
//* Detecting the figure or one of its control points	   *
//*  under mouse cursor					   *
//**********************************************************
int ShapeElFigure::itemAt( const QPointF &pos, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    w->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
	    arg(rRnd(pos.x()/w->xScale(true),POS_PREC_DIG)).arg(rRnd(pos.y()/w->yScale(true),POS_PREC_DIG)), 10000 );

    QPointF point;
    rect_num = -1;
    bool flag_break = false;
    if( !flag_copy && !flag_A )
	for(int j = 0; j <= rectItems.size()-1; j++)
	    if( rectItems[j].path.contains(pos) ) rect_num = j;
    index = -1;
    if( rect_num != -1)
	for( int i = 0; i <= shapeItems.size()-1; i++ )
	    switch( shapeItems[i].type )
	    {
		case 1:
		    if( (rectItems[rect_num].num == shapeItems[i].n1) ||
			(rectItems[rect_num].num == shapeItems[i].n2) )
			index = i;
		    break;
		case 2:
		    if( (rectItems[rect_num].num == shapeItems[i].n1) ||
			(rectItems[rect_num].num == shapeItems[i].n2) ||
			(rectItems[rect_num].num == shapeItems[i].n3) ||
			(rectItems[rect_num].num == shapeItems[i].n4) ||
			(rectItems[rect_num].num == shapeItems[i].n5) )
			index = i;
		    break;
		case 3:
		    if( (rectItems[rect_num].num == shapeItems[i].n1) ||
			(rectItems[rect_num].num == shapeItems[i].n2) ||
			(rectItems[rect_num].num == shapeItems[i].n3) ||
			(rectItems[rect_num].num == shapeItems[i].n4) )
			index = i;
		    break;
	    }
    if( rect_num == -1 )
    for( int i = shapeItems.size()-1; i > -1; i-- )
    {
	const ShapeItem &item = shapeItems[i];
	if( item.path.contains(pos) )
	{
	    index = i;
	    flag_break = true;
	}
	if( flag_break ) break;
	for( int j = 2; j > 0; j-- )
	{
	    point.setY(j);
	    point.setX(j);
	    if( item.path.contains(pos + point) || item.path.contains(pos - point) )
	    {
	       index = i;
	       flag_break = true;
	    }
	}
	if( flag_break ) break;
    }
   return index;
}

//**********************************************************
//* Building the path for the current figure (width > 0)   *
//**********************************************************
QPainterPath ShapeElFigure::painterPath( float width, float bWidth, int type, double ang, QPointF pBeg, QPointF pEnd,
    QPointF pCntr1, QPointF pCntr2, QPointF pCntr3, QPointF aT )
{
    QPainterPath circlePath = newPath;
    width = rRnd(vmax(1,width));
    bWidth = rRnd(vmax(1,bWidth));
    pBeg = QPointF(rRnd(pBeg.x(),POS_PREC_DIG,true), rRnd(pBeg.y(),POS_PREC_DIG,true));
    pEnd = QPointF(rRnd(pEnd.x(),POS_PREC_DIG,true), rRnd(pEnd.y(),POS_PREC_DIG,true));

    switch(type) {
	case ShT_Line:
	    bWidth = bWidth/2;
	    circlePath.moveTo(pBeg.x() + rotate(QPointF(-bWidth,-(width/2+bWidth)),ang).x(),
			      pBeg.y() - rotate(QPointF(-bWidth,-(width/2+bWidth)),ang).y());
	    circlePath.lineTo(pBeg.x() + rotate(QPointF(length(pEnd,pBeg)+bWidth,-(width/2+bWidth)),ang).x(),
			      pBeg.y() - rotate(QPointF(length(pEnd,pBeg)+bWidth,-(width/2+bWidth)),ang).y());
	    circlePath.lineTo(pBeg.x() + rotate(QPointF(length(pEnd,pBeg)+bWidth,(width/2+bWidth)),ang).x(),
			      pBeg.y() - rotate(QPointF(length(pEnd,pBeg)+bWidth,(width/2+bWidth)),ang).y());
	    circlePath.lineTo(pBeg.x() + rotate(QPointF(-bWidth,(width/2+bWidth)),ang).x(),
			      pBeg.y() - rotate(QPointF(-bWidth,(width/2+bWidth)),ang).y());
	    circlePath.closeSubpath();
	    circlePath.setFillRule(Qt::WindingFill);
	    break;
	case ShT_Arc: {
	    /*pCntr1 = QPointF(rRnd(pCntr1.x(),POS_PREC_DIG,true), rRnd(pCntr1.y(),POS_PREC_DIG,true));
	    pCntr2 = QPointF(rRnd(pCntr2.x(),POS_PREC_DIG,true), rRnd(pCntr2.y(),POS_PREC_DIG,true));
	    pCntr3 = QPointF(rRnd(pCntr3.x(),POS_PREC_DIG,true), rRnd(pCntr3.y(),POS_PREC_DIG,true));*/

	    double arc_a = length(pCntr3, pCntr1) + width/2 + bWidth/2,
		   arc_b = length(pCntr1, pCntr2) + width/2 + bWidth/2,
		   arc_a_small = arc_a - width - bWidth,
		   arc_b_small = arc_b - width - bWidth,
		   t_start = aT.x(), t_end = aT.y();
	    QPointF rotArc = rotate(arc(t_start,arc_a_small,arc_b_small), ang);
	    circlePath.moveTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
	    rotArc = rotate(arc(t_start,arc_a,arc_b), ang);
	    circlePath.lineTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
	    for(double t = t_start; true; t += ARC_STEP) {
		rotArc = rotate(arc(vmin(t,t_end),arc_a,arc_b), ang);
		circlePath.lineTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
		if(t >= t_end) break;
	    }
	    rotArc = rotate(arc(t_end,arc_a_small,arc_b_small), ang);
	    circlePath.lineTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
	    for(double t = t_end; true; t -= ARC_STEP) {
		rotArc = rotate(arc(vmax(t,t_start),arc_a_small,arc_b_small), ang);
		circlePath.lineTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
		if(t <= t_start) break;
	    }
	    circlePath.closeSubpath();
	    circlePath.setFillRule(Qt::WindingFill);
	    break;
	}
	case ShT_Bezier:
	    pCntr1 = QPointF(rRnd(pCntr1.x(),POS_PREC_DIG,true), rRnd(pCntr1.y(),POS_PREC_DIG,true));
	    pCntr2 = QPointF(rRnd(pCntr2.x(),POS_PREC_DIG,true), rRnd(pCntr2.y(),POS_PREC_DIG,true));

	    bWidth = bWidth/2;
	    QPointF pCntr1_tmp = unRotate(pCntr1, ang, pBeg),
		    pCntr2_tmp = unRotate(pCntr2, ang, pBeg),
		    pEnd_tmp = QPointF(length(pEnd,pBeg)+bWidth, 0);
	    circlePath.moveTo(pBeg.x() + rotate(QPointF(-bWidth,-(width/2+bWidth)),ang).x(),
			      pBeg.y() - rotate(QPointF(-bWidth,-(width/2+bWidth)),ang).y());
	    circlePath.cubicTo(QPointF(pBeg.x() + rotate(QPointF(pCntr1_tmp.x(),pCntr1_tmp.y()-(width/2+bWidth)),ang).x(),
				       pBeg.y() - rotate(QPointF(pCntr1_tmp.x(),pCntr1_tmp.y()-(width/2+bWidth)),ang).y()),
			       QPointF(pBeg.x() + rotate(QPointF(pCntr2_tmp.x(),pCntr2_tmp.y()-(width/2+bWidth)),ang).x(),
				       pBeg.y() - rotate(QPointF(pCntr2_tmp.x(),pCntr2_tmp.y()-(width/2+bWidth)),ang).y()),
			       QPointF(pBeg.x() + rotate(QPointF(pEnd_tmp.x(),pEnd_tmp.y()-(width/2+bWidth)),ang).x(),
				       pBeg.y() - rotate(QPointF(pEnd_tmp.x(),pEnd_tmp.y()-(width/2+bWidth)),ang).y()));
	    circlePath.lineTo(pBeg.x() + rotate(QPointF(pEnd_tmp.x(),width/2+bWidth),ang).x(),
			      pBeg.y() - rotate(QPointF(pEnd_tmp.x(),width/2+bWidth),ang).y());
	    circlePath.cubicTo(QPointF(pBeg.x() + rotate(QPointF(pCntr2_tmp.x(),pCntr2_tmp.y()+width/2+bWidth),ang).x(),
				       pBeg.y() - rotate(QPointF(pCntr2_tmp.x(),pCntr2_tmp.y()+width/2+bWidth),ang).y()),
			       QPointF(pBeg.x() + rotate(QPointF(pCntr1_tmp.x(),pCntr1_tmp.y()+width/2+bWidth),ang).x(),
				       pBeg.y() - rotate(QPointF(pCntr1_tmp.x(),pCntr1_tmp.y()+width/2+bWidth),ang).y()),
			       QPointF(pBeg.x() + rotate(QPointF(-bWidth,width/2+bWidth),ang).x(),
				       pBeg.y() - rotate(QPointF(-bWidth,width/2+bWidth),ang).y()));
	    circlePath.closeSubpath();
	    circlePath.setFillRule(Qt::WindingFill);
	break;
    }

    return circlePath;
}

//**********************************************************
//* Building path for the figure(if its border's width = 0)*
//**********************************************************
QPainterPath ShapeElFigure::painterPathSimple( int type, double ang, QPointF pBeg, QPointF pEnd,
    QPointF pCntr1, QPointF pCntr2, QPointF pCntr3, QPointF aT )
{
    QPainterPath circlePath = newPath;
    pBeg = QPointF(rRnd(pBeg.x(),POS_PREC_DIG,true), rRnd(pBeg.y(),POS_PREC_DIG,true));
    pEnd = QPointF(rRnd(pEnd.x(),POS_PREC_DIG,true), rRnd(pEnd.y(),POS_PREC_DIG,true));

    circlePath.moveTo(pBeg);
    switch(type) {
	case ShT_Line: circlePath.lineTo(pEnd); break;
	case ShT_Arc: {
	    double arc_a = length(pCntr3, pCntr1), arc_b = length(pCntr1, pCntr2),
		   t_start = aT.x(), t_end = aT.y();
	    QPointF rotArc;// = rotate(arc(t_start,arc_a,arc_b), ang);
	    //printf("TEST 00 Arc: ang=%g; t_start=%g; t_end=%g; pCntr1=[%g:%g]; pCntr2=[%g:%g]; pCntr3=[%g:%g]; arc_a=%g; arc_b=%g\n",
	    //	ang, t_start, t_end, pCntr1.x(), pCntr1.y(), pCntr2.x(), pCntr2.y(), pCntr3.x(), pCntr3.y(), arc_a, arc_b);
	    for(double t = t_start; true; t += ARC_STEP) {
		rotArc = rotate(arc(vmin(t,t_end),arc_a,arc_b), ang);
		circlePath.lineTo(pCntr1.x()+rotArc.x(), pCntr1.y()-rotArc.y());
		if(t >= t_end) break;
	    }
	    circlePath.lineTo(pEnd);
	    break;
	}
	case ShT_Bezier:
	    pCntr1 = QPointF(rRnd(pCntr1.x(),POS_PREC_DIG,true), rRnd(pCntr1.y(),POS_PREC_DIG,true));
	    pCntr2 = QPointF(rRnd(pCntr2.x(),POS_PREC_DIG,true), rRnd(pCntr2.y(),POS_PREC_DIG,true));

	    circlePath.cubicTo(pCntr1, pCntr2, pEnd);
	    break;
    }

    return circlePath;
}

//**********************************************************
//* Building the contiguity matrix using start and	   *
//*  end points of all figures				   *
//**********************************************************
int ShapeElFigure::buildMatrix( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    int N;
    for( int j = 0; j < 2*shapeItems.size()+1; j++ )
	vect.push_back(0);
    int j = 1;
    bool flag_vect_n1, flag_vect_n2;
    //for(int i = 0; i < shapeItems.size(); i++ )
    for( int i = shapeItems.size()-1; i >= 0; i-- )
    {
	flag_vect_n1 = false;
	flag_vect_n2 = false;
	for( int k = 1; k < j; k++ )
	{
	    if( vect[k] == shapeItems[i].n1 )
		flag_vect_n1 = true;
	    if( vect[k] == shapeItems[i].n2 )
		flag_vect_n2 = true;
	}
	if( !flag_vect_n1 )
	{
	    vect[j] = shapeItems[i].n1;
	    j++;
	}
	if( !flag_vect_n2 )
	{
	    vect[j] = shapeItems[i].n2;
	    j++;
	}
    }
    for( int i = 0; i < j; i++ )
    {
	QVector<int> el;
	for( int k = 0; k < j; k++ )  el.push_back(0);
	map_matrix.push_back(el);
    }
    N = j - 1;
    for( int v = 1; v < j; v++ )
	//for( int i = 0; i < shapeItems.size(); i++ )
	for( int i = shapeItems.size()-1; i >= 0; i-- )
	{
	    if( shapeItems[i].n1 == vect[v] )
		for( int p = 1; p < j; p++ )
		    if( vect[p] == shapeItems[i].n2 )
		    {
			map_matrix[v][p] = 1;
			map_matrix[p][v] = 1;
		    }
	    if( shapeItems[i].n2 == vect[v] )
		for( int p = 1; p < j; p++ )
		    if( vect[p] == shapeItems[i].n1 )
		    {
			map_matrix[v][p] = 1;
			map_matrix[p][v] = 1;
		    }
	}
    return N;
}

void ShapeElFigure::step( int s, int f, int p, const QVector<int> &vect, int N )
{
    int c;
    if( s == f && p > 4 )
    {
	if( len > 0 && len > clen )
	    found--;
	len = clen;
	found++;
	minroad[found][0] = len;
	for( int k = 1; k < p; k++ )
	    minroad[found][k] = road[k];
    }
    else
    {
	for( c = 1; c <= N; c++ )
	    if( map_matrix[s][c] && !incl[c]&& ( len == 0 || clen+map_matrix[s][c] <= len ) )
	    {
		road[p] = c; incl[c] = 1; clen = clen + map_matrix[s][c];
		step ( c, f, p+1, vect, N );
		incl[c] = 0; road[p] = 0; clen = clen - map_matrix[s][c];
	    }
    }
}

//**********************************************************
//* Detecting the figures for fill			   *
//**********************************************************
bool ShapeElFigure::inundation( const QPointF &point, int N, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;

    found = false;
    inundationPath = newPath;
    bool flag_push_back;
    QVector<int> work_sort;
    QVector<int> inundation_fig_num;
    for( int i = 0; i < 2*shapeItems.size()+1; i++ )
	work_sort.push_back(0);
    int i = 1;
    do
    {
	found = 0;
	minroad.clear();
	road.clear();
	incl.clear();
	for( int k = 0; k < 2*shapeItems.size()+1; k++ )
	{
	    QVector<int> el;
	    for( int z = 0; z < 2*shapeItems.size()+1; z++ )  el.push_back(0);
	    minroad.push_back(el);
	}
	for( int k = 0; k < 2*shapeItems.size()+1; k++ )
	{
	    road.push_back(0);
	    incl.push_back(0);
	}
	road[1] = i; incl[i] = 0;
	len = 0; clen = 0;
	step( i, i, 2, vect, N );
	for( int i_found = 1; i_found <= found; i_found++ )
	{
	    inundationPath = newPath;
	    for( int k = 1; k <= minroad[i_found][0]+1; k++ )
		for( int j = 0; j < shapeItems.size(); j++ )
		    if( (shapeItems[j].n1 == vect[minroad[i_found][k]] && shapeItems[j].n2 == vect[minroad[i_found][k+1]]) ||
			(shapeItems[j].n1 == vect[minroad[i_found][k+1]] && shapeItems[j].n2 == vect[minroad[i_found][k]]) )
		    { inundation_fig_num.push_back(j); break; }
	    inundationPath = createInundationPath( inundation_fig_num, pnts, w );
	    inundation_fig_num.clear();
	    if( inundationPath.contains(point) )
		for( int i_m = 1; i_m <= minroad[i_found][0]+1; i_m++ )
		    if( work_sort[0] > minroad[i_found][0] || !work_sort[0] )
			for( int i_p = 0; i_p <= minroad[i_found][0]+1; i_p++ )
			    work_sort[i_p] = minroad[i_found][i_p];
	}
	i++;
    }
    while( i <= N );
    inundationPath = newPath;
    for( int k = 1; k <= work_sort[0]; k++ )

	for( int j = 0; j < shapeItems.size(); j++ )
	if( (shapeItems[j].n1 == vect[work_sort[k]] && shapeItems[j].n2 == vect[work_sort[k+1]]) ||
	    (shapeItems[j].n1 == vect[work_sort[k+1]] && shapeItems[j].n2 == vect[work_sort[k]]) )
	{
		flag_push_back = true;
		for( int p = 0; p < inundation_fig_num.size(); p++ )
		    if( (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
			(shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1) )
			{
			    flag_push_back = false;
			    if( (shapeItems[inundation_fig_num[p]].type == 2 && shapeItems[j].type == 1) && (inundation_fig_num[p] != j) )
				inundation_fig_num[p] = j;
			    if( (shapeItems[inundation_fig_num[p]].type == 3 && shapeItems[j].type == 1) && (inundation_fig_num[p] != j) )
				inundation_fig_num[p] = j;
			    if( (shapeItems[inundation_fig_num[p]].type == 2 && shapeItems[j].type == 3) && (inundation_fig_num[p] != j) )
				inundation_fig_num[p] = j;
			}
		if( flag_push_back )
		    inundation_fig_num.push_back(j);
	}
    work_sort.clear();
    if( inundation_fig_num.size() > 0 )
    {
	inundationPath = createInundationPath( inundation_fig_num, pnts, w );
	inundation_fig_num = inundationSort( inundationPath, inundation_fig_num, w);
	if( fTransl ) inundationPath = createInundationPath( inundation_fig_num, pnts, w );
	for( int i = 0; i < inundation_fig_num.size(); i++ )
	    inundation_vector.push_back(0);
	inundation_vector = inundation_fig_num;
    }
    return true;
}

QVector<int> ShapeElFigure::inundationSort( const QPainterPath &inundationPath, QVector<int> &inundation_fig_num, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    PntMap &pnts = elFD->shapePnts;

    for(int j = 0; j < shapeItems.size(); j++)
	for(int p = 0; p < inundation_fig_num.size(); p++)
	    if((shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
	       (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1))
	    {
		if(shapeItems[j].type == 2  && p != j)
		    if(inundationPath.contains(scaleRotate(pnts[shapeItems[j].n4],w)))
			inundation_fig_num[p] = j;
		if(shapeItems[j].type == 3 && p != j && shapeItems[inundation_fig_num[p]].type != 2)
		    if(inundationPath.contains(scaleRotate(pnts[shapeItems[j].n4],w)) &&
			inundationPath.contains(scaleRotate(pnts[shapeItems[j].n3],w)))
			inundation_fig_num[p] = j;
	    }
    return inundation_fig_num;
}

//**********************************************************
//* Detecting the figures, which count <= 2, for filling   *
//**********************************************************
bool ShapeElFigure::inundation1_2( const QPointF &point, int number, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems;
    PntMap &pnts = elFD->shapePnts;

    QPainterPath inundationPath_1_2;
    QVector<int> in_fig_num;
    //for( int i = 0; i < shapeItems.size(); i++ )
    for( int i = shapeItems.size()-1; i >= 0; i-- )
    {
	if( shapeItems[i].type == 2 )
	    if(scaleRotate(pnts[shapeItems[i].n1],w).toPoint() == scaleRotate(pnts[shapeItems[i].n2],w).toPoint())
	    {
		if( in_fig_num.size() ) in_fig_num.clear();
		in_fig_num.push_back(i);
		inundationPath_1_2 = newPath;
		inundationPath_1_2 = createInundationPath( in_fig_num, pnts, w );
		if( inundationPath_1_2.contains(point) )
		{
		    for( int i = 0; i < inundItems.size(); i++ )
			if( inundItems[i].path == inundationPath_1_2 )
			{
			    inundItems.remove(i);
			    paintImage(w);
			    w->repaint();
			    return true;
			}
		    if(number == -1) inundItems.push_back(inundationItem(inundationPath_1_2,in_fig_num,SpI_DefFill,SpI_DefFillImg));
		    else
		    {
			if(!fTransl)
			{
			    inundation_vector = in_fig_num;
			    inundationPath = inundationPath_1_2;
			}
			else
			{
			    inundItems[number].path = inundationPath_1_2;
			    inundItems[number].n = in_fig_num;
			}
		    }
		    return true;
		}
	    }
	if( shapeItems[i].type == 2 || shapeItems[i].type == 3 )
	{
	    //for( int j = 0; j < shapeItems.size(); j++ )
	    for( int j = shapeItems.size()-1; j >= 0; j-- )
	    {
		inundationPath_1_2 = newPath;
		if( in_fig_num.size() ) in_fig_num.clear();
		if( i != j )
		    if( (shapeItems[i].n1 == shapeItems[j].n1 && shapeItems[i].n2 == shapeItems[j].n2) ||
			(shapeItems[i].n1 == shapeItems[j].n2 && shapeItems[i].n2 == shapeItems[j].n1) )
		    {
			in_fig_num.push_back(i);
			in_fig_num.push_back(j);
			inundationPath_1_2 = createInundationPath(in_fig_num, pnts, w);
			if(inundationPath_1_2.contains(point))
			{
			    for( int i = 0; i < inundItems.size(); i++ )
				if( inundItems[i].path == inundationPath_1_2 )
				{
				    inundItems.remove(i);
				    paintImage(w);
				    w->repaint();
				    return true;
				}
			    if(number == -1) inundItems.push_back(inundationItem(inundationPath_1_2,in_fig_num,SpI_DefFill,SpI_DefFillImg));
			    else {
				if(!fTransl)
				{
				    inundation_vector = in_fig_num;
				    inundationPath = inundationPath_1_2;
				}
				else
				{
				    inundItems[number].path = inundationPath_1_2;
				    inundItems[number].n = in_fig_num;
				}
			    }
			    return true;
			}
		    }
	     }
	}
    }
    return false;
}

QPointF ShapeElFigure::scaleRotate( const QPointF &point, WdgView *w, int8_t toScale, int8_t toTrans )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QPointF rpnt = point;

    if(toTrans > 0 || (toTrans < 0 && fTransl)) {
	QPointF center = (toScale > 0 || (toScale < 0 && fTransl)) ?
	    QPointF((w->sizeF().width())/(2*w->xScale(true)), (w->sizeF().height())/(2*w->yScale(true))).toPoint() :
	    QPointF(w->sizeF().width()/2, w->sizeF().height()/2).toPoint();

	rpnt = rpnt - center;
	if(elFD->mirror) rpnt.setX(-rpnt.x());	//Mirror
	rpnt = rotate(rpnt, elFD->orient);	//Rotate
	rpnt = rpnt + center;
    }
    //Scale
    if(toScale > 0 || (toScale < 0 && fTransl)) rpnt = QPointF(rpnt.x()*w->xScale(true), rpnt.y()*w->yScale(true));

    //Append remnant
    //!!!!: Disabled now for mostly prevent flaws between inundations.
    //QPointF add((w->posF().x()+0.5)-floor(w->posF().x()+0.5)-0.5, (w->posF().y()+0.5)-floor(w->posF().y()+0.5)-0.5);

    return rpnt;// + add;
}

QPointF ShapeElFigure::unScaleRotate( const QPointF &point, WdgView *w, int8_t toScale, int8_t toTrans )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;

    //Append remnant
    //!!!!: Disabled now for mostly prevent flaws between inundations.
    //QPointF sub((w->posF().x()+0.5)-floor(w->posF().x()+0.5)-0.5, (w->posF().y()+0.5)-floor(w->posF().y()+0.5)-0.5);
    QPointF rpnt = point;// - sub;

    //Scale
    if(toScale > 0 || (toScale < 0 && fTransl)) rpnt = QPointF(rpnt.x()/w->xScale(true), rpnt.y()/w->yScale(true));
    if(toTrans > 0 || (toTrans < 0 && fTransl)) {
	QPointF center = (toScale > 0 || (toScale < 0 && fTransl)) ?
	    QPointF((w->sizeF().width())/(2*w->xScale(true)), (w->sizeF().height())/(2*w->yScale(true))).toPoint() :
	    QPointF(w->sizeF().width()/2, w->sizeF().height()/2).toPoint();

	rpnt = rpnt - center;
	rpnt = rotate(rpnt, 360-elFD->orient);	//Rotate
	if(elFD->mirror) rpnt.setX(-rpnt.x());	//Mirror
	rpnt = rpnt + center;
    }

    return rpnt;
}

/******************************************************************************
* paintImage() - Paint the result shape image for follow paint to display     *
* !!!!: Needs for next observing to flaws between inundations, mostly for     *
*  widget "Rounded rectangle(valuable)" rotation 45 deg and scaling 1.1       *
*******************************************************************************/
void ShapeElFigure::paintImage( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundItems = elFD->inundItems;
    PntMap	&pnts = elFD->shapePnts;
    WidthMap	&widths = elFD->shapeWidths;
    ColorMap	&colors = elFD->shapeColors;
    ImageMap	&images = elFD->shapeImages;
    StyleMap	&styles = elFD->shapeStyles;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    elFD->pictObj = QPixmap(w->width(), w->height());
    //elF-D>pictObj = QImage(w->width(), w->height(), QImage::Format_ARGB32_Premultiplied);
    double scaleW = vmin(w->xScale(true), w->yScale(true));
    elFD->pictObj.fill(Qt::transparent);
    QPainter pnt(&elFD->pictObj);

    //Prepare draw area
    int margin = elFD->geomMargin;
    QRect draw_area = w->rect().adjusted(0, 0, -2*margin, -2*margin);
    pnt.setWindow(draw_area);
    pnt.setViewport(w->rect().adjusted(margin,margin,-margin,-margin));
    vector<int> shape_inund_all;

    //Drawing all fills(inundations)
    for(int i = 0; i < inundItems.size(); i++) {
	QImage img;
	string backimg = w->resGet(images[inundItems[i].brushImg]);
	img.loadFromData((const uchar*)backimg.data(), backimg.size());
	if(shapeItems.size() == 0 || ((inundItems[i].path == newPath) && img.isNull())) continue;

	//printf("TEST 10: %d: '%s'\n", i, w->id().c_str());

	// Sorting the figures in each fill(inundation)
	QVector<int> n;
	n = inundItems[i].n;
	std::sort(n.begin(), n.end());

	// Making the array of the figures to be drawn before the each fill
	vector<int> draw_before;
	bool fl_numb;
	for(int k = 0; k < shapeItems.size(); k++) {
	    fl_numb = false;
	    for(int j = 0; !fl_numb && j < n.size(); j++) fl_numb = (k >= n[j]);
	    if(!fl_numb) draw_before.push_back(k);
	}
	//  Drawing the figures and push_bask them into the array of the already drawn figures
	bool flag_dr;
	for(unsigned k = 0; k < draw_before.size(); k++) {
	    flag_dr = true;
	    for(unsigned j = 0; flag_dr && j < shape_inund_all.size(); j++) flag_dr = (draw_before[k] != shape_inund_all[j]);
	    // If the figure is out of this array, then draw it (it is the figures which are lower than the current fill)
	    if(!flag_dr) continue;

	    const ShapeItem &cShIt = shapeItems[draw_before[k]];
	    shape_inund_all.push_back(k);
	    if(widths[cShIt.borderWidth] > 0.01) {
		int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.borderWidth])));
		pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
		pnt.setBrush(QBrush(colors[cShIt.lineColor],Qt::SolidPattern));
		pnt.setPen(QPen(colors[cShIt.borderColor],curWdth,styles[cShIt.style],Qt::FlatCap,Qt::MiterJoin));
		pnt.drawPath(cShIt.path);
	    }
	    else if(widths[cShIt.borderWidth] >= 0 && fabs(widths[cShIt.borderWidth]-0) < 0.01) {
		int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.width])));
		pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
		pnt.setBrush(Qt::NoBrush);
		pnt.setPen(QPen(colors[cShIt.lineColor],curWdth,styles[cShIt.style],((curWdth<3)?Qt::SquareCap:Qt::FlatCap),Qt::RoundJoin));
		pnt.drawPath(cShIt.pathSimple);
	    }
	}
	if(!img.isNull()) {
	    QPainterPath in_path, in_path_rot;
	    in_path = newPath;

	    // Building the scaled and unrotated inundation path
	    QPointF p1;
	    PntMap tmp_pnts;
	    QVector<int> number_vector;
	    bool fl_;
	    for(int p = 0; p < inundItems[i].n.size(); p++) {
		const ShapeItem &cShIt = shapeItems[inundItems[i].n[p]];
		switch(cShIt.type) {
		    case ShT_Line:
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n1 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n1], w, true, true);
			    tmp_pnts[cShIt.n1] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n1);
			}
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n2 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n2], w, true, true);
			    tmp_pnts[cShIt.n2] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n2);
			}
			break;
		    case ShT_Arc:
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n1 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n1], w, true, true);
			    tmp_pnts[cShIt.n1] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n1);
			}
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n2 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n2], w, true, true);
			    tmp_pnts[cShIt.n2] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n2);
			}

			tmp_pnts[cShIt.n3] = unScaleRotate(scaleRotate(pnts[cShIt.n3],w,true,true), w, false, true);
			tmp_pnts[cShIt.n4] = unScaleRotate(scaleRotate(pnts[cShIt.n4],w,true,true), w, false, true);
			tmp_pnts[cShIt.n5] = unScaleRotate(scaleRotate(pnts[cShIt.n5],w,true,true), w, false, true);
			break;
		    case ShT_Bezier:
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n1 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n1], w, true, true);
			    tmp_pnts[cShIt.n1] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n1);
			}
			fl_ = false;
			for(int k = 0; !fl_ && k < number_vector.size(); k++) fl_ = (cShIt.n2 == number_vector[k]);
			if(!fl_) {
			    p1 = scaleRotate(pnts[cShIt.n2], w, true, true);
			    tmp_pnts[cShIt.n2] = unScaleRotate(p1, w, false, true);
			    number_vector.push_back(cShIt.n2);
			}
			tmp_pnts[cShIt.n3] = unScaleRotate(scaleRotate(pnts[cShIt.n3],w,true,true), w, false, true);
			tmp_pnts[cShIt.n4] = unScaleRotate(scaleRotate(pnts[cShIt.n4],w,true,true), w, false, true);
			break;
		}
	    }
	    number_vector.clear();
	    fTransl = false;
	    in_path = createInundationPath(inundItems[i].n, tmp_pnts, w);
	    fTransl = true;
	    tmp_pnts.clear();

	    QPointF pnt_ = in_path.pointAtPercent(0);
	    double xMax = pnt_.x(), xMin = pnt_.x(), yMax = pnt_.y(), yMin = pnt_.y();
	    double t = 0.01;
	    do {
		pnt_ = in_path.pointAtPercent(t);
		xMin = vmin(xMin, pnt_.x()); yMin = vmin(yMin, pnt_.y());
		xMax = vmax(xMax, pnt_.x()); yMax = vmax(yMax, pnt_.y());
		t += 0.01;
	    } while(t < 1);
	    //printf("TEST 11a: min[%g,%g]; max[%g,%g]\n", xMin, yMin, xMax, yMax);
	    xMin = rRnd(xMin, POS_PREC_DIG, true); yMin = rRnd(yMin, POS_PREC_DIG, true);
	    xMax = rRnd(xMax, POS_PREC_DIG, true); yMax = rRnd(yMax, POS_PREC_DIG, true);
	    //xMin = floor(xMin); xMax = ceil(xMax); yMin = floor(yMin); yMax = ceil(yMax);

	    in_path_rot = createInundationPath(inundItems[i].n, pnts, w);
	    pnt_ = in_path_rot.pointAtPercent(0);
	    double xMax_rot = pnt_.x(), xMin_rot = pnt_.x(), yMax_rot = pnt_.y(), yMin_rot = pnt_.y();
	    t = 0.01;
	    do {
		pnt_ = in_path_rot.pointAtPercent(t);
		xMin_rot = vmin(xMin_rot, pnt_.x()); yMin_rot = vmin(yMin_rot, pnt_.y());
		xMax_rot = vmax(xMax_rot, pnt_.x()); yMax_rot = vmax(yMax_rot, pnt_.y());
		t += 0.01;
	    } while(t < 1);
	    //printf("TEST 11b: minR[%g,%g]; maxR[%g,%g]\n", xMin_rot, yMin_rot, xMax_rot, yMax_rot);
	    xMin_rot = rRnd(xMin_rot, POS_PREC_DIG, true); yMin_rot = rRnd(yMin_rot, POS_PREC_DIG, true);
	    xMax_rot = rRnd(xMax_rot, POS_PREC_DIG, true); yMax_rot = rRnd(yMax_rot, POS_PREC_DIG, true);
	    //xMin_rot = floor(xMin_rot); xMax_rot = ceil(xMax_rot); yMin_rot = floor(yMin_rot); yMax_rot = ceil(yMax_rot);

	    // Scaling image for filling
	    img = img.scaled(QSize((int)rRnd(xMax-xMin)+1,(int)rRnd(yMax-yMin)+1), Qt::IgnoreAspectRatio);//, Qt::SmoothTransformation);

	    //printf("TEST 11: min[%g,%g]; minR[%g,%g]; max[%g,%g]; maxR[%g,%g]; pat[%d,%d]\n", xMin, yMin, xMin_rot, yMin_rot, xMax, yMax, xMax_rot, yMax_rot, img.width(), img.height());

	    // Creating the composition of the fill color and fill image
	    /*QPainter img_pnt(&img);
	    img_pnt.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	    QPixmap clr_img(img.size());
	    clr_img.fill(colors[inundItems[i].brush]);
	    img_pnt.drawPixmap(0, 0, clr_img);*/
	    QImage clr_img(elFD->pictObj.size(), QImage::Format_Mono);
	    clr_img.fill(0);
	    QPainter img_pnt(&clr_img);
	    img_pnt.setBrush(QBrush(Qt::white,Qt::SolidPattern));
	    img_pnt.setPen(Qt::NoPen);
	    img_pnt.drawPath(in_path_rot);
	    int im_x, im_y;
	    QColor color;
	    double alpha, alpha_rez, color_r, color_g, color_b;
	    double alpha_col = (double)colors[inundItems[i].brush].alpha()/255;
	    QRgb rgb;
	    QPointF drw_pnt;
	    QPoint drw_pnt1;
	    QPen im_pen;
	    im_y = (int)yMin_rot;
	    //QImage draw_img( (int)rRnd( xMax_rot - xMin_rot )+1, (int)rRnd( yMax_rot - yMin_rot )+1, QImage::Format_ARGB32_Premultiplied  );
	    QImage draw_img(w->width(), w->height(), QImage::Format_ARGB32_Premultiplied);
	    draw_img.fill(0);
	    QPainter draw_pnt(&draw_img);

	    // Calculating the resulting color of the image and drawing the scaled and rotated points of it into the inundation path
	    do {
		im_x = (int)xMin_rot;
		do {
		    if(im_x >= 0 && im_x < clr_img.width() && im_y >= 0 && im_y < clr_img.height() &&
			clr_img.pixel(QPoint(im_x,im_y)) == qRgb(255,255,255))
		    {
			drw_pnt = unScaleRotate(QPoint(im_x,im_y), w, false, true);
			drw_pnt1 = QPoint((int)rRnd(drw_pnt.x()-xMin,POS_PREC_DIG,true), (int)rRnd(drw_pnt.y()-yMin,POS_PREC_DIG,true));
			if(img.valid(drw_pnt1)) {
			    rgb = img.pixel(drw_pnt1);
			    alpha = (double)((rgb>>24)&0xff)/255;
			    color_r = alpha*((rgb>>16)&0xff) + (1-alpha)*alpha_col*colors[inundItems[i].brush].red();
			    color_g = alpha*((rgb>>8)&0xff) + (1-alpha)*alpha_col*colors[inundItems[i].brush].green();
			    color_b = alpha*(rgb&0xff) + (1-alpha)*alpha_col*colors[inundItems[i].brush].blue();
			    alpha_rez = (1-alpha_col) * (1-alpha);
			    im_pen.setColor(QColor(color_r,color_g,color_b,rRnd(255*(1-alpha_rez),POS_PREC_DIG,true)));
			    draw_pnt.setPen(im_pen);
			    drw_pnt = scaleRotate(drw_pnt, w, false, true);
			    drw_pnt1 = QPoint((int)rRnd(drw_pnt.x(),POS_PREC_DIG,true), (int)rRnd(drw_pnt.y(),POS_PREC_DIG,true));
			    draw_pnt.drawPoint(drw_pnt1);
			}
		    }
		    im_x++;
		} while(im_x > xMin_rot && im_x <= xMax_rot);
		im_y++;
	    } while(im_y > yMin_rot && im_y <= yMax_rot);
	    pnt.drawImage(QPoint(), draw_img);
	}
	else {
	    pnt.setBrush(colors[inundItems[i].brush]);
	    pnt.setPen(Qt::NoPen);
	    pnt.drawPath(inundItems[i].path);
	}

	// Drawing the fills' figures
	for(int j = 0; j < n.size(); j++) {
	    //  Making the resulting arrary of all figures which take part in all fills(inundations)
	    const ShapeItem &cShIt = shapeItems[n[j]];
	    shape_inund_all.push_back(n[j]);
	    if(widths[cShIt.borderWidth] > 0.01) {
		int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.borderWidth])));
		pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
		pnt.setBrush(QBrush(colors[cShIt.lineColor],Qt::SolidPattern));
		pnt.setPen(QPen(colors[cShIt.borderColor],curWdth,styles[cShIt.style],Qt::FlatCap,Qt::MiterJoin));
		pnt.drawPath(cShIt.path);
	    }
	    else if(widths[cShIt.borderWidth] >= 0 && fabs(widths[cShIt.borderWidth] - 0) < 0.01) {
		int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.width])));
		pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
		pnt.setBrush(Qt::NoBrush);
		pnt.setPen(QPen(colors[cShIt.lineColor],curWdth,styles[cShIt.style],((curWdth<3)?Qt::SquareCap:Qt::FlatCap),Qt::RoundJoin));
		pnt.drawPath(cShIt.pathSimple);
	    }
	}
    }
    bool flag_draw;

    //pnt.setRenderHint(QPainter::Antialiasing);
    //Drawing all el_figures
    // Checking if the figure is inside the array of figures which take part in all fills
    for(int k = 0; k < shapeItems.size(); k++) {
	flag_draw = true;
	for(unsigned j = 0; flag_draw && j < shape_inund_all.size(); j++) flag_draw = (k!=shape_inund_all[j]);
	//  If the figure is out of this array, then draw it (it is the figures which take part in none fill)
	if(!flag_draw)	continue;
	const ShapeItem &cShIt = shapeItems[k];
	if(widths[cShIt.borderWidth] > 0.01) {
	    int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.borderWidth])));
	    pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
	    pnt.setBrush(QBrush(colors[cShIt.lineColor],Qt::SolidPattern));
	    pnt.setPen(QPen(colors[cShIt.borderColor],curWdth,styles[cShIt.style],Qt::FlatCap,Qt::MiterJoin));
	    pnt.drawPath(cShIt.path);
	}
	else if(widths[cShIt.borderWidth] >= 0 && fabs(widths[cShIt.borderWidth]) < 0.01) {
	    int curWdth = (int)rRnd(vmin(1000,vmax(1,scaleW*widths[cShIt.width])));
	    pnt.setRenderHint(QPainter::Antialiasing, (curWdth>=2));
	    pnt.setBrush(Qt::NoBrush);
	    pnt.setPen(QPen(colors[cShIt.lineColor],curWdth,styles[cShIt.style],((curWdth<3)?Qt::SquareCap:Qt::FlatCap),Qt::RoundJoin));
	    pnt.drawPath(cShIt.pathSimple);
	}
    }

    //Drawing all rects for choosen el_figures
    pnt.setRenderHint(QPainter::Antialiasing, false);
    if(devW && devW->edit()) {
	for(int k = 0; k < rectItems.size(); k++) {
	    pnt.setBrush(rectItems[k].brush);
	    pnt.setPen(rectItems[k].pen);
	    pnt.drawPath(rectItems[k].path);
	}
	pnt.setBrush(Qt::NoBrush);
	pnt.setPen(QPen(Qt::gray,2,Qt::SolidLine,Qt::FlatCap,Qt::RoundJoin));
	pnt.setBrush(QBrush(QColor(127,127,127,127),Qt::SolidPattern));
	ellipse_draw_startPath.setFillRule(Qt::WindingFill);
	pnt.drawPath(ellipse_draw_startPath);
	ellipse_draw_endPath.setFillRule(Qt::WindingFill);
	pnt.drawPath(ellipse_draw_endPath);
	ellipse_draw_startPath = newPath;
	ellipse_draw_endPath = newPath;
    }
    pnt.end();
}

//**********************************************************
//* Building the path for fill				   *
//**********************************************************
QPainterPath ShapeElFigure::createInundationPath( const QVector<int> &in_fig_num, PntMap &pnts, WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;

    if(in_fig_num.size() < 2 && !(in_fig_num.size() == 1 && shapeItems[in_fig_num[0]].type == ShT_Arc)) return newPath;

    QPainterPath path = newPath;
    QPointF rotArc, scRtTmp, scRtEnd;
    int flag = -1, in_index = -1;
    bool flag_n1, flag_n2, flag_break;
    double arc_a, arc_b, t_start, t_end, t, ang;
    QLineF line1, line2;

    const ShapeItem &curShIt = shapeItems[in_fig_num[0]];
    if(curShIt.n1 < curShIt.n2) {
	scRtTmp = scaleRotate(pnts[curShIt.n1], w);
	scRtTmp = QPointF(rRnd(scRtTmp.x(),POS_PREC_DIG,true), rRnd(scRtTmp.y(),POS_PREC_DIG,true));
	//printf("TEST 100: Beg fig=%d(type=%d) pnt[%g,%g]\n", in_fig_num[0], curShIt.type, scRtTmp.x(), scRtTmp.y());
	path.moveTo(scRtTmp);
	scRtEnd = scaleRotate(pnts[curShIt.n2], w);
	scRtEnd = QPointF(rRnd(scRtEnd.x(),POS_PREC_DIG,true), rRnd(scRtEnd.y(),POS_PREC_DIG,true));
	//printf("TEST 101: End fig=%d(type=%d) pnt[%g,%g]\n", in_fig_num[0], curShIt.type, scRtEnd.x(), scRtEnd.y());
	switch(curShIt.type) {
	    case ShT_Line: path.lineTo(scRtEnd);	break;
	    case ShT_Arc: {
		if((flag_angle_temp && fabs(w->xScale(true)-1) < 0.001 && fabs(w->yScale(true)-1) < 0.001 && fabs(elFD->orient) < 0.001) || !fTransl)
		{
		    line2 = QLineF(pnts[curShIt.n3], QPointF(pnts[curShIt.n3].x()+10,pnts[curShIt.n3].y()));
		    line1 = QLineF(pnts[curShIt.n3], pnts[curShIt.n5]);
		    ang = angle(line1, line2);
		    if(pnts[curShIt.n5].y() > pnts[curShIt.n3].y()) ang = 360-ang;
		}
		else ang = curShIt.angle_temp;
		ang += curShIt.ang_t;

		//!!!! RealRound function using removed by painting low quality arcs.
		// Possible need RealRound apply only to all source points or only to begin and end points.
		arc_a = length(scaleRotate(pnts[curShIt.n3],w), scaleRotate(pnts[curShIt.n5],w));
		arc_b = length(scaleRotate(pnts[curShIt.n3],w), scaleRotate(pnts[curShIt.n4],w));
		t_start = curShIt.ctrlPos4.x();
		t_end = curShIt.ctrlPos4.y();
		for(t = t_start; true; t += ARC_STEP) {
		    scRtTmp = scaleRotate(pnts[curShIt.n3], w);
		    rotArc = rotate(arc(vmin(t,t_end),arc_a,arc_b), ang);
		    path.lineTo(scRtTmp.x()+rotArc.x(), scRtTmp.y()-rotArc.y());
		    if(t >= t_end) break;
		}
		path.lineTo(scRtEnd);
		break;
	    }
	    case ShT_Bezier: {
		QPointF pCntr1 = scaleRotate(pnts[curShIt.n3], w),
			pCntr2 = scaleRotate(pnts[curShIt.n4], w);
		//pCntr1 = QPointF(rRnd(pCntr1.x(),POS_PREC_DIG,true), rRnd(pCntr1.x(),POS_PREC_DIG,true));
		//pCntr2 = QPointF(rRnd(pCntr2.x(),POS_PREC_DIG,true), rRnd(pCntr2.x(),POS_PREC_DIG,true));
		path.cubicTo(pCntr1, pCntr2, scRtEnd);
		break;
	    }
	}
	flag_n2 = true;
	flag_n1 = false;
    }
    else {
	scRtTmp = scaleRotate(pnts[curShIt.n2], w);
	scRtTmp = QPointF(rRnd(scRtTmp.x(),POS_PREC_DIG,true), rRnd(scRtTmp.y(),POS_PREC_DIG,true));
	//printf("TEST 110: Beg fig=%d(type=%d) pnt[%g,%g]\n", in_fig_num[0], curShIt.type, scRtTmp.x(), scRtTmp.y());
	path.moveTo(scRtTmp);
	scRtEnd = scaleRotate(pnts[curShIt.n1], w);
	scRtEnd = QPointF(rRnd(scRtEnd.x(),POS_PREC_DIG,true), rRnd(scRtEnd.y(),POS_PREC_DIG,true));
	//printf("TEST 111: End fig=%d(type=%d) pnt[%g,%g]\n", in_fig_num[0], curShIt.type, scRtEnd.x(), scRtEnd.y());
	switch(curShIt.type) {
	    case ShT_Line: path.lineTo(scRtEnd);	break;
	    case ShT_Arc: {
		if((flag_angle_temp && fabs(w->xScale(true)-1) < 0.001 && fabs(w->yScale(true)-1) < 0.001 && fabs(elFD->orient) < 0.001) || !fTransl)
		{
		    line2 = QLineF(scaleRotate(pnts[curShIt.n3],w), scaleRotate(QPointF(pnts[curShIt.n3].x()+10,pnts[curShIt.n3].y()),w));
		    line1 = QLineF(scaleRotate(pnts[curShIt.n3],w), scaleRotate(pnts[curShIt.n5],w));
		    ang = angle(line1, line2);
		    if(scaleRotate(pnts[curShIt.n5],w).y() > scaleRotate(pnts[curShIt.n3],w).y()) ang = 360 - ang;
		}
		else ang = curShIt.angle_temp;
		ang += curShIt.ang_t;

		//!!!! RealRound function using removed by painting low quality arcs.
		// Possible need RealRound apply only to all source points or only to begin and end points.
		arc_a = length(scaleRotate(pnts[curShIt.n3],w), scaleRotate(pnts[curShIt.n5],w));
		arc_b = length(scaleRotate(pnts[curShIt.n3],w), scaleRotate(pnts[curShIt.n4],w));
		t_start = curShIt.ctrlPos4.x();
		t_end = curShIt.ctrlPos4.y();
		for(t = t_end; true; t -= ARC_STEP) {
		    scRtTmp = scaleRotate(pnts[curShIt.n3], w);
		    rotArc = rotate(arc(vmax(t,t_start),arc_a,arc_b), ang);
		    path.lineTo(scRtTmp.x()+rotArc.x(), scRtTmp.y()-rotArc.y());
		    if(t <= t_start) break;
		}
		path.lineTo(scRtEnd);
		break;
	    }
	    case ShT_Bezier: {
		QPointF pCntr1 = scaleRotate(pnts[curShIt.n4], w),
			pCntr2 = scaleRotate(pnts[curShIt.n3], w);
		path.cubicTo(pCntr1, pCntr2, scRtEnd);
		break;
	    }
	}
	flag_n2 = false;
	flag_n1 = true;
    }
    for(int i = 0, k = 0; i < (in_fig_num.size()-1); i++) {
	if(flag_n2) {
	    flag_break = false;
	    for(int j = 0; j < in_fig_num.size(); j++) {
		if(k != j && shapeItems[in_fig_num[k]].n2 == shapeItems[in_fig_num[j]].n1) {
		    flag = 1;
		    in_index = in_fig_num[j];
		    k = j;
		    flag_break = true;
		}
		if(flag_break) break;
		if(k != j && shapeItems[in_fig_num[k]].n2 == shapeItems[in_fig_num[j]].n2) {
		    flag = 2;
		    in_index = in_fig_num[j];
		    k = j;
		    flag_break = true;
		}
		if(flag_break) break;
	    }
	}
	if(flag_n1) {
	    flag_break = false;
	    for(int j = 0; j < in_fig_num.size(); j++) {
		if(k != j && shapeItems[in_fig_num[k]].n1 == shapeItems[in_fig_num[j]].n1) {
		    flag = 1;
		    in_index = in_fig_num[j];
		    k = j;
		    flag_break = true;
		}
		if(flag_break) break;
		if(k != j && shapeItems[in_fig_num[k]].n1 == shapeItems[in_fig_num[j]].n2) {
		    flag = 2;
		    in_index = in_fig_num[j];
		    k = j;
		    flag_break = true;
		}
		if(flag_break) break;
	    }
	}
	if(in_index < 0 || in_index >= shapeItems.size()) continue;
	const ShapeItem &inShIt = shapeItems[in_index];
	switch(flag) {
	    case 1:
		scRtEnd = scaleRotate(pnts[inShIt.n2], w);
		scRtEnd = QPointF(rRnd(scRtEnd.x(),POS_PREC_DIG,true), rRnd(scRtEnd.y(),POS_PREC_DIG,true));
		//printf("TEST 101: Next fig=%d(type=%d) pnt[%g,%g]\n", in_index, inShIt.type, scRtEnd.x(), scRtEnd.y());
		switch(inShIt.type) {
		    case ShT_Line: path.lineTo(scRtEnd);	break;
		    case ShT_Arc: {
			if((flag_angle_temp && fabs(w->xScale(true)-1) < 0.001 && fabs(w->yScale(true)-1) < 0.001 && fabs(elFD->orient) < 0.001)
			    || !fTransl)
			{
			    line2 = QLineF(pnts[inShIt.n3], QPointF(pnts[inShIt.n3].x()+10,pnts[inShIt.n3].y()));
			    line1 = QLineF(pnts[inShIt.n3], pnts[inShIt.n5]);
			    ang = angle(line1, line2);
			    if(pnts[inShIt.n5].y() > pnts[inShIt.n3].y()) ang = 360 - ang;
			}
			else ang = inShIt.angle_temp;
			ang += inShIt.ang_t;

			//!!!! RealRound function using removed by painting low quality arcs.
			// Possible need RealRound apply only to all source points or only to begin and end points.
			arc_a = length(scaleRotate(pnts[inShIt.n3],w), scaleRotate(pnts[inShIt.n5],w));
			arc_b = length(scaleRotate(pnts[inShIt.n3],w), scaleRotate(pnts[inShIt.n4],w));
			t_start = inShIt.ctrlPos4.x();
			t_end = inShIt.ctrlPos4.y();
			for(t = t_start; true; t += ARC_STEP) {
			    scRtTmp = scaleRotate(pnts[inShIt.n3], w);
			    rotArc = rotate(arc(vmin(t,t_end),arc_a,arc_b), ang);
			    path.lineTo(scRtTmp.x()+rotArc.x(), scRtTmp.y()-rotArc.y());
			    if(t >= t_end) break;
			}
			path.lineTo(scRtEnd);
			break;
		    }
		    case ShT_Bezier: {
			QPointF pCntr1 = scaleRotate(pnts[inShIt.n3], w),
				pCntr2 = scaleRotate(pnts[inShIt.n4], w);
			path.cubicTo(pCntr1, pCntr2, scRtEnd);
			break;
		    }
		}
		flag_n2 = true;
		flag_n1 = false;
		break;
	    case 2:
		scRtEnd = scaleRotate(pnts[inShIt.n1], w);
		scRtEnd = QPointF(rRnd(scRtEnd.x(),POS_PREC_DIG,true), rRnd(scRtEnd.y(),POS_PREC_DIG,true));
		//printf("TEST 111: Next fig=%d(type=%d) pnt[%g,%g]\n", in_index, inShIt.type, scRtEnd.x(), scRtEnd.y());
		switch(inShIt.type) {
		    case ShT_Line: path.lineTo(scRtEnd);	break;
		    case ShT_Arc: {
			if((flag_angle_temp &&  fabs(w->xScale(true)-1) < 0.001 && fabs(w->yScale(true)-1) < 0.001 && fabs(elFD->orient-0) < 0.001)
			    || !fTransl)
			{
			    line2 = QLineF(pnts[inShIt.n3], QPointF(pnts[inShIt.n3].x()+10,pnts[inShIt.n3].y()));
			    line1 = QLineF(pnts[inShIt.n3], pnts[inShIt.n5]);
			    ang = angle(line1, line2);
			    if(pnts[inShIt.n5].y() > pnts[inShIt.n3].y()) ang = 360 - ang;
			}
			else ang = inShIt.angle_temp;
			ang += inShIt.ang_t;

			//!!!! RealRound function using removed by painting low quality arcs.
			// Possible need RealRound apply only to all source points or only to begin and end points.
			arc_a = length(scaleRotate(pnts[inShIt.n3],w), scaleRotate(pnts[inShIt.n5],w));
			arc_b = length(scaleRotate(pnts[inShIt.n3],w), scaleRotate(pnts[inShIt.n4],w));

			t_start = inShIt.ctrlPos4.x();
			t_end = inShIt.ctrlPos4.y();
			for(t = t_end; true; t -= ARC_STEP) {
			    scRtTmp = scaleRotate(pnts[inShIt.n3], w);
			    rotArc = rotate(arc(vmax(t,t_start),arc_a,arc_b), ang);
			    path.lineTo(scRtTmp.x()+rotArc.x(), scRtTmp.y()-rotArc.y());
			    if(t <= t_start) break;
			}
			path.lineTo(scRtEnd);
			break;
		    }
		    case ShT_Bezier: {
			QPointF pCntr1 = scaleRotate(pnts[inShIt.n4], w),
				pCntr2 = scaleRotate(pnts[inShIt.n3], w);
			path.cubicTo(pCntr1, pCntr2, scRtEnd);
			break;
		    }
		}
		flag_n2 = false;
		flag_n1 = true;
		break;
	}
    }

    return path;
}

//*************************************************
//* Second sideo of shape widget		  *
//*************************************************

//**********************************************************
//* Adding the point to data model			   *
//**********************************************************
int ElFigDt::appendPoint( const QPointF &pos, bool flag_down )
{
    int i = SpI_NullIt;
    if(flag_down) for(i = SpI_StatIts; shapePnts.find(i) != shapePnts.end(); ) i--;
    else for(i = 1; shapePnts.find(i) != shapePnts.end(); ) i++;
    shapePnts[i] = pos;

    return i;
}

//**********************************************************
//* Deleting the point from data model			   *
//**********************************************************
void ElFigDt::dropPoint( int num, int num_shape )
{
    if(num == SpI_NullIt) return;
    bool equal = false;
    for(int i = 0; !equal && i < shapeItems.size(); i++)
	equal = (i != num_shape && (num == shapeItems[i].n1 || num == shapeItems[i].n2 ||
				    num == shapeItems[i].n3 || num == shapeItems[i].n4 || num == shapeItems[i].n5));
    if(!equal)	shapePnts.erase(num);
}

//**********************************************************
//* Adding the width to the widths map			   *
//**********************************************************
int ElFigDt::appendWidth( const float &width, bool flag_down )
{
    int i = SpI_NullIt;
    if(flag_down) for(i = SpI_StatIts; shapeWidths.find(i) != shapeWidths.end(); ) i--;
    else for(i = 1; shapeWidths.find(i) != shapeWidths.end(); ) i++;
    shapeWidths[i] = width;

    return i;
}

//**********************************************************
//* Adding the style to the style map			   *
//**********************************************************
int ElFigDt::appendStyle( const Qt::PenStyle &style, bool flag_down )
{
    int i = SpI_NullIt;
    if(flag_down) for(i = SpI_StatIts; shapeStyles.find(i) != shapeStyles.end(); ) i--;
    else for(i = 1; shapeStyles.find(i) != shapeStyles.end(); ) i++;
    shapeStyles[i] = style;

    return i;
}

//**********************************************************
//* Adding the color to the color map			   *
//**********************************************************
int ElFigDt::appendColor( const QColor &color, bool flag_down )
{
    int i = SpI_NullIt;
    if(flag_down) for(i = SpI_StatIts; shapeColors.find(i) != shapeColors.end(); ) i--;
    else for(i = 1; shapeColors.find(i) != shapeColors.end(); ) i++;
    shapeColors[i] = color;

    return i;
}

//**********************************************************
//* Adding the image to the image map			   *
//**********************************************************
int ElFigDt::appendImage( const string &image, bool flag_down )
{
    int i = SpI_NullIt;
    if(flag_down) for(i = SpI_StatIts; shapeImages.find(i) != shapeImages.end(); ) i--;
    else for(i = 1; shapeImages.find(i) != shapeImages.end(); ) i++;
    shapeImages[i] = image;

    return i;
}

//**********************************************************
//* Making a static value the dynamic one and vise versa   *
//**********************************************************
void ElFigDt::dynamic( )
{
    ShapeElFigure *elF = (ShapeElFigure*) mod->getWdgShape("ElFigure");
    PntMap &pnts = shapePnts;
    WidthMap &widths = shapeWidths;
    ColorMap &colors = shapeColors;
    ImageMap &images = shapeImages;
    StyleMap &styles = shapeStyles;

    QPointF Temp;
    float temp_w, temp_bw;
    QColor temp_c, temp_bc, temp_fc;
    Qt::PenStyle temp_s;
    string temp_fi;
    int tmp, real;
    int num = elF->dyn_num;
    if(sender()->objectName() == "d_point" || sender()->objectName() == "s_point") num = 0;
    else if(sender()->objectName() == "d_width" || sender()->objectName() == "s_width") num = 1;
    else if(sender()->objectName() == "d_color" || sender()->objectName() == "s_color") num = 2;
    else if(sender()->objectName() == "d_border_width" || sender()->objectName() == "s_border_width") num = 3;
    else if(sender()->objectName() == "d_border_color" || sender()->objectName() == "s_border_color") num = 4;
    else if(sender()->objectName() == "d_style" || sender()->objectName() == "s_style") num = 5;
    else if(sender()->objectName() == "delete_fill") num = 6;
    else if(sender()->objectName() == "d_fill_color" || sender()->objectName() == "s_fill_color") num = 7;
    else if(sender()->objectName() == "d_fill_image" || sender()->objectName() == "s_fill_image") num = 8;
    else if(sender()->objectName() == "static") num = 9;

    tmp = -4;
    real = -4;
    switch(num)
    {
	case 0:	// Dynamic/static point
	{
	    if(elF->rect_dyn == SpI_NullIt || elF->index == SpI_NullIt)	break;
	    ShapeItem &newIt = shapeItems[elF->index];
	    switch(elF->rect_dyn)
	    {
		case 0:
		    tmp = newIt.n1;
		    if(!vrng(newIt.n1,-9,0)) real = appendPoint(pnts[newIt.n1], (newIt.n1>0));
		    break;
		case 1:
		    tmp = newIt.n2;
		    if(!vrng(newIt.n2,-9,0)) real = appendPoint(pnts[newIt.n2], (newIt.n2>0));
		    break;
		case 2:
		    tmp = newIt.n3;
		    if(!vrng(newIt.n3,-9,0)) real = appendPoint(pnts[newIt.n3], (newIt.n3>0));
		    break;
		case 3:
		    tmp = newIt.n4;
		    if(!vrng(newIt.n4,-9,0)) real = appendPoint(pnts[newIt.n4], (newIt.n4>0));
		    break;
		case 4:
		    tmp = newIt.n5;
		    if(!vrng(newIt.n5,-9,0)) real = appendPoint(pnts[newIt.n5], (newIt.n5>0));
		    break;
	    }
	    break;
	}
	case 1:	// Dynamic/static width
	    if(elF->index != SpI_NullIt)
	    {
		if((shapeItems[elF->index].width == SpI_DefLine || shapeItems[elF->index].width <= SpI_StatIts) && !elF->flag_def_stat)
		{
		    temp_w = widths[shapeItems[elF->index].width];
		    tmp = shapeItems[elF->index].width;
		    real = appendWidth(temp_w, false);
		}
		else if(shapeItems[elF->index].width > 0 || (shapeItems[elF->index].width == SpI_DefLine && elF->flag_def_stat))
		{
		    temp_w = widths[shapeItems[elF->index].width];
		    tmp = shapeItems[elF->index].width;
		    real = appendWidth(temp_w, true);
		}
	    }
	    break;
	case 2:	// Dynamic/static color

	    if(elF->index != SpI_NullIt)
	    {
		if((shapeItems[elF->index].lineColor == SpI_DefLine || shapeItems[elF->index].lineColor <= SpI_StatIts) && !elF->flag_def_stat)
		{
		    temp_c = colors[shapeItems[elF->index].lineColor];
		    tmp = shapeItems[elF->index].lineColor;
		    real = appendColor(temp_c, false);
		}
		else if(shapeItems[elF->index].lineColor > 0 || (shapeItems[elF->index].lineColor == SpI_DefLine && elF->flag_def_stat))
		{
		    temp_c = colors[shapeItems[elF->index].lineColor];
		    tmp = shapeItems[elF->index].lineColor;
		    real = appendColor(temp_c, true);
		}
	    }
	    break;
	case 3:
	    if(elF->index != -1)
	    {
		if( (shapeItems[elF->index].borderWidth == -6 || shapeItems[elF->index].borderWidth <= -10) && !elF->flag_def_stat )
		{
		    temp_bw = widths[shapeItems[elF->index].borderWidth];
		    tmp = shapeItems[elF->index].borderWidth;
		    real = appendWidth( temp_bw, false );
		}
		else if( shapeItems[elF->index].borderWidth > 0 || (shapeItems[elF->index].borderWidth == -6 && elF->flag_def_stat) )
		{
		    temp_bw = widths[shapeItems[elF->index].borderWidth];
		    tmp = shapeItems[elF->index].borderWidth;
		    real = appendWidth( temp_bw, true );
		}
	    }
	    break;
	case 4:
	    if( elF->index != -1 )
	    {
		if( (shapeItems[elF->index].borderColor == -6 || shapeItems[elF->index].borderColor <= -10) && !elF->flag_def_stat )
		{
		    temp_bc = colors[shapeItems[elF->index].borderColor];
		    tmp = shapeItems[elF->index].borderColor;
		    real = appendColor( temp_bc, false );
		}
		else if( shapeItems[elF->index].borderColor > 0 || (shapeItems[elF->index].borderColor == -6 && elF->flag_def_stat) )
		{
		    temp_bc = colors[shapeItems[elF->index].borderColor];
		    tmp = shapeItems[elF->index].borderColor;
		    real = appendColor( temp_bc, true );
		}
	    }
	    break;
	case 5:
	    if( elF->index != -1 )
	    {
		if( (shapeItems[elF->index].style == -5 || shapeItems[elF->index].style <= -10) && !elF->flag_def_stat )
		{
		    temp_s = styles[shapeItems[elF->index].style];
		    tmp = shapeItems[elF->index].style;
		    real = appendStyle( temp_s, false );
		}
		else if( shapeItems[elF->index].style > 0 || (shapeItems[elF->index].style == -5 && elF->flag_def_stat ) )
		{
		    temp_s = styles[shapeItems[elF->index].style];
		    tmp = shapeItems[elF->index].style;
		    real = appendStyle( temp_s, true );
		}
	    }
	    break;
	case 6:
	    if( elF->index == -1 && elF->fill_index != -1 )
	    {
		inundItems.remove(elF->fill_index);
		if( elF->flag_dyn_save )
		{
		    elF->shapeSave( w );
		    elF->paintImage( w );
		    w->update();
		}
	    }
	    break;
	case 7:
	    if( elF->index == -1 && elF->fill_index != -1 )
	    {
		if( inundItems[elF->fill_index].brush == -7 || inundItems[elF->fill_index].brush <= -10 )
		{
		    temp_fc = colors[inundItems[elF->fill_index].brush];
		    tmp = inundItems[elF->fill_index].brush;
		    real = appendColor( temp_fc, false );
		}
		else if( inundItems[elF->fill_index].brush > 0 )
		{
		    temp_fc = colors[inundItems[elF->fill_index].brush];
		    tmp = inundItems[elF->fill_index].brush;
		    real = appendColor( temp_fc, true );
		}
	    }
	    break;
	case 8:
	    if( elF->index == -1 && elF->fill_index != -1 )
	    {
		if( inundItems[elF->fill_index].brushImg == -5 || inundItems[elF->fill_index].brushImg <= -10 )
		{
		    temp_fi = images[inundItems[elF->fill_index].brushImg];
		    tmp = inundItems[elF->fill_index].brushImg;
		    real = appendImage( temp_fi, false );
		}
		else if( inundItems[elF->fill_index].brushImg > 0 )
		{
		    temp_fi = images[inundItems[elF->fill_index].brushImg];
		    tmp = inundItems[elF->fill_index].brushImg;
		    real = appendImage( temp_fi, true );
		}
	    }
	    break;
	case 9:
	    for( int i = 0; i < shapeItems.size(); i++ )
	    {
		    if( shapeItems[i].n1 > 0 )
		    {
			Temp = pnts[shapeItems[i].n1];
			tmp = shapeItems[i].n1;
			shapeItems[i].n1 = appendPoint(Temp, true);
			for( int j = 0; j < shapeItems.size(); j++ )
			    if( i != j )
			    {
				if( shapeItems[j].n1 == tmp ) shapeItems[j].n1 = shapeItems[i].n1;
				else if( shapeItems[j].n2 == tmp ) shapeItems[j].n2 = shapeItems[i].n1;
			    }
		    }
		    if( shapeItems[i].n2 > 0 )
		    {
			Temp = pnts[shapeItems[i].n2];
			tmp = shapeItems[i].n2;
			shapeItems[i].n2 = appendPoint(Temp, true);
			for( int j = 0; j < shapeItems.size(); j++ )
			    if( i != j )
			    {
				if( shapeItems[j].n1 == tmp ) shapeItems[j].n1 = shapeItems[i].n2;
				else if( shapeItems[j].n2 == tmp ) shapeItems[j].n2 = shapeItems[i].n2;
			    }
		    }
		    if( shapeItems[i].n3 > 0 )
		    {
			Temp = pnts[shapeItems[i].n3];
			shapeItems[i].n3 = appendPoint(Temp, true);
		    }
		    if( shapeItems[i].n4 > 0 )
		    {
			Temp = pnts[shapeItems[i].n4];
			shapeItems[i].n4 = appendPoint(Temp, true);
		    }
		    if( shapeItems[i].n5 > 0 )
		    {
			Temp = pnts[shapeItems[i].n5];
			shapeItems[i].n5 = appendPoint(Temp, true);
		    }
		    if( shapeItems[i].width > 0 )
			shapeItems[i].width = appendWidth(widths[shapeItems[i].width], true);
		    if( shapeItems[i].borderWidth > 0 )
			shapeItems[i].borderWidth = appendWidth(widths[shapeItems[i].borderWidth], true );
		    if( shapeItems[i].lineColor > 0 )
			shapeItems[i].lineColor = appendColor(colors[shapeItems[i].lineColor], true );
		    if( shapeItems[i].borderColor > 0 )
			shapeItems[i].borderColor = appendColor(colors[shapeItems[i].borderColor], true );
		    if( shapeItems[i].style > 0 )
			shapeItems[i].style = appendStyle(styles[shapeItems[i].style], true );
	    }
	    for( int i = 0; i < inundItems.size(); i++ )
	    {
		if( inundItems[i].brush > 0 )
		    inundItems[i].brush = appendColor(colors[inundItems[i].brush], true );
		if( inundItems[i].brushImg > 0 )
		    inundItems[i].brushImg = appendImage( images[inundItems[i].brushImg], true );
	    }
	    for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); )
	    {
		if(pi->first > 0 ) pnts.erase ( pi++ );
		else ++pi;
	    }
	    for( WidthMap::iterator pi = widths.begin(); pi != widths.end(); )
	    {
		if(pi->first  > 0 ) widths.erase ( pi++ );
		else ++pi;
	    }
	    for( ColorMap::iterator pi = colors.begin(); pi != colors.end(); )
	    {
		if(pi->first  > 0 ) colors.erase ( pi++ );
		else ++pi;
	    }
	    for( ImageMap::iterator pi = images.begin(); pi != images.end(); )
	    {
		if(pi->first  > 0 ) images.erase ( pi++ );
		else ++pi;
	    }
	    for( StyleMap::iterator pi = styles.begin(); pi != styles.end(); )
	    {
		if(pi->first  > 0 ) styles.erase ( pi++ );
		else ++pi;
	    }
	    if( elF->flag_dyn_save )
	    {
		elF->shapeSave( w );
		if( elF->rectItems.size() )
		{
		    elF->rectItems.clear();
		    elF->paintImage(w);
		    w->update();
		}
	    }
	    break;
    }

    if(((tmp <= SpI_StatIts || tmp == SpI_DefLine || tmp == SpI_DefBord || tmp == SpI_DefFill) && real > 0) ||
	  (tmp > 0 && real <= SpI_StatIts) || ((tmp == SpI_DefLine || tmp == SpI_DefBord || tmp == SpI_DefFill) && real <= SpI_StatIts))
    {
	bool upd = false;
	switch(num)
	{
	    case 0:	// Dynamic/static point
		for(int i = 0; i < shapeItems.size(); i++)
		{
		    if( shapeItems[i].n1 == tmp ) { shapeItems[i].n1 = real; elF->rectItems.clear(); elF->rect_num = -1; upd = true; }
		    if( shapeItems[i].n2 == tmp ) { shapeItems[i].n2 = real; elF->rectItems.clear(); elF->rect_num = -1; upd = true; }
		    else if( shapeItems[i].n3 == tmp ) { shapeItems[i].n3 = real; elF->rectItems.clear(); elF->rect_num = -1; upd = true; }
		    else if( shapeItems[i].n4 == tmp ) { shapeItems[i].n4 = real; elF->rectItems.clear(); elF->rect_num = -1; upd = true; }
		    else if( shapeItems[i].n5 == tmp ) { shapeItems[i].n5 = real; elF->rectItems.clear(); elF->rect_num = -1; upd = true; }
		}
		dropPoint(tmp, elF->index);
		break;
	    case 1:	// Dynamic/static width
	    {
		shapeItems[elF->index].width = real;
		bool fl_keep = false;
		if( tmp > 0 )
		    for( int p = 0; p < shapeItems.size(); p++ )
			if( p != elF->index && tmp == shapeItems[p].width )
			{ fl_keep = true; break; }
		if( tmp != -5 && !fl_keep ) widths.erase(tmp);
		break;
	    }
	    case 2:	// Dynamic/static color
	    {
		shapeItems[elF->index].lineColor = real;
		bool fl_keep = false;
		if(tmp > 0)
		    for(int p = 0; p < shapeItems.size(); p++)
			if(p != elF->index && tmp == shapeItems[p].lineColor)
			{ fl_keep = true; break; }
		if(tmp != SpI_DefLine && !fl_keep) colors.erase(tmp);
		break;
	    }
	    case 3:
	    {
		shapeItems[elF->index].borderWidth = real;
		bool fl_keep = false;
		if( tmp > 0 )
		    for( int p = 0; p < shapeItems.size(); p++ )
			if( p != elF->index && tmp == shapeItems[p].borderWidth )
			{ fl_keep = true; break; }
		if( tmp != -6 && !fl_keep ) widths.erase(tmp);
		break;
	    }
	    case 4:
	    {
		shapeItems[elF->index].borderColor = real;
		bool fl_keep = false;
		if( tmp > 0 )
		    for( int p = 0; p < shapeItems.size(); p++ )
			if( p != elF->index && tmp == shapeItems[p].borderColor )
			{ fl_keep = true; break; }
		if( tmp != -6 && !fl_keep ) colors.erase(tmp);
		break;
	    }
	    case 5:
	    {
		shapeItems[elF->index].style = real;
		bool fl_keep = false;
		if( tmp > 0 )
		    for( int p = 0; p < shapeItems.size(); p++ )
			if( p != elF->index && tmp == shapeItems[p].style )
			{ fl_keep = true; break; }
		if( tmp != -5 && !fl_keep ) styles.erase(tmp);
		break;
	    }
	    case 7:
		inundItems[elF->fill_index].brush = real;
		if( tmp != -7 )
		    colors.erase(tmp);
		break;
	    case 8:
		inundItems[elF->fill_index].brushImg = real;
		if( tmp != -5 )
		    images.erase(tmp);
		break;

	}
	if(elF->flag_dyn_save)
	{
	    elF->shapeSave(w);
	    if(elF->rectItems.size() || upd)
	    {
		elF->rectItems.clear();
		elF->paintImage(w);
		w->update();
	    }
	}
    }
    elF->index_array.clear();
    elF->rect_array.clear();
    elF->flag_arc_rect_3_4 = elF->flag_rect = false;
    elF->rect_num = -1;
}

void ElFigDt::properties( )
{
    ShapeElFigure *elF = (ShapeElFigure*) mod->getWdgShape("ElFigure");
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    if(devW) devW->setPrevEdExitFoc(true);
    bool flag_hld = true;
    QVector<int> items_array_holds, items_array_up;
    if(elF->flag_rect_items)
    {
	for(int i = 0; i < elF->index_array.size(); i++)
	    if(elF->index_array[i] != -1)
		items_array_holds.push_back(elF->index_array[i]);
	std::sort(items_array_holds.begin(), items_array_holds.end());
    }
    else items_array_holds.push_back(elF->index);

    PntMap &pnts = shapePnts;
    WidthMap *widths = &shapeWidths;
    ColorMap *colors = &shapeColors;
    ImageMap *images = &shapeImages;
    StyleMap *styles = &shapeStyles;
    QLabel *l_lb, *lb_lb, *p_lb, *x_lb, *y_lb, *p3_lb, *p4_lb, *p5_lb;
    QSpinBox *l_width, *lb_width;
    LineEditProp *l_color, *lb_color, *f_color;
    QLineEdit *f_image;
    QComboBox *l_style;
    QCheckBox *lw_check, *lc_check, *ls_check, *lbw_check, *lbc_check, *fc_check, *fi_check;
    QPushButton *lw_en, *lc_en, *ls_en, *lbw_en, *lbc_en, *fc_en, *fi_en;
    QPushButton *p1_c, *p2_c, *p3_c, *p4_c, *p5_c, *fc_c, *fi_c, *lw_c, *lc_c, *ls_c, *lbw_c, *lbc_c;
    QDoubleSpinBox *p1_x, *p1_y, *p2_x, *p2_y, *p3_x, *p3_y, *p4_x, *p4_y, *p5_x, *p5_y;
    QVector<int> inund_Rebuild;
    QImage ico_t;
    bool fl_n1 = false, fl_n2 = false, fl_n1Block = false, fl_n2Block = false, fl_appN1 = false, fl_appN2 = false;
    QString str_mess;
    if(items_array_holds.size() > 1 && sender()->objectName() != "Fill")
    {
	for(int i = 0; i < items_array_holds.size(); i++) str_mess += QString("'%1', ").arg(items_array_holds[i]);
	str_mess = QString(_("Properties for the %1 figures.")).arg(str_mess);
    }
    else str_mess = QString(_("Properties for the '%1' figure: '%2'.")).arg((elF->fill_index==-1)?elF->index:elF->fill_index).arg(sender()->objectName());
    if(!ico_t.load(TUIS::icoGet("edit",NULL,true).c_str())) ico_t.load(":/images/edit.png");
    InputDlg propDlg(w->mainWin(), QPixmap::fromImage(ico_t), str_mess,_("Elementary figure properties."),false,false);
    if(!ico_t.load(TUIS::icoGet("disable",NULL,true).c_str())) ico_t.load(":/images/disable.png");
    l_lb = new QLabel(_("Line:"),&propDlg);
    l_width = new QSpinBox(&propDlg);
    lw_en = new QPushButton(&propDlg); lw_check = new QCheckBox(&propDlg); lw_c = new QPushButton(&propDlg);
    l_color = new LineEditProp(&propDlg, LineEditProp::Color, false);
    lc_en = new QPushButton(&propDlg); lc_check = new QCheckBox(&propDlg); lc_c = new QPushButton(&propDlg);
    l_style = new QComboBox(&propDlg);
    ls_en = new QPushButton(&propDlg); ls_check = new QCheckBox(&propDlg); ls_c = new QPushButton(&propDlg);
    lb_lb = new QLabel(_("Border:"),&propDlg);
    lbw_en = new QPushButton(&propDlg); lb_width = new QSpinBox(&propDlg); lbw_c = new QPushButton(&propDlg);
    lbw_check = new QCheckBox(&propDlg);
    lb_color = new LineEditProp(&propDlg, LineEditProp::Color, false);
    lbc_en = new QPushButton(&propDlg); lbc_check = new QCheckBox(&propDlg); lbc_c = new QPushButton(&propDlg);
    p_lb = new QLabel(_("Points:"),&propDlg);
    x_lb = new QLabel(_("x"),&propDlg);
    y_lb = new QLabel(_("y"),&propDlg);
    p1_x = new QDoubleSpinBox(&propDlg); p1_y = new QDoubleSpinBox(&propDlg); p1_c = new QPushButton(&propDlg);
    p2_x = new QDoubleSpinBox(&propDlg); p2_y = new QDoubleSpinBox(&propDlg); p2_c = new QPushButton(&propDlg);
    p3_lb = new QLabel(_("Point 3:"),&propDlg);
    p3_x = new QDoubleSpinBox(&propDlg); p3_y = new QDoubleSpinBox(&propDlg); p3_c = new QPushButton(&propDlg);
    p4_lb = new QLabel(_("Point 4:"),&propDlg);
    p4_x = new QDoubleSpinBox(&propDlg); p4_y = new QDoubleSpinBox(&propDlg); p4_c = new QPushButton(&propDlg);
    p5_lb = new QLabel(_("Point 5:"),&propDlg);
    p5_x = new QDoubleSpinBox(&propDlg); p5_y = new QDoubleSpinBox(&propDlg); p5_c = new QPushButton(&propDlg);
    f_color = new LineEditProp(&propDlg, LineEditProp::Color, false);
    fc_en = new QPushButton(&propDlg); fc_check = new QCheckBox(&propDlg); fc_c = new QPushButton(&propDlg);
    f_image = new QLineEdit(&propDlg);
    fi_en = new QPushButton(&propDlg); fi_check = new QCheckBox(&propDlg); fi_c = new QPushButton(&propDlg);
    //> Creating the fills' properties dialog
    if(sender()->objectName() == "Fill")
    {
	l_lb->hide(); l_width->hide(); l_color->hide(); l_style->hide();
	lb_lb->hide(); lb_width->hide(); lb_color->hide();
	lw_check->hide(); lc_check->hide(); ls_check->hide();
	lbw_check->hide(); lbc_check->hide();
	lw_c->hide(); lc_c->hide(); ls_c->hide();
	lbw_c->hide(); lbc_c->hide();
	lw_en->hide(); lc_en->hide(); ls_en->hide();
	lbw_en->hide(); lbc_en->hide();
	p_lb->hide(); x_lb->hide(); y_lb->hide();
	p1_x->hide(); p1_y->hide(); p1_c->hide();
	p2_x->hide(); p2_y->hide(); p2_c->hide();
	p3_lb->hide(); p3_x->hide();p3_y->hide(); p3_c->hide();
	p4_lb->hide(); p4_x->hide();p4_y->hide(); p4_c->hide();
	p5_lb->hide(); p5_x->hide();p5_y->hide(); p5_c->hide();
	propDlg.edLay()->addWidget( fc_en, 2, 0 );
	fc_en->setIcon(QPixmap::fromImage(ico_t));
	fc_en->setToolTip(_("Include(checked)/Exclude(unchecked) the fill color from the properties list."));
	fc_en->setCheckable( true ); fc_en->setChecked(true);
	QLabel *fc_lb = new QLabel(_("Fill Color"),&propDlg);
	propDlg.edLay()->addWidget( fc_lb, 2, 1 );
	propDlg.edLay()->addWidget( f_color, 2, 2 );
	fc_check->setToolTip(_("Set the default fill color."));
	propDlg.edLay()->addWidget( fc_check, 2, 3 );
	connect( fc_en, SIGNAL(toggled(bool)), fc_lb, SLOT(setEnabled(bool)) );
	connect( fc_en, SIGNAL(toggled(bool)), f_color, SLOT(setVisible(bool)) );
	connect( fc_en, SIGNAL(toggled(bool)), fc_c, SLOT(setVisible(bool)) );
	connect( fc_en, SIGNAL(toggled(bool)), fc_check, SLOT(setVisible(bool)) );
	connect( fc_check, SIGNAL(toggled(bool)), f_color, SLOT(setDisabled(bool)) );
	connect( fc_check, SIGNAL(toggled(bool)), fc_c, SLOT(setDisabled(bool)) );
	propDlg.edLay()->addWidget( fi_en, 3, 0 );
	fi_en->setIcon(QPixmap::fromImage(ico_t));
	fi_en->setToolTip(_("Include(checked)/Exclude(unchecked) the fill image from the properties list."));
	fi_en->setCheckable( true ); fi_en->setChecked(true);
	QLabel *fi_lb = new QLabel(_("Fill Image"),&propDlg);
	propDlg.edLay()->addWidget( fi_lb, 3, 1 );
	propDlg.edLay()->addWidget( f_image, 3, 2 );
	fi_check->setToolTip(_("Set the default fill image."));
	propDlg.edLay()->addWidget( fi_check, 3, 3 );
	fc_c->setToolTip(_("Make the fill color Dynamic(checked)/Static(unchecked).")); fc_c->setCheckable( true );
	fc_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(fc_c, 2, 4);
	fi_c->setToolTip(_("Make the fill image Dynamic(checked)/Static(unchecked).")); fi_c->setCheckable( true );
	fi_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(fi_c, 3, 4);
	connect( fi_en, SIGNAL(toggled(bool)), fi_lb, SLOT(setEnabled(bool)) );
	connect( fi_en, SIGNAL(toggled(bool)), f_image, SLOT(setVisible(bool)) );
	connect( fi_en, SIGNAL(toggled(bool)), fi_c, SLOT(setVisible(bool)) );
	connect( fi_en, SIGNAL(toggled(bool)), fi_check, SLOT(setVisible(bool)) );
	connect(fi_check, SIGNAL(toggled(bool)), f_image, SLOT(setDisabled(bool)));
	connect(fi_check, SIGNAL(toggled(bool)), fi_c, SLOT(setDisabled(bool)));

	f_color->setValue((*colors)[inundItems[elF->fill_index].brush].name() + "-" +
			    QString(i2s( (*colors)[inundItems[elF->fill_index].brush].alpha() ).c_str()));
	if(inundItems[elF->fill_index].brush == -7) fc_check->setChecked(true);
	f_image->setText(QString( (*images)[inundItems[elF->fill_index].brushImg].c_str()));
	if(inundItems[elF->fill_index].brushImg == -5) fi_check->setChecked(true);
	if( inundItems[elF->fill_index].brush > 0 ) fc_c->setChecked(true);
	else fc_c->setChecked(false);
	if( inundItems[elF->fill_index].brushImg > 0 ) fi_c->setChecked(true);
	else fi_c->setChecked(false);
	propDlg.edLay()->setColumnMinimumWidth ( 2, 100 );
	propDlg.edLay()->setColumnStretch ( 2, 1 );
    }
    //> Creating the items' of properties dialog
    else
    {
	f_color->hide(); f_image->hide();
	fc_check->hide(); fi_check->hide();
	fc_c->hide(); fi_c->hide();
	fc_en->hide(); fi_en->hide();
	QFont lb_fnt;
	lb_fnt.setStyle(QFont::StyleItalic);
	lb_fnt.setBold(true);
	l_lb->setFont(lb_fnt); propDlg.edLay()->addWidget(l_lb, 2, 0);
	lw_en->setIcon(QPixmap::fromImage(ico_t));
	lw_en->setToolTip(_("Include(checked)/Exclude(unchecked) the line width from the properties list."));
	lw_en->setCheckable( true );
	propDlg.edLay()->addWidget( lw_en, 3, 0 ); lw_en->setChecked(true);
	propDlg.edLay()->setAlignment ( lw_en, Qt::AlignRight );
	QLabel *lw_lb = new QLabel(_("Line width"),&propDlg);
	propDlg.edLay()->addWidget( lw_lb, 3, 1);
	l_width->setRange(0, 99);
	propDlg.edLay()->addWidget(l_width, 3, 2);
	lw_check->setToolTip(_("Set the default line width."));
	propDlg.edLay()->addWidget(lw_check, 3, 3);
	lw_c->setToolTip(_("Make the line width Dynamic(checked)/Static(unchecked).")); lw_c->setCheckable( true );
	lw_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(lw_c, 3, 4);
	connect( lw_en, SIGNAL(toggled(bool)), lw_lb, SLOT(setEnabled(bool)) );
	connect( lw_en, SIGNAL(toggled(bool)), l_width, SLOT(setVisible(bool)) );
	connect( lw_en, SIGNAL(toggled(bool)), lw_c, SLOT(setVisible(bool)) );
	connect( lw_en, SIGNAL(toggled(bool)), lw_check, SLOT(setVisible(bool)) );
	connect(lw_check, SIGNAL(toggled(bool)), l_width, SLOT(setDisabled(bool)) );
	connect(lw_check, SIGNAL(toggled(bool)), lw_c, SLOT(setDisabled(bool)) );
	lc_en->setIcon(QPixmap::fromImage(ico_t));
	lc_en->setToolTip(_("Include(checked)/Exclude(unchecked) the line color from the properties list."));
	lc_en->setCheckable( true );
	propDlg.edLay()->addWidget( lc_en, 4, 0 ); lc_en->setChecked(true);
	propDlg.edLay()->setAlignment ( lc_en, Qt::AlignRight );
	QLabel *lc_lb = new QLabel(_("Line color"),&propDlg);
	propDlg.edLay()->addWidget( lc_lb, 4, 1 );
	propDlg.edLay()->addWidget( l_color, 4, 2 );
	lc_check->setToolTip(_("Set the default line color."));
	propDlg.edLay()->addWidget( lc_check, 4, 3 );
	lc_c->setToolTip(_("Make the line color Dynamic(checked)/Static(unchecked).")); lc_c->setCheckable( true );
	lc_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(lc_c, 4, 4);
	connect( lc_en, SIGNAL(toggled(bool)), lc_lb, SLOT(setEnabled(bool)) );
	connect( lc_en, SIGNAL(toggled(bool)), l_color, SLOT(setVisible(bool)) );
	connect( lc_en, SIGNAL(toggled(bool)), lc_c, SLOT(setVisible(bool)) );
	connect( lc_en, SIGNAL(toggled(bool)), lc_check, SLOT(setVisible(bool)) );
	connect(lc_check, SIGNAL(toggled(bool)), l_color, SLOT(setDisabled(bool)) );
	connect(lc_check, SIGNAL(toggled(bool)), lc_c, SLOT(setDisabled(bool)) );
	ls_en->setIcon(QPixmap::fromImage(ico_t));
	ls_en->setToolTip(_("Include(checked)/Exclude(unchecked) the line style from the properties list."));
	ls_en->setCheckable( true );
	propDlg.edLay()->addWidget( ls_en, 5, 0 ); ls_en->setChecked(true);
	propDlg.edLay()->setAlignment ( ls_en, Qt::AlignRight );
	QLabel *ls_lb = new QLabel(_("Line style"),&propDlg);
	propDlg.edLay()->addWidget( ls_lb, 5, 1 );
	propDlg.edLay()->addWidget( l_style, 5, 2 );
	ls_check->setToolTip(_("Set the default line style."));
	propDlg.edLay()->addWidget( ls_check, 5, 3 );
	ls_c->setToolTip(_("Make the line style Dynamic(checked)/Static(unchecked).")); ls_c->setCheckable( true );
	ls_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(ls_c, 5, 4);
	connect( ls_en, SIGNAL(toggled(bool)), ls_lb, SLOT(setEnabled(bool)) );
	connect( ls_en, SIGNAL(toggled(bool)), l_style, SLOT(setVisible(bool)) );
	connect( ls_en, SIGNAL(toggled(bool)), ls_c, SLOT(setVisible(bool)) );
	connect( ls_en, SIGNAL(toggled(bool)), ls_check, SLOT(setVisible(bool)) );
	connect(ls_check, SIGNAL(toggled(bool)), l_style, SLOT(setDisabled(bool)) );
	connect(ls_check, SIGNAL(toggled(bool)), ls_c, SLOT(setDisabled(bool)) );
	lb_lb->setFont( lb_fnt ); propDlg.edLay()->addWidget( lb_lb, 6, 0 );
	lbw_en->setIcon(QPixmap::fromImage(ico_t));
	lbw_en->setToolTip(_("Include(checked)/Exclude(unchecked) the border width from the properties list."));
	lbw_en->setCheckable( true );
	propDlg.edLay()->addWidget( lbw_en, 7, 0 ); lbw_en->setChecked(true);
	propDlg.edLay()->setAlignment ( lbw_en, Qt::AlignRight );
	QLabel *lbw_lb= new QLabel(_("Border width"),&propDlg);
	propDlg.edLay()->addWidget( lbw_lb, 7, 1 );
	lb_width->setRange( 0, 99 );
	propDlg.edLay()->addWidget( lb_width, 7, 2 );
	lbw_check->setToolTip(_("Set the default line's border width."));
	propDlg.edLay()->addWidget( lbw_check, 7, 3 );
	lbw_c->setToolTip(_("Make the border width Dynamic(checked)/Static(unchecked).")); lbw_c->setCheckable( true );
	lbw_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(lbw_c, 7, 4);
	connect( lbw_en, SIGNAL(toggled(bool)), lbw_lb, SLOT(setEnabled(bool)) );
	connect( lbw_en, SIGNAL(toggled(bool)), lb_width, SLOT(setVisible(bool)) );
	connect( lbw_en, SIGNAL(toggled(bool)), lbw_c, SLOT(setVisible(bool)) );
	connect( lbw_en, SIGNAL(toggled(bool)), lbw_check, SLOT(setVisible(bool)) );
	connect(lbw_check, SIGNAL(toggled(bool)), lb_width, SLOT(setDisabled(bool)) );
	connect(lbw_check, SIGNAL(toggled(bool)), lbw_c, SLOT(setDisabled(bool)) );
	lbc_en->setIcon(QPixmap::fromImage(ico_t));
	lbc_en->setToolTip(_("Include(checked)/Exclude(unchecked) the border color from the properties list."));
	lbc_en->setCheckable( true );
	propDlg.edLay()->addWidget( lbc_en, 8, 0 ); lbc_en->setChecked(true);
	propDlg.edLay()->setAlignment ( lbc_en, Qt::AlignRight );
	QLabel *lbc_lb = new QLabel(_("Border color"),&propDlg);
	propDlg.edLay()->addWidget( lbc_lb, 8, 1 );
	propDlg.edLay()->addWidget( lb_color, 8, 2 );
	lbc_check->setToolTip(_("Set the default line's border color."));
	propDlg.edLay()->addWidget( lbc_check, 8, 3 );
	lbc_c->setToolTip(_("Make the border color Dynamic(checked)/Static(unchecked).")); lbc_c->setCheckable( true );
	lbc_c->setText( _("Dyn/Stat") );
	propDlg.edLay()->addWidget(lbc_c, 8, 4);
	connect( lbc_en, SIGNAL(toggled(bool)), lbc_lb, SLOT(setEnabled(bool)) );
	connect( lbc_en, SIGNAL(toggled(bool)), lb_color, SLOT(setVisible(bool)) );
	connect( lbc_en, SIGNAL(toggled(bool)), lbc_c, SLOT(setVisible(bool)) );
	connect( lbc_en, SIGNAL(toggled(bool)), lbc_check, SLOT(setVisible(bool)) );
	connect(lbc_check, SIGNAL(toggled(bool)), lb_color, SLOT(setDisabled(bool)) );
	connect(lbc_check, SIGNAL(toggled(bool)), lbc_c, SLOT(setDisabled(bool)) );
	if( !items_array_holds.size() || ((items_array_holds.size() == 1) && (items_array_holds[0] == elF->index)) )
	{
	    QFrame *sep = new QFrame(&propDlg);
	    sep->setFrameShape(QFrame::VLine);
	    propDlg.edLay()->addWidget(sep, 2, 5, 7, 1);
	    flag_hld = false;
	    p_lb->setFont(lb_fnt); propDlg.edLay()->addWidget(p_lb, 2, 6);
	    x_lb->setFont(lb_fnt); x_lb->setAlignment(Qt::AlignHCenter); propDlg.edLay()->addWidget(x_lb, 2, 7);
	    y_lb->setFont(lb_fnt); y_lb->setAlignment(Qt::AlignHCenter); propDlg.edLay()->addWidget(y_lb, 2, 8);
	    propDlg.edLay()->addWidget(new QLabel(_("Point 1:"),&propDlg), 3, 6);
	    p1_x->setRange(0, 10000); p1_x->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p1_x, 3, 7);
	    p1_y->setRange(0, 10000); p1_y->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p1_y, 3, 8);
	    p1_c->setToolTip(_("Make the 1 point Dynamic(checked)/Static(unchecked).")); p1_c->setCheckable( true ); p1_c->setText( _("Dyn/Stat") );
	    propDlg.edLay()->addWidget(p1_c, 3, 9);
	    propDlg.edLay()->addWidget(new QLabel(_("Point 2:"),&propDlg), 4, 6);
	    p2_x->setRange(0, 10000); p2_x->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p2_x, 4, 7);
	    p2_y->setRange(0, 10000); p2_y->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p2_y, 4, 8);
	    p2_c->setToolTip(_("Make the 2 point Dynamic(checked)/Static(unchecked).")); p2_c->setCheckable( true ); p2_c->setText( _("Dyn/Stat") );
	    propDlg.edLay()->addWidget(p2_c, 4, 9);
	    propDlg.edLay()->addWidget(p3_lb, 5, 6);
	    p3_x->setRange(0, 10000); p3_x->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p3_x, 5, 7);
	    p3_y->setRange(0, 10000); p3_y->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p3_y, 5, 8);
	    p3_c->setToolTip(_("Make the 3 point Dynamic(checked)/Static(unchecked).")); p3_c->setCheckable( true ); p3_c->setText( _("Dyn/Stat") );
	    propDlg.edLay()->addWidget(p3_c, 5, 9);
	    propDlg.edLay()->addWidget(p4_lb, 6, 6);
	    p4_x->setRange(0, 10000); p4_x->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p4_x, 6, 7);
	    p4_y->setRange(0, 10000); p4_y->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p4_y, 6, 8);
	    p4_c->setToolTip(_("Make the 4 point Dynamic(checked)/Static(unchecked).")); p4_c->setCheckable( true ); p4_c->setText( _("Dyn/Stat") );
	    propDlg.edLay()->addWidget(p4_c, 6, 9);
	    propDlg.edLay()->addWidget(p5_lb, 7, 6);
	    p5_x->setRange(0, 10000); p5_x->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p5_x, 7, 7);
	    p5_y->setRange(0, 10000); p5_y->setDecimals(POS_PREC_DIG);
	    propDlg.edLay()->addWidget(p5_y, 7, 8);
	    p5_c->setToolTip(_("Make the 5 point Dynamic(checked)/Static(unchecked).")); p5_c->setCheckable( true ); p5_c->setText( _("Dyn/Stat") );
	    propDlg.edLay()->addWidget(p5_c, 7, 9);

	    if( sender()->objectName() == "Line" )
	    {
		p1_x->setValue(pnts[shapeItems[elF->index].n1].x()); p1_y->setValue(pnts[shapeItems[elF->index].n1].y());
		if( shapeItems[elF->index].n1 > 0 ){ p1_c->setChecked(true); }
		else{ p1_c->setChecked(false); }
		p2_x->setValue(pnts[shapeItems[elF->index].n2].x()); p2_y->setValue(pnts[shapeItems[elF->index].n2].y());
		if( shapeItems[elF->index].n2 > 0 ) p2_c->setChecked(true); else p2_c->setChecked(false);
		p3_lb->setVisible(false);p3_x->setVisible(false);p3_y->setVisible(false); p3_c->setVisible(false);
		p4_lb->setVisible(false);p4_x->setVisible(false);p4_y->setVisible(false); p4_c->setVisible(false);
		p5_lb->setVisible(false);p5_x->setVisible(false);p5_y->setVisible(false); p5_c->setVisible(false);
	    }
	    else if( sender()->objectName() == "Bezier curve" )
	    {
		p1_x->setValue(pnts[shapeItems[elF->index].n1].x()); p1_y->setValue(pnts[shapeItems[elF->index].n1].y());
		if( shapeItems[elF->index].n1 > 0 ) p1_c->setChecked(true); else p1_c->setChecked(false);
		p2_x->setValue(pnts[shapeItems[elF->index].n2].x()); p2_y->setValue(pnts[shapeItems[elF->index].n2].y());
		if( shapeItems[elF->index].n2 > 0 ) p2_c->setChecked(true); else p2_c->setChecked(false);
		p3_x->setValue(pnts[shapeItems[elF->index].n3].x()); p3_y->setValue(pnts[shapeItems[elF->index].n3].y());
		if( shapeItems[elF->index].n3 > 0 ) p3_c->setChecked(true); else p3_c->setChecked(false);
		p4_x->setValue(pnts[shapeItems[elF->index].n4].x()); p4_y->setValue(pnts[shapeItems[elF->index].n4].y());
		if( shapeItems[elF->index].n4 > 0 ) p4_c->setChecked(true); else p4_c->setChecked(false);
		p5_lb->setVisible(false);p5_x->setVisible(false);p5_y->setVisible(false); p5_c->setVisible(false);
	    }
	    else
	    {
		p1_x->setValue(pnts[shapeItems[elF->index].n1].x()); p1_y->setValue(pnts[shapeItems[elF->index].n1].y());
		if( shapeItems[elF->index].n1 > 0 ) p1_c->setChecked(true); else p1_c->setChecked(false);
		p2_x->setValue(pnts[shapeItems[elF->index].n2].x()); p2_y->setValue(pnts[shapeItems[elF->index].n2].y());
		if( shapeItems[elF->index].n2 > 0 ) p2_c->setChecked(true); else p2_c->setChecked(false);
		p3_x->setValue(pnts[shapeItems[elF->index].n3].x()); p3_y->setValue(pnts[shapeItems[elF->index].n3].y());
		if( shapeItems[elF->index].n3 > 0 ) p3_c->setChecked(true); else p3_c->setChecked(false);
		p4_x->setValue(pnts[shapeItems[elF->index].n4].x()); p4_y->setValue(pnts[shapeItems[elF->index].n4].y());
		if( shapeItems[elF->index].n4 > 0 ) p4_c->setChecked(true); else p4_c->setChecked(false);
		p5_x->setValue(pnts[shapeItems[elF->index].n5].x()); p5_y->setValue(pnts[shapeItems[elF->index].n5].y());
		if( shapeItems[elF->index].n5 > 0 ) p5_c->setChecked(true); else p5_c->setChecked(false);
	    }
	    if( !items_array_holds.size() )
		items_array_holds.push_back(elF->index);
	    items_array_up.push_back(elF->index);
	    //>> Detecting the figures that connected to the current one and adding them to the updating array;
	    //>> if the current figure is connected with the arcs, then it's n1 and n2 points must be blocked for editing
	    for(int i = 0; i < shapeItems.size(); i++)
	    {
		fl_n1 = fl_n2 = false;
		if( i != elF->index  && (shapeItems[i].n1 == shapeItems[elF->index].n1 || shapeItems[i].n2 == shapeItems[elF->index].n1) )
		{
		    if( !elF->status_hold ) fl_appN1 = true;
		    else fl_n1 = true;
		}
		if( i != elF->index  && (shapeItems[i].n1 == shapeItems[elF->index].n2 || shapeItems[i].n2 == shapeItems[elF->index].n2) )
		{
		    if( !elF->status_hold ) fl_appN2 = true;
		    else fl_n2 = true;
		}
		if( fl_n1 || fl_n2 )
		{
		    items_array_up.push_back(i);	// array of the figures to be updated
		    if( shapeItems[elF->index].type != 2 && shapeItems[i].type == 2 )
		    {
			if( fl_n1 ) fl_n1Block = true;	//blocking the n1 point for editing
			if( fl_n2 ) fl_n2Block = true;	//blocking the n2 point for editing
		    }
		}
	    }
	    //>> Detecting if there is a necessity to rebuild the fill's path and if it is so push_back the fill to the array
	    for( int i = 0; i < inundItems.size(); i++ )
		for( int p = 0; p < inundItems[i].n.size(); p++ )
		    for( int z = 0; z < items_array_up.size(); z++ )
			if( (inundItems[i].n[p] == items_array_up[z]) && !inund_Rebuild.contains(i) )
			    inund_Rebuild.push_back(i);
	    p1_x->setReadOnly( fl_n1Block );p1_y->setReadOnly( fl_n1Block );
	    p2_x->setReadOnly( fl_n2Block );p2_y->setReadOnly( fl_n2Block );
	    //propDlg.resize(752,295);
	}
	else
	{
	    p_lb->hide(); x_lb->hide(); y_lb->hide();
	    p3_lb->hide(); p4_lb->hide(); p5_lb->hide();
	    p1_x->hide(); p1_y->hide(); p1_c->hide();
	    p2_x->hide(); p2_y->hide(); p2_c->hide();
	    p3_x->hide(); p3_y->hide(); p3_c->hide();
	    p4_x->hide(); p4_y->hide(); p4_c->hide();
	    p5_x->hide(); p5_y->hide(); p5_c->hide();
	}
	l_width->setValue((int)rRnd((*widths)[shapeItems[elF->index].width],POS_PREC_DIG));
	if( shapeItems[elF->index].width == -5 ) lw_check->setChecked(true);
	if( shapeItems[elF->index].width > 0 ) lw_c->setChecked(true);
	l_color->setValue( (*colors)[shapeItems[elF->index].lineColor].name() + "-" +
		QString(i2s( (*colors)[shapeItems[elF->index].lineColor].alpha() ).c_str()) );
	if( shapeItems[elF->index].lineColor == -5 ) lc_check->setChecked(true);
	if( shapeItems[elF->index].lineColor > 0 ) lc_c->setChecked(true);
	QStringList line_styles;
	line_styles << _("Solid") << _("Dashed") << _("Dotted");
	l_style->addItems(line_styles);
	l_style->setCurrentIndex((*styles)[shapeItems[elF->index].style]-1);
	if( shapeItems[elF->index].style == -5 ) ls_check->setChecked(true);
	if( shapeItems[elF->index].style > 0 ) ls_c->setChecked(true);
	lb_width->setValue((int)rRnd((*widths)[shapeItems[elF->index].borderWidth],POS_PREC_DIG));
	if( shapeItems[elF->index].borderWidth == -6 ) lbw_check->setChecked(true);
	if( shapeItems[elF->index].borderWidth > 0 ) lbw_c->setChecked(true);
	lb_color->setValue( (*colors)[shapeItems[elF->index].borderColor].name() + "-" +
		QString(i2s( (*colors)[shapeItems[elF->index].borderColor].alpha() ).c_str()) );
	if( shapeItems[elF->index].borderColor == -6 ) lbc_check->setChecked(true);
	if( shapeItems[elF->index].borderColor > 0 ) lbc_c->setChecked(true);
	propDlg.edLay()->setColumnMinimumWidth ( 2, 100 );
	propDlg.edLay()->setColumnStretch ( 2, 1 );
    }
    if(propDlg.exec() == QDialog::Accepted)
    {
	int k;
	elF->flag_dyn_save = false;
	//>> Applying the changes for the fills
	if(sender()->objectName() == "Fill")
	{
	    string res_fColor = f_color->value().toStdString();
	    QColor res_fClr = WdgShape::getColor(res_fColor);
	    if(fc_en->isChecked() && f_color->isEnabled())
	    {
		if(inundItems[elF->fill_index].brush == -7)
		{
		    k = -10;
		    while((*colors).find(k) != (*colors).end()) k--;
		    (*colors).insert(std::pair<int, QColor>(k, res_fClr));
		    inundItems[elF->fill_index].brush = k;
		}
		else (*colors)[inundItems[elF->fill_index].brush] = res_fClr;
	    }
	    else if(fc_en->isChecked() && fc_check->isChecked() && inundItems[elF->fill_index].brush != -7)
	    {
		(*colors).erase(inundItems[elF->fill_index].brush);
		inundItems[elF->fill_index].brush = -7;
	    }
	    if(fi_en->isChecked() && f_image->isEnabled())
	    {
		if(!f_image->text().isEmpty())
		{
		    if(inundItems[elF->fill_index].brushImg == -5)
		    {
			k = -10;
			while((*images).find(k) != (*images).end()) k--;
			(*images).insert(std::pair<int, string>(k,f_image->text().toStdString()));
			inundItems[elF->fill_index].brushImg = k;
		    }
		    else (*images)[inundItems[elF->fill_index].brushImg] = f_image->text().toStdString();
		}
		else if(f_image->text().isEmpty() && !(*images)[inundItems[elF->fill_index].brushImg].empty())
		{
		    (*images).erase(inundItems[elF->fill_index].brushImg);
		    inundItems[elF->fill_index].brushImg = -5;
		    inundItems[elF->fill_index].path = elF->createInundationPath(inundItems[elF->fill_index].n, pnts, w);
		}
	    }
	    else if(fi_en->isChecked() && fi_check->isChecked() && inundItems[elF->fill_index].brushImg != -5)
	    {
		(*images).erase(inundItems[elF->fill_index].brushImg);
		inundItems[elF->fill_index].brushImg = -5;
	    }
	    if(fc_en->isChecked() &&  fc_c->isEnabled() && ((fc_c->isChecked() &&
		(inundItems[elF->fill_index].brush <= -10 || inundItems[elF->fill_index].brush == -7)) ||
		(!fc_c->isChecked() && inundItems[elF->fill_index].brush > 0)))
	    {
		elF->rect_dyn = -1; elF->dyn_num = 7;
		dynamic();
	    }
	    if(fi_en->isChecked() && fi_c->isEnabled() && ((fi_c->isChecked() &&
		(inundItems[elF->fill_index].brushImg <= -10 || inundItems[elF->fill_index].brushImg == -5)) ||
		(!fi_c->isChecked() && inundItems[elF->fill_index].brushImg > 0)))
	    {
		elF->rect_dyn = -1; elF->dyn_num = 8;
		dynamic();
	    }
	}
	//>> Applying the changes for the figures
	else
	{
	    if(fl_appN1)
	    {
		if(shapeItems[elF->index].n1 > 0)
		    shapeItems[elF->index].n1 = appendPoint(pnts[shapeItems[elF->index].n1], false);
		else if(shapeItems[elF->index].n1 <= -10)
		    shapeItems[elF->index].n1 = appendPoint(pnts[shapeItems[elF->index].n1], true);
	    }
	    if(fl_appN2)
	    {
		if(shapeItems[elF->index].n2 > 0)
		    shapeItems[elF->index].n2 = appendPoint(pnts[shapeItems[elF->index].n1], false);
		else if(shapeItems[elF->index].n2 <= -10)
		    shapeItems[elF->index].n2 = appendPoint(pnts[shapeItems[elF->index].n1], true);
	    }
	    string res_lColor = l_color->value().toStdString();
	    string res_lbColor = lb_color->value().toStdString();
	    QColor res_lClr = WdgShape::getColor(res_lColor);
	    QColor res_lbClr = WdgShape::getColor(res_lbColor);
	    Qt::PenStyle ln_style = Qt::SolidLine;
	    switch(l_style->currentIndex())
	    {
		case EF_SOLID:	ln_style = Qt::SolidLine;	break;
		case EF_DASH:	ln_style = Qt::DashLine;	break;
		case EF_DOT:	ln_style = Qt::DotLine;		break;
	    }
	    QVector<int> tmp_shapes;
	    elF->flag_def_stat = false;//flag to use in the 'dynamic' function for making the default value the static one
	    //>>> Creating the tmp_shapes vector of figures which are not included to the vector of the selected ones (items_array_holds)
	    bool fl_eq = false;
	    for( int p = 0; p < shapeItems.size(); p++ )
	    {
		fl_eq = false;
		for( int h = 0; h < items_array_holds.size(); h++ )
		    if( p == items_array_holds[h] ){ fl_eq = true; break;}
		if( !fl_eq )tmp_shapes.push_back(p);
	    }
	    //>>> If the Dyn/Stat button is enabled for the line width
	    if( lw_en->isChecked() && lw_c->isEnabled() )
	    {
		if( items_array_holds.size() > 1 )
		{
		    if( (lw_c->isChecked() &&
			(shapeItems[elF->index].width <= -10 || shapeItems[elF->index].width == -5)) ||
			(!lw_c->isChecked() && (shapeItems[elF->index].width > 0 || shapeItems[elF->index].width == -5)) )
		    {
			if( !lw_c->isChecked() && shapeItems[elF->index].width == -5 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 1;
			dynamic();
		    }
		    else if( shapeItems[elF->index].width > 0 )
		    {
			bool fl_insert = false;
			//>>>> Detecting if there is any figure, from the figures which are not included to the selected ones,
			//>>>> with the same width's index. If it is so append the new width to the width map
			for( int p = 0; p < tmp_shapes.size(); p++ )
			{
			    for( int h = 0; h < items_array_holds.size(); h++ )
				if( (shapeItems[items_array_holds[h]].width > 0) && (shapeItems[tmp_shapes[p]].width == shapeItems[items_array_holds[h]].width) )
				{ fl_insert = true; break; }
			    if( fl_insert ) break;
			}
			if( fl_insert )
			    shapeItems[elF->index].width = appendWidth( (*widths)[shapeItems[elF->index].width], false );
		    }
		}
		else if( items_array_holds.size() == 1 )
		{
		    //>>>> Detecting if there is any figure in the shapeItems container with the same width's index.
		    bool fl_insert = false;
		    for( int p = 0; p < shapeItems.size(); p++ )
			if(  elF->index != p && shapeItems[elF->index].width > 0 && shapeItems[p].width == shapeItems[elF->index].width )
			{ fl_insert = true; break; }
		    if( (lw_c->isChecked() &&
			(shapeItems[elF->index].width <= -10 || shapeItems[elF->index].width == -5)) ||
			(!lw_c->isChecked() && (shapeItems[elF->index].width > 0 || shapeItems[elF->index].width == -5)) )
		    {
			if( !lw_c->isChecked() && shapeItems[elF->index].width == -5 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 1;
			dynamic();
		    }
		    else if( fl_insert )//>>>> If there is a figure with the same index, append the new width to the width map
			shapeItems[elF->index].width = appendWidth( (*widths)[shapeItems[elF->index].width], false );
		}
	    }
	    elF->flag_def_stat = false;
	    //>>> If the Dyn/Stat button is enabled for the line color
	    if( lc_en->isChecked() && lc_c->isEnabled() )
	    {
		if( items_array_holds.size() > 1 )
		{
		    if( (lc_c->isChecked() &&
			(shapeItems[elF->index].lineColor <= -10 || shapeItems[elF->index].lineColor == -5)) ||
			(!lc_c->isChecked() && (shapeItems[elF->index].lineColor > 0 || shapeItems[elF->index].lineColor == -5)) )
		    {
			if( !lc_c->isChecked() && shapeItems[elF->index].lineColor == -5 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 2;
			dynamic();
		    }
		    else if( shapeItems[elF->index].lineColor > 0 )
		    {
			bool fl_insert = false;
			//>>>> Detecting if there is any figure, from the figures which are not included to the selected ones,
			//>>>> with the same color's index. If it is so append the new color to the color's map
			for( int p = 0; p < tmp_shapes.size(); p++ )
			{
			    for( int h = 0; h < items_array_holds.size(); h++ )
				if( (shapeItems[items_array_holds[h]].lineColor > 0) && (shapeItems[tmp_shapes[p]].lineColor == shapeItems[items_array_holds[h]].lineColor) )
			    { fl_insert = true; break; }
			    if( fl_insert ) break;
			}
			if( fl_insert )
			    shapeItems[elF->index].lineColor = appendColor( (*colors)[shapeItems[elF->index].lineColor], false );
		    }
		}
		else if( items_array_holds.size() == 1 )
		{
		    //>>>> Detecting if there is any figure in the shapeItems container with the same color's index.
		    bool fl_insert = false;
		    for( int p = 0; p < shapeItems.size(); p++ )
			if(  elF->index != p && shapeItems[elF->index].lineColor > 0 && shapeItems[p].lineColor == shapeItems[elF->index].lineColor )
			{ fl_insert = true; break; }
		    if( (lc_c->isChecked() &&
			(shapeItems[elF->index].lineColor <= -10 || shapeItems[elF->index].lineColor == -5)) ||
			(!lc_c->isChecked() && (shapeItems[elF->index].lineColor > 0 || shapeItems[elF->index].lineColor == -5)) )
		    {
			if( !lc_c->isChecked() && shapeItems[elF->index].lineColor == -5 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 2;
			dynamic();
		    }
		    else if( fl_insert )//>>>> If there is a figure with the same index, append the new color to the color's map
			shapeItems[elF->index].lineColor = appendColor( (*colors)[shapeItems[elF->index].lineColor], false );
		}
	    }
	    elF->flag_def_stat = false;
	    //>>> If the Dyn/Stat button is enabled for the line style
	    if( ls_en->isChecked() && ls_c->isEnabled() )
	    {
		if( items_array_holds.size() > 1 )
		{
		    if( (ls_c->isChecked() &&
			(shapeItems[elF->index].style <= -10 || shapeItems[elF->index].style == -5)) ||
			(!ls_c->isChecked() && (shapeItems[elF->index].style > 0 || shapeItems[elF->index].style == -5)) )
		    {
			if( !ls_c->isChecked() && shapeItems[elF->index].style == -5 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 5;
			dynamic();
		    }
		    else if( shapeItems[elF->index].style > 0 )
		    {
			bool fl_insert = false;
			//>>>> Detecting if there is any figure, from the figures which are not included to the selected ones,
			//>>>> with the same style's index. If it is so append the new style to the style's map
			for( int p = 0; p < tmp_shapes.size(); p++ )
			{
			    for( int h = 0; h < items_array_holds.size(); h++ )
				if( (shapeItems[items_array_holds[h]].style > 0) && (shapeItems[tmp_shapes[p]].style == shapeItems[items_array_holds[h]].style) )
			    { fl_insert = true; break; }
			    if( fl_insert ) break;
			}
			if( fl_insert )
			    shapeItems[elF->index].style = appendStyle( (*styles)[shapeItems[elF->index].style], false );
		    }
		}
		else if( items_array_holds.size() == 1 )
		{
		    //>>>> Detecting if there is any figure in the shapeItems container with the same style's index.
		    bool fl_insert = false;
		    for( int p = 0; p < shapeItems.size(); p++ )
			if(  elF->index != p && shapeItems[elF->index].style > 0 && shapeItems[p].style == shapeItems[elF->index].style )
			{ fl_insert = true; break; }

		    if( (ls_c->isChecked() &&
			 (shapeItems[elF->index].style <= -10 || shapeItems[elF->index].style == -5)) ||
			 (!ls_c->isChecked() && (shapeItems[elF->index].style > 0 || shapeItems[elF->index].style == -5)) )
			{
			    if( !ls_c->isChecked() && shapeItems[elF->index].style == -5 ) elF->flag_def_stat = true;
			    elF->rect_dyn = -1; elF->dyn_num = 5;
			    dynamic();
			}
		    else if( fl_insert )//>>> If there is a figure with the same index, append the new style to the style's map
			shapeItems[elF->index].style = appendStyle( (*styles)[shapeItems[elF->index].style], false );
		}
	    }
	    elF->flag_def_stat = false;
	    //>>> If the Dyn/Stat button is enabled for the border width
	    if( lbw_en->isChecked() && lbw_c->isEnabled() )
	    {
		if( items_array_holds.size() > 1 )
		{
		    if( (lbw_c->isChecked() &&
			(shapeItems[elF->index].borderWidth <= -10 || shapeItems[elF->index].borderWidth == -6)) ||
			(!lbw_c->isChecked() && (shapeItems[elF->index].borderWidth > 0 || shapeItems[elF->index].borderWidth == -6)) )
		    {
			if( !lbw_c->isChecked() && shapeItems[elF->index].borderWidth == -6 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 3;
			dynamic();
		    }
		    else if( shapeItems[elF->index].borderWidth > 0 )
		    {
			bool fl_insert = false;
			//>>>> Detecting if there is any figure, from the figures which are not included to the selected ones,
			//>>>> with the same border width's index. If it is so append the new border width to the border width map
			for( int p = 0; p < tmp_shapes.size(); p++ )
			{
			    for( int h = 0; h < items_array_holds.size(); h++ )
				if( (shapeItems[items_array_holds[h]].borderWidth > 0) && (shapeItems[tmp_shapes[p]].borderWidth == shapeItems[items_array_holds[h]].borderWidth) )
				{ fl_insert = true; break; }
			    if( fl_insert ) break;
			}
			if( fl_insert )
			    shapeItems[elF->index].borderWidth = appendWidth( (*widths)[shapeItems[elF->index].borderWidth], false );
		    }
		}
		else if( items_array_holds.size() == 1 )
		{
		    //>>>> Detecting if there is any figure in the shapeItems container with the same border width's index.
		    bool fl_insert = false;
		    for( int p = 0; p < shapeItems.size(); p++ )
			if(  elF->index != p && shapeItems[elF->index].borderWidth > 0 && shapeItems[p].borderWidth == shapeItems[elF->index].borderWidth )
			{ fl_insert = true; break; }
		    if( (lbw_c->isChecked() &&
			(shapeItems[elF->index].borderWidth <= -10 || shapeItems[elF->index].borderWidth == -6)) ||
			(!lbw_c->isChecked() && (shapeItems[elF->index].borderWidth > 0 || shapeItems[elF->index].borderWidth == -6)) )
			{
			    if( !lbw_c->isChecked() && shapeItems[elF->index].borderWidth == -6 ) elF->flag_def_stat = true;
			    elF->rect_dyn = -1; elF->dyn_num = 3;
			    dynamic();
			}
		    else if( fl_insert )//>>>> If there is a figure with the same index, append the new border width to the border width map
			shapeItems[elF->index].borderWidth = appendWidth( (*widths)[shapeItems[elF->index].borderWidth], false );
		}
	    }
	    elF->flag_def_stat = false;
	    //>>> If the Dyn/Stat button is enabled for the border color
	    if( lbc_en->isChecked() && lbc_c->isEnabled() )
	    {
		if( items_array_holds.size() > 1 )
		{
		    if( (lbc_c->isChecked() &&
			(shapeItems[elF->index].borderColor <= -10 || shapeItems[elF->index].borderColor == -6)) ||
			(!lbc_c->isChecked() && (shapeItems[elF->index].borderColor > 0 || shapeItems[elF->index].borderColor == -6)) )
			{
			    if( !lbc_c->isChecked() && shapeItems[elF->index].borderColor == -6 ) elF->flag_def_stat = true;
			    elF->rect_dyn = -1; elF->dyn_num = 4;
			    dynamic();
			}
		    else if( shapeItems[elF->index].borderColor > 0 )
		    {
			bool fl_insert = false;
			//>>>> Detecting if there is any figure, from the figures which are not included to the selected ones,
			//>>>> with the same border color's index. If it is so append the new border color to the color's map
			for( int p = 0; p < tmp_shapes.size(); p++ )
			{
			    for( int h = 0; h < items_array_holds.size(); h++ )
				if( (shapeItems[items_array_holds[h]].borderColor > 0) && (shapeItems[tmp_shapes[p]].borderColor == shapeItems[items_array_holds[h]].borderColor) )
				{ fl_insert = true; break; }
			    if( fl_insert ) break;
			}
			if( fl_insert )
			    shapeItems[elF->index].borderColor = appendColor( (*colors)[shapeItems[elF->index].borderColor], false );
		    }
		}
		else if( items_array_holds.size() == 1 )
		{
		    //>>>> Detecting if there is any figure in the shapeItems container with the same border color's index.
		    bool fl_insert = false;
		    for( int p = 0; p < shapeItems.size(); p++ )
			if(  elF->index != p && shapeItems[elF->index].borderColor > 0 && shapeItems[p].borderColor == shapeItems[elF->index].borderColor )
			{ fl_insert = true; break; }
		    if( (lbc_c->isChecked() &&
			(shapeItems[elF->index].borderColor <= -10 || shapeItems[elF->index].borderColor == -6)) ||
			(!lbc_c->isChecked() && (shapeItems[elF->index].borderColor > 0 || shapeItems[elF->index].borderColor == -6)) )
		    {
			if( !lbc_c->isChecked() && shapeItems[elF->index].borderColor == -6 ) elF->flag_def_stat = true;
			elF->rect_dyn = -1; elF->dyn_num = 4;
			dynamic();
		    }
		    else if( fl_insert )//>>>> If there is a figure with the same index, append the new border color to the color's map
			shapeItems[elF->index].borderColor = appendColor( (*colors)[shapeItems[elF->index].borderColor], false );
		}
	    }
	    //>>> Applying the changes of the fugures properties for all figures in the items_array_holds array except th elF->index figure
	    for(int i = 0; i < items_array_holds.size(); i++)
	    {
		//>>>> Applying changes for the line width
		if(lw_en->isChecked())
		{
		    if(l_width->isEnabled())
		    {
			if(items_array_holds.size() > 1 && items_array_holds[i] != elF->index)
			{
			    if(shapeItems[elF->index].width > 0)
			    {
				//>>> Detecting if there is the necessity to erase the width from the map andchanging the width's index to those of the elF->index figure
				bool flag_save = false;
				for(int p = 0; !flag_save && p < tmp_shapes.size(); p++)
				    if(shapeItems[items_array_holds[i]].width > 0 && shapeItems[tmp_shapes[p]].width == shapeItems[items_array_holds[i]].width)
					flag_save = true;
				if(shapeItems[items_array_holds[i]].width != -5 && shapeItems[items_array_holds[i]].width != shapeItems[elF->index].width && !flag_save)
				    (*widths).erase(shapeItems[items_array_holds[i]].width);
				shapeItems[items_array_holds[i]].width = shapeItems[elF->index].width;
			    }
			    else if(shapeItems[elF->index].width <= -10)
			    {
				if(shapeItems[items_array_holds[i]].width == -5 || shapeItems[items_array_holds[i]].width > 0)
				{
				    //>>> Detecting if there is the necessity to erase the width from the map and appendint the new static width to the map
				    if(shapeItems[items_array_holds[i]].width > 0)
				    {
					bool fl_keep = false;
					for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
					    if(shapeItems[tmp_shapes[p]].width == shapeItems[items_array_holds[i]].width)
						fl_keep = true;
					if(!fl_keep) (*widths).erase(shapeItems[items_array_holds[i]].width);
				    }
				    shapeItems[items_array_holds[i]].width = appendWidth(l_width->value(), true);
				}
				//>>> If the figures width is already static just changing its value to the actual one.
				else if(shapeItems[items_array_holds[i]].width <= -10)
				    (*widths)[shapeItems[items_array_holds[i]].width] = l_width->value();
			    }
			}
			//>>> Appending the static point for the figure with the default width
			else if(shapeItems[items_array_holds[i]].width == -5)
			    shapeItems[items_array_holds[i]].width = appendWidth(l_width->value(), true);
			//Changing the width of the figure with the actual value without changing its index in the map
			else (*widths)[shapeItems[items_array_holds[i]].width] = l_width->value();
		    }
		    //>>> Making the figures width the default one
		    //>>> and detecting if there is the necessity to save the previous figure's width index.
		    else if(shapeItems[items_array_holds[i]].width != -5)
		    {
			bool fl_keep = false;
			if(shapeItems[items_array_holds[i]].width > 0)
			    for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
				if(shapeItems[items_array_holds[i]].width == shapeItems[tmp_shapes[p]].width)
			    	    fl_keep = true;
			if(!fl_keep) (*widths).erase(shapeItems[items_array_holds[i]].width);
			shapeItems[items_array_holds[i]].width = -5;
		    }
		}
		//>>>>Applying changes for the line color
		if(lc_en->isChecked())
		{
		    if(l_color->isEnabled())
		    {
			if(items_array_holds.size() > 1 && items_array_holds[i] != elF->index)
			{
			    if(shapeItems[elF->index].lineColor > 0)
			    {
				//>>>Detecting if there is the necessity to erase the color from the map and changing the color's index to those of the elF->index figure
				bool flag_save = false;
				for(int p = 0; !flag_save && p < tmp_shapes.size(); p++)
				    if(shapeItems[items_array_holds[i]].lineColor > 0 && shapeItems[tmp_shapes[p]].lineColor == shapeItems[items_array_holds[i]].lineColor)
					flag_save = true;
				if(shapeItems[items_array_holds[i]].lineColor != -5 && shapeItems[items_array_holds[i]].lineColor != shapeItems[elF->index].lineColor && !flag_save)
				    (*colors).erase(shapeItems[items_array_holds[i]].lineColor);
				shapeItems[items_array_holds[i]].lineColor = shapeItems[elF->index].lineColor;
			    }
			    else if(shapeItems[elF->index].lineColor <= -10)
			    {
				if(shapeItems[items_array_holds[i]].lineColor == -5 || shapeItems[items_array_holds[i]].lineColor > 0)
				{
				    //>>>Detecting if there is the necessity to erase the color from the map and appendint the new static color to the map
				    if(shapeItems[items_array_holds[i]].lineColor > 0)
				    {
					bool fl_keep = false;
					for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
					    if(shapeItems[tmp_shapes[p]].lineColor == shapeItems[items_array_holds[i]].lineColor)
						fl_keep = true;
					if(!fl_keep) (*colors).erase(shapeItems[items_array_holds[i]].lineColor);
				    }
				    shapeItems[items_array_holds[i]].lineColor = appendColor(res_lClr, true);
				}
				//>>> If the figure's color is already static just changing its value to the actual one.
				else if(shapeItems[items_array_holds[i]].lineColor <= -10)
				    (*colors)[shapeItems[items_array_holds[i]].lineColor] = res_lClr;
			    }
			}
			//>>> Appending the static point for the figure with the default color
			else if(shapeItems[items_array_holds[i]].lineColor == -5)
			    shapeItems[items_array_holds[i]].lineColor = appendColor(res_lClr, true);
			//Changing the color of the figure with the actual value without changing its index in the map
			else (*colors)[shapeItems[items_array_holds[i]].lineColor] = res_lClr;
		    }
		    //>>> Making the figures color the default one
		    //>>> and detecting if there is the necessity to save the previous figure's color index.
		    else if(shapeItems[items_array_holds[i]].lineColor != -5)
		    {
			bool fl_keep = false;
			if(shapeItems[items_array_holds[i]].lineColor > 0)
			    for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
				if(shapeItems[items_array_holds[i]].lineColor == shapeItems[tmp_shapes[p]].lineColor)
			    	    fl_keep = true;
			if(!fl_keep) (*colors).erase( shapeItems[items_array_holds[i]].lineColor );
			shapeItems[items_array_holds[i]].lineColor = -5;
		    }
		}
		//>>>>Applying changes for the line style
		if(ls_en->isChecked())
		{
		    if(l_style->isEnabled())
		    {
			if(items_array_holds.size() > 1 && items_array_holds[i] != elF->index)
			{
			    if(shapeItems[elF->index].style > 0)
			    {
				//>>> Detecting if there is the necessity to erase the color from the map and changing the color's index to those of the elF->index figure
				bool flag_save = false;
				for(int p = 0; !flag_save && p < tmp_shapes.size(); p++)
				    if(shapeItems[items_array_holds[i]].style > 0 && shapeItems[tmp_shapes[p]].style == shapeItems[items_array_holds[i]].style)
					flag_save = true;
				if(shapeItems[items_array_holds[i]].style != -5 && shapeItems[items_array_holds[i]].style != shapeItems[elF->index].style && !flag_save)
				    (*styles).erase(shapeItems[items_array_holds[i]].style);
				shapeItems[items_array_holds[i]].style = shapeItems[elF->index].style;
			    }
			    else if(shapeItems[elF->index].style <= -10)
			    {
				if(shapeItems[items_array_holds[i]].style == -5 || shapeItems[items_array_holds[i]].style > 0)
				{
				    //>>> Detecting if there is the necessity to erase the color from the map and appendint the new static color to the map
				    if(shapeItems[items_array_holds[i]].style > 0)
				    {
					bool fl_keep = false;
					for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
					    if(shapeItems[tmp_shapes[p]].style == shapeItems[items_array_holds[i]].style)
						fl_keep = true;
					if(!fl_keep) (*styles).erase(shapeItems[items_array_holds[i]].style);
				    }
				    shapeItems[items_array_holds[i]].style = appendStyle(ln_style, true);
				}
				//>>> If the figure's color is already static just changing its value to the actual one.
				else if(shapeItems[items_array_holds[i]].style <= -10)
				    (*styles)[shapeItems[items_array_holds[i]].style] = ln_style;
			    }
			}
			//>>> Appending the static point for the figure with the default color
			else if(shapeItems[items_array_holds[i]].style == -5)
			    shapeItems[items_array_holds[i]].style = appendStyle(ln_style, true);
			//Changing the color of the figure with the actual value without changing its index in the map
			else (*styles)[shapeItems[items_array_holds[i]].style] = ln_style;
		    }
		    //>>> Making the figures color the default one
		    //>>> and detecting if there is the necessity to save the previous figure's color index.
		    else if(shapeItems[items_array_holds[i]].style != -5)
		    {
			bool fl_keep = false;
			if(shapeItems[items_array_holds[i]].style > 0)
			    for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
				if(shapeItems[items_array_holds[i]].style == shapeItems[tmp_shapes[p]].style)
			    	    fl_keep = true;
			if(!fl_keep) (*styles).erase(shapeItems[items_array_holds[i]].style);
			shapeItems[items_array_holds[i]].style = -5;
		    }
		}
		//>>>> Applying changes for the border width
		if(lbw_en->isChecked())
		{
		    if(lb_width->isEnabled())
		    {
			if(items_array_holds.size() > 1 && items_array_holds[i] != elF->index)
			{
			    if(shapeItems[elF->index].borderWidth > 0)
			    {
				//>>> Detecting if there is the necessity to erase the border width from the map and changing the border width's index to those of the elF->index figure
				bool flag_save = false;
				for(int p = 0; !flag_save && p < tmp_shapes.size(); p++)
				    if(shapeItems[items_array_holds[i]].borderWidth > 0 && shapeItems[tmp_shapes[p]].borderWidth == shapeItems[items_array_holds[i]].borderWidth)
					flag_save = true;
				if(shapeItems[items_array_holds[i]].borderWidth != -6 && shapeItems[items_array_holds[i]].borderWidth != shapeItems[elF->index].borderWidth && !flag_save)
				    (*widths).erase(shapeItems[items_array_holds[i]].borderWidth);
				shapeItems[items_array_holds[i]].borderWidth = shapeItems[elF->index].borderWidth;
			    }
			    else if(shapeItems[elF->index].borderWidth <= -10)
			    {
				if(shapeItems[items_array_holds[i]].borderWidth == -6 || shapeItems[items_array_holds[i]].borderWidth > 0)
				{
				    //>>> Detecting if there is the necessity to erase the border width from the map and appending the new static border width to the map
				    if(shapeItems[items_array_holds[i]].borderWidth > 0)
				    {
					bool fl_keep = false;
					for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
					    if( shapeItems[tmp_shapes[p]].borderWidth == shapeItems[items_array_holds[i]].borderWidth )
						fl_keep = true;
					if(!fl_keep) (*widths).erase(shapeItems[items_array_holds[i]].borderWidth);
				    }
				    shapeItems[items_array_holds[i]].borderWidth = appendWidth(lb_width->value(), true);
				}
				//>>> If the figures border width is already static just changing its value to the actual one.
				else if(shapeItems[items_array_holds[i]].borderWidth <= -10)
				    (*widths)[shapeItems[items_array_holds[i]].borderWidth] = lb_width->value();
			    }
			}
			//>>> Appending the static point for the figure with the default borderWidth
			else if(shapeItems[items_array_holds[i]].borderWidth == -6)
			    shapeItems[items_array_holds[i]].borderWidth = appendWidth(lb_width->value(), true);
			//Changing the border width of the figure with the actual value without changing its index in the map
			else (*widths)[shapeItems[items_array_holds[i]].borderWidth] = lb_width->value();
		    }
		    //>>> Making the figures border width the default one
		    //>>> and detecting if there is the necessity to save the previous figure's border width index.
		    else if(shapeItems[items_array_holds[i]].borderWidth != -6)
		    {
			bool fl_keep = false;
			if(shapeItems[items_array_holds[i]].borderWidth > 0)
			    for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++ )
				if(shapeItems[items_array_holds[i]].borderWidth == shapeItems[tmp_shapes[p]].borderWidth)
			    	    fl_keep = true;
			if(!fl_keep) (*widths).erase(shapeItems[items_array_holds[i]].borderWidth);
			shapeItems[items_array_holds[i]].borderWidth = -6;
		    }
		}
		//>>>> Applying changes for the border color
		if(lbc_en->isChecked())
		{
		    if(lb_color->isEnabled())
		    {
			if(items_array_holds.size() > 1 && items_array_holds[i] != elF->index)
			{
			    if(shapeItems[elF->index].borderColor > 0)
			    {
				//>>>Detecting if there is the necessity to erase the color from the map and changing the color's index to those of the elF->index figure
				bool flag_save = false;
				for(int p = 0; !flag_save && p < tmp_shapes.size(); p++)
				    if(shapeItems[items_array_holds[i]].borderColor > 0 && shapeItems[tmp_shapes[p]].borderColor == shapeItems[items_array_holds[i]].borderColor)
					flag_save = true;
				if(shapeItems[items_array_holds[i]].borderColor != -6 && shapeItems[items_array_holds[i]].borderColor != shapeItems[elF->index].borderColor && !flag_save)
				    (*colors).erase( shapeItems[items_array_holds[i]].borderColor );
				shapeItems[items_array_holds[i]].borderColor = shapeItems[elF->index].borderColor;
			    }
			    else if(shapeItems[elF->index].borderColor <= -10)
			    {
				if(shapeItems[items_array_holds[i]].borderColor == -6 || shapeItems[items_array_holds[i]].borderColor > 0)
				{
				    //>>> Detecting if there is the necessity to erase the color from the map and appendint the new static color to the map
				    if(shapeItems[items_array_holds[i]].borderColor > 0)
				    {
					bool fl_keep = false;
					for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
					    if(shapeItems[tmp_shapes[p]].borderColor == shapeItems[items_array_holds[i]].borderColor)
						fl_keep = true;
					if(!fl_keep) (*colors).erase(shapeItems[items_array_holds[i]].borderColor);
				    }
				    shapeItems[items_array_holds[i]].borderColor = appendColor(res_lbClr, true);
				}
				//>>> If the figure's color is already static just changing its value to the actual one.
				else if(shapeItems[items_array_holds[i]].borderColor <= -10)
				    (*colors)[shapeItems[items_array_holds[i]].borderColor] = res_lbClr;
			    }
			}
			//>>> Appending the static point for the figure with the default color
			else if(shapeItems[items_array_holds[i]].borderColor == -6)
			    shapeItems[items_array_holds[i]].borderColor = appendColor(res_lbClr, true);
			//Changing the color of the figure with the actual value without changing its index in the map
			else (*colors)[shapeItems[items_array_holds[i]].borderColor] = res_lbClr;
		    }
		    //>>> Making the figures color the default one
		    //>>> and detecting if there is the necessity to save the previous figure's color index.
		    else if(shapeItems[items_array_holds[i]].borderColor != -6)
		    {
			bool fl_keep = false;
			if(shapeItems[items_array_holds[i]].borderColor > 0)
			    for(int p = 0; !fl_keep && p < tmp_shapes.size(); p++)
				if( shapeItems[items_array_holds[i]].borderColor == shapeItems[tmp_shapes[p]].borderColor )
			    	    fl_keep = true;
			if(!fl_keep) (*colors).erase(shapeItems[items_array_holds[i]].borderColor);
			shapeItems[items_array_holds[i]].borderColor = -6;
		    }
		}
	    }
	    if(!flag_hld)
	    {
		if(sender()->objectName() == "Line")
		{
		    if(!fl_n1Block)
		    {
			pnts[shapeItems[elF->index].n1].setX(p1_x->value());
			pnts[shapeItems[elF->index].n1].setY(p1_y->value());
		    }
		    if(!fl_n2Block)
		    {
			pnts[shapeItems[elF->index].n2].setX(p2_x->value());
			pnts[shapeItems[elF->index].n2].setY(p2_y->value());
		    }
		}
		else if(sender()->objectName() == "Bezier curve")
		{
		    if(!fl_n1Block)
		    {
			pnts[shapeItems[elF->index].n1].setX(p1_x->value());
			pnts[shapeItems[elF->index].n1].setY(p1_y->value());
		    }
		    if(!fl_n2Block)
		    {
			pnts[shapeItems[elF->index].n2].setX(p2_x->value());
			pnts[shapeItems[elF->index].n2].setY(p2_y->value());
		    }
		    pnts[shapeItems[elF->index].n3].setX(p3_x->value()); pnts[shapeItems[elF->index].n3].setY(p3_y->value());
		    pnts[shapeItems[elF->index].n4].setX(p4_x->value()); pnts[shapeItems[elF->index].n4].setY(p4_y->value());
		}
		else if( sender()->objectName() == "Arc" )
		{
		    pnts[shapeItems[elF->index].n1].setX(p1_x->value()); pnts[shapeItems[elF->index].n1].setY(p1_y->value());
		    pnts[shapeItems[elF->index].n2].setX(p2_x->value()); pnts[shapeItems[elF->index].n2].setY(p2_y->value());
		    pnts[shapeItems[elF->index].n3].setX(p3_x->value()); pnts[shapeItems[elF->index].n3].setY(p3_y->value());
		    pnts[shapeItems[elF->index].n4].setX(p4_x->value()); pnts[shapeItems[elF->index].n4].setY(p4_y->value());
		    pnts[shapeItems[elF->index].n5].setX(p5_x->value()); pnts[shapeItems[elF->index].n5].setY(p5_y->value());
		    //>> Calcylating t_start and t_end for the arc, using the start and end points(n1,n2)
		    shapeItems[elF->index].ctrlPos4 = elF->getArcStartEnd(pnts[shapeItems[elF->index].n1], pnts[shapeItems[elF->index].n2],
				pnts[shapeItems[elF->index].n3], pnts[shapeItems[elF->index].n4], pnts[shapeItems[elF->index].n5]);
		}
		if(p1_c->isEnabled() &&
	    	    ((p1_c->isChecked() && shapeItems[elF->index].n1 <= -10) || (!p1_c->isChecked() && shapeItems[elF->index].n1 > 0)))
		{
		    elF->rect_dyn = 0; elF->dyn_num = 0;
		    dynamic();
		}
		if(p2_c->isEnabled() &&
	    	    ((p2_c->isChecked() && shapeItems[elF->index].n2 <= -10) || (!p2_c->isChecked() && shapeItems[elF->index].n2 > 0)))
		{
		    elF->rect_dyn = 1; elF->dyn_num = 0;
		    dynamic();
		}
		if(p3_c->isEnabled() &&
	    	    ((p3_c->isChecked() && shapeItems[elF->index].n3 <= -10) || (!p3_c->isChecked() && shapeItems[elF->index].n3 > 0)))
		{
		    elF->rect_dyn = 2; elF->dyn_num = 0;
		    dynamic();
		}
		if(p4_c->isEnabled() &&
	    	    ((p4_c->isChecked() && shapeItems[elF->index].n4 <= -10) || (!p4_c->isChecked() && shapeItems[elF->index].n4 > 0)))
		{
		    elF->rect_dyn = 3; elF->dyn_num = 0;
		    dynamic();
		}
		if(p5_c->isEnabled() &&
	    	    ((p5_c->isChecked() && shapeItems[elF->index].n5 <= -10) || (!p5_c->isChecked() && shapeItems[elF->index].n5 > -10)))
		{
		    elF->rect_dyn = 4; elF->dyn_num = 0;
		    dynamic();
		}
	    }
	    //>> Adding the figures to be updated to the ones in the items_array_holds array
	    if(items_array_up.size() > 1)
		for(int z = 1; z < items_array_up.size(); z++)
		    items_array_holds.push_back(items_array_up[z]);
	    //>> Rebuilding the figures, which are connected with the current one
	    elF->initShapeItems(QPointF(), items_array_holds, w);

	    //>> Rebuilding the fills' paths if the current figure is included to it
	    for(int i = 0; i < inund_Rebuild.size(); i++)
		if(elF->status_hold)
		    inundItems[inund_Rebuild[i]].path = elF->createInundationPath(inundItems[inund_Rebuild[i]].n, pnts, w);
		else	//>> Removing the fills if the current figure is included to their paths
		{
		    inundItems.remove(inund_Rebuild[i]);
		    for(int j = i+1; j < inund_Rebuild.size(); j++)
			if(inund_Rebuild[j] > inund_Rebuild[i]) inund_Rebuild[j]--;
		}
	}
	elF->rectItems.clear();
	elF->paintImage(w);
	elF->shapeSave(w);
	elF->flag_dyn_save = true;
	elF->status = false;
	elF->flag_ctrl = elF->flag_A = elF->flag_copy = elF->flag_up = elF->flag_down = elF->flag_left = elF->flag_right =  false;
	elF->itemInMotion = 0;
	elF->count_Shapes = 0;
	elF->index = -1;
	elF->index_array.clear();
	w->update();
	if(devW) devW->setPrevEdExitFoc(false);
    }
}

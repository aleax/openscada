//OpenSCADA system module UI.VISION file: vis_shape_elfig.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Lysenko Maxim (mlisenko@ukr.net)
 *   			     by Yashina Kseniya (sobacurka@ukr.net)
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

#include <QToolBar>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QStatusBar>

#include <tsys.h>

#include "tvision.h"
#include "vis_devel.h"
#include "vis_widgs.h"
#include "vis_devel_widgs.h"
#include "vis_run_widgs.h"
#include "vis_shape_elfig.h"


using namespace VISION;

ShapeElFigure::ShapeElFigure( ) :
    WdgShape("ElFigure"), itemInMotion(0), flag_down(false), flag_up(false), flag_left(false), flag_right(false), flag_A(false), flag_ctrl(false),
             status_hold(true), flag_rect(false), flag_hold_move(false), flag_m(false), flag_scale(true), flag_release(false), flag_rotate(true), flag_hold_arc(false), flag_angle_temp(false),
                flag_arc_rect_3_4(false), flag_first_move(false), flag_inund_break(false), flag_copy(false), flag_move(false), flag_check_pnt_inund(false), current_ss(-1), current_se(-1), current_es(-1), current_ee(-1),
    count_Shapes(0), count_holds(0), count_rects(0), rect_num_arc(-1), rect_num(-1), index_del(-1), rect_dyn(-1)
{
    newPath.addEllipse( QRect(0,0,0,0) );
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
    ElFigDt *elFD = (ElFigDt*)w->shpData;

    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);
    bool rel_list	= false;				//change signal
    bool up		= false;
    status		= false;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundationItems = elFD->inundationItems;
    PntMap *pnts = &elFD->shapePnts;
    QLineF line1, line2;
    double ang;
    QPointF StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4;
    double t_start, t_end, a, b, ang_t;
    float MotionWidth;
    QPainterPath circlePath;
    QVector<inundationItem> inundationItems_temp;
    QVector<ShapeItem> shapeItems_temp;
    switch( uiPrmPos )
    {
	case -1:	//load
	    rel_list = true;
	    break;
	case 5:		//en
	    if( !runW )	break;
            elFD->en = (bool)atoi(val.c_str());
	    w->setVisible(atoi(val.c_str()));
	    break;
	case 6:		//active
	    if( !runW )	break;
            elFD->active = (bool)atoi(val.c_str());
	    w->setFocusPolicy( (atoi(val.c_str()) && ((RunWdgView*)w)->permCntr()) ? Qt::TabFocus : Qt::NoFocus );
	    break;
	case 12:	//geomMargin
            elFD->geomMargin = atoi(val.c_str());
	    up=true;
	    break;
	case 20:	//lineWdth
            elFD->lineWdth = atof(val.c_str());
            rel_list = true;
            break;
        case 21:	//lineClr
            elFD->lineClr = QColor(val.c_str());
            rel_list = true;
            break;
        case 22:	//lineDecor
            elFD->lineDecor = atoi(val.c_str());
            rel_list = true;
            break;
        case 23:	//bordWdth
            elFD->bordWdth = atoi(val.c_str());
            rel_list = true;
            break;
        case 24:	//bordClr
            elFD->bordClr = QColor(val.c_str());
            rel_list = true;
            break;
        case 25:	//fillClr
            elFD->fillClr = QColor(val.c_str());
	    rel_list = true;
            break;
        case 26:	//fillImg
        {
	    QImage img;
	    string backimg = w->resGet(val);
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
                elFD->fillImg = QBrush(img);
            else elFD->fillImg = QBrush();
	    rel_list = true;
	    break;
        }
        case 28:
        {
            elFD->orient = atof(val.c_str());
            rel_list = true;
            break;   
        }
        case 27:	//elLst
            elFD->elLst = val.c_str();
            rel_list = true;
            break;
        case 29:
            elFD->lineStyle = val.c_str();
            rel_list = true;
            break;
        default:
            if( uiPrmPos >= 30 )
            {
                int pnt  = (uiPrmPos-30)/5;
                int patr = (uiPrmPos-30)%5;
                double pval  = atof(val.c_str());
                QPointF pnt_ = (*pnts)[pnt];
                if( patr == 0 ) pnt_.setX(pval);
                else pnt_.setY(pval);
                (*pnts)[pnt] = pnt_;
                rel_list = true;
            }
    }
    if( rel_list && !w->allAttrLoad( ) )
    {
        // Deleting the pairs in the map with the key <= -10 
        for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
            if(pi->first <= -10 ) (*pnts).erase ( pi );
        
        int map_index = -10;
        flag_ctrl = flag_A = flag_copy = false;
        index_array.clear();
        count_Shapes = 0;
        itemInMotion = 0;
        if( shapeItems.size() )
        {
            shapeItems_temp = shapeItems;
            shapeItems.clear();
        }
        if( inundationItems.size() )
        {
            inundationItems_temp = inundationItems;
            inundationItems.clear();
        }
	//- Parse last attributes list and make point list -
        string sel, ln_st;
        Qt::PenStyle style;
        int p[5];
        double scale;
        float width;
        float bord_width;
        QPointF ip[5];
        if( w->xScale(true) < w->yScale(true) ) scale = w->xScale(true);
        else scale = w->yScale(true);
        for( int off = 0; (sel=TSYS::strSepParse(elFD->elLst,0,'\n',&off)).size(); )
        {
            int el_off = 0;
            string el = TSYS::strSepParse(sel,0,':',&el_off);
            if( el == "line" )
            {
                //-- Reading anf setting attributes for the current line --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = (int)TSYS::realRound(elFD->lineWdth);
                width = width*scale;
                QColor color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !color.isValid() ) color = elFD->lineClr;
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = (int)TSYS::realRound(elFD->bordWdth);
                bord_width = bord_width*scale;
                QColor bord_color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_color.isValid() )  bord_color = elFD->bordClr;
                ln_st = TSYS::strSepParse(sel,0,':',&el_off);
                if( !ln_st.size() ) ln_st = elFD->lineStyle;
                if( ln_st == "solid" ) style = Qt::SolidLine;
                else if( ln_st == "dashed" ) style = Qt::DashLine;
                else if( ln_st == "dotted" ) style = Qt::DotLine;
                else style = Qt::SolidLine;
		//-- Building the path of the line and adding it to container --
                if( bord_width > 0 )
                {
                    circlePath = newPath;
                    ShapeItem item_temp( circlePath, newPath, p[0], p[1], -1, -1, -1, QPointF(0,0),
                                        QBrush( color, Qt::SolidPattern ),
                                        QPen( bord_color, bord_width, style, Qt::FlatCap, Qt::MiterJoin ),
                                        QPen( color, width, Qt::NoPen, Qt::FlatCap, Qt::MiterJoin ), width, bord_width, 1, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
                if( bord_width == 0 )
                {
                    circlePath = newPath;
                    QPainterPath bigPath = newPath;
                    ShapeItem item_temp;
                    if( width < 3 )
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], -1, -1, -1, QPointF(0,0),
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( bord_color, bord_width, Qt::NoPen, Qt::SquareCap, Qt::RoundJoin ),
                                        QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin ), width, bord_width, 1, angle_temp );
                    else
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], -1, -1, -1, QPointF(0,0),
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( bord_color, bord_width, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin ),
                                        QPen( color, width, style, Qt::FlatCap, Qt::RoundJoin ), width, bord_width, 1, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
            }
            if( el == "arc" )
            {
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str());
                else 
                {
                        p[2] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str());
                else 
                {
                        p[3] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[4]  = atoi(el_s.c_str());
                else 
                {
                    p[4] = map_index;
                    (*pnts)[map_index] = QPointF(x_s,y_s);
                    map_index -= 1;
                }
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = (int)TSYS::realRound(elFD->lineWdth) ;
                width = width*scale;
                QColor color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !color.isValid() ) color = elFD->lineClr;
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = (int)TSYS::realRound(elFD->bordWdth);
                bord_width = bord_width*scale;
                QColor bord_color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !bord_color.isValid() ) bord_color = elFD->bordClr;
                ln_st=TSYS::strSepParse(sel,0,':',&el_off).c_str();
                if( ln_st.size() == 0 ) ln_st=elFD->lineStyle;
                if( ln_st == "solid" ) style = Qt::SolidLine;
                else if( ln_st == "dashed" ) style = Qt::DashLine;
                else if( ln_st == "dotted" ) style = Qt::DotLine;
                else style = Qt::SolidLine;
                
		//-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = (*pnts)[p[i_p]];

                //-- Building the current arc --
                StartMotionPos  = ip[0];
                EndMotionPos    = ip[1];
                CtrlMotionPos_1 = ip[2];
                CtrlMotionPos_2 = ip[3];
                CtrlMotionPos_3 = ip[4];
                MotionWidth     = width;
                line2 = QLineF( QPointF( CtrlMotionPos_1.x(), CtrlMotionPos_1.y() ),
                		QPointF( CtrlMotionPos_1.x()+10, CtrlMotionPos_1.y() ) );
                line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
                if( CtrlMotionPos_3.y() < CtrlMotionPos_1.y() ) ang = angle(line1,line2);
                else ang = 360-angle(line1,line2);
                a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
                b = length( CtrlMotionPos_2, CtrlMotionPos_1 );

                CtrlMotionPos_2 = QPointF( CtrlMotionPos_1.x()+rotate(arc(0.25,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(0.25,a,b),ang).y() );
                CtrlMotionPos_3 = QPointF( CtrlMotionPos_1.x()+rotate(arc(0,a,b),ang).x(), CtrlMotionPos_1.y()-rotate(arc(0,a,b),ang).y() );
                StartMotionPos  = unRotate( StartMotionPos, ang, CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
                if( StartMotionPos.x() >= a )	StartMotionPos = QPointF( a, (StartMotionPos.y()/StartMotionPos.x())*a );
                if( StartMotionPos.x() < -a )	StartMotionPos = QPointF( -a, (StartMotionPos.y()/StartMotionPos.x())*(-a) );
		t_start = acos(StartMotionPos.x()/a)/(2*M_PI);
		if( StartMotionPos.y() > 0 )	t_start = 1-t_start;
                EndMotionPos = unRotate( EndMotionPos, ang, CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
                if( EndMotionPos.x() < -a )	EndMotionPos = QPointF( -a, (EndMotionPos.y()/EndMotionPos.x())*(-a) );
                if( EndMotionPos.x() >= a )	EndMotionPos = QPointF( a, (EndMotionPos.y()/EndMotionPos.x())*(a) );
		t_end = acos(EndMotionPos.x()/a)/(2*M_PI);
                if( EndMotionPos.y() > 0 )	t_end = 1-t_end;
                if( t_start > t_end ) 		t_end+=1;
                if( (t_end-1) > t_start ) 	t_end-=1;
                if( t_start == t_end ) 		t_end+=1;
                if( t_end > t_start && t_start >= 1 && t_end > 1 )	{ t_start-=1; t_end-=1; }
                CtrlMotionPos_4 = QPointF( t_start, t_end );
               
		//-- Building the path of the line and adding it to container --
                if( bord_width > 0 )
                {
                    circlePath = newPath;
                    ShapeItem item_temp( circlePath, newPath, p[0], p[1], p[2], p[3], p[4], CtrlMotionPos_4,
                                         QBrush( color, Qt::SolidPattern ),
                                         QPen( bord_color, bord_width, style, Qt::FlatCap, Qt::RoundJoin ),
		    			 QPen( color, width, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin ), width, bord_width, 2, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }

                if( bord_width == 0 )
                {
                    QPainterPath bigPath = newPath;
                    circlePath = newPath;
                    ShapeItem item_temp;
                    if( width < 3 )
                        item_temp = ShapeItem ( bigPath, circlePath, p[0], p[1], p[2], p[3], p[4], CtrlMotionPos_4, 
                                            QBrush( color,Qt::NoBrush ),
                                            QPen( bord_color, bord_width, Qt::NoPen, Qt::SquareCap, Qt::RoundJoin ),
                                            QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin ), width, bord_width, 2, angle_temp );
                    else
                        item_temp = ShapeItem ( bigPath, circlePath, p[0], p[1], p[2], p[3], p[4], CtrlMotionPos_4, 
                                         QBrush( color,Qt::NoBrush ),
                                         QPen( bord_color, bord_width, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin ),
                                         QPen( color, width, style, Qt::FlatCap, Qt::RoundJoin ), width, bord_width, 2, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
            }
            if( el == "bezier" )
            {
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                //if( pi.key() >= p[0] )
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str());
                else 
                {
                        p[2] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str());
                else 
                {
                        p[3] = map_index;
                        (*pnts)[map_index] = QPointF(x_s,y_s);
                        map_index -= 1;
                }
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = (int)TSYS::realRound(elFD->lineWdth);
                width = width*scale;
                QColor color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !color.isValid() ) color = elFD->lineClr;
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = (int)TSYS::realRound(elFD->bordWdth);
                bord_width = bord_width*scale;
                QColor bord_color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !bord_color.isValid() ) bord_color = elFD->bordClr;
                ln_st=TSYS::strSepParse(sel,0,':',&el_off).c_str();
                if( ln_st.size() == 0 ) ln_st=elFD->lineStyle;
                if( ln_st == "solid" ) style = Qt::SolidLine;
                else if( ln_st == "dashed" ) style = Qt::DashLine;
                else if( ln_st == "dotted" ) style = Qt::DotLine;
                else style = Qt::SolidLine;
                
                if( bord_width > 0 )
                {
                    circlePath = newPath;
                    ShapeItem item_temp( circlePath, newPath, p[0], p[1], p[2], p[3], -1, QPointF(0,0),
                                         QBrush( color, Qt::SolidPattern ),
                                         QPen( bord_color, bord_width, style, Qt::FlatCap, Qt::MiterJoin ),
                                         QPen( color, width, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin), width, bord_width, 3, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
                if( bord_width==0 )
                {
                    QPainterPath bigPath = newPath;
                    circlePath = newPath;
                    ShapeItem item_temp;
                    if( width < 3 )
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], p[2], p[3], -1, QPointF(0,0), 
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( bord_color, bord_width, Qt::NoPen, Qt::SquareCap, Qt::MiterJoin ),
                                        QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin), width, bord_width, 3, angle_temp );
                    else
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], p[2], p[3], -1, QPointF(0,0), 
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( bord_color, bord_width, Qt::NoPen, Qt::FlatCap, Qt::MiterJoin ),
                                        QPen( color, width, style, Qt::FlatCap, Qt::RoundJoin), width, bord_width, 3, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
            }
            
            if( el == "fill" )
            {
                int zero_pnts = 0;
                string fl_color, fl_img;
                QVector<int> fl_pnts;
                float x_s, y_s;
                int vl;
                while( true )
                {
                    string svl = TSYS::strSepParse( sel, 0, ':', &el_off );
                    if( sscanf(svl.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) vl = atoi(svl.c_str());
                    else 
                    {
                        bool fl = false;
                        //-- Detecting if there is a point with same coordinates in the map --
                        for( PntMap::reverse_iterator pi = pnts->rbegin(); pi != pnts->rend(); pi++ )
                            if(pi->first <= -10 )
                            {
                                if( (fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x(),2)) < 0.01) &&
                                    (fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y(),2)) < 0.01) )  
                                {
                                    vl = pi->first;
                                    fl = true;
                                    break;
                                }
                            }
                        if( !fl )
                        {
                            vl = map_index;
                            (*pnts)[map_index] = QPointF(x_s,y_s);
                            map_index -= 1;
                        }
                    }
                    if( vl ) fl_pnts.push_back(vl);
                    else if( zero_pnts == 0 ) { fl_color = svl; zero_pnts++; }
                    else if( zero_pnts == 1 ) { fl_img = svl; zero_pnts++; }
                    else break;
                }
                //- Check fill color -
                QColor color( fl_color.c_str() );
                color.setAlpha( 255 );
                if( !color.isValid() )	color = elFD->fillClr;
                //- Check fill image -
		QBrush brsh;
		QImage img;
		string backimg = w->resGet(fl_img);
		if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		    brsh.setTextureImage(img);
                else brsh.setTextureImage(elFD->fillImg.textureImage ());
                //- Make elements -
                if( fl_pnts.size() > 1 ) inundationItems.push_back(inundationItem(newPath,color,brsh, fl_pnts, fl_pnts));
            }
        }
        for( int i=0; i < shapeItems_temp.size(); i++ )
        {
            dropPoint( shapeItems_temp[i].n1, shapeItems.size()+1, shapeItems, pnts );
            dropPoint( shapeItems_temp[i].n2, shapeItems.size()+1, shapeItems, pnts );
            dropPoint( shapeItems_temp[i].n3, shapeItems.size()+1, shapeItems, pnts );
            dropPoint( shapeItems_temp[i].n4, shapeItems.size()+1, shapeItems, pnts );
            dropPoint( shapeItems_temp[i].n5, shapeItems.size()+1, shapeItems, pnts );
        }
        up = true;
    }	
    if( up && !w->allAttrLoad( ) )
    {
        //- Repainting all shapes by calling moveItemTo to each shape -
        QVector<int> inundation_fig_num;
        bool flag_push_back, fl_brk;
        for( int i=0; i < shapeItems.size(); i++ )
            if( shapeItems[i].type == 2 )
            {
                //- Detecting if the start or end point of this arc matches the start or end point of the other arc and, if so, add this point to the map again -
                for( int j=0; j < shapeItems.size(); j++ )
                {
                    if( (shapeItems[j].n1 == shapeItems[i].n1 || shapeItems[j].n1 == shapeItems[i].n2) && i != j && shapeItems[j].type == 2 )
                        shapeItems[j].n1 = appendPoint( (*pnts)[shapeItems[j].n1], shapeItems, pnts, 1 );
                    if( (shapeItems[j].n2 == shapeItems[i].n1 || shapeItems[j].n2 == shapeItems[i].n2) && i != j && shapeItems[j].type == 2 )
                        shapeItems[j].n2 = appendPoint( (*pnts)[shapeItems[j].n2], shapeItems, pnts, 1 );
                }
                count_moveItemTo = 1;
                count_Shapes     = 1;
                flag_ctrl_move   = false;
                flag_ctrl        = true;
                offset = QPointF( 0, 0 );
                index            = i;
                itemInMotion = &shapeItems[index];
                moveItemTo( QPointF(0,0), shapeItems, pnts, w );
            }
        for( int i=0; i < shapeItems.size(); i++ )
            if( shapeItems[i].type != 2 )
            {
                count_moveItemTo = 1;
                count_Shapes     = 1;
                flag_ctrl_move   = false;
                flag_ctrl        = true;
                offset = QPointF(0,0);
                index            = i;
                itemInMotion = &shapeItems[index];
                moveItemTo( QPointF(0,0), shapeItems, pnts, w );
            }
        bool fill_build = false;
        bool flag_all = false;
        QVector<inundationItem> in_build;
        QVector<int> fill_number;
        //- Detecting if there is a necessity to rebuild inundationItem -
        if( shapeItems.size() != shapeItems_temp.size() ) fill_build = true;
        else
            for( int i = 0; i < shapeItems.size(); i++ )
            {
                //-- Detecting if the shapeItems have changed --
                if( shapeItems[i].path != shapeItems_temp[i].path ) { fill_build = true; break; }
                if( shapeItems[i].pathSimple != shapeItems_temp[i].pathSimple ) { fill_build = true; break; }
                if( (int)TSYS::realRound(shapeItems[i].ctrlPos4.x(), 3, true) != 
                    (int)TSYS::realRound(shapeItems_temp[i].ctrlPos4.x(), 3, true) ||
                    (int)TSYS::realRound(shapeItems[i].ctrlPos4.y(), 3, true) != 
                    (int)TSYS::realRound(shapeItems_temp[i].ctrlPos4.y(), 3, true) ) { fill_build = true; break; }
                if( shapeItems[i].n1 != shapeItems_temp[i].n1 ) { fill_build = true; break; }
                if( shapeItems[i].n2 != shapeItems_temp[i].n2 ) { fill_build = true; break; }
                if( shapeItems[i].n3 != shapeItems_temp[i].n3 ) { fill_build = true; break; }
                if( shapeItems[i].n4 != shapeItems_temp[i].n4 ) { fill_build = true; break; }
                if( shapeItems[i].n5 != shapeItems_temp[i].n5 ) { fill_build = true; break; }
                if( shapeItems[i].brush != shapeItems_temp[i].brush ) { fill_build = true; break; }
                if( shapeItems[i].pen != shapeItems_temp[i].pen ) { fill_build = true; break; }
                if( shapeItems[i].penSimple != shapeItems_temp[i].penSimple ) { fill_build = true; break; }
                if( shapeItems[i].width != shapeItems_temp[i].width ) { fill_build = true; break; }
                if( shapeItems[i].border_width != shapeItems_temp[i].border_width ) { fill_build = true; break; }
                if( shapeItems[i].type != shapeItems_temp[i].type ) { fill_build = true; break; }
                if( (int)TSYS::realRound(shapeItems[i].angle_temp, 2, true) != 
                    (int)TSYS::realRound(shapeItems_temp[i].angle_temp, 2, true) ) { fill_build = true; break; }
            }
        //-- Detecting the difference beetwen inundationItems --
        if( inundationItems.size() != inundationItems_temp.size() )
        {
            flag_all = true;
            in_build = inundationItems;
        }
        else
            for( int i_f = 0; i_f < inundationItems.size(); i_f++ )
            {
                if( (inundationItems[i_f].number_shape != inundationItems_temp[i_f].number_point) || fill_build )// && shapeItems != shapeItems_temp
                {
                    in_build.push_back(inundationItems[i_f]);
                    fill_number.push_back(i_f);
                    continue;
                }
                if( inundationItems[i_f].brush != inundationItems_temp[i_f].brush )
                     inundationItems_temp[i_f].brush = inundationItems[i_f].brush;
                if( inundationItems[i_f].brush_img != inundationItems_temp[i_f].brush_img )
                    inundationItems_temp[i_f].brush_img = inundationItems[i_f].brush_img;
            }

            inundationItems.clear();
            inundationItems = in_build;
        //-Building fills-
        if( shapeItems.size() )
            for( int i=0; i < inundationItems.size(); i++ )
            {
                if( inundationItems[i].number_shape.size() > 2 )
                {
                    for( int k=0; k < inundationItems[i].number_shape.size()-1; k++ )
                        for( int j=0; j < shapeItems.size(); j++ )
                            if( (shapeItems[j].n1 == inundationItems[i].number_shape[k] && shapeItems[j].n2 == inundationItems[i].number_shape[k+1]) ||
                                (shapeItems[j].n1 == inundationItems[i].number_shape[k+1] && shapeItems[j].n2 == inundationItems[i].number_shape[k]) )
                            {
                                flag_push_back = true;
                                for( int p=0; p < inundation_fig_num.size(); p++ )
                                    if( (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
                                        (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1) )
                                    {
                                        flag_push_back = false;
                                        if( (shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j) )
                                            inundation_fig_num[p]=j;
                                        if( (shapeItems[inundation_fig_num[p]].type==3 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j) )
                                            inundation_fig_num[p]=j;
                                        if( (shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==3) && (inundation_fig_num[p]!=j) )
					    inundation_fig_num[p]=j;
                                    }
                                if( flag_push_back ) inundation_fig_num.push_back(j);
                            }
                    for( int j=0; j < shapeItems.size(); j++ )
                        if( (shapeItems[j].n1 == inundationItems[i].number_shape[inundationItems[i].number_shape.size()-1] && shapeItems[j].n2 == inundationItems[i].number_shape[0]) ||
                            (shapeItems[j].n1 == inundationItems[i].number_shape[0] && shapeItems[j].n2 == inundationItems[i].number_shape[inundationItems[i].number_shape.size()-1]) )
                        {
                            flag_push_back = true;
                            for( int p=0; p < inundation_fig_num.size(); p++ )
                                if( (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
				    (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1) )
                                {
                                    flag_push_back = false;
                                    if( (shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j) )
                                        inundation_fig_num[p] = j;
                                    if( (shapeItems[inundation_fig_num[p]].type==3 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j) )
                                        inundation_fig_num[p] = j;
                                    if( (shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==3) && (inundation_fig_num[p]!=j) )
                                        inundation_fig_num[p] = j;
                                }
                                if( flag_push_back ) inundation_fig_num.push_back(j);
                        }
                    QPainterPath temp_path = createInundationPath( inundation_fig_num, shapeItems, pnts,w );
                    inundation_fig_num = inundationSort( temp_path, inundation_fig_num, shapeItems, pnts, w );
                    inundationItems[i].path = createInundationPath( inundation_fig_num, shapeItems, pnts, w );
                    inundationItems[i].number_shape = inundation_fig_num;
                    if( inundation_fig_num.size() > inundationItems[inundationItems.size()-1].number_shape.size() ) 
                        inundationItems[inundationItems.size()-1].path = newPath;
                }
                if( inundationItems[i].number_shape.size()==2 )
                {
                    for( int j=0; j < shapeItems.size(); j++ )
                    {
                        if( ((shapeItems[j].n1 == inundationItems[i].number_shape[0]) && (shapeItems[j].n2 == inundationItems[i].number_shape[1])) ||
                            ((shapeItems[j].n1 == inundationItems[i].number_shape[1]) && (shapeItems[j].n2 == inundationItems[i].number_shape[0])) )
                            inundation_fig_num.push_back(j);
                        if( inundation_fig_num.size() == 2 ) break;
                    }
                    inundationItems[i].path = createInundationPath( inundation_fig_num, shapeItems, pnts, w );
                    inundationItems[i].number_shape = inundation_fig_num;
                }
                inundation_fig_num.clear();
            }
         if( inundationItems_temp.size() && shapeItems.size() && !flag_all )
        {
            for( int k = 0; k < fill_number.size(); k++ )
                inundationItems_temp.replace( fill_number[k], inundationItems[k] );
            inundationItems.clear();
            inundationItems = inundationItems_temp;
        }
        itemInMotion = 0;
        index        = -1;
        if( rectItems.size() )	rectItems.clear();
        flag_ctrl    = false;
    }

    if( up && !w->allAttrLoad( ) && uiPrmPos != -1 )
        w->update();
    return up;
}


//-Saving shapes' attributes to data model-
bool ShapeElFigure::shapeSave( WdgView *w )
{
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundationItems = elFD->inundationItems;
    PntMap *pnts = &elFD->shapePnts;
    string elList, ln_st;
    Qt::PenStyle style;
    double scale;
    if( w->xScale(true) < w->yScale(true) ) scale = w->xScale(true);
    else scale = w->yScale(true);
    //- Building attributes for all el_figures and fills -
    //--for el_figures--
    for( int i_s = 0; i_s < shapeItems.size(); i_s++ )
    {
	switch( shapeItems[i_s].type )
	{
	    case 1:
                shapeItems[i_s].width = shapeItems[i_s].width/scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width/scale;
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else
                    style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
                elList+="line:";
                if( shapeItems[i_s].n1 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n1)+":";
                else if( shapeItems[i_s].n1 <= -10 )
                    elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[i_s].n1].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n1].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n2 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n2)+":";
                else if( shapeItems[i_s].n2 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].y(),2) ) + ")" + ":";
                elList += (((int)TSYS::realRound(shapeItems[i_s].width) == (int)TSYS::realRound(elFD->lineWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
                          ((shapeItems[i_s].brush.color().name() == elFD->lineClr.name()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
                          (((int)TSYS::realRound(shapeItems[i_s].border_width) == (int)TSYS::realRound(elFD->bordWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
                          ((shapeItems[i_s].pen.color().name() == elFD->bordClr.name()) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                          ((ln_st == elFD->lineStyle) ? "" : ln_st)+"\n";
                shapeItems[i_s].width = shapeItems[i_s].width*scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width*scale;
		break;
	    case 2:
                shapeItems[i_s].width = shapeItems[i_s].width/scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width/scale;
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
                elList+="arc:";
                if( shapeItems[i_s].n1 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n1)+":";
                else if( shapeItems[i_s].n1 <= -10 )
                    elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[i_s].n1].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n1].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n2 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n2)+":";
                else if( shapeItems[i_s].n2 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n3 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n3)+":";
                else if( shapeItems[i_s].n3 <= -10 )
                    elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[i_s].n3].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n3].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n4 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n4)+":";
                else if( shapeItems[i_s].n4 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n4].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n4].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n5 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n5)+":";
                else if( shapeItems[i_s].n5 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n5].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n5].y(),2) ) + ")" + ":";
                elList += (((int)TSYS::realRound(shapeItems[i_s].width) == (int)TSYS::realRound(elFD->lineWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
                          ((shapeItems[i_s].brush.color().name() == elFD->lineClr.name()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
                          (((int)TSYS::realRound(shapeItems[i_s].border_width) == (int)TSYS::realRound(elFD->bordWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
                          ((shapeItems[i_s].pen.color().name() == elFD->bordClr.name()) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                          ((ln_st == elFD->lineStyle) ? "" : ln_st)+"\n";
                shapeItems[i_s].width = shapeItems[i_s].width*scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width*scale;
		break;
	    case 3:
                shapeItems[i_s].width = shapeItems[i_s].width/scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width/scale;
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
                elList+="bezier:";
                if( shapeItems[i_s].n1 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n1)+":";
                else if( shapeItems[i_s].n1 <= -10 )
                    elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[i_s].n1].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n1].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n2 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n2)+":";
                else if( shapeItems[i_s].n2 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n2].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n3 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n3)+":";
                else if( shapeItems[i_s].n3 <= -10 )
                    elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[i_s].n3].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n3].y(),2) ) + ")" + ":";
                if( shapeItems[i_s].n4 > 0 )
                    elList += TSYS::int2str(shapeItems[i_s].n4)+":";
                else if( shapeItems[i_s].n4 <= -10 )
                    elList +="(" + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n4].x(),2) ) + "," 
                                 + TSYS::real2str(TSYS::realRound((*pnts)[shapeItems[i_s].n4].y(),2) ) + ")" + ":";
               elList += (((int)TSYS::realRound(shapeItems[i_s].width) == (int)TSYS::realRound(elFD->lineWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
                         ((shapeItems[i_s].brush.color().name() == elFD->lineClr.name()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
                         (((int)TSYS::realRound(shapeItems[i_s].border_width) == (int)TSYS::realRound(elFD->bordWdth)) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
                         ((shapeItems[i_s].pen.color().name() == elFD->bordClr.name()) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                         ((ln_st == elFD->lineStyle) ? "" : ln_st)+"\n";
                shapeItems[i_s].width = shapeItems[i_s].width*scale;
                shapeItems[i_s].border_width = shapeItems[i_s].border_width*scale;
		break;
	}
    }
    //--for fills--
    for( int i=0; i < inundationItems.size(); i++ )
    {
        bool flag_n1 = false;
        bool flag_n2 = false;
        bool fl_br   = false;
        bool exist_fill = false;
        QVector<int> temp;
        int index_element = -1;
        bool flag_for_break = false;
	elList += "fill:";

	for( int k=0; k < inundationItems[i].number_shape.size()-1; k++ )
	{
    	    if( inundationItems[i].number_shape.size() > 2 )
	    if( k==0 )
	    {
		if( shapeItems[inundationItems[i].number_shape[k]].n1 == shapeItems[inundationItems[i].number_shape[k+1]].n1 )
                {
                    if( shapeItems[inundationItems[i].number_shape[k]].n2 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n2 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].y(),2) ) + ")" + ":";
                    if( shapeItems[inundationItems[i].number_shape[k]].n1 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n1 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].y(),2) ) + ")" + ":";
                }
		else
                {
                    if( shapeItems[inundationItems[i].number_shape[k]].n1 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n1 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].y(),2) ) + ")" + ":";
                    if( shapeItems[inundationItems[i].number_shape[k]].n2 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n2 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].y(),2) ) + ")" + ":";
                }
		temp.push_back( shapeItems[inundationItems[i].number_shape[k]].n1 );
		temp.push_back( shapeItems[inundationItems[i].number_shape[k]].n2 );
	    }
	    else
	    {
		for( int p=0; p < temp.size(); p++ )
		{
		    if( shapeItems[inundationItems[i].number_shape[k]].n1 == temp[p] )
		    { flag_n1 = true; flag_n2 = false; break; }
		    else { flag_n2=true; flag_n1=false; }
		}
    		if( flag_n1 && temp.size()<inundationItems[i].number_shape.size() )
		{
                    if( shapeItems[inundationItems[i].number_shape[k]].n2 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n2 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n2].y(),2) ) + ")" + ":";

		    temp.push_back( shapeItems[inundationItems[i].number_shape[k]].n2 );
		}
		if( flag_n2 && temp.size()<inundationItems[i].number_shape.size() )
		{
                    if( shapeItems[inundationItems[i].number_shape[k]].n1 > 0 )
                        elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
                    else if( shapeItems[inundationItems[i].number_shape[k]].n1 <= -10 )
                        elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].x(),2) ) + "," 
                                     + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[k]].n1].y(),2) ) + ")" + ":";
		    temp.push_back(shapeItems[inundationItems[i].number_shape[k]].n1);
		}
	    }
	}
	if( inundationItems[i].number_shape.size() <= 2 )
        {
            if( shapeItems[inundationItems[i].number_shape[0]].n1 > 0 )
                elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n1)+":";
            else if( shapeItems[inundationItems[i].number_shape[0]].n1 <= -10 )
                elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[0]].n1].x(),2) ) + "," 
                             + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[0]].n1].y(),2) ) + ")" + ":";
            if( shapeItems[inundationItems[i].number_shape[0]].n2 > 0 )
                elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n2)+":";
            else if( shapeItems[inundationItems[i].number_shape[0]].n2 <= -10 )
                elList +="(" + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[0]].n2].x(),2) ) + "," 
                             + TSYS::real2str( TSYS::realRound((*pnts)[shapeItems[inundationItems[i].number_shape[0]].n2].y(),2) ) + ")" + ":";
        }
        elList = elList + ((inundationItems[i].brush.color().name() == elFD->fillClr.name()) ? "" : inundationItems[i].brush.color().name().toAscii().data()) + "\n";
    }
    w->attrSet( "elLst", elList );

    //- Write shapes points to data model -
    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
    {
        if(pi->first > 0 )
        {
            w->attrSet( "p"+TSYS::int2str(pi->first)+"x", TSYS::real2str(TSYS::realRound(pi->second.x(),2)) );
            w->attrSet( "p"+TSYS::int2str(pi->first)+"y", TSYS::real2str(TSYS::realRound(pi->second.y(),2)) );
        }
    }
    devW->setSelect(true,false);
}

void ShapeElFigure::editEnter( WdgView *view )
{    
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(true);
    
    //- Self-shape tools -
    connect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    //-- Init actions' address --
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(true);
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setProperty("wdgAddr",TSYS::addr2str(view).c_str());
    }
    //- Main tools (copy) -
    connect( ((VisDevelop *)view->mainWin())->visItToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->actVisItCopy->setProperty("wdgAddr",TSYS::addr2str(view).c_str());
    ((VisDevelop *)view->mainWin())->actVisItPaste->setProperty("wdgAddr",TSYS::addr2str(view).c_str());
    ((VisDevelop *)view->mainWin())->actVisItCut->setVisible(false); 
    ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(false); 
    ((VisDevelop *)view->mainWin())->actVisItPaste->setEnabled(false);
    //- Figures level tools -
    connect( ((VisDevelop *)view->mainWin())->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) ); 
    ((VisDevelop *)view->mainWin())->actLevRise->setProperty("wdgAddr",TSYS::addr2str(view).c_str());
    ((VisDevelop *)view->mainWin())->actLevLower->setProperty("wdgAddr",TSYS::addr2str(view).c_str());
    ((VisDevelop *)view->mainWin())->actLevRise->setEnabled(false);
    ((VisDevelop *)view->mainWin())->actLevLower->setEnabled(false);
    status_hold = true;
}

void ShapeElFigure::editExit( WdgView *view )
{
    disconnect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(false);
    //- Clear action's address -
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setProperty("wdgAddr","");
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(false);
    }
    ((VisDevelop *)view->mainWin())->actElFigLine->setChecked(false);
    ((VisDevelop *)view->mainWin())->actElFigArc->setChecked(false);
    ((VisDevelop *)view->mainWin())->actElFigBesie->setChecked(false);
    ((VisDevelop *)view->mainWin())->actElFigCheckAct->setChecked(true);
    ((VisDevelop *)view->mainWin())->actElFigCursorAct->setChecked(true);
 
    disconnect( ((VisDevelop *)view->mainWin())->visItToolBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->actVisItCopy->setProperty("wdgAddr","");
    ((VisDevelop *)view->mainWin())->actVisItPaste->setProperty("wdgAddr","");
    ((VisDevelop *)view->mainWin())->actVisItCut->setVisible(true); 
    disconnect( ((VisDevelop *)view->mainWin())->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->actLevRise->setProperty("wdgAddr","");
    ((VisDevelop *)view->mainWin())->actLevLower->setProperty("wdgAddr","");
    
    shapeSave(view);
    view->unsetCursor();
    status = false;
    flag_ctrl = flag_A = flag_copy = false;
    itemInMotion = 0;
    count_Shapes = 0;
    index_array.clear();
    if( rectItems.size() ) rectItems.clear();
}

void ShapeElFigure::wdgPopup( WdgView *w, QMenu &menu )
{
    if( qobject_cast<DevelWdgView*>(w) )
    {
        bool actDyn = false;
        ElFigDt *elFD = (ElFigDt*)w->shpData;
        QVector<ShapeItem> &shapeItems = elFD->shapeItems;
	for( int i_a = 0; i_a < ((VisDevelop *)w->mainWin())->elFigTool->actions().size(); i_a++ )
	    menu.addAction(((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a));
	menu.addSeparator();
        menu.addAction(((VisDevelop *)w->mainWin())->actVisItCopy);
        menu.addAction(((VisDevelop *)w->mainWin())->actVisItPaste);
        menu.addSeparator();
        menu.addAction(((VisDevelop *)w->mainWin())->actLevRise);
        menu.addAction(((VisDevelop *)w->mainWin())->actLevLower);
        menu.addSeparator();
        switch( rect_num )
        {
            case 0:
                if( shapeItems[index].n1 <= -10 )
                    actDyn = true;
                break;
            case 1:
                if( shapeItems[index].n2 <= -10 )
                    actDyn = true;
                break;
            case 2:
                if( shapeItems[index].n3 <= -10 )
                    actDyn = true;
                break;
            case 3:
                if( shapeItems[index].n4 <= -10 )
                    actDyn = true;
                break;
            case 4:
                if( shapeItems[index].n5 <= -10 )
                    actDyn = true;
                break;
        }
        if( actDyn )
        {
            QAction *actDynamicPoint = new QAction( _("Make this point dynamic"), this );
            connect( actDynamicPoint, SIGNAL(activated()), elFD, SLOT(dynamicPoint()) ); 
            menu.addAction(actDynamicPoint);
            menu.addSeparator();
        }
    }
}
	
void ShapeElFigure::toolAct( QAction *act )
{
    bool ptr_line,ptr_arc,ptr_bezier;
    WdgView *w = (WdgView*)TSYS::str2addr(act->property("wdgAddr").toString().toAscii().data());
    if( !w ) return;
    ElFigDt *elFD = (ElFigDt*)w->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundationItems = elFD->inundationItems;
    PntMap *pnts = &elFD->shapePnts;

    if( !w )    return;
    if( act->objectName() == "cursor" || act->objectName() == "line" || 
            act->objectName() == "arc" || act->objectName() == "besier" )
        for( int i_a = 0; i_a < ((VisDevelop *)w->mainWin())->elFigTool->actions().size(); i_a++ )
        {
            if( ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->objectName() != "hold" )
                ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->setChecked(false);
        }
    if( act->objectName() == "line" )
    {
        act->setChecked(true);
        shapeType = 1;
        if( flag_A )
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
        w->repaint();
    }
    else if( act->objectName() == "arc" )
    {
        act->setChecked(true);
        shapeType = 2;
        if( flag_A )
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
        w->repaint();
    }
    else if( act->objectName() == "besier" )
    {
        act->setChecked(true);
        shapeType = 3;
        if( flag_A )
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
        w->repaint();
    }
    else if( act->objectName() == "hold" )
    {
        status_hold = act->isChecked();
        if( flag_A )
        {
            flag_ctrl = flag_A = flag_copy = false;
            flag_check_pnt_inund = false;
            itemInMotion = 0;
            count_Shapes = 0;
        }
        index_array.clear();
        index_array_copy.clear();
        rectItems.clear();
        w->repaint();
    }
    else if( act->objectName() == "cursor" )
    {
        act->setChecked(true);
        w->unsetCursor();
        status = false;
        w->update();
    }
    else if( act->objectName() == "editcopy" )
    {
        if( copy_index.size() ) copy_index.clear();
        if( flag_A ) 
        {
            if( index_array_copy_flag_A.size() ) copy_index = index_array_copy_flag_A;
            else copy_index = index_array;
        }
        else if( index_array.size() && !status_hold && index_array[0] != -1 ) copy_index = index_array;
        else if( status_hold && index_array_copy.size() ) copy_index = index_array_copy;
        else if( index_temp != -1 ) copy_index.push_back( index_temp );
        index_array_copy_flag_A.clear();
        ((VisDevelop *)w->mainWin())->actVisItPaste->setEnabled(true);
    }
    else if( act->objectName() == "editpaste" )
    {
        QPointF Temp;
        if ( index_array.size() ) index_array.clear();
        for( int i=0; i < copy_index.size(); i++ )
        {
            if( copy_index[i] != -1 )
            {
                shapeItems.push_back( shapeItems[copy_index[i]] );
                index_array.push_back( shapeItems.size()-1 );
                switch( shapeItems[copy_index[i]].type )
                {
                    case 1:
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n1];
                        if( shapeItems[shapeItems.size()-1].n1 > 0 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if(  shapeItems[shapeItems.size()-1].n1 <= -10 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n2];
                        if( shapeItems[shapeItems.size()-1].n2 > 0 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n2 <= -10 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 1 );
                        break;
                    case 2:
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n1];
                        if( shapeItems[shapeItems.size()-1].n1 > 0 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n1 <= -10 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n2];
                        if( shapeItems[shapeItems.size()-1].n2 > 0 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n2 <= -10 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n3];
                        if( shapeItems[shapeItems.size()-1].n3 > 0 )
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n3 <= -10 )
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n4];
                        if( shapeItems[shapeItems.size()-1].n4 > 0 )
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n4 <= -10 )
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n5];
                        if( shapeItems[shapeItems.size()-1].n5 > 0 )
                            shapeItems[shapeItems.size()-1].n5 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n5 <= -10 )
                            shapeItems[shapeItems.size()-1].n5 = appendPoint( Temp, shapeItems, pnts, 1 );
                        break;
                    case 3:
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n1];
                        if( shapeItems[shapeItems.size()-1].n1 > 0 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n1 <= -10 )
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n2];
                        if( shapeItems[shapeItems.size()-1].n2 > 0 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n2 <= -10 )
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n3];
                        if( shapeItems[shapeItems.size()-1].n3 > 0 )
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n3 <= -10 )
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( Temp, shapeItems, pnts, 1 );
                        Temp = (*pnts)[shapeItems[shapeItems.size()-1].n4];
                        if( shapeItems[shapeItems.size()-1].n4 > 0 )
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( Temp, shapeItems, pnts, 0 );
                        else if( shapeItems[shapeItems.size()-1].n4 <= -10 )
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( Temp, shapeItems, pnts, 1 );
                        break;
                }
            }
        }
        if( index_array.size() )
        {
            flag_A = true;
            flag_copy = true;
            flag_ctrl_move = 1;
            count_Shapes = index_array.size();
            moveAll( QPointF(0,0), shapeItems, pnts, inundationItems, w );
            w->repaint();
            shapeSave( w );
        }
        ((VisDevelop *)w->mainWin())->actVisItPaste->setEnabled(false);
    }
    else if( act->objectName() == "level_rise" )
    {
        int index_array_inund;   
        if( index_array.size() && !status_hold && index_array[0] != -1 )
        {
            ShapeItem item_temp;
            for( int i = 0; i < index_array.size(); i++  )
                if( index_array[i] != -1 )
            {
                index_array_inund = index_array[i];
                item_temp = shapeItems[index_array[i]];
                for( int j = index_array[i]; j < shapeItems.size()-1; j++ )
                {
                    shapeItems[j] = shapeItems[j+1];
                    for( int k = 0; k < index_array.size(); k++ )
                        if( j == index_array[k] && index_array[k] != -1 ) 
                            index_array[k] = index_array[k] - 1;
                }
                shapeItems[shapeItems.size()-1] = item_temp;
                for( int j = 0; j < inundationItems.size(); j++  )
                    for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                        if( inundationItems[j].number_shape[p] == index_array_inund )
                            inundationItems[j].number_shape[p] = shapeItems.size()-1;
                        else if( inundationItems[j].number_shape[p] > index_array_inund )
                            inundationItems[j].number_shape[p]--;
            }
        }
        else if( status_hold && ( index_array_copy.size() || 
                 (index_array_copy.size() == 0 && ( index_array.size() && index_array[0] != -1) ) ) )
        {
            if( (index_array_copy.size() == 0 && ( index_array.size() && index_array[0] != -1) ) )
                index_array_copy = index_array;
            ShapeItem item_temp;
            for( int i = 0; i < index_array_copy.size(); i++  )
                if( index_array_copy[i] != -1 )
                {
                    index_array_inund = index_array_copy[i];
                    item_temp = shapeItems[index_array_copy[i]];
                    for( int j = index_array_copy[i]; j < shapeItems.size()-1; j++ )
                    {
                        shapeItems[j] = shapeItems[j+1];
                        for( int k = 0; k < index_array_copy.size(); k++ )
                            if( j == index_array_copy[k] && index_array_copy[k] != -1 ) 
                                index_array_copy[k] = index_array_copy[k] - 1;
                    }
                    shapeItems[shapeItems.size()-1] = item_temp;
                    for( int j = 0; j < inundationItems.size(); j++  )
                        for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                            if( inundationItems[j].number_shape[p] == index_array_inund )
                                inundationItems[j].number_shape[p] = shapeItems.size()-1;
                            else if( inundationItems[j].number_shape[p] > index_array_inund )
                                inundationItems[j].number_shape[p]--;
                }
        }
        else if( index_temp != -1 )
        {
            shapeItems.push_back(shapeItems[index_temp]);
            shapeItems.remove( index_temp );
            for( int j = 0; j < inundationItems.size(); j++  )
                for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                    if( inundationItems[j].number_shape[p] == index_temp )
                        inundationItems[j].number_shape[p] = shapeItems.size()-1;
                    else if( inundationItems[j].number_shape[p] > index_temp )
                        inundationItems[j].number_shape[p]--;
        }
        shapeSave( w );
        itemInMotion = 0;
        rectItems.clear();
        index_array.clear();
        index_array_copy.clear();
        index = index_temp = -1;
        flag_ctrl = flag_A = flag_copy = false;
        ((VisDevelop *)w->mainWin())->actLevRise->setEnabled(false);
        ((VisDevelop *)w->mainWin())->actLevLower->setEnabled(false);
        w->repaint();
    }
    else if( act->objectName() == "level_lower" )
    {
        int index_array_inund;
        if( index_array.size() && !status_hold && index_array[0] != -1 )
        {
            ShapeItem item_temp;
            for( int i = 0; i < index_array.size(); i++  )
                if( index_array[i] != -1 )
            {
                index_array_inund = index_array[i];
                item_temp = shapeItems[index_array[i]];
                for( int j = index_array[i]; j > 0; j-- )
                {
                    shapeItems[j] = shapeItems[j-1];
                    for( int k = 0; k < index_array.size(); k++ )
                        if( j == index_array[k] && index_array[k] != -1 ) 
                            index_array[k] = index_array[k] + 1;
                }
                shapeItems[0] = item_temp;
                for( int j = 0; j < inundationItems.size(); j++  )
                    for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                        if( inundationItems[j].number_shape[p] == index_array_inund )
                            inundationItems[j].number_shape[p] = 0;
                        else if( inundationItems[j].number_shape[p] < index_array_inund )
                            inundationItems[j].number_shape[p]++;
            }
        }
        else if( status_hold && ( index_array_copy.size() || 
                 (index_array_copy.size() == 0 && ( index_array.size() && index_array[0] != -1) ) ) )
        {
            if( (index_array_copy.size() == 0 && ( index_array.size() && index_array[0] != -1) ) )
                index_array_copy = index_array;
            ShapeItem item_temp;
            for( int i = 0; i < index_array_copy.size(); i++  )
                if( index_array_copy[i] != -1 )
                {
                    index_array_inund = index_array_copy[i];
                    item_temp = shapeItems[index_array_copy[i]];
                    for( int j = index_array_copy[i]; j > 0; j-- )
                    {
                        shapeItems[j] = shapeItems[j-1];
                        for( int k = 0; k < index_array_copy.size(); k++ )
                            if( j == index_array_copy[k] && index_array_copy[k] != -1 ) 
                                index_array_copy[k] = index_array_copy[k] + 1;
                    }
                    shapeItems[0] = item_temp;
                    for( int j = 0; j < inundationItems.size(); j++  )
                        for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                            if( inundationItems[j].number_shape[p] == index_array_inund )
                                inundationItems[j].number_shape[p] = 0;
                            else if( inundationItems[j].number_shape[p] < index_array_inund )
                                inundationItems[j].number_shape[p]++;
                }
        }
        else if( index_temp != -1 )
        {
            shapeItems.push_front(shapeItems[index_temp]);
            shapeItems.remove( index_temp + 1 );
            for( int j = 0; j < inundationItems.size(); j++  )
                for( int p = 0; p < inundationItems[j].number_shape.size(); p++ ) 
                    if( inundationItems[j].number_shape[p] == index_temp )
                        inundationItems[j].number_shape[p] = 0;
                    else if( inundationItems[j].number_shape[p] < index_temp )
                        inundationItems[j].number_shape[p]++;
        }
        shapeSave( w );
        itemInMotion = 0;
        rectItems.clear();
        index_array.clear();
        index_array_copy.clear();
        index = index_temp = -1;
        flag_ctrl = flag_A = flag_copy = false;
        ((VisDevelop *)w->mainWin())->actLevRise->setEnabled(false);
        ((VisDevelop *)w->mainWin())->actLevLower->setEnabled(false);
        w->repaint();
    }
}
//- Making a static point the dynamic one -
void ElFigDt::dynamicPoint()
{
    ShapeElFigure *elF = (ShapeElFigure*) mod->getWdgShape("ElFigure");
    PntMap *pnts = &shapePnts;

    QPointF Temp;
    int tmp, real;
    tmp = -5;
    real = -5;
    if( elF->rect_dyn != -1)
        switch( elF->rect_dyn )
        {
            case 0:
                if( shapeItems[elF->index].n1 <= -10 )
                {
                    Temp = (*pnts)[shapeItems[elF->index].n1];
                    tmp = shapeItems[elF->index].n1;
                    real = elF->appendPoint( Temp, shapeItems, pnts, 0 );
                }
                break;
            case 1:
                if( shapeItems[elF->index].n2 <= -10 )
                {
                    Temp = (*pnts)[shapeItems[elF->index].n2];
                    tmp = shapeItems[elF->index].n2;
                    real = elF->appendPoint( Temp, shapeItems, pnts, 0 );
                }
                break;
            case 2:
                if( shapeItems[elF->index].n3 <= -10 )
                {
                    Temp = (*pnts)[shapeItems[elF->index].n3];
                    tmp = shapeItems[elF->index].n3;
                    real = elF->appendPoint( Temp, shapeItems, pnts, 0 );
                }
                break;
            case 3:
                if( shapeItems[elF->index].n4 <= -10 )
                {
                    Temp = (*pnts)[shapeItems[elF->index].n4];
                    tmp = shapeItems[elF->index].n4;
                    real = elF->appendPoint( Temp, shapeItems, pnts, 0 );
                }
                break;
            case 4:
                if( shapeItems[elF->index].n5 <= -10 )
                {
                    Temp = (*pnts)[shapeItems[elF->index].n5];
                    tmp = shapeItems[elF->index].n5;
                    real = elF->appendPoint( Temp, shapeItems, pnts, 0 );
                }
                break;
        }
    if( tmp <= -10 && real > 0  )
    {
        for( int i = 0; i < shapeItems.size(); i++ )
        {
            if( shapeItems[i].n1 == tmp ) shapeItems[i].n1 = real;
            else if( shapeItems[i].n2 == tmp ) shapeItems[i].n2 = real;
            else if( shapeItems[i].n3 == tmp ) shapeItems[i].n3 = real;
            else if( shapeItems[i].n4 == tmp ) shapeItems[i].n4 = real;
            else if( shapeItems[i].n5 == tmp ) shapeItems[i].n5 = real;
        }
        elF->dropPoint( tmp, elF->index, shapeItems, pnts );
        elF->shapeSave( w );
    }
}


bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    ElFigDt *elFD = (ElFigDt*)view->shpData;
    QVector<ShapeItem> &shapeItems = elFD->shapeItems;
    QVector<inundationItem> &inundationItems = elFD->inundationItems;
    PntMap *pnts = &elFD->shapePnts;
    bool flag_hold_rect;
    flag_hold_rect = false;
    switch( event->type() )
    { 
        case QEvent::Paint:
        {
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            QPainter pnt( view );
            //- Prepare draw area -
            int margin = elFD->geomMargin;
            QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
            pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
            //- Drawing all fills(inundations) -
            for( int i=0; i < inundationItems.size(); i++ )
            {
                if( !inundationItems[i].brush_img.textureImage().isNull() )
                {
                    QPainterPath in_path;
                    in_path = newPath;
                    //-- Building the scaled and unrotated inundation path --
                    flag_rotate = false;
                    in_path = createInundationPath( inundationItems[i].number_shape, shapeItems, pnts, view );
                    flag_rotate = true;
                    //-- Scaling image for filling --
                    double xMax = in_path.pointAtPercent ( 0 ).x();
                    double xMin = in_path.pointAtPercent ( 0 ).x();
                    double yMax = in_path.pointAtPercent ( 0 ).y();
                    double yMin = in_path.pointAtPercent ( 0 ).y();
                    double t = 0.01;
                    do
                    {
                        QPointF pnt_ = in_path.pointAtPercent ( t );
                        if( pnt_.x() < xMin ) xMin = pnt_.x();
                        if( pnt_.x() > xMax ) xMax = pnt_.x();
                        if( pnt_.y() < yMin ) yMin = pnt_.y();
                        if( pnt_.y() > yMax ) yMax = pnt_.y();
                        t += 0.01;
                    }
                    while( t < 1 );
                    QImage im = inundationItems[i].brush_img.textureImage();
                    im = im.scaled ( QSize( (int)TSYS::realRound( xMax - xMin )+1, (int)TSYS::realRound( yMax - yMin )+1 ), Qt::IgnoreAspectRatio, Qt::SmoothTransformation  );
                    double im_x, im_y;
                    QColor color;
                    double alpha, color_r, color_g, color_b;
                    QRgb rgb;
                    QPointF drw_pnt,drw_pnt1;
                    QPen im_pen;
                    im_y = yMin;
                    //-- Calculating the resulting color of the image and drawing the scaled and rotated points of it into the inundation path --
                    do
                    {
                        im_x = xMin;
                        do
                        {
                            if( in_path.contains( QPointF(im_x, im_y) ) )
                            {
                                rgb = im.pixel ( (int)TSYS::realRound( im_x - xMin, 2, true ),  (int)TSYS::realRound( im_y - yMin, 2, true ) );
                                alpha = (double)((rgb>>24)&0xff)/255;
                                color_r = alpha*((rgb>>16)&0xff) + (1-alpha)*inundationItems[i].brush.color().red();
                                color_g = alpha*((rgb>>8)&0xff) + (1-alpha)*inundationItems[i].brush.color().green();
                                color_b = alpha*(rgb&0xff) + (1-alpha)*inundationItems[i].brush.color().blue();
                                im_pen.setColor ( QColor((int)(color_r), (int)(color_g), (int)(color_b) ) );
                                pnt.setPen( im_pen );
                                drw_pnt = unScaleRotate( QPointF( im_x, im_y ), view, flag_scale, false );
                                drw_pnt1 = QPointF( scaleRotate( drw_pnt, view, true, true ).x(),
                                                    scaleRotate( drw_pnt, view, true, true ).y() ) ;

                                pnt.drawPoint( QPointF( (int)TSYS::realRound( drw_pnt1.x(), 2, true),
                                                        (int)TSYS::realRound( drw_pnt1.y(), 2, true) ) );
                            }
                            im_x += 0.7;// 0.7 - to avoid the free points, wich happen during the rotation of the image
                        }
                        while( im_x > xMin && im_x < xMax );
                        im_y += 0.7;
                    }
                    while( im_y > yMin && im_y < yMax );
                }
               else
               {
                   pnt.setBrush( inundationItems[i].brush );
                   pnt.setPen( Qt::NoPen );
                   pnt.drawPath( inundationItems[i].path );
               }
            }
            //- Drawing all el_figures -
            for( int k=0; k < shapeItems.size(); k++ )
            {
                pnt.setBrush( shapeItems[k].brush );
                pnt.setPen( shapeItems[k].pen );
                pnt.drawPath( shapeItems[k].path );
                if( shapeItems[k].penSimple.widthF() < 1 ) shapeItems[k].penSimple.setWidthF(1);
                else shapeItems[k].penSimple.setWidthF( TSYS::realRound( shapeItems[k].penSimple.widthF() ) );
                pnt.setPen( shapeItems[k].penSimple );
                pnt.setBrush( Qt::NoBrush );
                pnt.drawPath( shapeItems[k].pathSimple );
            }
            //- Drawing all rects for choosen el_figures -
            pnt.setRenderHint( QPainter::Antialiasing, false );
            if( devW && devW->edit() )
            {
                for( int k = 0; k < rectItems.size(); k++ )
                {
                    pnt.setBrush( rectItems[k].brush );
                    pnt.setPen( rectItems[k].pen );
                    pnt.drawPath( rectItems[k].path );
                }
                pnt.setBrush( Qt::NoBrush );
                pnt.setPen( QPen( Qt::gray , 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) );
                pnt.setBrush( QBrush( QColor(127,127,127,127), Qt::SolidPattern ) );
                ellipse_draw_startPath.setFillRule ( Qt::WindingFill );
                pnt.drawPath( ellipse_draw_startPath );
                ellipse_draw_endPath.setFillRule ( Qt::WindingFill );
                pnt.drawPath( ellipse_draw_endPath );
                ellipse_draw_startPath = newPath;
                ellipse_draw_endPath = newPath;
                if( dashedRect.isValid() )
                {
                    pnt.setBrush( Qt::NoBrush );
                    pnt.setPen( Qt::white );
                    pnt.drawRect( dashedRect );
                    pnt.setBrush( Qt::NoBrush );
                    pnt.setPen( Qt::black );
                    pnt.setPen( Qt::DashLine );
                    pnt.drawRect( dashedRect );
                }
            }
            return true;
        }
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if( runW && elFD->active && runW->permCntr() )
            {
                string sev;
                for( int i=0; i < inundationItems.size(); i++ )
                    if( inundationItems[i].path.contains(ev->pos()) )
                    	sev="ws_Fig"+TSYS::int2str(i);
                if( !sev.empty() )
		{
		    if( !runW->hasFocus() )	runW->setFocus( Qt::MouseFocusReason );
		    if( ev->buttons() & Qt::LeftButton )	sev += "Left";
		    if( ev->buttons() & Qt::RightButton ) 	sev += "Right";
		    if( ev->buttons() & Qt::MidButton )      	sev += "Midle";
		    view->attrSet( "event", sev );
		    return true;
		}
            }	    
            else if( devW )
            {
                if( flag_down==0 && flag_up==0 && flag_left==0 && flag_right==0 )
                {
                    if( ev->button()==Qt::LeftButton && !status )
                    {
                        //- initialization for holds - 
                        current_ss = current_se = current_es = current_ee = -1;
                        flag_holds = true;
                        // - getting the index of the figure -
                        index = itemAt( ev->pos(), shapeItems, view );
                        if( index == -1 )
                        {
                            ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(false);
                            ((VisDevelop *)view->mainWin())->actLevRise->setEnabled(false);
                            ((VisDevelop *)view->mainWin())->actLevLower->setEnabled(false);
                        }
                        itemInMotion = 0;
                        index_temp = index;
                        index_del = index;
                        previousPosition_all = ev->pos();
                        previousPosition = ev->pos();
                        count_holds = 0;
                        // - getting figures or rect number for moveItemTo -
                        if( index != -1 )
                        {
                            ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(true);
                            ((VisDevelop *)view->mainWin())->actVisItPaste->setEnabled(false);
                            ((VisDevelop *)view->mainWin())->actLevRise->setEnabled(true);
                            ((VisDevelop *)view->mainWin())->actLevLower->setEnabled(true);
                            if( !flag_copy )
                            {
                                itemInMotion = &shapeItems[index];
                                if( (flag_ctrl && !status_hold) || (flag_ctrl && flag_A) )
                                {
                                    bool fn = false;
                                    for( int i=0; i < count_Shapes; i++ )
                                        if( index_array[i] == index) fn = true;
                                    if( !fn )
                                    {
                                        index_array[count_Shapes] = index;
                                        count_Shapes += 1;
                                        itemInMotion = &shapeItems[index];
                                        flag_ctrl_move = 1;
                                        offset = QPointF(0,0);
                                        moveItemTo( previousPosition, shapeItems, pnts, view );
                                    }
                                    flag_check_pnt_inund = true;
                                }
                            }
                            else
                            {
                                flag_check_pnt_inund = true;
                                if( index_array.size() && index_array[0] != -1 )
                                    itemInMotion = &shapeItems[index_array[0]];
                            }
                            if( status_hold && !flag_A )
                            {
                                holds( shapeItems, pnts );
                                if( count_holds )
                                {
                                    if( rect_num != -1 )
                                    {
                                        int rect_num_temp = rect_num;
                                        rect_num = realRectNum( rect_num, shapeItems );
                                        itemInMotion = &shapeItems[index];
                                        if( (rect_num==2 || rect_num==3) && shapeItems[index].type==3 )	flag_rect=false;
                                        if( rect_num==0 || rect_num==1 ) rectNum0_1( shapeItems, rect_num_temp, pnts, view );
                                        if( (rect_num==3 ||rect_num==4) && shapeItems[index].type==2 )
                                        {
                                            Prev_pos_1 = scaleRotate( (*pnts)[shapeItems[index].n1], view, flag_scale, flag_rotate );
                                            Prev_pos_2 = scaleRotate( (*pnts)[shapeItems[index].n2], view, flag_scale, flag_rotate );
                                            rectNum3_4( shapeItems );
                                        }
                                    }
                                    if( rect_num == -1 )
                                    {
                                        count_moveItemTo = 0;
                                        num_vector.clear();
                                        offset = QPointF(0,0);
                                        for( int i=0; i < count_holds; i++ )
                                        {
                                            count_moveItemTo += 1;
                                            flag_ctrl_move = 0;
                                            flag_ctrl = 1;
                                            itemInMotion = &shapeItems[index_array[i]];
                                            index = index_array[i];
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            view->repaint();
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if( flag_A )
                            {
                                flag_ctrl = flag_A = flag_copy = false;
                                flag_check_pnt_inund = false;
                                index_array.clear();
                                itemInMotion = 0;
				count_Shapes = 0;
                            }
                            if( rectItems.size() ) rectItems.clear();
                            view->repaint();
                        }
                    }
                    if( ev->button()==Qt::RightButton && !status )
                    {
                        index = itemAt( ev->pos(), shapeItems, view );
                        count_holds = 0;
                        if( status_hold && !flag_A &&  index != -1 )
                        {
                            holds( shapeItems, pnts );
                            if( count_holds )
                            {
                                if( rect_num != -1 )
                                {
                                    int rect_num_temp = rect_num;
                                    rect_num = realRectNum( rect_num, shapeItems );
                                    itemInMotion = &shapeItems[index];
                                    if( (rect_num==2 || rect_num==3) && shapeItems[index].type==3 )	flag_rect=false;
                                    if( rect_num==0 || rect_num==1 ) rectNum0_1( shapeItems, rect_num_temp, pnts, view );
                                    if( (rect_num==3 ||rect_num==4) && shapeItems[index].type==2 )
                                    {
                                        Prev_pos_1 = scaleRotate( (*pnts)[shapeItems[index].n1], view, flag_scale, flag_rotate );
                                        Prev_pos_2 = scaleRotate( (*pnts)[shapeItems[index].n2], view, flag_scale, flag_rotate );
                                        rectNum3_4( shapeItems );
                                    }
                                }
                            }
                        }
                        rect_dyn = rect_num;
                    }
                    // - getting start point for drawing-
                    if( (ev->button() == Qt::LeftButton) && status )
                    {
                        flag_release = false;
                        StartLine = ev->pos();
                    }
                    // - repainting figures by mouse click -
                    if( (ev->button() == Qt::LeftButton) && (itemInMotion || rect_num != -1) && !status && flag_ctrl != 1 && count_holds == 0 )
                    {
                        count_Shapes = 1;
                        count_moveItemTo = 1;
                        offset = QPointF(0,0);
                        moveItemTo( ev->pos(), shapeItems, pnts, view );
                        view->repaint();
                    }
                }
        	return true;
            }
	    break;
        }
        case QEvent::MouseButtonDblClick:
        {
            bool flag_arc_inund = false,
		 flag_break_move, fl_brk;
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);

            if( runW && elFD->active && runW->permCntr() )
	    {
		string sev;
		for( int i=0; i < inundationItems.size(); i++ )
		    if( inundationItems[i].path.contains(ev->pos()) )
			sev="ws_Fig"+TSYS::int2str(i);
		if( !sev.empty() )
		{
		    view->attrSet( "event", sev+"DblClick" );
		    return true;
		}
            }
            else if( devW )
            {
                if( !flag_down && !flag_up && !flag_left && !flag_right )
                {
                    index = itemAt( ev->pos(), shapeItems, view );
                   
                    // - getting fill by double click -
                    if( ev->button() == Qt::LeftButton && shapeItems.size() && index == -1 /*&& status_hold*/ )
                    {
                        flag_angle_temp = true;
                        QBrush fill_brush( QColor(0,0,0,0), Qt::SolidPattern ), fill_img_brush;
                        fill_brush.setColor( elFD->fillClr );
                        fill_img_brush = elFD->fillImg;
                        if( !inundation1_2( ev->pos(), shapeItems, inundationItems, pnts, view, -1) )
                        {
                            if( buildMatrix(shapeItems) != 2 )
                            {
                                fl_brk = false;
                                inundation( ev->pos(), shapeItems, pnts, vect, buildMatrix(shapeItems), view );
                                if( inundationPath != newPath )
                                {
                                    for( int i=0; i < inundationItems.size(); i++ )
                                        if( inundationItems[i].path == inundationPath )
                                        {
                                            inundationItems.remove(i);
                                            fl_brk = true;
                                            break;
                                        }
                                    if( !fl_brk && status_hold )
                                        inundationItems.push_back(inundationItem(inundationPath,fill_brush,fill_img_brush, inundation_vector, inundation_vector));
                                }
                            }
                            inundation_vector.clear();
                            vect.clear();
                            map_matrix.clear();
                        }
                        flag_angle_temp = false;
                    }
                    view->repaint();
                }
                shapeSave(view);
		
		return true;
            }
    	    break;    
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if( devW )
            {
                if( !flag_down && !flag_up && !flag_left && !flag_right )
                {
                    if( flag_move && ev->button() == Qt::LeftButton )
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
                            flag_A = true;
                            flag_copy = true;
                            index_array_copy.clear();
                            flag_ctrl_move = 1;
                            count_Shapes = index_array.size();
                            moveAll( QPointF(0,0), shapeItems, pnts, inundationItems, view );
                        }
                        ((VisDevelop *)view->mainWin())->actLevRise->setEnabled(true);
                        ((VisDevelop *)view->mainWin())->actLevLower->setEnabled(true);
                        ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(true);
                        view->repaint();
                    }
                    if( ev->button() == Qt::LeftButton && itemInMotion && (status==false) )
                    {
                        flag_inund_break = false;
                        itemInMotion = &shapeItems[index];
                        index_temp = index;
                        // - connecting the figures -
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
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if( shapeItems[current_ss].n1==shapeItems[i].n2 && i!=index )
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_ss].n1==itemInMotion->n2) )
                                    {
                                        dropPoint( itemInMotion->n1, index, shapeItems, pnts );
                                        itemInMotion->n1 = shapeItems[current_ss].n1;
                                        count_moveItemTo = 1;
                                        flag_ctrl_move = 0;
                                        flag_ctrl = 1;
                                        count_Shapes = 1;
                                        offset = QPointF(0,0);
                                        moveItemTo( ev->pos(), shapeItems, pnts, view );
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
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if( shapeItems[current_se].n1==shapeItems[i].n2 && i!=index )
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                {
                                    if( !(itemInMotion->type==3 && shapeItems[current_se].n1==itemInMotion->n1) )
                                    {
                                        dropPoint( itemInMotion->n2, index, shapeItems, pnts );
                                        itemInMotion->n2 = shapeItems[current_se].n1;
                                        count_moveItemTo = 1;
                                        flag_ctrl_move = 0;
                                        flag_ctrl = 1;
                                        count_Shapes = 1;
                                        offset = QPointF(0,0);
                                        moveItemTo( ev->pos(), shapeItems, pnts, view );
                                    }
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
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = 1;
                                            count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if( shapeItems[current_es].n2==shapeItems[i].n2 && i!=index )
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = 1;
                                            count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_es].n2==itemInMotion->n2) )
                                    {
                                        dropPoint( itemInMotion->n1, index, shapeItems, pnts );
                                        itemInMotion->n1 = shapeItems[current_es].n2;
                                        count_moveItemTo = 1;
                                        flag_ctrl_move = 0;
                                        flag_ctrl = 1;
                                        count_Shapes = 1;
                                        offset = QPointF(0,0);
                                        moveItemTo( ev->pos(), shapeItems, pnts, view );
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
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = 1;
                                            count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if( shapeItems[current_ee].n2==shapeItems[i].n2 && i!=index )
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo = 1;
                                            count_Shapes = 1;
                                            offset = QPointF(0,0);
                                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                                            index = index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_ee].n2==itemInMotion->n1) )
                                    {
                                        dropPoint( itemInMotion->n2, index, shapeItems, pnts );
                                        itemInMotion->n2 = shapeItems[current_ee].n2;
                                        count_moveItemTo = 1;
                                        flag_ctrl_move = 0;
                                        flag_ctrl = 1;
                                        count_Shapes = 1;
                                        offset = QPointF(0,0);
                                        moveItemTo( ev->pos(), shapeItems, pnts, view );
                                    }
                            }
                        }
                        if( itemInMotion->type != 2 )
                        {
                            // - if simple figure and status_hold -
                            if( !flag_ctrl && status_hold )
                            {
                                count_Shapes = 1;
                                count_moveItemTo = 1;
                                offset = QPointF(0,0);
                                moveItemTo( ev->pos(), shapeItems, pnts, view );
                            }
                            view->repaint(); 
                        }
                        ellipse_draw_startPath = newPath;
                        ellipse_draw_endPath = newPath;
                        // - calling moveItemTo for figures, connected with the arc, if there was moving 3 or 4 rexts of the arc -
                        if( count_holds && (flag_arc_rect_3_4 || (flag_rect && shapeItems[index_array[0]].type==2)))
                        {
                            count_moveItemTo = 0;
                            flag_ctrl_move = false;
                            flag_ctrl = true;
                            flag_hold_arc = true;
                            offset = QPointF(0,0);
                            moveAll( ev->pos(), shapeItems, pnts, inundationItems, view );
                        }
                        shapeSave( view );
                        itemInMotion = 0;
                        flag_check_pnt_inund = false;
                        if( status_hold && !flag_A )
                        {
                            count_moveItemTo = 0;
                            flag_hold_move = false;
                            flag_ctrl = false;
                            count_Shapes = 0;
                            count_holds = 0;
                            if ( index_array.size() )
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
                            flag_arc_release = false;
                            count_rects = 0;
                            flag_hold_arc = false;
                            rect_num_arc = -1;
                            flag_arc_rect_3_4 = false;
                        }
                        itemInMotion = 0;                        
		     	flag_m=false;
                    }
                    if( ev->button()==Qt::RightButton && !status )
                    {
                        if( status_hold && !flag_A )
                        {
                            count_moveItemTo = 0;
                            flag_hold_move = false;
                            flag_ctrl = false;
                            count_Shapes = 0;
                            count_holds = 0;
                            if ( index_array.size() )
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
                            flag_arc_release = false;
                            count_rects = 0;
                            flag_hold_arc = false;
                            rect_num_arc = -1;
                            flag_arc_rect_3_4 = false;
                        }                        
                    }
                    double ang;
                    // - getting the end point of the figure and building it -
                    if( ev->button() == Qt::LeftButton && status )
                    { 
                        float Wdth = 20.0;
                        double scale;
                        QLineF line1, line2;
                        EndLine = ev->pos();
                        if( EndLine == StartLine || flag_release )
                        {
                            flag_release = true;
                            break;
                        }
                        QPainterPath circlePath, bigPath;
                        Qt::PenStyle style;
                        string ln_st;
                        if( view->xScale(true) < view->yScale(true) ) scale = view->xScale(true);
                        else scale = view->yScale(true);
                        //-- if line --
                        if( shapeType == 1 )
                        {
                            ln_st = elFD->lineStyle;
                            if( ln_st == "solid" ) style = Qt::SolidLine;
                            else if( ln_st == "dashed" ) style = Qt::DashLine;
                            else if( ln_st == "dotted" ) style = Qt::DotLine;
                            //--- if orto ---
                            if( flag_key )
                            {
                                if( (EndLine.y()-StartLine.y()) > 20 || (StartLine.y()-EndLine.y()) > 20 )
                                {
                                    if( StartLine.y() <= EndLine.y() )	ang = 270;
                                    else ang = 90;
                                    EndLine = QPointF( StartLine.x(), EndLine.y() );
                                }
                                else
                                {
                                    if( StartLine.x() <= EndLine.x() )	ang=0;
                                    else ang = 180;
                                    EndLine = QPointF( EndLine.x(), StartLine.y() );
                                }
                                if( elFD->bordWdth > 0 )
                                {
                                    circlePath = painterPath( elFD->lineWdth*scale, elFD->bordWdth*scale, 1, ang, StartLine,EndLine );
                                    shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr, Qt::SolidPattern),
                                            QPen( elFD->bordClr, elFD->bordWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( elFD->lineClr, elFD->lineWdth*scale, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            elFD->lineWdth*scale, elFD->bordWdth*scale, 1, angle_temp) );
                                    //shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                                }
                                else
                                {
                                    circlePath = painterPathSimple( 1, ang, StartLine, EndLine );
                                    QPainterPath bigPath = painterPath( vmax(1,elFD->lineWdth*scale)+1, elFD->bordWdth, 1, ang, StartLine, EndLine);
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr,Qt::NoBrush),
                                            QPen( elFD->bordClr, elFD->bordWdth, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( elFD->lineClr, elFD->lineWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                            elFD->lineWdth*scale, elFD->bordWdth, 1, angle_temp) );
                                    shapeItems[shapeItems.size()-1].brush.setColor( elFD->lineClr );
                                }
                                StartLine = unScaleRotate( StartLine, view, flag_scale, flag_rotate );
                                EndLine = unScaleRotate( EndLine, view, flag_scale, flag_rotate );
                                shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts, 1 );
                                shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts, 1 );
                                shapeSave( view );
                                view->repaint();
                            }
                            //--- if !orto ---
                            else
                            {
                                line2 = QLineF( StartLine, QPointF(StartLine.x()+10,StartLine.y()) );
                                line1 = QLineF( StartLine, EndLine );
                                if( StartLine.y() <= EndLine.y() )	ang = 360-angle( line1, line2 );
                                else                                   	ang = angle( line1, line2 );
                               
                                if( elFD->bordWdth > 0 )
                                {
                                    circlePath = painterPath( elFD->lineWdth*scale, elFD->bordWdth*scale,1, ang, StartLine, EndLine );
                                    shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr, Qt::SolidPattern),
                                            QPen( elFD->bordClr, elFD->bordWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( elFD->lineClr, elFD->lineWdth*scale, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            elFD->lineWdth*scale, elFD->bordWdth*scale, 1, angle_temp) );
                                }
                                else
                                {
                                    circlePath = painterPathSimple( 1, ang, StartLine, EndLine );
                                    QPainterPath bigPath = painterPath( vmax(1,elFD->lineWdth*scale)+1, elFD->bordWdth, 1, ang, StartLine, EndLine );
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr,Qt::NoBrush),
                                            QPen( elFD->bordClr, elFD->bordWdth, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( elFD->lineClr, elFD->lineWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                            elFD->lineWdth*scale, elFD->bordWdth, 1, angle_temp) );
                                    shapeItems[shapeItems.size()-1].brush.setColor( elFD->lineClr );
                                }
                                StartLine = unScaleRotate( StartLine, view, flag_scale, flag_rotate );
                                EndLine = unScaleRotate( EndLine, view, flag_scale, flag_rotate );
                                shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts, 1 );
                                shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts, 1 );
                                shapeSave( view );
                                view->repaint();
                            }
                        }
                        //-- if bezier --
                        if( shapeType==3 )
                        {
                            
                            QPointF CtrlPos_1, CtrlPos_2, EndLine_temp;
                            ln_st=elFD->lineStyle;
                            if( ln_st == "solid" ) style = Qt::SolidLine;
                            else if( ln_st == "dashed" ) style = Qt::DashLine;
                            else if( ln_st == "dotted" ) style = Qt::DotLine;
                            CtrlPos_1 = QPointF( length(EndLine,StartLine)/3, 0 );
                            CtrlPos_2 = QPointF( 2*length(EndLine,StartLine)/3, 0 );
                            line2 = QLineF( StartLine, QPointF(StartLine.x()+10,StartLine.y()) );
                            line1 = QLineF( StartLine, EndLine );
                            if( StartLine.y() <= EndLine.y() )	ang = 360-angle(line1,line2);
                            else                                ang = angle(line1,line2);
                            CtrlPos_1 = QPointF( StartLine.x()+rotate(CtrlPos_1,ang).x(), StartLine.y()-rotate(CtrlPos_1,ang).y() );
                            CtrlPos_2 = QPointF( StartLine.x()+rotate(CtrlPos_2,ang).x(), StartLine.y()-rotate(CtrlPos_2,ang).y() );
                            if( elFD->bordWdth > 0 )
                            {
                                circlePath = painterPath( elFD->lineWdth*scale, elFD->bordWdth*scale, 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr,Qt::SolidPattern),
                                        QPen( elFD->bordClr, elFD->bordWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( elFD->lineClr, elFD->lineWdth*scale, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        elFD->lineWdth*scale,elFD->bordWdth*scale, 3, angle_temp) );
                            }
                            else
                            {
                                bigPath = painterPath( vmax(1,elFD->lineWdth*scale)+1, elFD->bordWdth,3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                circlePath = painterPathSimple( 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(elFD->lineClr,Qt::NoBrush),
                                        QPen( elFD->bordClr, elFD->bordWdth, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( elFD->lineClr, elFD->lineWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                        elFD->lineWdth*scale, elFD->bordWdth, 3, angle_temp) );
                                shapeItems[shapeItems.size()-1].brush.setColor( elFD->lineClr );
                            }
                            StartLine = unScaleRotate( StartLine, view, flag_scale, flag_rotate );
                            EndLine = unScaleRotate( EndLine, view, flag_scale, flag_rotate );
                            CtrlPos_1 = unScaleRotate( CtrlPos_1, view, flag_scale, flag_rotate );
                            CtrlPos_2 = unScaleRotate( CtrlPos_2, view, flag_scale, flag_rotate );
                            
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( CtrlPos_1, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( CtrlPos_2, shapeItems, pnts, 1 );
                            shapeSave( view );
                            view->repaint();
                        }
                        //-- if arc --
                        if( shapeType==2 )
                        {
                            QPointF CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4, Temp, StartLine_small, EndLine_small, pnt;
                            double a, a_small, b, b_small;
                            double t;
                            ln_st=elFD->lineStyle;;
                            if( ln_st == "solid" ) style = Qt::SolidLine;
                            else if( ln_st == "dashed" ) style = Qt::DashLine;
                            else if( ln_st == "dotted" ) style = Qt::DotLine;
                            CtrlPos_1 = QPointF( StartLine.x()+(EndLine.x()-StartLine.x())/2,
                                                 StartLine.y()+(EndLine.y()-StartLine.y())/2 );
                            a = length(EndLine,StartLine)/2;
                            b = a+50;
                            line2 = QLineF( CtrlPos_1,QPointF(CtrlPos_1.x()+10, CtrlPos_1.y()) );
                            line1 = QLineF( CtrlPos_1, StartLine );
                            if( StartLine.y() <= EndLine.y() )	ang = angle(line1,line2);
                            else 				ang = 360-angle(line1,line2);
			    CtrlPos_2 = QPointF( CtrlPos_1.x()+rotate(arc(0.25,a,b),ang).x(),
                                                 CtrlPos_1.y()-rotate(arc(0.25,a,b),ang).y() );
                            CtrlPos_3 = StartLine;
                            CtrlPos_4 = QPointF(0,0.5);

                            if( elFD->bordWdth > 0 )
                            {
                                circlePath = painterPath( elFD->lineWdth*scale, elFD->bordWdth*scale,2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2,  CtrlPos_3, CtrlPos_4 );
                                shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1, -1,CtrlPos_4,QBrush(elFD->lineClr,Qt::SolidPattern),
                                        QPen( elFD->bordClr, elFD->bordWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( elFD->lineClr, elFD->lineWdth*scale, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        elFD->lineWdth*scale, elFD->bordWdth*scale, 2, ang) );
                            }
                            else
                            {
                                QPainterPath bigPath = painterPath( vmax(1,elFD->lineWdth*scale)+1, elFD->bordWdth, 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                circlePath = painterPathSimple( 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1, -1,CtrlPos_4, QBrush(elFD->lineClr,Qt::NoBrush),
                                        QPen( elFD->bordClr, elFD->bordWdth, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( elFD->lineClr, elFD->lineWdth*scale, style, Qt::FlatCap, Qt::RoundJoin),
                                        elFD->lineWdth*scale, elFD->bordWdth, 2, ang) );
                                shapeItems[shapeItems.size()-1].brush.setColor( elFD->lineClr );
                            }
                            StartLine = unScaleRotate( StartLine, view, flag_scale, flag_rotate );
                            EndLine = unScaleRotate( EndLine, view, flag_scale, flag_rotate );
                            CtrlPos_1 = unScaleRotate( CtrlPos_1, view, flag_scale, flag_rotate );
                            CtrlPos_2 = unScaleRotate( CtrlPos_2, view, flag_scale, flag_rotate );
                            CtrlPos_3 = unScaleRotate( CtrlPos_3, view, flag_scale, flag_rotate );
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( CtrlPos_1, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( CtrlPos_2, shapeItems, pnts, 1 );
                            shapeItems[shapeItems.size()-1].n5 = appendPoint( CtrlPos_3, shapeItems, pnts, 1 );
                            shapeSave( view );
                            view->repaint();
                        }
                        flag_key = 0;
                    }
                } 
            }
            return true;
        }
        case QEvent::MouseMove:
        {
            int fl;
            int index_arc;// - index of the arc, wich take part in moving -
            int temp;
            bool flag_break_move, 
		 flag_arc_inund = false;
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if( devW )
	    {
                Mouse_pos = ev->pos();

                if( ev->buttons()&Qt::LeftButton && itemInMotion && !status )
                {
                    flag_m = true;
                    if( count_holds )
                    {
                        count_Shapes = count_holds+1; 
                        flag_ctrl = true;
                        flag_hold_move = true;
                    }
                    //- calling moveItemTo for all connected figures -
                    if( flag_ctrl && count_Shapes && ((rect_num==-1 && rect_num_arc==-1) || flag_rect || flag_arc_rect_3_4) )
                    {
                        if( flag_check_pnt_inund )
                        {
                            checkPoint_checkInundation( shapeItems, pnts, inundationItems );
                            flag_check_pnt_inund = false;
                        }
                        offset = ev->pos()-previousPosition_all;
                        count_moveItemTo = 0;
                        if( flag_rect || flag_arc_rect_3_4 )		count_Shapes = count_rects;
                        flag_hold_arc = false;
                        if( shapeItems[index_array[0]].type == 2 ) 	flag_hold_arc = true;
                        moveAll( ev->pos(), shapeItems, pnts, inundationItems, view );
                        view->repaint();
                    }
                    // - moving the rect, which belongs only for one figure -
                    else
                    {
                        if( count_holds )
                        {
                            offset = ev->pos()-previousPosition_all;
                            flag_ctrl = true;
                            flag_rect = false;
                            count_moveItemTo = 1;
                            flag_ctrl_move = false;
                            count_Shapes = 1;
                            itemInMotion = &shapeItems[index];
                            if( rect_num==2 && itemInMotion->type==2 ) offset = QPointF(0,0);
                            if( itemInMotion->type==2 && rect_num!=2 ) rect_num = rect_num_arc;
                            num_vector.clear();
                            moveItemTo( ev->pos(), shapeItems, pnts, view );
                            if( inundationItems.size() )
                                for( int i=0; i < inundationItems.size(); i++ )
                                    for( int j=0; j < inundationItems[i].number_shape.size(); j++ )
				    {
					flag_break_move = false;
                                        if( inundationItems[i].number_shape[j] == index )
					{
					    inundationPath = createInundationPath( inundationItems[i].number_shape, shapeItems, pnts, view );
					    inundationItems[i].path = inundationPath;
					    flag_break_move = true;
					    break;
					}
					if( flag_break_move ) break;
				    }
                            flag_hold_rect = true;
                            view->repaint();
                        }
                        else if( !flag_ctrl )
		    	{
			    for( int i=0; i < shapeItems.size(); i++ )
			    {
    				if( i != index )
				{
				    if( shapeItems[index].type == 2 )
				    {
					if( shapeItems[index].n5==shapeItems[i].n1 || shapeItems[index].n5==shapeItems[i].n2 || shapeItems[index].n5==shapeItems[i].n3 || 
						shapeItems[index].n5==shapeItems[i].n4 || shapeItems[index].n5==shapeItems[i].n5 )
                                        {
                                            if( shapeItems[index].n5 > 0 )
                                                shapeItems[index].n5 = appendPoint( QPointF( (*pnts)[shapeItems[index].n5].x(), (*pnts)[shapeItems[index].n5].y() ), shapeItems, pnts, 0 );
                                            else if( shapeItems[index].n5 <= -10 )
                                                shapeItems[index].n5 = appendPoint( QPointF( (*pnts)[shapeItems[index].n5].x(), (*pnts)[shapeItems[index].n5].y() ), shapeItems, pnts, 1 );
                                        }
				    }
				    if( shapeItems[index].type==2 || shapeItems[index].type==3 )
				    {
					if( shapeItems[index].n4==shapeItems[i].n1 || shapeItems[index].n4==shapeItems[i].n2 || shapeItems[index].n4==shapeItems[i].n3 || 
						shapeItems[index].n4==shapeItems[i].n4 || shapeItems[index].n4==shapeItems[i].n5 )
                                        {
                                            if( shapeItems[index].n4 > 0 )
                                                shapeItems[index].n4 = appendPoint( QPointF( (*pnts)[shapeItems[index].n4].x(), (*pnts)[shapeItems[index].n4].y() ), shapeItems, pnts, 0 );
                                            else if( shapeItems[index].n4 <= -10 )
                                                shapeItems[index].n4 = appendPoint( QPointF( (*pnts)[shapeItems[index].n4].x(), (*pnts)[shapeItems[index].n4].y() ), shapeItems, pnts, 1 );
                                        }
					if( shapeItems[index].n3==shapeItems[i].n1 || shapeItems[index].n3==shapeItems[i].n2 || shapeItems[index].n3==shapeItems[i].n3 || 
						shapeItems[index].n3==shapeItems[i].n4 || shapeItems[index].n3==shapeItems[i].n5 )
                                        {
                                            if( shapeItems[index].n3 > 0 )
                                                shapeItems[index].n3 = appendPoint( QPointF( (*pnts)[shapeItems[index].n3].x(), (*pnts)[shapeItems[index].n3].y() ), shapeItems, pnts, 0 );
                                            else if( shapeItems[index].n3 <= -10 )
                                                shapeItems[index].n3 = appendPoint( QPointF( (*pnts)[shapeItems[index].n3].x(), (*pnts)[shapeItems[index].n3].y() ), shapeItems, pnts, 1 );
                                        }
				    }
				    if( shapeItems[index].n1==shapeItems[i].n1 || shapeItems[index].n1==shapeItems[i].n2 || shapeItems[index].n1==shapeItems[i].n3 || 
					    shapeItems[index].n1==shapeItems[i].n4 || shapeItems[index].n1==shapeItems[i].n5 )
                                    {
                                        if( shapeItems[index].n1 > 0 )
                                            shapeItems[index].n1 = appendPoint( QPointF( (*pnts)[shapeItems[index].n1].x(), (*pnts)[shapeItems[index].n1].y() ), shapeItems, pnts, 0 );
                                        else if( shapeItems[index].n1 <= -10 )
                                            shapeItems[index].n1 = appendPoint( QPointF( (*pnts)[shapeItems[index].n1].x(), (*pnts)[shapeItems[index].n1].y() ), shapeItems, pnts, 1 );
                                    }
				    if( shapeItems[index].n2==shapeItems[i].n1 || shapeItems[index].n2==shapeItems[i].n2 || shapeItems[index].n2==shapeItems[i].n3 ||
                                            shapeItems[index].n2==shapeItems[i].n4 || shapeItems[index].n2==shapeItems[i].n5 )
                                    {
                                        if( shapeItems[index].n2 > 0 )
                                            shapeItems[index].n2 = appendPoint( QPointF( (*pnts)[shapeItems[index].n2].x(), (*pnts)[shapeItems[index].n2].y() ), shapeItems, pnts, 0 );
                                        else if( shapeItems[index].n2 <=-10 )
                                            shapeItems[index].n2 = appendPoint( QPointF( (*pnts)[shapeItems[index].n2].x(), (*pnts)[shapeItems[index].n2].y() ), shapeItems, pnts, 1 );
                                    }
			  	}
			    }
			    count_Shapes = 1;
			    count_moveItemTo = 1;
			    offset = ev->pos()-previousPosition_all;
			    itemInMotion = &shapeItems[index];
			    moveItemTo( ev->pos(), shapeItems, pnts, view );
			    //- deleting fill's -
			    if( inundationItems.size() )
			      	for( int i=0; i < inundationItems.size(); i++ )
			  	{
		      		    if( shapeItems[index].type==2 && inundationItems[i].number_shape.size()==1 && inundationItems[i].number_shape[0]==index )
				    {
					if( rect_num==0 || rect_num==1 )
					{
					    inundationItems.remove(i);
				       	    shapeSave(view);
				   	    flag_arc_inund = true;
					}
				     	else
				 	{
			     		    inundationItems[i].path = createInundationPath( inundationItems[i].number_shape, shapeItems, pnts, view );
					    flag_arc_inund = true;
					}
				    }
				}
			    if( inundationItems.size() && !flag_inund_break && !flag_arc_inund )
			    {
		       		for( int i=0; i < inundationItems.size(); i++ )
		   		    for( int j=0; j < inundationItems[i].number_shape.size(); j++ )
                                    {
                                        flag_break_move = false;
                                        if( inundationItems[i].number_shape[j] == index )
                                        {
                                            inundationItems.remove(i);
                                            shapeSave(view);
                                            flag_break_move = true;
                                            break;
                                        }
                                        if( flag_break_move )	break;
                                    }
                                    flag_inund_break = true;
			    }
			    flag_arc_inund = false;
			    view->repaint();
			}
                        if( rect_num != -1 )	temp = realRectNum( rect_num, shapeItems );
                        //- if the figure or it's rect is not connected to other one -
                        if( status_hold && (rect_num==-1|| ((temp==0 || temp==1) && !flag_rect)) )
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
                                if( i != index )
                                {
                                    ellipse_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).x()-8, 
                                                                  scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                    if( ellipse_startPath.contains( scaleRotate((*pnts)[shapeItems[index].n1], view, flag_scale, flag_rotate) ) )
                                    { 
                                        if( temp==0 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).x()-8,
                                                                               scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                            current_ss = i;
                                        }
                                    }
                                    if( ellipse_startPath.contains(scaleRotate((*pnts)[shapeItems[index].n2], view, flag_scale, flag_rotate)) )
                                    {
                                        if( temp==1 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).x()-8,
                                                                               scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                            current_se = i;
                                        }
                                    }
                                    ellipse_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).x()-8, 
                                                                scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                    if( ellipse_endPath.contains( scaleRotate((*pnts)[shapeItems[index].n2], view, flag_scale, flag_rotate) ) )
                                    {
                                        if( temp==1 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).x()-8,
                                                                             scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                            current_ee = i;
                                        }
                                    }
                                    if( ellipse_endPath.contains( scaleRotate((*pnts)[shapeItems[index].n1], view, flag_scale, flag_rotate) ) )
                                    {
                                        if( temp==0 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).x()-8,
                                                                             scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).y()-8, 16, 16 );
                                            current_es = i;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if( ev->buttons()&Qt::LeftButton && itemInMotion == 0 && !status && index_temp == -1 )
                {
                    if( !flag_move )
                    {
                        stPointDashedRect = ev->pos();
                        dashedRect = QRect( stPointDashedRect, stPointDashedRect );
                        flag_move = true;
                    }
                    else dashedRect = QRect( stPointDashedRect, ev->pos() ).normalized();
                    view->repaint();
                }
                else //if( flag_cursor==0 )
                {
                    if( flag_down || flag_left || flag_right || flag_up ) break;
                    
                    if( flag_first_move && !flag_A )
                    {
                        shapeSave( view );
                        flag_inund_break = false;
                        offset = QPointF(0,0);
                        if( flag_ctrl && status_hold )
                        {
                            count_moveItemTo = 0;
                            flag_hold_move = false;
                            if( flag_rect )
                            {
                                offset = QPointF(0,0);
                                rect_array.clear();
                            }
                            if( flag_arc_rect_3_4 )
                            {
                                offset = QPointF(0,0);
                                arc_rect_array.clear();
                                fig_rect_array.clear();
                            }
                            count_Shapes = 0;
                            count_holds  = 0;
                            count_rects  = 0;
                            rect_num_arc = -1;
                            flag_ctrl    = false;
                            flag_rect    = false;
                            flag_arc_release  = false;
                            flag_arc_rect_3_4 = false;
                            flag_hold_rect    = false;
                            index_array.clear();
                        }
                        flag_first_move = false;  
                    }
                    if( !flag_m )
                    {
                        fl = itemAt( ev->pos(), shapeItems, view );
                        if( fl != -1 && rect_num == -1 && !status )		view->setCursor( Qt::SizeAllCursor );
                        else if( !status && rect_num != -1 )			view->setCursor( Qt::SizeHorCursor );
                        else if( status )
                        {
                            if( view->cursor().shape() != Qt::CrossCursor )	view->setCursor( Qt::CrossCursor );
                        }
                        else if( view->cursor().shape() != Qt::ArrowCursor )	view->unsetCursor();
                    }
                }
            }
            
            return true;
        }
        case QEvent::KeyPress:
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            bool flag_break_move;
            if( devW )
            {
                if( flag_m )  break;
                if( ev->key() == Qt::Key_Shift ) flag_key=true;
                if( ev->key() == Qt::Key_Control )
                {
                    if( flag_A ) index_array_copy_flag_A = index_array;
                    if( status_hold || flag_copy )	break;
                    flag_ctrl = true;
                    index_array.clear();
                    for( int i=0; i < shapeItems.size(); i++ )
                        index_array.push_back(-1);
                    count_Shapes = 0;
                }
                if( ev->key() == Qt::Key_Delete && (index_del != -1 || flag_A) )
                {
                    bool flag_arc_inund = false;
                    bool flag_break_move, flag_in_break, flag_ar_break;
                    QVector<int> drop_inunds;
                    if( flag_A || ( index_array_copy.size() && index_array_copy[0] != -1 ) || ( index_array.size() && index_array[0] != -1 ) )
                    {
                        flag_ar_break = false;
                        for( int i=0; i < index_array.size(); i++ )
                            if( index_array[i] == -1 )
                            {
                                flag_ar_break = true;
                                break;
                            }
                        if( (index_array.size() == shapeItems.size()) && !flag_ar_break )
                        {
                            (*pnts).clear();
                            inundationItems.clear();
                            shapeItems.clear();
                            rectItems.clear();
                            shapeSave(view);
                            flag_ctrl = false;
                            flag_A = false;
                            flag_copy = false;
                            index_array.clear();
                            itemInMotion = 0;
                            count_Shapes = 0;
                            view->repaint();
                        }
                        else
                        {
                            if( index_array_copy.size() )
                                if( index_array_copy[0] != -1 && status_hold ) index_array = index_array_copy;//!!!!
                            for( int i=0; i < inundationItems.size(); i++ )
                            {
                                for( int j = 0; j < inundationItems[i].number_shape.size(); j++ )
                                {
                                    flag_in_break = false;
                                    for( int p = 0; p < index_array.size(); p++ )
                                    {
                                        if( index_array[p] != -1 && inundationItems[i].number_shape[j] == index_array[p] )
                                        {
                                            drop_inunds.push_back( i );
                                            flag_in_break = true;
                                            break;
                                        }
                                    }
                                    if( flag_in_break ) break;
                                }
                            }
                            for( int i = 0; i < drop_inunds.size(); i++ )
                            {
                                inundationItems.remove( drop_inunds[i] );
                                for( int j = i+1; j < drop_inunds.size(); j++ )
                                    if( drop_inunds[j] > drop_inunds[i] )
                                        drop_inunds[j]--;
                            }
                            for( int p = 0; p < index_array.size(); p++ )
                            {
                                if( index_array[p] != -1 )
                                {
                                switch( shapeItems[index_array[p]].type )
                                {
                                    case 1:
                                        dropPoint( shapeItems[index_array[p]].n1, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n2, index_array[p], shapeItems, pnts );
                                        break;
                                    case 2:
                                        dropPoint( shapeItems[index_array[p]].n1, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n2, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n3, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n4, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n5, index_array[p], shapeItems, pnts );
                                        break;
                                    case 3:
                                        dropPoint( shapeItems[index_array[p]].n1, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n2, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n3, index_array[p], shapeItems, pnts );
                                        dropPoint( shapeItems[index_array[p]].n4, index_array[p], shapeItems, pnts );
                                        break;
                                }
                                shapeItems.remove(index_array[p]);
                                for( int j = 0; j < inundationItems.size(); j++ )
                                    for( int k = 0; k < inundationItems[j].number_shape.size(); k++ )
                                        if( inundationItems[j].number_shape[k] > index_array[p] )
                                            inundationItems[j].number_shape[k]--;
                                for( int i = p+1; i < index_array.size(); i++ )
                                    if( index_array[i] > index_array[p] ) index_array[i]--;
                                }
                            }
                            rectItems.clear();
                            shapeSave(view);
                            flag_ctrl = flag_A = flag_copy = false;
                            index_array.clear();
                            index_array_copy.clear();
                            drop_inunds.clear();
                            itemInMotion = 0;
                            count_Shapes = 0;
                            view->repaint();
                        }
                    }
                    else
                    {
 			switch( shapeItems[index_del].type )
			{
			    case 1:
				dropPoint( shapeItems[index_del].n1, index_del, shapeItems, pnts );
				dropPoint( shapeItems[index_del].n2, index_del, shapeItems, pnts );
				break;
    			    case 2:
				dropPoint( shapeItems[index_del].n1, index_del, shapeItems, pnts );
	    			dropPoint( shapeItems[index_del].n2, index_del, shapeItems, pnts );
				dropPoint( shapeItems[index_del].n3, index_del, shapeItems, pnts );
		    		dropPoint( shapeItems[index_del].n4, index_del, shapeItems, pnts );
				dropPoint( shapeItems[index_del].n5, index_del, shapeItems, pnts );
			    	break;
    			    case 3:
				dropPoint( shapeItems[index_del].n1, index_del, shapeItems, pnts );
				dropPoint( shapeItems[index_del].n2, index_del, shapeItems, pnts );
	    			dropPoint( shapeItems[index_del].n3, index_del, shapeItems, pnts );
		    		dropPoint( shapeItems[index_del].n4, index_del, shapeItems, pnts );
				break;
			}
			// - deleting fill if deleted figure was in the fill's path -
			for( int i=0; i < inundationItems.size(); i++ )
			    if( shapeItems[index_del].type==2 && inundationItems[i].number_shape.size()==1 &&
				inundationItems[i].number_shape[0]==index_del )
			    {
				inundationItems.remove(i);
				flag_arc_inund = true;
			    }
			if( !flag_arc_inund )
			{
			    for( int i=0; i < inundationItems.size(); i++ )
				for( int j=0; j < inundationItems[i].number_shape.size(); j++ )
				{
				    flag_break_move = false;
				    if( inundationItems[i].number_shape[j] == index_del )
				    {
					inundationItems.remove(i);
					flag_break_move = true;
					break;
				    }
				    if( flag_break_move ) break;
				}
			}
			for( int i=0; i < inundationItems.size(); i++ )
			    for( int j=0; j < inundationItems[i].number_shape.size(); j++ )
				if( inundationItems[i].number_shape[j] > index_del )
				    inundationItems[i].number_shape[j] -= 1;
			shapeItems.remove(index_del);
			rectItems.clear();
			shapeSave(view);
			view->repaint();
                    }
                    view->unsetCursor();
                    index_del = -1;
                    flag_A = false;
                }
                if( ev->key() == Qt::Key_A && !status )
                {
                    if( !(QApplication::keyboardModifiers()&Qt::ControlModifier) )	break;
                    flag_A = true;
                    flag_ctrl_move = 1;
                    if( index_array.size() )	index_array.clear();
                    for( int i=0; i < shapeItems.size(); i++ )
                        index_array.push_back(i);
                    count_Shapes = shapeItems.size();
                    moveAll( QPointF(0,0), shapeItems, pnts, inundationItems, view );
                    if( index_array.size() )
                        ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(true);
                    view->repaint();
                }
                if( ev->key() == Qt::Key_Up && (index_temp!=-1 || flag_A) )
                {
                    if( (flag_down || flag_left || flag_right || index<0 || index>shapeItems.size()-1) && !flag_A ) break;
                    flag_up = true;
                    offset = QPointF(0,-1);
                    moveUpDown( shapeItems, pnts, inundationItems, view );
                    view->repaint();
                }
                if( ev->key() == Qt::Key_Down && (index_temp!=-1 || flag_A) )
                {
                    if( (flag_up || flag_left || flag_right || index<0 || index>shapeItems.size()-1) && !flag_A ) break;
                    flag_down = true;
                    offset = QPointF(0,1);
                    moveUpDown( shapeItems, pnts, inundationItems, view );
                    view->repaint();
                }

                if( ev->key() == Qt::Key_Left && (index_temp!=-1 || flag_A) )
                {
                    if( (flag_down || flag_up || flag_right || index<0 || index>shapeItems.size()-1) && !flag_A ) break;
                    flag_left = true;
                    offset = QPointF(-1,0);
                    moveUpDown( shapeItems, pnts, inundationItems, view );
                    view->repaint();
                }
                if( ev->key() == Qt::Key_Right && (index_temp!=-1 || flag_A) )
                {
                    if( (flag_down || flag_left || flag_up || index<0 || index>shapeItems.size()-1) && !flag_A ) break;
                    flag_right = true;
                    offset = QPointF(1,0);
                    moveUpDown( shapeItems, pnts, inundationItems, view );
                    view->repaint();
                }
        	return true;		
            }
	    break;
        }
        case QEvent::KeyRelease:
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if( devW )
            {
                if( ev->key() == Qt::Key_Shift ) flag_key=false;
                if( ev->key() == Qt::Key_Control )
                {
                    if( status_hold || flag_A ) break;
                    if( count_Shapes )
                    {
                        count_Shapes = 0;
                        rectItems.clear();
                        index_array.clear();
                        view->repaint();
                        itemInMotion = 0;
                        index_temp = -1;
                        ((VisDevelop *)view->mainWin())->actVisItCopy->setEnabled(false);
                        flag_ctrl = false;
                    }
                    else flag_ctrl = false;
                }
                if( ev->key() == Qt::Key_Up )
                {
                    if( flag_down || flag_left || flag_right ) break;
                    flag_up = false;
                    itemInMotion = 0;
                    flag_first_move = true;
                }
                if( ev->key() == Qt::Key_Down )
                {
                    if( flag_up || flag_left || flag_right ) break;
                    flag_down = false;
                    itemInMotion = 0;
                    flag_first_move = true;
                }
                if( ev->key() == Qt::Key_Left )
                {
                    if( flag_down || flag_up || flag_right ) break;
                    flag_left = false;
                    itemInMotion = 0;
                    flag_first_move = true;
                }
                if( ev->key() == Qt::Key_Right )
                {
                    if( flag_down || flag_left || flag_up ) break;
                    flag_right = false;
                    itemInMotion = 0;
                    flag_first_move = true;
                }
        	return true;		
            }
        }
	break;
    } 
    return false;
} 

//- rotation of the point -
QPointF ShapeElFigure::rotate( const QPointF &pnt, double alpha )
{
    return QPointF( pnt.x()*cos((alpha*M_PI)/180)-pnt.y()*sin((alpha*M_PI)/180),
                    pnt.x()*sin((alpha*M_PI)/180)+pnt.y()*cos((alpha*M_PI)/180) );
}

//- unrotation of the point -
QPointF ShapeElFigure::unRotate( const QPointF &pnt, double alpha, double a, double b )
{
    return QPointF( (pnt.x()-a)*cos((alpha*M_PI)/180)-(pnt.y()-b)*sin((alpha*M_PI)/180),
                    -(pnt.x()-a)*sin((alpha*M_PI)/180)-(pnt.y()-b)*cos((alpha*M_PI)/180) );
}

//- getting the point of the arc, using t as parameter and a,b as radiuses -
QPointF ShapeElFigure::arc( double t, double a, double b )
{
    return QPointF( a*cos(t*M_PI*2), -b*sin(t*M_PI*2) );
}

//- getting the rotation angle -
double ShapeElFigure::angle( const QLineF &l, const QLineF &l1 )
{
    if( l.isNull() || l1.isNull() ) return 0;

    return acos( vmax( -1, vmin( 1, (l.dx()*l1.dx() + l.dy()*l1.dy()) / (l.length()*l1.length()))) ) * 180 / M_PI;
}

//- detecting the length between two points -
double ShapeElFigure::length( const QPointF &pt1, const QPointF &pt2 )
{
    double x = pt2.x() - pt1.x();
    double y = pt2.y() - pt1.y();
    return sqrt(x*x + y*y);
}
//- moving the figure -
void ShapeElFigure::moveItemTo( const QPointF &pos, QVector<ShapeItem> &shapeItems, PntMap *pnts, WdgView *view )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
    ShapeItem temp_shape;
    double ang_t;
    double a, a_small, b, b_small;
    double ang;
    QPointF StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4;
    int MotionNum_1 = itemInMotion->n1;
    int MotionNum_2 = itemInMotion->n2;
    int MotionNum_3 = itemInMotion->n3;
    int MotionNum_4 = itemInMotion->n4;
    int MotionNum_5 = itemInMotion->n5;
    shapeType = itemInMotion->type;
    QBrush MotionBrush = itemInMotion->brush;
    QPen MotionPen = itemInMotion->pen;
    QPen MotionPenSimple = itemInMotion->penSimple;
    float MotionWidth = itemInMotion->width;
    float MotionBorderWidth = itemInMotion->border_width;
    QLineF line1, line2;
    QPointF Temp, EndMotionPos_temp, CtrlMotionPos_1_temp, CtrlMotionPos_2_temp;
    bool flag_MotionNum_1=false,
	 flag_MotionNum_2=false,
	 flag_MotionNum_3=false,
	 flag_MotionNum_4=false,
	 flag_MotionNum_5=false;
    for ( int i = 0; i < num_vector.size(); i++ )
    {
        if ( num_vector[i] == MotionNum_1 ) flag_MotionNum_1 = true;
        if ( num_vector[i] == MotionNum_2 ) flag_MotionNum_2 = true;
        if ( num_vector[i] == MotionNum_3 ) flag_MotionNum_3 = true;
        if ( num_vector[i] == MotionNum_4 ) flag_MotionNum_4 = true;
        if ( num_vector[i] == MotionNum_5 ) flag_MotionNum_5 = true;
    }
    //- moving the whole figure -
    if( rect_num == -1 )
    {
        if( (status_hold && count_holds > 1) || (flag_ctrl && count_Shapes > 1) )
        {
            if( !flag_MotionNum_1 )			// moving this point in the first time
            {
                StartMotionPos = scaleRotate( ( *pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
                StartMotionPos += offset;
                num_vector.append( MotionNum_1 );	//adding this point in the vector of common points
            }
            else StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );	// don't change point's coordinates
            if( !flag_MotionNum_2 )
            {
                EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
                EndMotionPos += offset;
                num_vector.append( MotionNum_2 );
            }
            else EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
            if( !flag_MotionNum_3 )
            {
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                CtrlMotionPos_1 += offset;
                num_vector.append( MotionNum_3 );
            }
            else CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            if( !flag_MotionNum_4 )
            {
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
                CtrlMotionPos_2 += offset;
                num_vector.append( MotionNum_4 );
            }
            else CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            if( !flag_MotionNum_5 )
            {
                CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
                CtrlMotionPos_3 += offset;
                num_vector.append( MotionNum_5 );
            }
            else CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
        }
        else
        {
            StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
            EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
            StartMotionPos += offset;
            EndMotionPos += offset; 
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_1 += offset;
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 += offset;
        }  
        if( shapeType==2 )
        {
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            //CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            //CtrlMotionPos_1 += offset;
            //CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            //CtrlMotionPos_2 += offset;
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
            CtrlMotionPos_3 += offset;
            a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
            b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
            line2 = QLineF( CtrlMotionPos_1, QPointF( CtrlMotionPos_1.x()+10, CtrlMotionPos_1.y() ) );
            line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
            if( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y() )	ang = angle( line1, line2 );
            else 						ang = 360 - angle( line1, line2 );
            angle_temp = ang;
            t_start = CtrlMotionPos_4.x();
            t_end = CtrlMotionPos_4.y();
            StartMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_start,a,b),ang).x(),
                                      CtrlMotionPos_1.y() - rotate(arc(t_start,a,b),ang).y() );
            EndMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_end,a,b),ang).x(),
                                    CtrlMotionPos_1.y() - rotate(arc(t_end,a,b),ang).y() );
        }
    }
    //- moving the start point of the figure -
    if( rect_num==0 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n1].x()).arg((*pnts)[itemInMotion->n1].y()), 10000 );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );

        if( shapeType==2 )
        {
            StartMotionPos = Mouse_pos;
            if( flag_up || flag_down || flag_right || flag_left ) StartMotionPos = (*pnts)[itemInMotion->n1]+offset;
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
            b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
            t_end = CtrlMotionPos_4.y();
            line2 = QLineF( CtrlMotionPos_1, QPointF( CtrlMotionPos_1.x() + 10, CtrlMotionPos_1.y() ) );
            line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
            if( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y() )	ang = angle( line1, line2 );
            else 						ang = 360 - angle( line1, line2 );
            angle_temp = ang;
            StartMotionPos = unRotate( StartMotionPos, ang, CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
            if( StartMotionPos.x() >= a )	StartMotionPos = QPointF( a, (StartMotionPos.y()/StartMotionPos.x())*a );
            if (StartMotionPos.x() < -a)	StartMotionPos = QPointF( -a, (StartMotionPos.y()/StartMotionPos.x())*(-a) );
            if(StartMotionPos.y()<=0)	t_start = acos( StartMotionPos.x()/a )/( 2*M_PI );
            else			t_start = 1-acos( StartMotionPos.x()/a )/( 2*M_PI );
            if( t_start < 0 )		t_start = 1+t_start;
            if( t_start > t_end)	t_end += 1;
            if( (t_end-1) > t_start ) 	t_end -= 1;
            if( t_start == t_end ) 	t_end += 1;
            if( t_end > t_start && t_start >= 1 && t_end > 1 )	{ t_start -= 1; t_end -= 1; }
            StartMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_start,a,b),ang).x(),
                                      CtrlMotionPos_1.y() - rotate(arc(t_start,a,b),ang).y() );
            EndMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_end,a,b),ang).x(),
                                    CtrlMotionPos_1.y() - rotate(arc(t_end,a,b),ang).y() );
        }
        else
        {
	    if( !flag_hold_arc && !flag_arc_rect_3_4 )		// if the figure is not connected to the arc
            {
                if( status_hold && count_holds && flag_rect )
                    if( !flag_MotionNum_1 )
                    {
                        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
                        StartMotionPos += offset;
                        num_vector.append(MotionNum_1);
                    }
                    else StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
                else 
                {
                    StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
                    StartMotionPos += offset;
                }
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            }
            if( flag_hold_arc || flag_arc_rect_3_4 )	// if the figure is connected to the arc
            {
                if( arc_rect==0 )
                {
                    StartMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scale, flag_rotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
                }
                if( arc_rect==1 )
                {
                    StartMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scale, flag_rotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
                }
            }
        }
    }
    //- moving the end point of the figure -
    if ( rect_num == 1 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n2].x()).arg((*pnts)[itemInMotion->n2].y()), 10000 );
        StartMotionPos=scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
        if ( shapeType == 2 )
        {
            EndMotionPos = Mouse_pos;
            if ( flag_up || flag_down || flag_right || flag_left )
            {
                EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
                EndMotionPos += offset;
            }
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
            a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
            line2 = QLineF( CtrlMotionPos_1, QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()) );
            line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
            if ( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y()) ang=angle( line1,line2 );
            else ang = 360 - angle( line1,line2 );
            angle_temp = ang;
            EndMotionPos = unRotate( EndMotionPos, ang, CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
            if ( EndMotionPos.x() <= -a )
            {
                EndMotionPos.setY( (EndMotionPos.y()/EndMotionPos.x())*(-a) );
                EndMotionPos.setX( -a );
            }
            if ( EndMotionPos.x() > a )
            {
                EndMotionPos.setY( (EndMotionPos.y()/EndMotionPos.x())*(a) );
                EndMotionPos.setX( a );
            }
            if( EndMotionPos.y() <= 0 )
                t_end=acos( EndMotionPos.x()/a )/( 2*M_PI );
            else
                t_end=1-acos( EndMotionPos.x()/a )/( 2*M_PI );
            if ( t_start > t_end ) t_end += 1;
            if ( (t_end-1) > t_start ) t_end -= 1;
            if ( t_start == t_end ) t_end += 1;
            if ( (t_end > t_start) && t_start >= 1 && t_end > 1)
            {
                t_start -= 1;
                t_end -= 1;
            }
            EndMotionPos=QPointF( CtrlMotionPos_1.x() + rotate( arc(t_end, a, b ), ang ).x(),
                                 CtrlMotionPos_1.y() - rotate( arc( t_end, a, b ), ang ).y() );
        }
        else
        {
            if ( !flag_hold_arc && !flag_arc_rect_3_4 )
            { 
                if ( status_hold && count_holds && flag_rect )
                    if ( !flag_MotionNum_2 )
                    {
                        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
                        EndMotionPos += offset;
                        num_vector.append( MotionNum_2 );
                    }
                    else EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
                else
                {
                    EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
                    EndMotionPos += offset;
                }
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            }
            if ( flag_hold_arc || flag_arc_rect_3_4 )
            {
                if (arc_rect == 0)
                {
                    EndMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scale, flag_rotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
                }
                if ( arc_rect == 1 )
                {
                    EndMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scale, flag_rotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
                }
            }

        }
    }
    //- moving the first control point of the figure -
    if ( rect_num == 2 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n3].x()).arg((*pnts)[itemInMotion->n3].y()), 10000 );
        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
        if (shapeType == 2)
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
            a = length( CtrlMotionPos_3, CtrlMotionPos_1);
            line2 = QLineF( CtrlMotionPos_1, QPointF( CtrlMotionPos_1.x()+10, CtrlMotionPos_1.y()) );
            line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
            if ( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y() ) ang = angle( line1, line2 );
            else ang = 360 - angle( line1, line2 );
            angle_temp = ang;
            t_start = CtrlMotionPos_4.x();
            t_end = CtrlMotionPos_4.y();
        }
        else
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_1 += offset;
        }
    }
    //- moving the second control point of the figure -
    if (rect_num == 3)
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n4].x()).arg((*pnts)[itemInMotion->n4].y()), 10000 );
        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scale, flag_rotate );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
        if ( shapeType == 2 )
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 += offset;
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
            a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
            b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
            t_start = CtrlMotionPos_4.x();
            t_end = CtrlMotionPos_4.y();
            line2 = QLineF( CtrlMotionPos_1, QPointF( CtrlMotionPos_1.x() + 10, CtrlMotionPos_1.y() ) );
            line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
            if ( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y() ) ang = angle( line1,line2 ); 
            else ang = 360 - angle( line1, line2 );
            angle_temp = ang;
            CtrlMotionPos_3 = unRotate( CtrlMotionPos_3, ang,CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
            CtrlMotionPos_2 = unRotate( CtrlMotionPos_2, ang, CtrlMotionPos_1.x(), CtrlMotionPos_1.y() );
            line2 = QLineF( QPointF(0,0), QPointF(-100,0) );
            line1 = QLineF( QPointF(0,0), CtrlMotionPos_2 );
            ang_t = angle( line1, line2 ) - 90;
            ang = ang + ang_t;
            StartMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_start,a,b),ang).x(),
                                   CtrlMotionPos_1.y() - rotate(arc(t_start,a,b),ang).y() );
            EndMotionPos = QPointF( CtrlMotionPos_1.x() + rotate(arc(t_end,a,b),ang).x(),
                                 CtrlMotionPos_1.y() - rotate(arc(t_end,a,b),ang).y() );
        }
        else
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
            CtrlMotionPos_2 += offset;
        }
    }
    //- moving the third control point of the figure -
    if ( rect_num == 4 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n5].x()).arg((*pnts)[itemInMotion->n5].y()), 10000 );
        CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scale, flag_rotate );
        CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scale, flag_rotate );
        CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scale, flag_rotate );
        CtrlMotionPos_3 += offset;
        CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scale, flag_rotate );
        a = length( CtrlMotionPos_3, CtrlMotionPos_1 );
        b = length( CtrlMotionPos_2, CtrlMotionPos_1 );
        t_start = CtrlMotionPos_4.x();
        t_end = CtrlMotionPos_4.y();
        line2 = QLineF( CtrlMotionPos_1, QPointF( CtrlMotionPos_1.x() + 100, CtrlMotionPos_1.y() ) );
        line1 = QLineF( CtrlMotionPos_1, CtrlMotionPos_3 );
        if ( CtrlMotionPos_3.y() <= CtrlMotionPos_1.y() ) ang = angle( line1, line2 );
        else ang = 360 - angle( line1, line2 );
        angle_temp = ang;
        StartMotionPos = QPointF( CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(),
                               CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y() );
        EndMotionPos = QPointF( CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(),
                             CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y() );
    }
    shapeItems.remove( index );
    if ( !flag_ctrl || ( !flag_ctrl_move && count_Shapes == count_moveItemTo + count_Shapes - 1 ) )
        rectItems.clear();
    //- building the line -
    if ( shapeType == 1 )
    {
        line2 = QLineF( StartMotionPos, QPointF( StartMotionPos.x() + 10, StartMotionPos.y() ) );
        line1 = QLineF( StartMotionPos, EndMotionPos );
        if ( StartMotionPos.y() <= EndMotionPos.y() ) ang = 360 - angle( line1, line2 );
        else ang = angle( line1, line2 );
        if( vmax(1,MotionWidth) == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth) + 1,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos ), painterPathSimple( 1, ang, StartMotionPos,EndMotionPos ),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1,angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth), vmax(1,MotionBorderWidth), 1, ang, StartMotionPos, EndMotionPos ), painterPathSimple( 1, ang, StartMotionPos,EndMotionPos ),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1,angle_temp ) );
        if( devW && devW->edit() )
        {
            rectPath.addRect( QRectF( QPointF( StartMotionPos.x()-4, StartMotionPos.y()-4 ), QSize(8,8) ) );
	
            rectItems.append( RectItem( rectPath, MotionNum_1, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( QPointF( EndMotionPos.x()-4, EndMotionPos.y()-4 ), QSize(8,8)) );
            rectItems.append( RectItem( rectPath, MotionNum_2, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
        }
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scale, flag_rotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scale, flag_rotate );
        (*pnts)[MotionNum_1] = StartMotionPos;
        (*pnts)[MotionNum_2] = EndMotionPos;
    }
    //- building the arc -
    if ( shapeType == 2 )
    {
        double t;
        CtrlMotionPos_2 = QPointF( CtrlMotionPos_1.x() + rotate( arc( 0.25, a, b ), ang ).x(),
                                CtrlMotionPos_1.y() - rotate( arc( 0.25, a, b ), ang ).y() );
        CtrlMotionPos_4 = QPointF( t_start, t_end );
        CtrlMotionPos_3 = QPointF( CtrlMotionPos_1.x() + rotate( arc( 0, a, b ), ang ).x(),
                                CtrlMotionPos_1.y() - rotate( arc( 0, a, b ), ang ).y() );
        if( vmax(1,MotionWidth) == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth) + 1, MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                               painterPathSimple( 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, MotionNum_5, CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2, angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth), vmax(1,MotionBorderWidth), 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                               painterPathSimple( 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, MotionNum_5, CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2, angle_temp ) );
        if( devW && devW->edit() )
        {
            rectPath.addRect( QRectF( QPointF( StartMotionPos.x() - 4, StartMotionPos.y() - 4 ),QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_1, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( QPointF( EndMotionPos.x() - 4, EndMotionPos.y() - 4 ), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_2, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( CtrlMotionPos_1.toPoint(), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_3, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ), 
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( QPointF( CtrlMotionPos_2.x() - 4, CtrlMotionPos_2.y() - 4), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_4, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ), 
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) ) );
            rectPath = newPath;
            Temp = QPointF( CtrlMotionPos_3.x() - 20, CtrlMotionPos_3.y() - 4);
            rectPath.addRect( QRectF( Temp, QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_5, QBrush( QColor( 0, 0, 0, 255) ,Qt::SolidPattern ), 
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) ) );
            rectPath = newPath;
        }
        
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scale, flag_rotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scale, flag_rotate );
        CtrlMotionPos_1 = unScaleRotate( CtrlMotionPos_1, view, flag_scale, flag_rotate );
        CtrlMotionPos_2 = unScaleRotate( CtrlMotionPos_2, view, flag_scale, flag_rotate );
        CtrlMotionPos_3 = unScaleRotate( CtrlMotionPos_3, view, flag_scale, flag_rotate );
        (*pnts)[MotionNum_1] = StartMotionPos;
        (*pnts)[MotionNum_2] = EndMotionPos;
        (*pnts)[MotionNum_3] = CtrlMotionPos_1;
        (*pnts)[MotionNum_4] = CtrlMotionPos_2;
        (*pnts)[MotionNum_5] = CtrlMotionPos_3;
    }
    //- building the bezier curve -
    if ( shapeType == 3 )
    {
        line2 = QLineF( StartMotionPos, QPointF( StartMotionPos.x() + 10, StartMotionPos.y() ) );
        line1 = QLineF( StartMotionPos, EndMotionPos );
        if ( StartMotionPos.y() <= EndMotionPos.y() )
            ang = 360 - angle( line1, line2 );
        else
            ang = angle( line1, line2 );
        if( vmax(1,MotionWidth) == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth) + 1, MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                               painterPathSimple( 3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 3, angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( vmax(1,MotionWidth), vmax(1,MotionBorderWidth), 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                               painterPathSimple( 3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 3, angle_temp ) );
        if( devW && devW->edit() )
        {
            rectPath.addRect( QRectF( QPointF( StartMotionPos.x() - 4, StartMotionPos.y() - 4), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_1, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( QPointF( EndMotionPos.x() - 4, EndMotionPos.y() - 4 ), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_2, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( CtrlMotionPos_1.toPoint(), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_3, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
            rectPath.addRect( QRectF( CtrlMotionPos_2.toPoint(), QSize(8,8) ) );
            rectItems.append( RectItem( rectPath, MotionNum_4, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                              QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
            rectPath = newPath;
        }
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scale, flag_rotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scale, flag_rotate );
        CtrlMotionPos_1 = unScaleRotate( CtrlMotionPos_1, view, flag_scale, flag_rotate );
        CtrlMotionPos_2 = unScaleRotate( CtrlMotionPos_2, view, flag_scale, flag_rotate );
        (*pnts)[MotionNum_1] = StartMotionPos;
        (*pnts)[MotionNum_2] = EndMotionPos;
        (*pnts)[MotionNum_3] = CtrlMotionPos_1;
        (*pnts)[MotionNum_4] = CtrlMotionPos_2;
    }

    for ( int i = shapeItems.size() - 1; i > index; i-- )
    {
        temp_shape = shapeItems[i-1];
        shapeItems[i-1] = shapeItems[i];
        shapeItems[i] = temp_shape;
    }
    if ( count_moveItemTo == count_Shapes ) 
        previousPosition_all = pos;
}

//- detecting connected figures -
bool ShapeElFigure::holds( const QVector<ShapeItem> &shapeItems, PntMap *pnts )
{
    int num,index_hold;
    bool flag_equal;
    if(index_array.size())
        index_array.clear();
    for(int i = 0; i < shapeItems.size(); i++)
        index_array.push_back(-1);
    index_array[0] = index;
    num = 0;
    do
    {
        index_hold = index_array[num];
        for (int i = 0; i < shapeItems.size(); i++)
            if( i != index_hold && ((shapeItems[index_hold].n1 == shapeItems[i].n1) ||
			(shapeItems[index_hold].n2 == shapeItems[i].n2) ||
			(shapeItems[index_hold].n1 == shapeItems[i].n2) ||
			(shapeItems[index_hold].n2 == shapeItems[i].n1)) &&
			ellipse_draw_startPath == newPath && ellipse_draw_endPath == newPath )
	    {
                    flag_equal = 0;
                    for ( int j = 0; j <= count_holds; j++ )
                        if( index_array[j] == i ) flag_equal = 1;
                    if ( flag_equal == 0 )
                    {
                        count_holds++;
                        index_array[count_holds] = i;
                    }
            }
        num++;
    }
    while( num != count_holds + 1 );
    if ( count_holds > 0 ) return true;
    else return false;
}

//- detecting the figures(figure), or their points to move by key(up, down, left, rught) events -
void ShapeElFigure::moveUpDown( QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems, WdgView *view )
{
    int rect_num_temp;
    count_moveItemTo = 0;
    bool flag_break_move;
    if ( flag_ctrl && count_Shapes )
    {
        moveAll( QPointF(0,0), shapeItems, pnts, inundationItems, view );
    }
    else 
        if ( !flag_first_move )
        {
            count_holds = 0;
            flag_rect = false;
            if ( status_hold )
                holds( shapeItems, pnts );//calling "holds" to detect is the figure connected with other or not.
            if ( count_holds )// if the Ctrl is pressed and there are connected figures
            {
                count_Shapes = count_holds + 1;//getting the number of figures to move
                if ( rect_num != -1 )// if ther is rect under the mouse pointer
                {
                    rect_num_temp = rect_num;
                    rect_num = realRectNum ( rect_num, shapeItems );// detecting the number of the rect for moveItemTo
                    if( ( rect_num == 2 || rect_num == 3 ) && shapeItems[index].type == 3 )// if there is rect of unconnected point of bexier curve
                        flag_rect=false;
                    if( rect_num == 0 || rect_num == 1 )// if there is the rect of start or end point of the figure
                    {
                        rectNum0_1( shapeItems, rect_num_temp, pnts, view );
                    }
                    if( (rect_num == 3 || rect_num == 4) && shapeItems[index].type == 2 )
                        rectNum3_4( shapeItems );
                }
            }
        if ( flag_rect || flag_arc_rect_3_4 )// if there is the rect number of figures to move becomes equal to the number of figures, connected with this rect
            count_Shapes = count_rects;
        }
    if ( flag_rect || flag_arc_rect_3_4 || ( rect_num == -1 && count_holds ) )
        moveAll( QPointF(0,0), shapeItems, pnts, inundationItems, view );
    if ( ( !flag_ctrl  || (!flag_rect && rect_num != -1 ) ) && !flag_arc_rect_3_4 )// if there is simple figure or one of its rects
        if (index != -1)
        {
            num_vector.clear();
            flag_ctrl = true;
            flag_ctrl_move = false;
            count_moveItemTo = 1;
            count_Shapes = 1;
            if ( !status_hold )
                for ( int i = 0; i < shapeItems.size(); i++ )
                {
                    if ( i != index )
                    {
                        if ( shapeItems[index].type == 2 )
                        {
                            if ( (shapeItems[index].n5 == shapeItems[i].n1) || 
                                (shapeItems[index].n5 == shapeItems[i].n2) ||
                                (shapeItems[index].n5 == shapeItems[i].n3) || 
                                (shapeItems[index].n5 == shapeItems[i].n4) ||
                                (shapeItems[index].n5 == shapeItems[i].n5) )
                            {
                                QPointF Temp = (*pnts)[shapeItems[index].n5];
                                if( shapeItems[index].n5 > 0 )
                                    shapeItems[index].n5 = appendPoint( Temp, shapeItems, pnts, 0 );
                                else if( shapeItems[index].n5 <= -10 )
                                    shapeItems[index].n5 = appendPoint( Temp, shapeItems, pnts, 1 );
                            }
                        }
                        if ( shapeItems[index].type == 2 || shapeItems[index].type == 3 )
                        {
                            if ( (shapeItems[index].n4 == shapeItems[i].n1) || 
                                (shapeItems[index].n4 == shapeItems[i].n2) ||
                                (shapeItems[index].n4 == shapeItems[i].n3) || 
                                (shapeItems[index].n4 == shapeItems[i].n4) ||
                                (shapeItems[index].n4 == shapeItems[i].n5) )
                            {
                                QPointF Temp = (*pnts)[shapeItems[index].n4];
                                if( shapeItems[index].n4 > 0 )
                                    shapeItems[index].n4 = appendPoint( Temp, shapeItems, pnts, 0 );
                                else if( shapeItems[index].n4 <= -10 )
                                    shapeItems[index].n4 = appendPoint( Temp, shapeItems, pnts, 1 );
                            }
                            if ( (shapeItems[index].n3 == shapeItems[i].n1) || 
                                (shapeItems[index].n3 == shapeItems[i].n2) ||
                                (shapeItems[index].n3 == shapeItems[i].n3) || 
                                (shapeItems[index].n3 == shapeItems[i].n4) ||
                                (shapeItems[index].n3 == shapeItems[i].n5) )
                            {
                                QPointF Temp = (*pnts)[shapeItems[index].n3];
                                if( shapeItems[index].n3 > 0 )
                                    shapeItems[index].n3 = appendPoint( Temp, shapeItems, pnts, 0 );
                                else if( shapeItems[index].n3 <= -10 )
                                    shapeItems[index].n3 = appendPoint( Temp, shapeItems, pnts, 1 );
                            }
                        }
                        if( (shapeItems[index].n1 == shapeItems[i].n1) || 
                            (shapeItems[index].n1 == shapeItems[i].n2) ||
                            (shapeItems[index].n1 == shapeItems[i].n3) || 
                            (shapeItems[index].n1 == shapeItems[i].n4) ||
                            (shapeItems[index].n1 == shapeItems[i].n5) )
                        {
                            QPointF Temp = (*pnts)[shapeItems[index].n1];
                            if( shapeItems[index].n1 > 0 )
                                shapeItems[index].n1 = appendPoint( Temp, shapeItems, pnts, 0 );
                            else if( shapeItems[index].n1 <= -10 )
                                shapeItems[index].n1 = appendPoint( Temp, shapeItems, pnts, 1 );
                        }
                        if( (shapeItems[index].n2 == shapeItems[i].n1) || 
                            (shapeItems[index].n2 == shapeItems[i].n2) ||
                            (shapeItems[index].n2 == shapeItems[i].n3) ||
                            (shapeItems[index].n2 == shapeItems[i].n4) ||
                            (shapeItems[index].n2 == shapeItems[i].n5) )
                        {
                            QPointF Temp = (*pnts)[shapeItems[index].n2];
                            if( shapeItems[index].n2 > 0 )
                                shapeItems[index].n2 = appendPoint( Temp, shapeItems, pnts, 0 );
                            else if( shapeItems[index].n2 <= -10 )
                                shapeItems[index].n2 = appendPoint( Temp, shapeItems, pnts, 1 );
                        }
                    }
                }
            itemInMotion = &shapeItems[index];
            moveItemTo( (*pnts)[itemInMotion->n1], shapeItems, pnts, view );
            if( inundationItems.size() )
                for( int i = 0; i < inundationItems.size(); i++ )
                    for( int j = 0; j < inundationItems[i].number_shape.size(); j++ )
                    {
                        flag_break_move = false;
                        if( inundationItems[i].number_shape[j] == index )
                        {
                            inundationPath = createInundationPath( inundationItems[i].number_shape, shapeItems, pnts, view );
                            inundationItems[i].path = inundationPath;
                            flag_break_move = true;
                            break;
                        }
                        if ( flag_break_move ) break;
                    }
            flag_ctrl = false;
        }
        else
            itemInMotion = 0;
    if( inundationItems.size() && !flag_inund_break && ( !status_hold || flag_A ) )
    {
        if( count_Shapes > 1 )
            checkPoint_checkInundation( shapeItems, pnts, inundationItems );
        else
        {
            for( int i = 0; i < inundationItems.size(); i++ )
                for( int j = 0; j < inundationItems[i].number_shape.size(); j++ )
                {
                    flag_break_move = false;
                    if( inundationItems[i].number_shape[j] == index )
                    {
                        inundationItems.remove(i);
                        shapeSave( view );
                        flag_break_move = true;
                        break;
                    }
                    if ( flag_break_move ) break;
                }
        }
    }
    flag_inund_break = true;
}

int ShapeElFigure::realRectNum( int rect_num_old, const QVector<ShapeItem> &shapeItems )
{
    int rect_num_new;
    bool flag_isArc = false;
    //- detecting the correct index of the figure -
    for ( int i = 0; i <= shapeItems.size()-1; i++ )
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
    // - detecting the number of the rect for moveItemTo -
    switch( shapeItems[index].type )
    {
	case 1:
	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new = 0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new = 1;
	    break;
	case 2:
	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new = 0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new = 1;
	    if( rectItems[rect_num_old].num == shapeItems[index].n3 )	rect_num_new = 2;
	    if( rectItems[rect_num_old].num == shapeItems[index].n4 )	rect_num_new = 3;
	    if( rectItems[rect_num_old].num == shapeItems[index].n5 )	rect_num_new = 4;
	    break;
	case 3:
    	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new = 0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new = 1;
	    if( rectItems[rect_num_old].num == shapeItems[index].n3 )	rect_num_new = 2;
	    if( rectItems[rect_num_old].num == shapeItems[index].n4 )	rect_num_new = 3;
	    break;
    }
    return rect_num_new;
}

void ShapeElFigure::rectNum0_1( const QVector<ShapeItem> &shapeItems, int rect_num_temp, PntMap *pnts, WdgView *view )
{
    flag_rect=true;
    count_rects=0;
    QVector <int> index_array_temp;
    for(int i=0; i<count_holds+1; i++)
    {
        index_array_temp.push_back(-1);
        rect_array.push_back(0);
    }
    //- detecting the common points for all connected figures -
    for (int i=0; i<=count_holds; i++)
    {
        if ((*pnts)[rectItems[rect_num_temp].num]==(*pnts)[shapeItems[index_array[i]].n1])
        {
            index_array_temp[count_rects]=index_array[i];
            rect_array[count_rects]=0;
            count_rects++;
            flag_rect=true;
        }
        if ((*pnts)[rectItems[rect_num_temp].num]==(*pnts)[shapeItems[index_array[i]].n2])
        {
            index_array_temp[count_rects]=index_array[i];
            rect_array[count_rects]=1;
            count_rects++;
            flag_rect=true;
        }

    }
    index_array.clear();
    for(int i=0; i<count_rects+1; i++)
        index_array.push_back(-1);
    for (int i=0; i<count_rects; i++)
        index_array[i]=index_array_temp[i];
    // if there is an arc in "index_array" we put it on the first place in it.
    int num_arc=-1;
    for (int i=0; i<count_rects; i++)
        if (shapeItems[index_array[i]].type==2)
        {
            flag_hold_arc=true;
            num_arc=i;
        }
    if (num_arc!=-1)
    {
        int index_0=index_array[0];
        int rect_0=rect_array[0];
        index_array[0]=index_array[num_arc];
        index_array[num_arc]=index_0;
        rect_array[0]=rect_array[num_arc];
        rect_array[num_arc]=rect_0;
    }
    if (count_rects==1)
    {
        flag_rect=false;
        if (shapeItems[index_array[0]].type==2)
        {
            rect_num_arc=rect_num;
            flag_hold_arc=false;
        }
    }
    index_array_temp.clear();
}
void ShapeElFigure::rectNum3_4( const QVector<ShapeItem> &shapeItems)
{
    flag_arc_rect_3_4 = true;
    QVector<int> index_array_temp;
    for( int i = 0; i < count_holds + 5; i++ )
    {
        fig_rect_array.push_back(0);
        arc_rect_array.push_back(0);
        index_array_temp.push_back(-1);
    }
    flag_rect = false;
    index_array_temp[0] = index;
    if ( rect_num == 3 )
    {
        arc_rect_array[0] = 3;
        fig_rect_array[0] = 3;
    }
    if ( rect_num == 4 )
    {
        arc_rect_array[0] = 4;
        fig_rect_array[0] = 4;
    }
    count_rects = 1;
    for( int i = 0; i <= count_holds; i++ )
    {
        if( index_array[i] != index )
        {
            if( shapeItems[index].n1 == shapeItems[index_array[i]].n1 )
            {
                index_array_temp[count_rects] = index_array[i];
                arc_rect_array[count_rects] = 0;
                fig_rect_array[count_rects] = 0;
                count_rects++;
            }
            if( shapeItems[index].n1 == shapeItems[index_array[i]].n2 )
            {
                index_array_temp[count_rects] = index_array[i];
                arc_rect_array[count_rects] = 0;
                fig_rect_array[count_rects] = 1;
                count_rects++;
            }
            if( shapeItems[index].n2 == shapeItems[index_array[i]].n1 )
            {
                index_array_temp[count_rects] = index_array[i];
                arc_rect_array[count_rects] = 1;
                fig_rect_array[count_rects] = 0;
                count_rects++;
            }
            if( shapeItems[index].n2 == shapeItems[index_array[i]].n2 )
            {
                index_array_temp[count_rects] = index_array[i];
                arc_rect_array[count_rects] = 1;
                fig_rect_array[count_rects] = 1;
                count_rects++;
            }
        }
    }
    index_array.clear();
    for ( int i = 0; i < count_rects; i++ )
        index_array.push_back(-1);
    for ( int i = 0; i < count_rects; i++ )
        index_array[i] = index_array_temp[i];
    index_array_temp.clear();
}

//- moving all connected figures -
void ShapeElFigure::moveAll( const QPointF &pos, QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems, WdgView *view )
{
    num_vector.clear();
    int rect_num_temp;
    bool flag_break;
    for ( int i = 0; i < count_Shapes; i++ )
    {
        count_moveItemTo += 1;
        flag_ctrl_move = false;
        flag_ctrl = true;
        itemInMotion = &shapeItems[index_array[i]];
        if ( flag_rect )
        {
            rect_num = rect_array[i];
            arc_rect = rect_array[0];
        }
        if ( flag_arc_rect_3_4 )
        {
            if ( i == 0 && !flag_down && !flag_up && !flag_right && !flag_left ) offset = pos - previousPosition_all;
            if ( i > 0 )
                if ( arc_rect_array[i] == 0 ) 
                    offset = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scale, flag_rotate ) - Prev_pos_1;
                else 
                    offset = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scale, flag_rotate ) - Prev_pos_2;
            rect_num = fig_rect_array[i];
            arc_rect = arc_rect_array[i];
        }
        index = index_array[i];
        moveItemTo( pos, shapeItems, pnts, view );
        if ( i == 0 && flag_arc_rect_3_4 )
        {
            Prev_pos_1 = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scale, flag_rotate );
            Prev_pos_2 = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scale, flag_rotate );
        }
    }
    if( inundationItems.size() )
    {
        for( int i = 0; i < inundationItems.size(); i++ )
            for(int j = 0; j < inundationItems[i].number_shape.size(); j++ )
            {
                flag_break = false;
                for(int k = 0; k < index_array.size(); k++ )
                    if( inundationItems[i].number_shape[j] == index_array[k] )
                    {
                        inundationPath = createInundationPath( inundationItems[i].number_shape, shapeItems, pnts, view);
                        inundationItems[i].path = inundationPath;
                        flag_break = true;
                        break;
                    }
                    if ( flag_break ) break;
            }
    }
}

//- Checking if appending of the point and deleting of the fill(inundation) is needed when several(choosen with one of the methods) figures are moving -
void ShapeElFigure::checkPoint_checkInundation( QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems )
{
    QPointF Temp;
    bool flag_in_n;
    bool fl_in_brk;
    bool flag_ravno;
    QVector<int> rem_inund;
    int figuresCount;
    if( index_array.size() > count_Shapes ) figuresCount = count_Shapes;
    else figuresCount = index_array.size();
    for( int i = 0; i < figuresCount; i++ )
    {
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
                    Temp = (*pnts)[shapeItems[index_array[i]].n1];
                    if( shapeItems[index_array[i]].n1 > 0 )
                        shapeItems[index_array[i]].n1 = appendPoint( Temp, shapeItems, pnts, 0 );
                    else if( shapeItems[index_array[i]].n1 <= -10 )
                        shapeItems[index_array[i]].n1 = appendPoint( Temp, shapeItems, pnts, 1 );
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
                    Temp = (*pnts)[shapeItems[index_array[i]].n2];
                    if( shapeItems[index_array[i]].n2 > 0 )
                        shapeItems[index_array[i]].n2 = appendPoint( Temp, shapeItems, pnts, 0 );
                    else if( shapeItems[index_array[i]].n2 <= -10 )
                        shapeItems[index_array[i]].n2 = appendPoint( Temp, shapeItems, pnts, 1 );
                }
            }
        }
        //-- Detecting the all inundations(fills) in which the mooving figures are present --
        for( int j = 0; j < inundationItems.size(); j++ )
            for( int k = 0; k < inundationItems[j].number_shape.size(); k++ )
        {
            fl_in_brk = false;
            if( index_array[i] == inundationItems[j].number_shape[k] )
            {
                if( rem_inund.size() )
                {
                    flag_ravno = false;
                    for( int p = 0; p < rem_inund.size(); p++ )
                        if( j == rem_inund[p] )
                        {
                            flag_ravno = true;
                            break;
                        }
                    if( !flag_ravno )
                    {
                        rem_inund.push_back(j);
                        fl_in_brk = true;
                        break;
                    }
                }
                else 
                {
                    rem_inund.push_back(j);
                    break;
                }
            }
            if( fl_in_brk ) break;
        }
    }
    bool flag_exist;
    int cnt;
    int tmp_inund;
    QVector<int> real_rem_inund;
    //-- Removing the fill(inundation) which has one or more figures that are not moving --
    for( int j = 0; j < rem_inund.size(); j++ ) 
    {
        tmp_inund = 0;
        for( int p = 0; p < real_rem_inund.size(); p++ )
            if( rem_inund[j] > real_rem_inund[p] ) tmp_inund++;

        for( int k = 0; k < inundationItems[rem_inund[j]-tmp_inund].number_shape.size(); k++ )
        {
            flag_exist = false;
            for( int i = 0; i < figuresCount; i++ )
            {
                if( inundationItems[rem_inund[j]-tmp_inund].number_shape[k] == index_array[i] )
                {
                    flag_exist = true;
                    break;
                }
            }
            if( flag_exist == false )
            {
                inundationItems.remove(rem_inund[j]-tmp_inund);
                real_rem_inund.push_back( rem_inund[j] );
                break;
            }
        }
    }
    rem_inund.clear();
    real_rem_inund.clear();
}



//-- detecting the figure or one of its control points under mouse cursor --
int ShapeElFigure::itemAt( const QPointF &pos, const QVector<ShapeItem> &shapeItems ,WdgView *w )
{
    QPointF point;
    rect_num = -1;
    bool flag_break = false;
    for (int j = 0; j <= rectItems.size()-1; j++) 
    {
        const RectItem &item1 = rectItems[j];
        if ( item1.path.contains(pos) ) rect_num = j;
    }
    index = -1;
    if ( rect_num != -1)
        for ( int i = 0; i <= shapeItems.size()-1; i++ )
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
    if ( rect_num == -1 )
    //for ( int i = 0; i < shapeItems.size(); i++ )
    for ( int i = shapeItems.size()-1; i > -1; i-- )  
    {
        const ShapeItem &item = shapeItems[i];
        if ( item.path.contains(pos) )
        {
            index = i;
            flag_break = true;
        }
        if ( flag_break ) break;
        for( int j = 2; j > 0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if ( item.path.contains(pos + point) ) 
            {
               index = i;
               flag_break = true;
            }
        }
        if ( flag_break ) break;
        for(int j = 2; j > 0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if ( item.path.contains(pos - point) ) 
            {
                index = i;
                flag_break = true;
            }
        }
        if ( flag_break ) break;
    }
   return index;
}

//- Building the path for the current figure -
QPainterPath ShapeElFigure::painterPath( float el_width, float el_border_width, int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6 )
{
    double arc_a_small, arc_b_small, t, t_start, t_end;
    QPointF  CtrlMotionPos_1_temp, CtrlMotionPos_2_temp, EndMotionPos_temp;
    QPainterPath circlePath;
    double arc_a, arc_b;
    circlePath = newPath;
    if( el_width < 1 ) el_width = 1;
    else el_width = TSYS::realRound(el_width);
    if( el_border_width < 1 ) el_border_width = 1;
    else el_border_width = TSYS::realRound(el_border_width);
    //-- if line --
    if( el_type == 1 )
    {
        el_border_width = el_border_width/2;
        circlePath.moveTo( TSYS::realRound( el_p1.x() + rotate( QPointF(-el_border_width, -(el_width/2+el_border_width) ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).y(), 2, true ) );
        circlePath.lineTo( TSYS::realRound( el_p1.x() + rotate( QPointF( length(el_p2,el_p1) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( length(el_p2,el_p1) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).y(), 2, true ) );
        circlePath.lineTo( TSYS::realRound( el_p1.x() + rotate( QPointF( length(el_p2,el_p1) + el_border_width, (el_width/2+el_border_width) ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( length(el_p2,el_p1) + el_border_width, (el_width/2+el_border_width) ), el_ang ).y(), 2, true ) );
        circlePath.lineTo( TSYS::realRound( el_p1.x() + rotate( QPointF( -el_border_width, (el_width/2+el_border_width) ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( -el_border_width, (el_width/2+el_border_width) ), el_ang ).y(), 2, true ) );
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    //-- if arc --
    if(el_type==2)
    {
        arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width/2;
        arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width/2;
        arc_a_small = arc_a - el_width - el_border_width;
        arc_b_small = arc_b - el_width - el_border_width;
        t_start = el_p6.x();
        t_end = el_p6.y();
        circlePath.moveTo( TSYS::realRound( el_p3.x() + rotate( arc( t_start, arc_a_small, arc_b_small ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p3.y() - rotate( arc( t_start, arc_a_small, arc_b_small ), el_ang ).y(), 2, true ) );
        circlePath.lineTo( TSYS::realRound( el_p3.x() + rotate( arc( t_start, arc_a, arc_b ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p3.y() - rotate( arc( t_start, arc_a, arc_b ), el_ang ).y(), 2, true ) );
        for ( t = t_start; t < t_end + 0.00277777777778; t += 0.00277777777778 ) 
            circlePath.lineTo( QPointF( TSYS::realRound( el_p3.x() + rotate( arc( t, arc_a, arc_b ), el_ang ).x(), 2, true ),
                                       TSYS::realRound( el_p3.y() - rotate( arc( t, arc_a, arc_b ), el_ang ).y(), 2, true ) ) );
        circlePath.lineTo( TSYS::realRound( el_p3.x() + rotate( arc( t_end, arc_a_small, arc_b_small ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p3.y() - rotate( arc( t_end, arc_a_small, arc_b_small ), el_ang ).y(), 2, true ) );
        for ( t = t_end; t > t_start; t -= 0.00277777777778 ) 
            circlePath.lineTo( QPointF( TSYS::realRound( el_p3.x() + rotate( arc( t, arc_a_small, arc_b_small ), el_ang ).x(), 2, true ),
                                       TSYS::realRound( el_p3.y() - rotate( arc( t, arc_a_small, arc_b_small ), el_ang ).y(), 2, true ) ) );
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    //-- if bezier --
    if(el_type == 3)
    {

        el_border_width = el_border_width/2;
        CtrlMotionPos_1_temp = unRotate( el_p3, el_ang, el_p1.x(), el_p1.y() );
        CtrlMotionPos_2_temp = unRotate( el_p4, el_ang, el_p1.x(), el_p1.y() );
        EndMotionPos_temp = QPointF( length( el_p2, el_p1) + el_border_width, 0 );
        circlePath.moveTo( TSYS::realRound( el_p1.x() + rotate( QPointF( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).y(), 2, true ) );
        circlePath.cubicTo( QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( CtrlMotionPos_1_temp.x(), CtrlMotionPos_1_temp.y() - (el_width/2 + el_border_width) ),el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( CtrlMotionPos_1_temp.x(), CtrlMotionPos_1_temp.y() - (el_width/2 + el_border_width) ), el_ang ).y(), 2, true ) ),
                           QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( CtrlMotionPos_2_temp.x(), CtrlMotionPos_2_temp.y() - (el_width/2 + el_border_width) ), el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( CtrlMotionPos_2_temp.x(), CtrlMotionPos_2_temp.y() - (el_width/2 + el_border_width) ), el_ang ).y(), 2, true ) ),
                           QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( EndMotionPos_temp.x(), EndMotionPos_temp.y() - (el_width/2 + el_border_width) ), el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( EndMotionPos_temp.x(), EndMotionPos_temp.y() - (el_width/2 + el_border_width) ),el_ang ).y(), 2, true ) ) );
        circlePath.lineTo( TSYS::realRound( el_p1.x() + rotate( QPointF( EndMotionPos_temp.x(), el_width/2 + el_border_width ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p1.y() - rotate( QPointF( EndMotionPos_temp.x(), el_width/2 + el_border_width ), el_ang ).y(), 2, true ) );
        circlePath.cubicTo( QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( CtrlMotionPos_2_temp.x(), CtrlMotionPos_2_temp.y() + el_width/2 + el_border_width ),el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( CtrlMotionPos_2_temp.x(), CtrlMotionPos_2_temp.y() + el_width/2 + el_border_width ), el_ang ).y(), 2, true ) ),
                           QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( CtrlMotionPos_1_temp.x(), CtrlMotionPos_1_temp.y() + el_width/2 + el_border_width ), el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( CtrlMotionPos_1_temp.x(), CtrlMotionPos_1_temp.y() + el_width/2 + el_border_width ), el_ang ).y(), 2, true ) ),
                           QPointF( TSYS::realRound( el_p1.x() + rotate( QPointF( -el_border_width, el_width/2 + el_border_width ), el_ang ).x(), 2, true ),
                                    TSYS::realRound( el_p1.y() - rotate( QPointF( -el_border_width, el_width/2 + el_border_width ), el_ang ).y(), 2, true ) ) );
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    return circlePath;
}

//- building path for the figure(if its border's width=0) -
QPainterPath ShapeElFigure::painterPathSimple( int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6 )
{
    QPainterPath circlePath;
    double t;
    circlePath = newPath;
    double arc_a, arc_b;
    if( el_type == 1 )
    {
        circlePath.moveTo( TSYS::realRound( el_p1.x(), 2, true ), TSYS::realRound( el_p1.y(), 2, true ) );
        circlePath.lineTo( TSYS::realRound( el_p2.x(), 2, true ), TSYS::realRound( el_p2.y(), 2, true ) );
    }
    if( el_type == 2 )
    {
        arc_a = length( el_p5, el_p3 );
        arc_b = length( el_p3, el_p4 );
        t_start = el_p6.x();
        t_end = el_p6.y();
        circlePath.moveTo( TSYS::realRound( el_p3.x() + rotate( arc( t_start, arc_a, arc_b ), el_ang ).x(), 2, true ),
                           TSYS::realRound( el_p3.y() - rotate( arc( t_start, arc_a, arc_b ), el_ang ).y(), 2, true ) );
        for ( t = t_start; t < t_end + 0.00277777777778; t += 0.00277777777778 ) 
            circlePath.lineTo( TSYS::realRound( el_p3.x() + rotate( arc( t, arc_a, arc_b ), el_ang ).x(), 2, true ),
                               TSYS::realRound( el_p3.y() - rotate( arc( t, arc_a, arc_b ), el_ang ).y(), 2, true ) );
    }
    if( el_type == 3 )
    {
        circlePath.moveTo( TSYS::realRound( el_p1.x(), 2, true ), TSYS::realRound( el_p1.y(), 2, true ) );
        circlePath.cubicTo( TSYS::realRound( el_p3.x(), 2, true ), TSYS::realRound( el_p3.y(), 2, true ), 
                            TSYS::realRound( el_p4.x(), 2, true ), TSYS::realRound( el_p4.y(), 2, true ), 
                            TSYS::realRound( el_p2.x(), 2, true ), TSYS::realRound( el_p2.y(), 2, true ) );
    }
    return circlePath;
}

//- adding the point to data model -
int ShapeElFigure::appendPoint( const QPointF &pos, const QVector<ShapeItem> &shapeItems, PntMap *pnts, bool flag_down )
{
    if( !flag_down )
    {
        int i = 1;
        while( (*pnts).find(i) != (*pnts).end() ) i++;
        (*pnts).insert( std::pair<int, QPointF> (i, pos) );
        return i;
    }
    else
    {
        int i = -10;
        while( (*pnts).find(i) != (*pnts).end() ) i--;
        (*pnts).insert( std::pair<int, QPointF> (i, pos) );
        return i;
    }
}

//- deleting the point from data model -
void ShapeElFigure::dropPoint( int num, int num_shape, const QVector<ShapeItem> &shapeItems, PntMap *pnts )
{
    bool equal = false;
    for( int i = 0; i < shapeItems.size(); i++ )
        if( i != num_shape && (num == shapeItems[i].n1 || num == shapeItems[i].n2 || num == shapeItems[i].n3 || num == shapeItems[i].n4
           || num == shapeItems[i].n5) )
        {
            equal = true;
            break;
        }
    if( !equal ) (*pnts).erase(num);
}
//- Building the contiguity matrix using start and end points of all figures -
int ShapeElFigure::buildMatrix( const QVector<ShapeItem> &shapeItems )
{
    int N;
    for ( int j = 0; j < 2*shapeItems.size()+1; j++ )
        vect.push_back(0);
    int j = 1;
    bool flag_vect_n1, flag_vect_n2;
    for (int i = 0; i < shapeItems.size(); i++ )
    {
        flag_vect_n1 = false;
        flag_vect_n2 = false;
        for ( int k = 1; k < j; k++ )
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
    for ( int v = 1; v < j; v++ )
        for( int i = 0; i < shapeItems.size(); i++ )
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
    if ( s == f && p > 4 ) 
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
            if ( map_matrix[s][c] && !incl[c]&& ( len == 0 || clen+map_matrix[s][c] <= len ) )
            {
                road[p] = c; incl[c] = 1; clen = clen + map_matrix[s][c];
                step ( c, f, p+1, vect, N );
                incl[c] = 0; road[p] = 0; clen = clen - map_matrix[s][c];
            }
    }
}

//- detecting the figures for fill -
bool ShapeElFigure::inundation( const QPointF &point, const QVector<ShapeItem> &shapeItems, PntMap *pnts, const QVector<int> &vect, int N, WdgView *view )
{
    found = false;
    inundationPath = newPath;
    bool flag_break = false;
    bool flag_push_back;
    QVector<int> work_sort;
    QVector<int> inundation_fig_num;
    for( int i = 0; i < 2*shapeItems.size()+1; i++ )
        work_sort.push_back(0);
    int i,j;
    i = 1;
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
        for ( int k = 0; k < 2*shapeItems.size()+1; k++ )
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
                    {
                        inundation_fig_num.push_back(j);
                        break;
                    }
            inundationPath = createInundationPath( inundation_fig_num, shapeItems, pnts, view );
            inundation_fig_num.clear();
            if ( inundationPath.contains(point) )
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
                if ( flag_push_back )
                    inundation_fig_num.push_back(j);
        }
    work_sort.clear();
    if( inundation_fig_num.size() > 0 )
    {
        inundationPath = createInundationPath( inundation_fig_num, shapeItems, pnts, view );
        inundation_fig_num = inundationSort( inundationPath, inundation_fig_num, shapeItems, pnts, view );
        if( flag_scale, flag_rotate )
            inundationPath = createInundationPath( inundation_fig_num, shapeItems, pnts, view );
        for( int i = 0; i < inundation_fig_num.size(); i++ )
            inundation_vector.push_back(0);
        inundation_vector = inundation_fig_num;
    }
    return true;
}

QVector<int> ShapeElFigure::inundationSort( const QPainterPath &inundationPath, QVector<int> &inundation_fig_num, const QVector<ShapeItem> &shapeItems, PntMap *pnts, WdgView *view )
{
    for( int j = 0; j < shapeItems.size(); j++ )
        for( int p = 0; p < inundation_fig_num.size(); p++ )
            if( (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n2) ||
                (shapeItems[inundation_fig_num[p]].n1 == shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2 == shapeItems[j].n1) )
            {
                if( shapeItems[j].type == 2  && p != j )
                    if( inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n4], view, flag_scale, flag_rotate ) ) )
                        inundation_fig_num[p] = j;
                if( shapeItems[j].type == 3 && p != j && shapeItems[inundation_fig_num[p]].type != 2 )
                    if( inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n4], view, flag_scale, flag_rotate ) ) && 
                        inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n3], view, flag_scale, flag_rotate ) ) )
                        inundation_fig_num[p] = j;
            }
    return inundation_fig_num;
}

//- detecting the figures, which count <=2, for filling -
bool ShapeElFigure::inundation1_2( const QPointF &point, const QVector<ShapeItem> &shapeItems, QVector<inundationItem> &inundationItems, PntMap *pnts, WdgView *view, int number )
{
    ElFigDt *elFD = (ElFigDt*)view->shpData;
    QPainterPath inundationPath_1_2;
    QVector<int> in_fig_num;
    bool flag_break, fl_brk;
    flag_break = false;
    fl_brk = false;
    QBrush fill_brush( QColor(0,0,0,0), Qt::SolidPattern ), fill_img_brush;
    fill_brush.setColor( elFD->fillClr );
    fill_img_brush = elFD->fillImg;
    for( int i = 0; i < shapeItems.size(); i++ )
    {
        if( shapeItems[i].type == 2 )
            if( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scale, flag_rotate).toPoint()==scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scale, flag_rotate).toPoint() )
            {
                if( in_fig_num.size() )
                    in_fig_num.clear();
                in_fig_num.push_back(i);
                inundationPath_1_2 = newPath;
                inundationPath_1_2 = createInundationPath( in_fig_num, shapeItems, pnts, view );
                if( inundationPath_1_2.contains(point) )
                {
                    for( int i = 0; i < inundationItems.size(); i++ )
                        if( inundationItems[i].path == inundationPath_1_2 )
                        {
                            inundationItems.remove(i);
                            fl_brk = true;
                            break;
                        }
                    if( !fl_brk )
                    {
                        if( number == -1 && status_hold )
                            inundationItems.push_back( inundationItem( inundationPath_1_2, fill_brush, fill_img_brush, in_fig_num, in_fig_num ) );
                        else
                        {
                            if( !flag_scale && !flag_rotate )
                            {
                                inundation_vector = in_fig_num;
                                inundationPath = inundationPath_1_2;
                            }
                            else
                            {
                                inundationItems[number].path = inundationPath_1_2;
                                inundationItems[number].number_shape = in_fig_num;
                            }
                        }
                        flag_break = true;
                    }
                }
            }
        if( flag_break )
            return true;
        if( shapeItems[i].type == 2 || shapeItems[i].type == 3 )
        {
            for( int j = 0; j < shapeItems.size(); j++ )
            {
                inundationPath_1_2 = newPath;
                if( in_fig_num.size() )
                    in_fig_num.clear();
                if( i != j )
                    if( (shapeItems[i].n1 == shapeItems[j].n1 && shapeItems[i].n2 == shapeItems[j].n2) ||
                        (shapeItems[i].n1 == shapeItems[j].n2 && shapeItems[i].n2 == shapeItems[j].n1) )
                    {
                        in_fig_num.push_back(i);
                        in_fig_num.push_back(j);
                        inundationPath_1_2 = createInundationPath( in_fig_num, shapeItems, pnts, view );
                        if( inundationPath_1_2.contains(point) )
                        {
                            for( int i = 0; i < inundationItems.size(); i++ )
                                if ( inundationItems[i].path == inundationPath_1_2 )
                                {
                                    inundationItems.remove(i);
                                    fl_brk = true;
                                    break;
                                }
                            if( !fl_brk )
                            {
                                if( number == -1  && status_hold )
                                    inundationItems.push_back( inundationItem( inundationPath_1_2, fill_brush, fill_img_brush, in_fig_num, in_fig_num ) );
                                else
                                {
                                    if( !flag_scale && !flag_rotate )
                                    {
                                        inundation_vector = in_fig_num;
                                        inundationPath = inundationPath_1_2;
                                    }
                                    else
                                    {
                                        inundationItems[number].path = inundationPath_1_2;
                                        inundationItems[number].number_shape = in_fig_num;
                                    }
                                }
                                flag_break = true;
                            }
                        }
                    }
                if( flag_break )
                    return true;
            }
        }
    }
    return false;
}

QPointF ShapeElFigure::scaleRotate( const QPointF &point, WdgView *view, bool flag_scale, bool flag_rotate )
{
    ElFigDt *elFD = (ElFigDt*)view->shpData;
    QPointF rpnt = point;
    if( flag_rotate )
    {
        QPointF center = QPointF( (view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true)) ).toPoint();
        rpnt = rpnt - center;
        rpnt = rotate( rpnt, elFD->orient );
        rpnt = rpnt + center;
    }
    if( flag_scale ) rpnt = QPointF( rpnt.x()*view->xScale(true), rpnt.y()*view->yScale(true) );
    return rpnt;
}

QPointF ShapeElFigure::unScaleRotate( const QPointF &point, WdgView *view, bool flag_scale, bool flag_rotate )
{
    ElFigDt *elFD = (ElFigDt*)view->shpData;
    QPointF rpnt = point;
    if(flag_scale) rpnt = QPointF( rpnt.x()/view->xScale(true), rpnt.y()/view->yScale(true) );
    if( flag_rotate )
    {
        QPointF center= QPointF( (view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true)) ).toPoint();
        rpnt = rpnt - center;
        rpnt = rotate( rpnt, 360 - elFD->orient );
        rpnt = rpnt + center;
    }
    return rpnt;
}

//- Building the path for fill -
QPainterPath ShapeElFigure::createInundationPath( const QVector<int> &in_fig_num, const QVector<ShapeItem> &shapeItems, PntMap *pnts, WdgView *view )
{
    QPainterPath path;
    int flag, in_index;
    bool flag_remove = false;
    bool flag_n1, flag_n2, flag_break;
    double arc_a,arc_b,t_start,t_end,t,ang;
    QLineF line1,line2;
    path = newPath;
    if( in_fig_num.size() >= 2 )
    {
    if( shapeItems[in_fig_num[0]].n1 < shapeItems[in_fig_num[0]].n2 )
    {
        path.moveTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).x(), 2, true ),
                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).y(), 2, true ) );
        switch( shapeItems[in_fig_num[0]].type )
        {
            case 1:
                path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).x() ),
                             TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).y(), 2, true ) );
                break;
            case 2:
                if( (flag_angle_temp &&  view->xScale(true) == 1 && view->yScale(true) == 1) || (flag_scale && !flag_rotate) )
                {
                    line2 = QLineF( (*pnts)[shapeItems[in_fig_num[0]].n3].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].x()+10,
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y() );
                    line1 = QLineF( (*pnts)[shapeItems[in_fig_num[0]].n3].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n5].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n5].y() );
                    if ( (*pnts)[shapeItems[in_fig_num[0]].n5].y() <= (*pnts)[shapeItems[in_fig_num[0]].n3].y() ) 
                        ang = angle( line1, line2 );
                    else ang = 360 - angle( line1, line2 );
                }
                else ang = shapeItems[in_fig_num[0]].angle_temp;
                
                if( !flag_scale && !flag_rotate )
                {
                    arc_a = length( (*pnts)[shapeItems[in_fig_num[0]].n3], (*pnts)[shapeItems[in_fig_num[0]].n5] );
                    arc_b = length( (*pnts)[shapeItems[in_fig_num[0]].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].x(), 2 ),
                                   TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].y(), 2 ) ) );
                }
                else
                {
                    arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scale, flag_rotate ).y() ) );
                    arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).y() ) );
                }
                t_start = shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end = shapeItems[in_fig_num[0]].ctrlPos4.y();
                for ( t = t_start; t < t_end+0.00277777777778; t += 0.00277777777778) 
                    path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                 TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                break;
            case 3:
                path.cubicTo( TSYS::realRound( scaleRotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).y(), 2, true ) );
    
                break;
        }
        flag_n2 = true;
        flag_n1 = false;
    }
    else
    {
        path.moveTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).x(), 2, true ),
                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scale, flag_rotate ).y(), 2, true ) );
        switch( shapeItems[in_fig_num[0]].type )
        {
            case 1:
                path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).x(), 2, true ),
                             TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).y(), 2, true ) );
                break;
            case 2:
                if( (flag_angle_temp &&  view->xScale(true) == 1 && view->yScale(true) == 1) || ( flag_scale && !flag_rotate ) )
                {
                    line2 = QLineF( (*pnts)[shapeItems[in_fig_num[0]].n3].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].x()+10,
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y() );
                    line1 = QLineF( (*pnts)[shapeItems[in_fig_num[0]].n3].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n3].y(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n5].x(),
                                    (*pnts)[shapeItems[in_fig_num[0]].n5].y() );
                    if ( (*pnts)[shapeItems[in_fig_num[0]].n5].y() <= (*pnts)[shapeItems[in_fig_num[0]].n3].y() ) 
                        ang = angle( line1, line2 );
                    else ang = 360 - angle( line1, line2 );
                }
                else ang = shapeItems[in_fig_num[0]].angle_temp;
                if( !flag_scale && !flag_rotate )
                {
                    arc_a = length( (*pnts)[shapeItems[in_fig_num[0]].n3], (*pnts)[shapeItems[in_fig_num[0]].n5] );
                    arc_b = length( (*pnts)[shapeItems[in_fig_num[0]].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].x(), 2 ),
                                   TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].y(), 2 ) ) );
                }
                else
                {
                    arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scale, flag_rotate ).y() ) );
                    arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).y() ) );
                }

                t_start = shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end = shapeItems[in_fig_num[0]].ctrlPos4.y();
                for ( t = t_end; t > t_start-0.00277777777778; t -= 0.00277777777778) 
                    path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                 TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                break;
            case 3:
                path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scale, flag_rotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scale, flag_rotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scale, flag_rotate ).y(), 2, true ) );
                break;
        }
        flag_n2 = false;
        flag_n1 = true;
    }
    int k = 0;
    for( int i = 0; i < in_fig_num.size()-1; i++ )
    {
        if( flag_n2 )
        {
            flag_break = false;
            for ( int j = 0; j < in_fig_num.size(); j++ )
            {
                if( (k != j) && ( shapeItems[in_fig_num[k]].n2 == shapeItems[in_fig_num[j]].n1 ) )
                {
                    flag = 1;
                    in_index = in_fig_num[j];
                    k = j;
                    flag_break = true;
                }
                if ( flag_break ) break;
                if( (k != j) && ( shapeItems[in_fig_num[k]].n2 == shapeItems[in_fig_num[j]].n2 ) )
                {
                    flag = 2;
                    in_index = in_fig_num[j];
                    k = j;
                    flag_break = true;
                }
                if ( flag_break ) break;
            }
        }
        if( flag_n1 )
        {
            flag_break = false;
            for ( int j = 0; j < in_fig_num.size(); j++ )
            {
                if( (k != j) && ( shapeItems[in_fig_num[k]].n1 == shapeItems[in_fig_num[j]].n1 ) )
                {
                    flag = 1;
                    in_index = in_fig_num[j];
                    k = j;
                    flag_break = true;
                }
                if ( flag_break ) break;
                if( (k != j) && ( shapeItems[in_fig_num[k]].n1 == shapeItems[in_fig_num[j]].n2 ) )
                {
                    flag = 2;
                    in_index = in_fig_num[j];
                    k = j;
                    flag_break = true;
                }
                if ( flag_break ) break;
            }
        }
        switch( flag )
        {
            case 1:
                switch( shapeItems[in_index].type )
                {
                    case 1:
                        path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scale, flag_rotate ).x(), 2, true ),
                                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scale, flag_rotate ).y(), 2, true ) );
                        break;
                    case 2:
                        if( (flag_angle_temp &&  view->xScale(true) == 1 && view->yScale(true) == 1) || ( flag_scale && !flag_rotate ) )
                        {
                            line2 = QLineF( (*pnts)[shapeItems[in_index].n3].x(),
                                            (*pnts)[shapeItems[in_index].n3].y(),
                                            (*pnts)[shapeItems[in_index].n3].x()+10,  
                                            (*pnts)[shapeItems[in_index].n3].y() );
                            line1 = QLineF( (*pnts)[shapeItems[in_index].n3].x(),
                                            (*pnts)[shapeItems[in_index].n3].y(),
                                            (*pnts)[shapeItems[in_index].n5].x(),
                                            (*pnts)[shapeItems[in_index].n5].y() );
                            if ( (*pnts)[shapeItems[in_index].n5].y() <= (*pnts)[shapeItems[in_index].n3].y() ) 
                                ang = angle( line1, line2 );
                            else ang = 360 - angle( line1, line2 );
                        }
                        else ang = shapeItems[in_index].angle_temp;
                        if( !flag_scale && !flag_rotate )
                        {
                            arc_a = length( (*pnts)[shapeItems[in_index].n3], (*pnts)[shapeItems[in_index].n5] );
                            arc_b = length( (*pnts)[shapeItems[in_index].n3], QPointF(TSYS::realRound( (*pnts)[shapeItems[in_index].n4].x(), 2 ),
                                           TSYS::realRound( (*pnts)[shapeItems[in_index].n4].y(), 2 ) ) );
                        }
                        else
                        {
                            arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scale, flag_rotate ).y() ) );
                            arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).y() ) );
                        }

                        t_start = shapeItems[in_index].ctrlPos4.x();
                        t_end = shapeItems[in_index].ctrlPos4.y();
                        for ( t = t_start; t < t_end+0.00277777777778; t += 0.00277777777778 ) 
                            path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                         TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                        break;
                
                    case 3:
                        path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scale, flag_rotate ).y(), 2, true ) );
                        break;
                }
                flag_n2 = true;
                flag_n1 = false;
                break;
            case 2:
                switch( shapeItems[in_index].type )
                {
                    case 1:
                        path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scale, flag_rotate ).x(), 2, true ),
                                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scale, flag_rotate ).y(), 2, true ) );
                        break;
                    case 2:
                        if( (flag_angle_temp &&  view->xScale(true) == 1 && view->yScale(true) == 1) || ( flag_scale && !flag_rotate ) )
                        {
                            line2 = QLineF( (*pnts)[shapeItems[in_index].n3].x(),
                                            (*pnts)[shapeItems[in_index].n3].y(),
                                            (*pnts)[shapeItems[in_index].n3].x()+10,
                                            (*pnts)[shapeItems[in_index].n3].y() );
                            line1 = QLineF( (*pnts)[shapeItems[in_index].n3].x(),
                                            (*pnts)[shapeItems[in_index].n3].y(),
                                            (*pnts)[shapeItems[in_index].n5].x(),
                                            (*pnts)[shapeItems[in_index].n5].y() );
                            if ( (*pnts)[shapeItems[in_index].n5].y() <= (*pnts)[shapeItems[in_index].n3].y() ) 
                                ang = angle( line1, line2 );
                            else ang = 360 - angle( line1, line2 );
                        }
                        else ang = shapeItems[in_index].angle_temp;
                        if( !flag_scale && !flag_rotate )
                        {
                            arc_a = length( (*pnts)[shapeItems[in_index].n3], (*pnts)[shapeItems[in_index].n5] );
                            arc_b = length( (*pnts)[shapeItems[in_index].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_index].n4].x(),2),
                                           TSYS::realRound((*pnts)[shapeItems[in_index].n4].y(), 2 ) ) );
                        }
                        else
                        {
                            arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scale, flag_rotate).y() ) );
                            arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).y() ) );
                        }

                        t_start = shapeItems[in_index].ctrlPos4.x();
                        t_end = shapeItems[in_index].ctrlPos4.y();
                        for ( t = t_end; t > t_start-0.00277777777778; t -= 0.00277777777778 ) 
                            path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                         TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                        break;
                    case 3:
                        path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scale, flag_rotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scale, flag_rotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scale, flag_rotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scale, flag_rotate ).y(), 2, true ) );

                        break;
                }
                flag_n2 = false;
                flag_n1 = true;
                break;
        }
    }
    return path;
    }
    else return newPath;
}

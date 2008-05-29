//OpenSCADA system module UI.VISION file: vis_shape_elfig.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Lysenko Maxim (mlisenko@ukr.net)
 *   			by Yashina Kseniya (sobacurka@ukr.net) 
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
    status_hold(false), flag_rect(false), flag_hold_move(false), flag_m(false), flag_scaleRotate(true), flag_hold_arc(false), flag_angle_temp(false), 
    flag_arc_rect_3_4(false), flag_first_move(false), flag_hold_checked(false), current_ss(-1), current_se(-1), current_es(-1), current_ee(-1), 
    count_Shapes(0), count_holds(0), count_rects(0), rect_num_arc(-1), rect_num(-1), index_del(-1)
{
    newPath.addEllipse( QRect(0,0,0,0) );
}

void ShapeElFigure::init( WdgView *w )
{
    QVector<ShapeItem> *shapeItems = new QVector<ShapeItem>();
    w->dc()["shapeItems"].setValue( (void*)shapeItems );
    QVector<inundationItem> *inundationItems = new QVector<inundationItem>();
    w->dc()["inundationItems"].setValue( (void*)inundationItems );
    PntMap *pnts = new PntMap;
    w->dc()["shapePnts"].setValue( (void*)pnts );
}

void ShapeElFigure::destroy( WdgView *w )
{
    delete (QVector<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    delete (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    delete (QVector<inundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    rectItems.clear();
}

bool ShapeElFigure::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);
    bool rel_list = false;					//change signal
    bool up 	  = false;
    status	  = false;
    QVector<ShapeItem> &shapeItems = *(QVector<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    QVector<inundationItem> &inundationItems = *(QVector<inundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    QLineF line1, line2;
    double ang;
    QPointF StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4;
    double t_start, t_end, a, b, ang_t;
    float MotionWidth;
    QPainterPath circlePath;
    
    switch( uiPrmPos )
    {
	case -1:	//load
	    rel_list = true;
	    break;
        case 5:		//en
            if( !runW )	break;
	    w->dc()["en"] = (bool)atoi(val.c_str());
    	    w->setVisible(atoi(val.c_str()));
            break;
        case 6:		//active
            if( !runW )	break;    
	    w->dc()["active"] = (bool)atoi(val.c_str());
	    w->setFocusPolicy( (bool)atoi(val.c_str()) ? Qt::TabFocus : Qt::NoFocus );
            break;
        case 12:	//geomMargin
            w->dc()["geomMargin"] = atoi(val.c_str());
            up=true;
            break;
        case 20:	//lineWdth
            w->dc()["lineWdth"] = atof(val.c_str());
            rel_list = true;
            break;
        case 21:	//lineClr
            w->dc()["lineClr"] = QColor(val.c_str());
            rel_list = true;
            break;
        case 22:	//lineDecor
            w->dc()["lineDecor"] = atoi(val.c_str());
            rel_list = true;
            break;
        case 23:	//bordWdth
            w->dc()["bordWdth"] = atoi(val.c_str());
            rel_list = true;
            break;
        case 24:	//bordClr
            w->dc()["bordClr"] = QColor(val.c_str());
            rel_list = true;
            break;
        case 25:	//fillClr
            w->dc()["fillClr"] = QColor(val.c_str());
	    rel_list = true;
            break;
        case 26:	//fillImg
        {
	    QImage img;
	    string backimg = w->resGet(val);
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		w->dc()["fillImg"] = QBrush(img);
	    else w->dc()["fillImg"] = QBrush();
	    rel_list = true;
	    break;
        }
        case 28:
        {
            w->dc()["orient"] = atof(val.c_str());
            rel_list = true;
            break;   
        }
        case 27:	//elLst
            w->dc()["elLst"] = val.c_str();
            rel_list = true;
            break;
        case 29:
            w->dc()["lineStyle"] = val.c_str();
            rel_list = true;
            break;
        default:
            if( uiPrmPos >= 30 )
            {
                int pnt  = (uiPrmPos-30)/2;
                int patr = (uiPrmPos-30)%2;
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
        QVector<ShapeItem> shapeItems_temp;
        if(shapeItems.size())
        {
            shapeItems_temp = shapeItems;
            shapeItems.clear();
        }
        if( inundationItems.size() ) inundationItems.clear();
        
	//- Parse last attributes list and make point list -
        string sel, ln_st;
        Qt::PenStyle style;
        int p[5];
        int width;
        int bord_width;
        QPointF ip[5];
        for( int off = 0; (sel=TSYS::strSepParse(w->dc()["elLst"].toString().toAscii().data(),0,'\n',&off)).size(); )
        {
            int el_off = 0;
            string el = TSYS::strSepParse(sel,0,':',&el_off);
            if( el == "line" )
            {
                //-- Reading anf setting attributes for the current line --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = w->dc()["lineWdth"].toInt() ;
                QColor color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !color.isValid() ) color = w->dc()["lineClr"].value<QColor>();
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = w->dc()["bordWdth"].toInt();
                QColor bord_color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_color.isValid() )  bord_color = w->dc()["bordClr"].value<QColor>();
                ln_st = TSYS::strSepParse(sel,0,':',&el_off);
                if( !ln_st.size() ) ln_st = w->dc()["lineStyle"].toString().toAscii().data();
                if( ln_st == "solid" ) style = Qt::SolidLine;
                else if( ln_st == "dashed" ) style = Qt::DashLine;
                else if( ln_st == "dotted" ) style = Qt::DotLine;
                else style = Qt::SolidLine;

                //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 2; i_p++ )
                    ip[i_p] = scaleRotate( (*pnts)[p[i_p]], w, flag_scaleRotate );
                
		//-- Detecting the rotation angle of the line --
                line2 = QLineF( ip[0], QPointF(ip[0].x()+10,ip[0].y()) );
                line1 = QLineF( ip[0], ip[1] );
                if( ip[0].y() <= ip[1].y() ) ang = 360 - angle( line1, line2 );
                else ang = angle( line1, line2 );
               
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
                                        QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::SquareCap, Qt::RoundJoin ),
                                        QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin ), width, w->dc()["bordWdth"].toInt(), 1, angle_temp );
                    else
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], -1, -1, -1, QPointF(0,0),
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin ),
                                        QPen( color, width, style, Qt::FlatCap, Qt::RoundJoin ), width, w->dc()["bordWdth"].toInt(), 1, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );
                }
            }
            if( el == "arc" )
            {
                //-- Reading anf setting attributes for the current arc --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[2]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[3]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[4]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = w->dc()["lineWdth"].toInt() ;
                QColor color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !color.isValid() ) color = w->dc()["lineClr"].value<QColor>();
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = w->dc()["bordWdth"].toInt();
                QColor bord_color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !bord_color.isValid() ) bord_color = w->dc()["bordClr"].value<QColor>();
                ln_st=TSYS::strSepParse(sel,0,':',&el_off).c_str();
                if( ln_st.size() == 0 ) ln_st=w->dc()["lineStyle"].toString().toAscii().data();
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
                StartMotionPos = scaleRotate( QPointF( CtrlMotionPos_1.x()+rotate(arc(t_start,a,b),ang).x(),
                                       		       CtrlMotionPos_1.y()-rotate(arc(t_start,a,b),ang).y() ),
					      w, flag_scaleRotate );
                EndMotionPos = scaleRotate( QPointF( CtrlMotionPos_1.x()+rotate(arc(t_end,a,b),ang).x(),
                                     		     CtrlMotionPos_1.y()-rotate(arc(t_end,a,b),ang).y()),
					    w, flag_scaleRotate );
                CtrlMotionPos_1 = scaleRotate( CtrlMotionPos_1, w, flag_scaleRotate );
                CtrlMotionPos_2 = scaleRotate( CtrlMotionPos_2, w, flag_scaleRotate );
                CtrlMotionPos_3 = scaleRotate( CtrlMotionPos_3, w, flag_scaleRotate );
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
                                            QPen( w->dc()["bordClr"].value<QColor>(), bord_width, Qt::NoPen, Qt::SquareCap, Qt::RoundJoin ),
                                            QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin ), width, bord_width, 2, angle_temp );
                    else
                        item_temp = ShapeItem ( bigPath, circlePath, p[0], p[1], p[2], p[3], p[4], CtrlMotionPos_4, 
                                         QBrush( color,Qt::NoBrush ),
                                         QPen( w->dc()["bordClr"].value<QColor>(), bord_width, Qt::NoPen, Qt::FlatCap, Qt::RoundJoin ),
                                         QPen( color, width, style, Qt::FlatCap, Qt::RoundJoin ), width, bord_width, 2, angle_temp );
                    item_temp.brush.setColor( color );
                    shapeItems.push_back( item_temp );  
                }
            }
            if( el == "bezier" )
            {
                //-- Reading anf setting attributes for the current arc --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[2]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[3]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width = w->dc()["lineWdth"].toInt() ;
                QColor color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !color.isValid() ) color = w->dc()["lineClr"].value<QColor>();
                bord_width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !bord_width ) bord_width = w->dc()["bordWdth"].toInt();
                QColor bord_color( TSYS::strSepParse(sel,0,':',&el_off).c_str() );
                if( !bord_color.isValid() ) bord_color = w->dc()["bordClr"].value<QColor>();
                ln_st=TSYS::strSepParse(sel,0,':',&el_off).c_str();
                if( ln_st.size() == 0 ) ln_st=w->dc()["lineStyle"].toString().toAscii().data();
                if( ln_st == "solid" ) style = Qt::SolidLine;
                else if( ln_st == "dashed" ) style = Qt::DashLine;
                else if( ln_st == "dotted" ) style = Qt::DotLine;
                else style = Qt::SolidLine;
                for( int i_p = 0; i_p < 4; i_p++ )
                    ip[i_p] = scaleRotate( (*pnts)[p[i_p]], w, flag_scaleRotate );
                line2 = QLineF( ip[0], QPointF(ip[0].x()+10,ip[0].y()) );
                line1 = QLineF( ip[0], ip[1] );
                if( ip[0].y() <= ip[1].y() )	ang = 360-angle(line1,line2);
                else                   		ang = angle(line1,line2);
                
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
                                        QPen( w->dc()["bordClr"].value<QColor>(), bord_width, Qt::NoPen, Qt::SquareCap, Qt::MiterJoin ),
                                        QPen( color, width, style, Qt::SquareCap, Qt::RoundJoin), width, bord_width, 3, angle_temp );
                    else
                        item_temp = ShapeItem( bigPath, circlePath, p[0], p[1], p[2], p[3], -1, QPointF(0,0), 
                                        QBrush( color, Qt::NoBrush ),
                                        QPen( w->dc()["bordClr"].value<QColor>(), bord_width, Qt::NoPen, Qt::FlatCap, Qt::MiterJoin ),
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
                while( true )
                {
                    string svl = TSYS::strSepParse( sel, 0, ':', &el_off );
                    int vl = atoi(svl.c_str());
                    if( vl ) fl_pnts.push_back(vl);
                    else if( zero_pnts == 0 ) { fl_color = svl; zero_pnts++; }
                    else if( zero_pnts == 1 ) { fl_img = svl; zero_pnts++; }
                    else break;
                }
                //- Check fill color -
                QColor color( fl_color.c_str() );
                color.setAlpha( 255 );
                if( !color.isValid() )	color = w->dc()["fillClr"].value<QColor>();
                //- Check fill image -
		QBrush brsh;
		QImage img;
		string backimg = w->resGet(fl_img);
		if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		    brsh.setTextureImage(img);
                //- Make elements -
                if( fl_pnts.size() > 1 ) inundationItems.push_back(inundationItem(newPath,color,brsh, fl_pnts));
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
        shapeItems_temp.clear();
        up = true;
    }	
    if( up && !w->allAttrLoad( ) )
    {
        //-Repainting all shapes by calling moveItemTo to each shape-
        QVector<int> inundation_fig_num;
        bool flag_push_back, fl_brk;
        for( int i=0; i < shapeItems.size(); i++ )
            if( shapeItems[i].type == 2 )
            {
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
                        if( (shapeItems[j].n1 == inundationItems[i].number_shape[0] && shapeItems[j].n2 == inundationItems[i].number_shape[1]) ||
                            	(shapeItems[j].n1 == inundationItems[i].number_shape[1] && shapeItems[j].n2 == inundationItems[i].number_shape[0]) )
                            inundation_fig_num.push_back(j);
                        if( inundation_fig_num.size() == 2 ) break;
                    }
                    inundationItems[i].path = createInundationPath( inundation_fig_num, shapeItems, pnts, w );
                    inundationItems[i].number_shape = inundation_fig_num;
                }
                inundation_fig_num.clear();
            }
        itemInMotion = 0;
        index        = -1;
        if( rectItems.size() )	rectItems.clear();
        flag_ctrl    = false;
        if( uiPrmPos != -1 ) w->update();
    }    
    return up;
}

//-Saving shapes' attributes to data model-
bool ShapeElFigure::shapeSave( WdgView *w )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    QVector<ShapeItem> &shapeItems = *(QVector<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    QVector<inundationItem> &inundationItems = *(QVector<inundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    string elList, ln_st;
    Qt::PenStyle style;

    //- Building attributes for all el_figures and fills -
    //--for el_figures--
    for( int i_s = 0; i_s < shapeItems.size(); i_s++ )
	switch( shapeItems[i_s].type )
	{
	    case 1:
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else
                    style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
	      	elList+="line:"+TSYS::int2str(shapeItems[i_s].n1)+":"+
		     TSYS::int2str(shapeItems[i_s].n2)+":"+
		     ((shapeItems[i_s].width == w->dc()["lineWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
		     ((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
		     ((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
		     ((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"]) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                     ((ln_st == w->dc()["lineStyle"].toString().toAscii().data()) ? "" : ln_st)+"\n";

		break;
	    case 2:
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
		elList+="arc:"+TSYS::int2str(shapeItems[i_s].n1)+":"+
		    TSYS::int2str(shapeItems[i_s].n2)+":"+
	     	    TSYS::int2str(shapeItems[i_s].n3)+":"+
		    TSYS::int2str(shapeItems[i_s].n4)+":"+
	    	    TSYS::int2str(shapeItems[i_s].n5)+":"+
		    ((shapeItems[i_s].width == w->dc()["lineWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
		    ((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
		    ((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
                    ((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"]) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                    ((ln_st == w->dc()["lineStyle"].toString().toAscii().data()) ? "" : ln_st)+"\n";

		break;
	    case 3:
                if( shapeItems[i_s].border_width > 0 ) style = shapeItems[i_s].pen.style();
                else style = shapeItems[i_s].penSimple.style();
                if( style == Qt::SolidLine ) ln_st = "solid";
                else if( style == Qt::DashLine ) ln_st = "dashed";
                else if( style == Qt::DotLine ) ln_st = "dotted";
                elList+="bezier:"+TSYS::int2str(shapeItems[i_s].n1)+":"+
		    TSYS::int2str(shapeItems[i_s].n2)+":"+
		    TSYS::int2str(shapeItems[i_s].n3)+":"+
		    TSYS::int2str(shapeItems[i_s].n4)+":"+
		    ((shapeItems[i_s].width == w->dc()["lineWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].width))+":"+
		    ((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString()) ? "" : shapeItems[i_s].brush.color().name().toAscii().data())+":"+
		    ((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt()) ? "" : TSYS::int2str((int)shapeItems[i_s].border_width))+":"+
                    ((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"]) ? "" : shapeItems[i_s].pen.color().name().toAscii().data())+":"+
                    ((ln_st == w->dc()["lineStyle"].toString().toAscii().data()) ? "" : ln_st)+"\n";

		break;
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
		    elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":"+
		    	      TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
		else
		    elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":"+
		   	      TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
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
		    elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2) + ":";
		    temp.push_back( shapeItems[inundationItems[i].number_shape[k]].n2 );
		}
		if( flag_n2 && temp.size()<inundationItems[i].number_shape.size() )
		{
		    elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1) + ":";
		    temp.push_back(shapeItems[inundationItems[i].number_shape[k]].n1);
		}
	    }
	}
	if( inundationItems[i].number_shape.size() <= 2 )
	    elList += TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n1)+":"+
		      TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n2)+":";
	elList = elList + ((inundationItems[i].brush.color().name() == w->dc()["fillClr"].value<QColor>().name()) ? "" : inundationItems[i].brush.color().name().toAscii().data()) + "\n";
    }
    w->attrSet( "elLst", elList );

    //- Write shapes points to data model -
    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
    {
        w->attrSet( "p"+TSYS::int2str(pi.key())+"x", TSYS::real2str(TSYS::realRound(pi.value().x(),2)) );
        w->attrSet( "p"+TSYS::int2str(pi.key())+"y", TSYS::real2str(TSYS::realRound(pi.value().y(),2)) );
    }
    devW->setSelect(true);
}

void ShapeElFigure::editEnter( WdgView *view )
{    
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(true);
    
    connect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    //- Init actions' address -
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(true);
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText(TSYS::addr2str(view).c_str());
    }
}

void ShapeElFigure::editExit( WdgView *view )
{
    disconnect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)), this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(false);
    //- Clear action;s address -
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText("");
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(false);
    }
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
	if( ( (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->objectName() == "arc") &&
		  (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->isChecked()) ) || 
		( (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->objectName() == "line") &&
		  (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->isChecked()) ) ||
		( (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->objectName() == "besier") &&
		  (((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->isChecked()) ) )
	    ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setChecked(false);
        if( ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->objectName() == "cursor" )
            ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setChecked(true);
    }
    shapeSave(view);
    view->unsetCursor();
    status = false;
    flag_A = false;
    flag_ctrl = false;
    rectItems.clear();
}

void ShapeElFigure::wdgPopup( WdgView *w, QMenu &menu )
{
    if( qobject_cast<DevelWdgView*>(w) )
    {
	for( int i_a = 0; i_a < ((VisDevelop *)w->mainWin())->elFigTool->actions().size(); i_a++ )
	    menu.addAction(((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a));
	menu.addSeparator();
    }
}
	
void ShapeElFigure::toolAct( QAction *act )
{
    bool ptr_line,ptr_arc,ptr_bezier;
    WdgView *w= (WdgView*)TSYS::str2addr(act->iconText().toAscii().data());
    if( act->objectName() != "hold" )
        for( int i_a = 0; i_a < ((VisDevelop *)w->mainWin())->elFigTool->actions().size(); i_a++ )
        {
            ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->setCheckable(true);
            if( ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->objectName() != "hold" )
                ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->setChecked(false);
        }
    if( act->objectName() == "line" )
    {
        act->setChecked(true);
        shapeType = 1;
        status = true;
    }
    if( act->objectName() == "arc" )
    {
        act->setChecked(true);
        shapeType = 2;
        status = true;
    }
    if( act->objectName() == "besier" )
    {
        act->setChecked(true);
        shapeType = 3;
        status = true;
    }
    if( act->objectName() == "hold" )
    {        
        if( flag_hold_checked )
        {
            act->setChecked(false);
            flag_hold_checked = false;
        }
        else
        {
            act->setCheckable(true);
            act->setChecked(true);
            flag_hold_checked = true;
        }
        status_hold = act->isChecked();
        rectItems.clear();
        w->update();
    }
    if( act->objectName() == "cursor" )
    {
        act->setChecked(true);
        w->unsetCursor();
        status = false;
        w->update();
    }
}

bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    QVector<ShapeItem> &shapeItems = *(QVector<ShapeItem> *)view->dc().value("shapeItems",(void*)0).value< void* >();
    QVector<inundationItem> &inundationItems = *(QVector<inundationItem> *)view->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)view->dc().value("shapePnts",(void*)0).value< void* >();
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
            int margin = view->dc()["geomMargin"].toInt();
            QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
            pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
            
            //- Drawing all fills(inundations) -
            for( int i=0; i < inundationItems.size(); i++ )
            {
                pnt.setBrush( inundationItems[i].brush );
                pnt.setPen( Qt::NoPen );
                pnt.drawPath( inundationItems[i].path );
                pnt.setBrush( inundationItems[i].brush_img );
                pnt.drawPath( inundationItems[i].path );
            }
            //- Drawing all el_figures -
            for( int k=0; k < shapeItems.size(); k++ )
            {
                
                pnt.setBrush( shapeItems[k].brush );
                pnt.setPen( shapeItems[k].pen );
                pnt.drawPath( shapeItems[k].path );
                pnt.setPen( shapeItems[k].penSimple );
                pnt.setBrush( Qt::NoBrush );
                pnt.drawPath( shapeItems[k].pathSimple );
            }
            //- Drawing all rects for choosen el_figures -
            pnt.setRenderHint( QPainter::Antialiasing, false );
            if( devW && devW->edit() )
            {
                for( int k=0; k <= rectItems.size()-1; k++ )
                {
                    pnt.setBrush( rectItems[k].brush );
                    pnt.setPen( rectItems[k].pen );
                    pnt.drawPath( rectItems[k].path );
                }
                pnt.setPen( QColor(0,0,0,255) );
                pnt.drawPath( ellipse_draw_startPath );
                pnt.drawPath( ellipse_draw_endPath );
            }
            return true;
        }
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if( runW && runW->dc()["active"].toBool() ) 
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
                        index_temp = index;
                        index_del = index;
                        previousPosition_all = ev->pos();
                        previousPosition = ev->pos();
                        count_holds = 0;
                        // - getting figures or rect number for moveItemTo -
                        if( index != -1 )
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
                                            Prev_pos_1 = scaleRotate( (*pnts)[shapeItems[index].n1], view, flag_scaleRotate );
                                            Prev_pos_2 = scaleRotate( (*pnts)[shapeItems[index].n2], view, flag_scaleRotate );
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
                                flag_ctrl = flag_A = false;
                                index_array.clear();
                                itemInMotion = 0;
				count_Shapes = 0;
                            }
                            rectItems.clear();
                            view->repaint();
                        }
                    }
                    // - getting start point for drawing-
                    if( (ev->button() == Qt::LeftButton) && (status==true) )
                        StartLine = ev->pos();
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
            
	    if( runW && runW->dc()["active"].toBool() ) 
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
                    if( ev->button() == Qt::LeftButton && shapeItems.size() && index == -1 && status_hold )
                    {
                        flag_angle_temp = true;
                        QBrush fill_brush( QColor(0,0,0,0), Qt::SolidPattern ), 
			       fill_img_brush;
                        fill_brush.setColor( view->dc()["fillClr"].value<QColor>() );
                        fill_img_brush = view->dc()["fillImg"].value<QBrush>();
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
                                    if( !fl_brk )
                                        inundationItems.push_back(inundationItem(inundationPath,fill_brush,fill_img_brush, inundation_vector));
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
                    flag_cursor = false;
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
                        if( status_hold && !flag_A )
                        {
                            count_moveItemTo = 0;
                            flag_hold_move = false;
                            flag_ctrl = false;
                            count_Shapes = 0;
                            count_holds = 0;
                            index_array.clear();
                            if( count_rects && !flag_arc_rect_3_4 && flag_rect ) rect_array.clear();
                            if( count_rects && flag_arc_rect_3_4 )
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
		     	flag_m=false;
                    }
                    double ang;
                    // - getting the end point of the figure and building it -
                    if( ev->button() == Qt::LeftButton && status )
                    { 
                        float Wdth = 20.0;
                        QLineF line1, line2;
                        EndLine = ev->pos();
                        if( EndLine == StartLine ) break;
                        QPainterPath circlePath, bigPath;
                        Qt::PenStyle style;
                        string ln_st;
                        //-- if line --
                        if( shapeType == 1 )
                        {
                            ln_st=view->dc()["lineStyle"].toString().toAscii().data();
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
                                if( view->dc()["bordWdth"].toInt() > 0 )
                                {
                                    circlePath = painterPath( view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, ang, StartLine,EndLine );
                                    shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(), Qt::SolidPattern),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
					    view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, angle_temp) );
                                    //shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                                }
                                else
                                {
                                    circlePath = painterPathSimple( 1, ang, StartLine, EndLine );
                                   
                                    QPainterPath bigPath = painterPath( view->dc()["lineWdth"].toInt()+1, view->dc()["bordWdth"].toInt(), 1, ang, StartLine, EndLine);
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
					    QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					    view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, angle_temp) );
                                    shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                                }
                                StartLine = unScaleRotate( StartLine, view, flag_scaleRotate );
                                EndLine = unScaleRotate( EndLine, view, flag_scaleRotate );
                                shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts );
                                shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts);
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
                               
                                if( view->dc()["bordWdth"].toInt() > 0 )
                                {
                                    circlePath = painterPath( view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(),1, ang, StartLine, EndLine );
                                    shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(), Qt::SolidPattern),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
				       	    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
					    view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, angle_temp) );
                                }
                                else
                                {
                                    circlePath = painterPathSimple( 1, ang, StartLine, EndLine );
                                    QPainterPath bigPath = painterPath( view->dc()["lineWdth"].toInt()+1, view->dc()["bordWdth"].toInt(), 1, ang, StartLine, EndLine );
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
					    QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                            QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					    view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, angle_temp) );
                                    shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                                }
                                StartLine = unScaleRotate( StartLine, view, flag_scaleRotate );
                                EndLine = unScaleRotate( EndLine, view, flag_scaleRotate );
                                shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts );
                                shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts );
                                shapeSave( view );
                                view->repaint();
                            }
                        }
                        //-- if bezier --
                        if( shapeType==3 )
                        {
                            
                            QPointF CtrlPos_1, CtrlPos_2, EndLine_temp;
                            ln_st=view->dc()["lineStyle"].toString().toAscii().data();
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
                            if( view->dc()["bordWdth"].toInt() > 0 )
                            {
                                circlePath = painterPath(view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                shapeItems.push_back( ShapeItem(circlePath, newPath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
				 	QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
					view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(), 3, angle_temp) );
                            }
                            else
                            {
                                bigPath = painterPath( view->dc()["lineWdth"].toInt()+1, view->dc()["bordWdth"].toInt(),3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                circlePath = painterPathSimple( 3, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2 );
                                shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 3, angle_temp) );
                                shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                            }
                            StartLine = unScaleRotate( StartLine, view, flag_scaleRotate );
                            EndLine = unScaleRotate( EndLine, view, flag_scaleRotate );
                            CtrlPos_1 = unScaleRotate( CtrlPos_1, view, flag_scaleRotate );
                            CtrlPos_2 = unScaleRotate( CtrlPos_2, view, flag_scaleRotate );
                            
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( CtrlPos_1, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( CtrlPos_2, shapeItems, pnts );
                            shapeSave( view );
                            view->repaint();
                        }
                        //-- if arc --
                        if( shapeType==2 )
                        {
                            QPointF CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4, Temp, StartLine_small, EndLine_small, pnt;
                            double a, a_small, b, b_small;
                            double t;
                            ln_st=view->dc()["lineStyle"].toString().toAscii().data();
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

                            if( view->dc()["bordWdth"].toInt() > 0 )
                            {
                                circlePath = painterPath( view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(),2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2,  CtrlPos_3, CtrlPos_4 );
                                shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1, -1,CtrlPos_4,QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
					view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2, angle_temp) );
                            }
                            else
                            {
                                QPainterPath bigPath = painterPath( view->dc()["lineWdth"].toInt()+1, view->dc()["bordWdth"].toInt(), 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                circlePath = painterPathSimple( 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1, -1,CtrlPos_4, QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen, Qt::FlatCap, Qt::RoundJoin),
                                        QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), style, Qt::FlatCap, Qt::RoundJoin),
					view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2, angle_temp) );
                                shapeItems[shapeItems.size()-1].brush.setColor( view->dc()["lineClr"].value<QColor>() );
                            }
                            StartLine = unScaleRotate( StartLine, view, flag_scaleRotate );
                            EndLine = unScaleRotate( EndLine, view, flag_scaleRotate );
                            CtrlPos_1 = unScaleRotate( CtrlPos_1, view, flag_scaleRotate );
                            CtrlPos_2 = unScaleRotate( CtrlPos_2, view, flag_scaleRotate );
                            CtrlPos_3 = unScaleRotate( CtrlPos_3, view, flag_scaleRotate );
                            shapeItems[shapeItems.size()-1].n1 = appendPoint( StartLine, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n2 = appendPoint( EndLine, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n3 = appendPoint( CtrlPos_1, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n4 = appendPoint( CtrlPos_2, shapeItems, pnts );
                            shapeItems[shapeItems.size()-1].n5 = appendPoint( CtrlPos_3, shapeItems, pnts );
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
					    shapeItems[index].n5 = appendPoint( QPointF( (*pnts)[shapeItems[index].n5].x(), (*pnts)[shapeItems[index].n5].y() ), shapeItems, pnts );
				    }
				    if( shapeItems[index].type==2 || shapeItems[index].type==3 )
				    {
					if( shapeItems[index].n4==shapeItems[i].n1 || shapeItems[index].n4==shapeItems[i].n2 || shapeItems[index].n4==shapeItems[i].n3 || 
						shapeItems[index].n4==shapeItems[i].n4 || shapeItems[index].n4==shapeItems[i].n5 )
				  	    shapeItems[index].n4 = appendPoint( QPointF( (*pnts)[shapeItems[index].n4].x(), (*pnts)[shapeItems[index].n4].y() ), shapeItems, pnts );
					if( shapeItems[index].n3==shapeItems[i].n1 || shapeItems[index].n3==shapeItems[i].n2 || shapeItems[index].n3==shapeItems[i].n3 || 
						shapeItems[index].n3==shapeItems[i].n4 || shapeItems[index].n3==shapeItems[i].n5 )
					    shapeItems[index].n3 = appendPoint( QPointF( (*pnts)[shapeItems[index].n3].x(),(*pnts)[shapeItems[index].n3].y() ), shapeItems,pnts);
				    }
				    if( shapeItems[index].n1==shapeItems[i].n1 || shapeItems[index].n1==shapeItems[i].n2 || shapeItems[index].n1==shapeItems[i].n3 || 
					    shapeItems[index].n1==shapeItems[i].n4 || shapeItems[index].n1==shapeItems[i].n5 )
					shapeItems[index].n1 = appendPoint( QPointF( (*pnts)[shapeItems[index].n1].x(), (*pnts)[shapeItems[index].n1].y() ), shapeItems, pnts );
				    if( shapeItems[index].n2==shapeItems[i].n1 || shapeItems[index].n2==shapeItems[i].n2 || shapeItems[index].n2==shapeItems[i].n3 ||
                                            shapeItems[index].n2==shapeItems[i].n4 || shapeItems[index].n2==shapeItems[i].n5 )
					shapeItems[index].n2 = appendPoint( QPointF( (*pnts)[shapeItems[index].n2].x(), (*pnts)[shapeItems[index].n2].y() ), shapeItems, pnts );
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
                                    ellipse_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).x()-8, 
                                                                  scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).y()-8, 16, 16 );
                                    if( ellipse_startPath.contains( scaleRotate((*pnts)[shapeItems[index].n1], view, flag_scaleRotate) ) )
                                    { 
                                        if( temp==0 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).x()-8,
                                                                               scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).y()-8, 16, 16 );
                                            current_ss = i;
                                        }
                                    }
                                    if( ellipse_startPath.contains(scaleRotate((*pnts)[shapeItems[index].n2], view, flag_scaleRotate)) )
                                    {
                                        if( temp==1 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_startPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).x()-8,
                                                                               scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).y()-8, 16, 16 );
                                            current_se = i;
                                        }
                                    }
                                    ellipse_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).x()-8, 
                                                                scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).y()-8, 16, 16 );
                                    if( ellipse_endPath.contains( scaleRotate((*pnts)[shapeItems[index].n2], view, flag_scaleRotate) ) )
                                    {
                                        if( temp==1 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).x()-8,
                                                                             scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).y()-8, 16, 16 );
                                            current_ee = i;
                                        }
                                    }
                                    if( ellipse_endPath.contains( scaleRotate((*pnts)[shapeItems[index].n1], view, flag_scaleRotate) ) )
                                    {
                                        if( temp==0 || rect_num==-1 )
                                        {
                                            if( itemInMotion->type==2 && shapeItems[i].type==2 ) break;
                                            ellipse_draw_endPath.addEllipse( scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).x()-8,
                                                                             scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).y()-8, 16, 16 );
                                            current_es = i;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if( flag_cursor==0 )
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
                    if( status_hold )	break;
                    flag_ctrl = true;
                    for( int i=0; i < shapeItems.size(); i++ )
                        index_array.push_back(-1);
                    count_Shapes = 0;
                }
                if( ev->key() == Qt::Key_Delete && (index_del!=-1 || flag_A) )
                {
                    bool flag_arc_inund = false;
                    bool flag_break_move;
                    if( flag_A )
                    {
                        for( int i=0; i < shapeItems.size(); i++ )
                            switch( shapeItems[i].type )
                            {
				case 1:
				    dropPoint( shapeItems[i].n1, index_del, shapeItems,pnts );
				    dropPoint( shapeItems[i].n2, i, shapeItems, pnts );
				    break;
				case 2:
				    dropPoint( shapeItems[i].n1, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n2, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n3, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n4, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n5, i, shapeItems, pnts );
				    break;
				case 3:
				    dropPoint( shapeItems[i].n1, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n2, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n3, i, shapeItems, pnts );
				    dropPoint( shapeItems[i].n4, i, shapeItems, pnts );
    				    break;
                            }
                            inundationItems.clear();
                            shapeItems.clear();
                            rectItems.clear();
                            shapeSave(view);
                            flag_ctrl = false;
                            flag_A = false;
                            index_array.clear();
                            itemInMotion = 0;
                            count_Shapes = 0;
                            view->repaint();
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
			// - deleting fill if deleted figure was in the fill's pfth -
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
                StartMotionPos = scaleRotate( ( *pnts)[itemInMotion->n1], view, flag_scaleRotate );
                StartMotionPos += offset;
                num_vector.append( MotionNum_1 );	//adding this point in the vector of common points
            }
            else StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );	// don't change point's coordinates
            if( !flag_MotionNum_2 )
            {
                EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
                EndMotionPos += offset;
                num_vector.append( MotionNum_2 );
            }
            else EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
            if( !flag_MotionNum_3 )
            {
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                CtrlMotionPos_1 += offset;
                num_vector.append( MotionNum_1 );
            }
            else CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            if( !flag_MotionNum_4 )
            {
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
                CtrlMotionPos_2 += offset;
                num_vector.append( MotionNum_2 );
            }
            else CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            if( !flag_MotionNum_5 )
            {
                CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
                CtrlMotionPos_3 += offset;
                num_vector.append( MotionNum_2 );
            }
            else CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
        }
        else
        {
            StartMotionPos = (*pnts)[itemInMotion->n1];
            EndMotionPos = (*pnts)[itemInMotion->n2];
            /*line2 = QLineF( StartMotionPos, QPointF( StartMotionPos.x() + 10, StartMotionPos.y() ) );
            line1 = QLineF( StartMotionPos, EndMotionPos );
            if ( StartMotionPos.y() <= EndMotionPos.y() ) ang = 360 - angle( line1, line2 );
            else ang = angle( line1, line2 );
            EndMotionPos = unRotate( EndMotionPos, ang, StartMotionPos.x(), StartMotionPos.y() );
            EndMotionPos.setX( EndMotionPos.x() + 1 );
            EndMotionPos = QPointF( StartMotionPos.x() + rotate(EndMotionPos, ang).x(), StartMotionPos.y() - rotate(EndMotionPos, ang).y() );*/
            
            StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
            EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
            StartMotionPos += offset;
            EndMotionPos += offset; 
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_1 += offset;
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_2 += offset;
        }  
        if( shapeType==2 )
        {
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
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
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );

        if( shapeType==2 )
        {
            StartMotionPos = Mouse_pos;
            if( flag_up || flag_down || flag_right || flag_left ) StartMotionPos = (*pnts)[itemInMotion->n1]+offset;
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
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
                        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
                        StartMotionPos += offset;
                        num_vector.append(MotionNum_1);
                    }
                    else StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
                else 
                {
                    StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
                    StartMotionPos += offset;
                }
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            }
            if( flag_hold_arc || flag_arc_rect_3_4 )	// if the figure is connected to the arc
            {
                if( arc_rect==0 )
                {
                    StartMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scaleRotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
                }
                if( arc_rect==1 )
                {
                    StartMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scaleRotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
                }
            }
        }
    }
    //- moving the end point of the figure -
    if ( rect_num == 1 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n2].x()).arg((*pnts)[itemInMotion->n2].y()), 10000 );
        StartMotionPos=scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
        if ( shapeType == 2 )
        {
            EndMotionPos = Mouse_pos;
            if ( flag_up || flag_down || flag_right || flag_left )
            {
                EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
                EndMotionPos += offset;
            }
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
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
                        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
                        EndMotionPos += offset;
                        num_vector.append( MotionNum_2 );
                    }
                    else EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
                else
                {
                    EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
                    EndMotionPos += offset;
                }
                CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            }
            if ( flag_hold_arc || flag_arc_rect_3_4 )
            {
                if (arc_rect == 0)
                {
                    EndMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scaleRotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
                }
                if ( arc_rect == 1 )
                {
                    EndMotionPos = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scaleRotate );
                    CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
                    CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
                }
            }

        }
    }
    //- moving the first control point of the figure -
    if ( rect_num == 2 )
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n3].x()).arg((*pnts)[itemInMotion->n3].y()), 10000 );
        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
        if (shapeType == 2)
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
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
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_1 += offset;
        }
    }
    //- moving the second control point of the figure -
    if (rect_num==3)
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n4].x()).arg((*pnts)[itemInMotion->n4].y()), 10000 );
        StartMotionPos = scaleRotate( (*pnts)[itemInMotion->n1], view, flag_scaleRotate );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
        if ( shapeType == 2 )
        {
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_2 += offset;
            CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
            CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
            EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
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
            CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
            CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
            CtrlMotionPos_2 += offset;
        }
    }
    //- moving the third control point of the figure -
    if (rect_num==4)
    {
        view->mainWin()->statusBar()->showMessage(QString(_("Coordinates(x,y): (%1, %2)")).
                arg((*pnts)[itemInMotion->n5].x()).arg((*pnts)[itemInMotion->n5].y()), 10000 );
        CtrlMotionPos_1 = scaleRotate( (*pnts)[itemInMotion->n3], view, flag_scaleRotate );
        CtrlMotionPos_2 = scaleRotate( (*pnts)[itemInMotion->n4], view, flag_scaleRotate );
        CtrlMotionPos_3 = scaleRotate( (*pnts)[itemInMotion->n5], view, flag_scaleRotate );
        CtrlMotionPos_3 += offset;
        CtrlMotionPos_4 = QPointF( itemInMotion->ctrlPos4.x(), itemInMotion->ctrlPos4.y() );
        EndMotionPos = scaleRotate( (*pnts)[itemInMotion->n2], view, flag_scaleRotate );
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
        
       
       /* EndMotionPos = unRotate( EndMotionPos, ang, StartMotionPos.x(), StartMotionPos.y() );
        EndMotionPos.setX( EndMotionPos.x() + 1 );
        EndMotionPos = QPointF( StartMotionPos.x() + rotate(EndMotionPos, ang).x(), StartMotionPos.y() - rotate(EndMotionPos, ang).y() );*/
        
        if( MotionWidth == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( MotionWidth+1,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos ), painterPathSimple( 1, ang, StartMotionPos,EndMotionPos ),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1,angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( MotionWidth,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos ), painterPathSimple( 1, ang, StartMotionPos,EndMotionPos ),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1,angle_temp ) );
        rectPath.addRect( QRectF( QPointF( StartMotionPos.x()-4, StartMotionPos.y()-4 ), QSize(8,8) ) );
	
        rectItems.append( RectItem( rectPath, MotionNum_1, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                          QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin) ) );
        rectPath = newPath;
        rectPath.addRect( QRectF( QPointF( EndMotionPos.x()-4, EndMotionPos.y()-4 ), QSize(8,8)) );
        rectItems.append( RectItem( rectPath, MotionNum_2, QBrush( QColor( 127,127,127,128 ), Qt::SolidPattern ),
                          QPen( QColor( 0, 0, 0 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin ) ) );
        rectPath = newPath;
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scaleRotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scaleRotate );
        (*pnts).insert( MotionNum_1, StartMotionPos );
        (*pnts).insert( MotionNum_2, EndMotionPos );
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
        if( MotionWidth == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( MotionWidth+1, MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                               painterPathSimple( 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, MotionNum_5, CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2, angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( MotionWidth, MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                               painterPathSimple( 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, MotionNum_5, CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2, angle_temp ) );
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
        
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scaleRotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scaleRotate );
        CtrlMotionPos_1 = unScaleRotate( CtrlMotionPos_1, view, flag_scaleRotate );
        CtrlMotionPos_2 = unScaleRotate( CtrlMotionPos_2, view, flag_scaleRotate );
        CtrlMotionPos_3 = unScaleRotate( CtrlMotionPos_3, view, flag_scaleRotate );
        (*pnts).insert( MotionNum_1, StartMotionPos );
        (*pnts).insert( MotionNum_2, EndMotionPos );
        (*pnts).insert( MotionNum_3, CtrlMotionPos_1 );
        (*pnts).insert( MotionNum_4, CtrlMotionPos_2 );
        (*pnts).insert( MotionNum_5, CtrlMotionPos_3 );
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
        if( MotionWidth == 1 && MotionBorderWidth == 0 )
            shapeItems.append( ShapeItem( painterPath( MotionWidth + 1, MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                               painterPathSimple( 3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 3, angle_temp ) );
        else
            shapeItems.append( ShapeItem( painterPath( MotionWidth, MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                               painterPathSimple( 3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2 ),
                                       MotionNum_1, MotionNum_2, MotionNum_3, MotionNum_4, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 3, angle_temp ) );
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
        StartMotionPos = unScaleRotate( StartMotionPos, view, flag_scaleRotate );
        EndMotionPos = unScaleRotate( EndMotionPos, view, flag_scaleRotate );
        CtrlMotionPos_1 = unScaleRotate( CtrlMotionPos_1, view, flag_scaleRotate );
        CtrlMotionPos_2 = unScaleRotate( CtrlMotionPos_2, view, flag_scaleRotate );
        (*pnts).insert( MotionNum_1, StartMotionPos );
        (*pnts).insert( MotionNum_2, EndMotionPos );
        (*pnts).insert(MotionNum_3,CtrlMotionPos_1);
        (*pnts).insert( MotionNum_4, CtrlMotionPos_2 );
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
                        if( index_array[j] == i )	flag_equal = 1;
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
                                shapeItems[index].n5 = appendPoint( Temp, shapeItems, pnts);
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
                                shapeItems[index].n4 = appendPoint( Temp, shapeItems, pnts);
                            }
                            if ( (shapeItems[index].n3 == shapeItems[i].n1) || 
                                (shapeItems[index].n3 == shapeItems[i].n2) ||
                                (shapeItems[index].n3 == shapeItems[i].n3) || 
                                (shapeItems[index].n3 == shapeItems[i].n4) ||
                                (shapeItems[index].n3 == shapeItems[i].n5) )
                            {
                                QPointF Temp = (*pnts)[shapeItems[index].n3];
                                shapeItems[index].n3 = appendPoint( Temp, shapeItems, pnts );
                            }
                        }
                        if( (shapeItems[index].n1 == shapeItems[i].n1) || 
                            (shapeItems[index].n1 == shapeItems[i].n2) ||
                            (shapeItems[index].n1 == shapeItems[i].n3) || 
                            (shapeItems[index].n1 == shapeItems[i].n4) ||
                            (shapeItems[index].n1 == shapeItems[i].n5) )
                        {
                            QPointF Temp = (*pnts)[shapeItems[index].n1];
                            shapeItems[index].n1 = appendPoint( Temp, shapeItems, pnts );
                        }
                        if( (shapeItems[index].n2 == shapeItems[i].n1) || 
                            (shapeItems[index].n2 == shapeItems[i].n2) ||
                            (shapeItems[index].n2 == shapeItems[i].n3) ||
                            (shapeItems[index].n2 == shapeItems[i].n4) ||
                            (shapeItems[index].n2 == shapeItems[i].n5) )
                        {
                            QPointF Temp = (*pnts)[shapeItems[index].n2];
                            shapeItems[index].n2 = appendPoint( Temp, shapeItems, pnts);
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
    if( inundationItems.size() && !flag_inund_break && !status_hold )
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
    flag_inund_break = true;
}

int ShapeElFigure::realRectNum( int rect_num_old, const QVector<ShapeItem> &shapeItems )
{
    int rect_num_new;
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
                    offset = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scaleRotate ) - Prev_pos_1;
                else 
                    offset = scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scaleRotate ) - Prev_pos_2;
            rect_num = fig_rect_array[i];
            arc_rect = arc_rect_array[i];
        }
        index = index_array[i];
        moveItemTo( pos, shapeItems, pnts, view );
        if ( i == 0 && flag_arc_rect_3_4 )
        {
            Prev_pos_1 = scaleRotate( (*pnts)[shapeItems[index_array[0]].n1], view, flag_scaleRotate );
            Prev_pos_2=scaleRotate( (*pnts)[shapeItems[index_array[0]].n2], view, flag_scaleRotate );
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
//- detecting the figure or one of its control points under mouse cursor -
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
    for ( int i = 0; i < shapeItems.size(); i++ ) 
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
        
        //-el_width-el_border_width;
        
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
                           TSYS::realRound( el_p2.x(), 2, true ),TSYS::realRound( el_p2.y(), 2, true ) );
    }
    return circlePath;
}

//- adding the point to data model -
int ShapeElFigure::appendPoint( const QPointF &pos, const QVector<ShapeItem> &shapeItems, PntMap *pnts )
{
    int i = 1;
    while( (*pnts).contains(i) ) i++;
    (*pnts).insert( i, pos );
    return i;
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
    if( !equal )
        (*pnts).remove(num);
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
        if( flag_scaleRotate )
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
                    if( inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n4], view, flag_scaleRotate ) ) )
                        inundation_fig_num[p] = j;
                if( shapeItems[j].type == 3 && p != j && shapeItems[inundation_fig_num[p]].type != 2 )
                    if( inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n4], view, flag_scaleRotate ) ) && 
                       inundationPath.contains( scaleRotate( (*pnts)[shapeItems[j].n3], view, flag_scaleRotate ) ) )
                        inundation_fig_num[p] = j;
            }
    return inundation_fig_num;
}

//- detecting the figures, which count <=2, for filling -
bool ShapeElFigure::inundation1_2( const QPointF &point, const QVector<ShapeItem> &shapeItems, QVector<inundationItem> &inundationItems, PntMap *pnts, WdgView *view, int number )
{
    QPainterPath inundationPath_1_2;
    QVector<int> in_fig_num;
    bool flag_break, fl_brk;
    flag_break = false;
    fl_brk = false;
    QBrush fill_brush( QColor(0,0,0,0), Qt::SolidPattern ), fill_img_brush;
    fill_brush.setColor( view->dc()["fillClr"].value<QColor>() );
    fill_img_brush = view->dc()["fillImg"].value<QBrush>();
    for( int i = 0; i < shapeItems.size(); i++ )
    {
        if( shapeItems[i].type == 2 )
            if( scaleRotate((*pnts)[shapeItems[i].n1], view, flag_scaleRotate).toPoint()==scaleRotate((*pnts)[shapeItems[i].n2], view, flag_scaleRotate).toPoint() )
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
                        if( number == -1 )
                            inundationItems.push_back( inundationItem( inundationPath_1_2, fill_brush, fill_img_brush, in_fig_num ) );
                        else
                        {
                            if( !flag_scaleRotate )
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
        if( shapeItems[i].type == 2 || shapeItems[i].type == 3 && status_hold )
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
                                if( number == -1 )
                                    inundationItems.push_back( inundationItem( inundationPath_1_2, fill_brush, fill_img_brush, in_fig_num ) );
                                else
                                {
                                    if( !flag_scaleRotate )
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

QPointF ShapeElFigure::scaleRotate( const QPointF &point, WdgView *view, bool flag_scale )
{
    QPointF rpnt = point;
    if( flag_scale )
    {
        QPointF center = QPointF( (view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true)) ).toPoint();
        rpnt = rpnt - center;
        rpnt = rotate( rpnt, view->dc()["orient"].toDouble() );
        rpnt = rpnt + center;
        rpnt = QPointF( rpnt.x()*view->xScale(true), rpnt.y()*view->yScale(true) );
    }
    return rpnt;
}

QPointF ShapeElFigure::unScaleRotate( const QPointF &point, WdgView *view, bool flag_scale )
{
    QPointF rpnt = point;
    if(flag_scale)
    {
        QPointF center= QPointF( (view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true)) ).toPoint();
        rpnt = QPointF( rpnt.x()/view->xScale(true), rpnt.y()/view->yScale(true) );
        rpnt = rpnt - center;
        rpnt = rotate( rpnt, 360-view->dc()["orient"].toDouble() );
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
    if( shapeItems[in_fig_num[0]].n1 < shapeItems[in_fig_num[0]].n2 )
    {
        path.moveTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).x(), 2, true ),
                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).y(), 2, true ) );
        switch( shapeItems[in_fig_num[0]].type )
        {
            case 1:
                path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).x() ),
                             TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).y(), 2, true ) );
                break;
            case 2:
                if( flag_angle_temp || !flag_scaleRotate )
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
                else
                    ang = shapeItems[in_fig_num[0]].angle_temp;
                
                if( !flag_scaleRotate )
                {
                    arc_a = length( (*pnts)[shapeItems[in_fig_num[0]].n3], (*pnts)[shapeItems[in_fig_num[0]].n5] );
                    arc_b = length( (*pnts)[shapeItems[in_fig_num[0]].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].x(), 2 ),
                                   TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].y(), 2 ) ) );
                }
                else
                {
                    arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scaleRotate ).y() ) );
                    arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).y() ) );
                }
                t_start = shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end = shapeItems[in_fig_num[0]].ctrlPos4.y();
                for ( t = t_start; t < t_end+0.00277777777778; t += 0.00277777777778) 
                    path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                 TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                break;
            case 3:
                path.cubicTo( TSYS::realRound( scaleRotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).y(), 2, true ) );
    
                break;
        }
        flag_n2 = true;
        flag_n1 = false;
    }
    else
    {
        path.moveTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).x(), 2, true ),
                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n2], view, flag_scaleRotate ).y(), 2, true ) );
        switch( shapeItems[in_fig_num[0]].type )
        {
            case 1:
                path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).x(), 2, true ),
                             TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).y(), 2, true ) );
                break;
            case 2:
                if( flag_angle_temp || !flag_scaleRotate )
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
                else
                    ang = shapeItems[in_fig_num[0]].angle_temp;
                if( !flag_scaleRotate )
                {
                    arc_a = length( (*pnts)[shapeItems[in_fig_num[0]].n3], (*pnts)[shapeItems[in_fig_num[0]].n5] );
                    arc_b = length( (*pnts)[shapeItems[in_fig_num[0]].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].x(), 2 ),
                                   TSYS::realRound( (*pnts)[shapeItems[in_fig_num[0]].n4].y(), 2 ) ) );
                }
                else
                {
                    arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n5], view, flag_scaleRotate ).y() ) );
                    arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() ),
                                    QPointF( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).x(),
                                             scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).y() ) );
                }

                t_start = shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end = shapeItems[in_fig_num[0]].ctrlPos4.y();
                for ( t = t_end; t > t_start+0.00277777777778; t -= 0.00277777777778) 
                    path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                 TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                break;
            case 3:
                path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n4], view, flag_scaleRotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n3], view, flag_scaleRotate ).y(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).x(), 2, true ),
                              TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_fig_num[0]].n1], view, flag_scaleRotate ).y(), 2, true ) );
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
                        path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scaleRotate ).x(), 2, true ),
                                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scaleRotate ).y(), 2, true ) );
                        break;
                    case 2:
                        if( flag_angle_temp || !flag_scaleRotate )
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
                        else
                            ang = shapeItems[in_index].angle_temp;
                        if( !flag_scaleRotate )
                        {
                            arc_a = length( (*pnts)[shapeItems[in_index].n3], (*pnts)[shapeItems[in_index].n5] );
                            arc_b = length( (*pnts)[shapeItems[in_index].n3], QPointF(TSYS::realRound( (*pnts)[shapeItems[in_index].n4].x(), 2 ),
                                           TSYS::realRound( (*pnts)[shapeItems[in_index].n4].y(), 2 ) ) );
                        }
                        else
                        {
                            arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scaleRotate ).y() ) );
                            arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).y() ) );
                        }

                        t_start = shapeItems[in_index].ctrlPos4.x();
                        t_end = shapeItems[in_index].ctrlPos4.y();
                        for ( t = t_start; t < t_end+0.00277777777778; t += 0.00277777777778 ) 
                            path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                         TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                        break;
                
                    case 3:
                        path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n2], view, flag_scaleRotate ).y(), 2, true ) );
                        break;
                }
                flag_n2 = true;
                flag_n1 = false;
                break;
            case 2:
                switch( shapeItems[in_index].type )
                {
                    case 1:
                        path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scaleRotate ).x(), 2, true ),
                                     TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scaleRotate ).y(), 2, true ) );
                        break;
                    case 2:
                        if( flag_angle_temp || !flag_scaleRotate )
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
                        else
                            ang = shapeItems[in_index].angle_temp;
                        if( !flag_scaleRotate )
                        {
                            arc_a = length( (*pnts)[shapeItems[in_index].n3], (*pnts)[shapeItems[in_index].n5] );
                            arc_b = length( (*pnts)[shapeItems[in_index].n3], QPointF( TSYS::realRound( (*pnts)[shapeItems[in_index].n4].x(),2),
                                           TSYS::realRound((*pnts)[shapeItems[in_index].n4].y(), 2 ) ) );
                        }
                        else
                        {
                            arc_a = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n5], view, flag_scaleRotate).y() ) );
                            arc_b = length( QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() ),
                                            QPointF( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).x(),
                                                     scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).y() ) );
                        }

                        t_start = shapeItems[in_index].ctrlPos4.x();
                        t_end = shapeItems[in_index].ctrlPos4.y();
                        for ( t = t_end; t > t_start+0.00277777777778; t -= 0.00277777777778 ) 
                            path.lineTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x() + rotate( arc( t, arc_a, arc_b ), ang ).x(), 2, true ),
                                         TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y() - rotate( arc( t, arc_a, arc_b ), ang ).y(), 2, true ) ); 
                        break;
                    case 3:
                        path.cubicTo( TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n4], view, flag_scaleRotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n3], view, flag_scaleRotate ).y(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scaleRotate ).x(), 2, true ),
                                      TSYS::realRound( scaleRotate( (*pnts)[shapeItems[in_index].n1], view, flag_scaleRotate ).y(), 2, true ) );

                        break;
                }
                flag_n2 = false;
                flag_n1 = true;
                break;
        }
    }
    return path;
}

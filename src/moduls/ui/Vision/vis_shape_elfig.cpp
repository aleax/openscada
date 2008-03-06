
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

#include <QtGui>
#include <stdlib.h>
#include <string.h>

#include <tsys.h>
#include "tvision.h"
#include "vis_devel.h"
#include "vis_widgs.h"
#include "vis_shape_elfig.h"
#include "vis_devel_widgs.h"
#include "vis_run_widgs.h"


using namespace VISION;

ShapeElFigure::ShapeElFigure() : 
    WdgShape("ElFigure"), itemInMotion(0), flag_down(false), flag_up(false), flag_left(false), flag_right(false), flag_A(false), flag_ctrl(false),status_hold(false), 
             flag_rect(false), flag_hold_move(false), flag_m(false), flag_hold_arc(false), flag_arc_rect_3_4(false), flag_first_move(false), flag_hold_checked(false), 
    current_ss(-1), current_se(-1), current_es(-1), current_ee(-1), count_Shapes(0), count_holds(0), count_rects(0), rect_num_arc(-1), rect_num(-1)
{
    newPath.addEllipse(QRect(0, 0, 0, 0));
}

void ShapeElFigure::init( WdgView *w )
{
    QVector <ShapeItem> *shapeItems = new QVector <ShapeItem>();
    w->dc()["shapeItems"].setValue( (void*)shapeItems );
    QVector <InundationItem> *inundationItems = new QVector <InundationItem>();
    w->dc()["inundationItems"].setValue( (void*)inundationItems );
    PntMap *pnts = new PntMap;
    w->dc()["shapePnts"].setValue( (void*)pnts );
}

void ShapeElFigure::destroy( WdgView *w )
{
    delete (QVector <ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    delete (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    delete (QVector <InundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    rectItems.clear();
}

bool ShapeElFigure::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);
    bool rel_list=false;					//change signal
    bool up=false;
    status=false;
    QVector <ShapeItem> &shapeItems = *(QVector <ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    QVector <InundationItem> &inundationItems = *(QVector <InundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    QLineF line1, line2;
    double ang;
    QPointF StartMotionPos;
    QPointF EndMotionPos;
    QPointF CtrlMotionPos_1;
    QPointF CtrlMotionPos_2;
    QPointF CtrlMotionPos_3;
    QPointF CtrlMotionPos_4;
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
            rel_list=true;
            break;
        case 21:	//lineClr
            w->dc()["lineClr"] =  QColor(val.c_str());
            rel_list=true;
            break;
        case 22:	//lineDecor
            w->dc()["lineDecor"] = atoi(val.c_str());
            rel_list=true;
            break;
        case 23:	//bordWdth
            w->dc()["bordWdth"] = atoi(val.c_str());
            rel_list=true;
            break;
        case 24:	//bordClr
            w->dc()["bordClr"] = QColor(val.c_str());
            rel_list=true;
            break;
        case 25:	//fillClr
            w->dc()["fillClr"] = QColor(val.c_str());
	    rel_list=true;
            break;
        case 26:	//fillImg
        {
	    QImage img;
	    string backimg = w->resGet(val);
	    if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		w->dc()["fillImg"] = QBrush(img);
	    else w->dc()["fillImg"] = QBrush();
	    rel_list=true;
	    break;
        }
        case 28:
        {
            w->dc()["orient"] = atof(val.c_str());
            rel_list=true;
            break;   
        }
        case 27:	//elLst
            w->dc()["elLst"] = val.c_str();
            rel_list=true;
            break;
        default:
            if( uiPrmPos >= 30 )
            {
                int pnt  = (uiPrmPos-30)/2;
                int patr = (uiPrmPos-30)%2;
                int pval  = atoi(val.c_str());
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
            shapeItems_temp=shapeItems;
            shapeItems.clear();
        }
        if(inundationItems.size())
            inundationItems.clear();
        //- Parse last attributes list and make point list -
        string sel;
        int  p[5];
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
                if( !width ) width=w->dc()["lineWdth"].toInt() ;
                QColor color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !color.isValid() ) color=w->dc()["lineClr"].value<QColor>();
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=w->dc()["bordWdth"].toInt();
                QColor bord_color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_color.isValid())  bord_color=w->dc()["bordClr"].value<QColor>(); 
                //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 2; i_p++ )
                    ip[i_p] = scale_rotate((*pnts)[p[i_p]], w, true);//*w->xScale(true),(*pnts)[p[i_p]].y()*w->yScale(true) );
                //-- Detecting the rotation angle of the line --
                line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                line1=QLineF(ip[0],ip[1]);
                if( ip[0].y()<=ip[1].y() ) ang=360-line1.angle(line2);
                else ang=line1.angle(line2);
                //-- Building the path of the line and adding it to container --
                if (bord_width>0)
                {
                    circlePath = painter_path(width,bord_width,1, ang, ip[0],ip[1]);
                    shapeItems.push_back( ShapeItem(circlePath, newPath, p[0],p[1],-1,-1,-1,QPointF(0,0), 
                                       QBrush(color,Qt::SolidPattern),
                                       QPen(bord_color,bord_width,Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                       QPen(color,width,Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),width,bord_width,1));
                }
                if (bord_width==0)
                {
                    circlePath = painter_path_simple( 1, ang, ip[0],ip[1] );
                    QPainterPath bigPath = painter_path(width,w->dc()["bordWdth"].toInt()+1,1, ang, ip[0], ip[1] );
                    shapeItems.push_back( ShapeItem(bigPath,circlePath,p[0],p[1],-1,-1,-1,QPointF(0,0), 
                                       QBrush(color,Qt::NoBrush),
                                       QPen(w->dc()["bordClr"].value<QColor>(),w->dc()["bordWdth"].toInt(),Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                       QPen(color,width, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                       width,w->dc()["bordWdth"].toInt(),1));
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
                if( !width ) width=w->dc()["lineWdth"].toInt() ;
                QColor color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !color.isValid() ) color=w->dc()["lineClr"].value<QColor>();
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=w->dc()["bordWdth"].toInt();
                QColor bord_color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_color.isValid())  bord_color=w->dc()["bordClr"].value<QColor>();
                //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = scale_rotate((*pnts)[p[i_p]], w, true);

                //-- Building the current arc --
                StartMotionPos=ip[0];
                EndMotionPos=ip[1];
                CtrlMotionPos_1=ip[2];
                CtrlMotionPos_2=ip[3];
                CtrlMotionPos_3=ip[4];
                MotionWidth=width;
                line2=QLineF(CtrlMotionPos_1 ,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
                line1=QLineF( CtrlMotionPos_1,CtrlMotionPos_3);
                if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=line1.angle(line2);
                else ang=360-line1.angle(line2);

                a=Length(CtrlMotionPos_3, CtrlMotionPos_1);
                b=Length(CtrlMotionPos_2, CtrlMotionPos_1);

                CtrlMotionPos_2=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                        CtrlMotionPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
                StartMotionPos=UNROTATE(StartMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
                QPoint StartMotionPos_i=StartMotionPos.toPoint();
                if (StartMotionPos.x()>=a)
                {
                    StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*a);
                    StartMotionPos.setX(a);
                }
                if (StartMotionPos.x()<-a)
                {
                    StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*(-a));
                    StartMotionPos.setX(-a);
                }
                if(StartMotionPos.y()<=0)

                    t_start=acos(StartMotionPos.x()/a)/(2*M_PI);
                else
                    t_start=1-acos(StartMotionPos.x()/a)/(2*M_PI);
                EndMotionPos=UNROTATE(EndMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
                if (EndMotionPos.x()<-a)
                {
                    EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(-a));
                    EndMotionPos.setX(-a);
                }
                if (EndMotionPos.x()>=a)
                {
                    EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(a));
                    EndMotionPos.setX(a);
                }
                if(EndMotionPos.y()<=0)
                    t_end=acos(EndMotionPos.x()/a)/(2*M_PI);
                else
                    t_end=1-acos(EndMotionPos.x()/a)/(2*M_PI);
                if (t_start>t_end) t_end+=1;
                if ((t_end-1)>t_start) t_end-=1;
                if (t_start==t_end) t_end+=1;
                if (t_end>t_start && t_start>=1 && t_end>1)
                {
                    t_start-=1;
                    t_end-=1;
                }
                StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                                       CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
                EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                     CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
                CtrlMotionPos_4=QPointF(t_start, t_end);
                
		//-- Building the path of the line and adding it to container --
                if (bord_width>0)
                {
                    circlePath = painter_path(width,bord_width,2, ang, StartMotionPos, EndMotionPos,CtrlMotionPos_1, CtrlMotionPos_2,
                                              CtrlMotionPos_3, CtrlMotionPos_4 );
                    shapeItems.push_back( ShapeItem(circlePath, newPath, p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,
                                          QBrush(color,Qt::SolidPattern),
                                                  QPen(bord_color,bord_width, Qt::SolidLine,Qt::FlatCap,Qt::RoundJoin),
                                                          QPen(color,width,Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),width,bord_width, 2) );
                }

                if (bord_width==0)
                {
                    QPainterPath bigPath = painter_path(width+1,bord_width,2, ang, StartMotionPos, EndMotionPos,CtrlMotionPos_1,
                            CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 );
                    circlePath = painter_path_simple(2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2,  CtrlMotionPos_3, CtrlMotionPos_4 );
                    shapeItems.push_back( ShapeItem(bigPath,circlePath,p[0],p[1],p[2],p[3], p[4],CtrlMotionPos_4, 
                                          QBrush(color,Qt::NoBrush),
                                                  QPen(w->dc()["bordClr"].value<QColor>(),bord_width, Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                          QPen(color,width, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),width,bord_width,2) );  
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
                if( !width ) width=w->dc()["lineWdth"].toInt() ;
                QColor color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !color.isValid() ) color=w->dc()["lineClr"].value<QColor>();
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=w->dc()["bordWdth"].toInt();
                QColor bord_color(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_color.isValid())  bord_color=w->dc()["bordClr"].value<QColor>();  
                for( int i_p = 0; i_p < 4; i_p++ )
                    ip[i_p] = scale_rotate((*pnts)[p[i_p]],w, true);
                line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                line1=QLineF(ip[0],ip[1]);
                if (ip[0].y()<=ip[1].y())
                    ang=360-line1.angle(line2);
                else
                    ang=line1.angle(line2);
                
                if (bord_width>0)
                {
                    circlePath = painter_path(width,bord_width,3, ang, ip[0], ip[1], ip[2], ip[3] );
                    shapeItems.push_back( ShapeItem(circlePath, newPath, p[0], p[1], p[2], p[3],-1,QPointF(0,0),
                                       QBrush(color,Qt::SolidPattern),
                                       QPen(bord_color,bord_width,Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin),
                                       QPen(color,width, Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                       width,bord_width, 3) );
                }
                if (bord_width==0)
                {
                    QPainterPath bigPath = painter_path(width+1,bord_width, 3, ang,ip[0],ip[1],ip[2],ip[3]);
                    circlePath = painter_path_simple(3, ang, ip[0], ip[1], ip[2], ip[3]);
                    shapeItems.push_back( ShapeItem(bigPath,circlePath, p[0], p[1], p[2], p[3], -1,QPointF(0,0), 
                                       QBrush(color,Qt::NoBrush),
                                       QPen(w->dc()["bordClr"].value<QColor>(),bord_width, Qt::NoPen,Qt::FlatCap,Qt::MiterJoin),
                                       QPen(color,width, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin), 
                                       width,bord_width,3) );
                }
            }
            
            if( el == "fill" )
            {
                int zero_pnts = 0;
                QVector<int> fl_pnts;
                string fl_color, fl_img;
                while( true )
                {
                    string svl = TSYS::strSepParse(sel,0,':',&el_off);
                    int vl = atoi(svl.c_str());
                    if( vl ) fl_pnts.push_back(vl);
                    else if( zero_pnts == 0 ) { fl_color = svl; zero_pnts++; }
                    else if( zero_pnts == 1 ) { fl_img = svl; zero_pnts++; }
                    else break;
                }
                //- Check fill color -
                QColor color(fl_color.c_str());
                color.setAlpha(255);
                if( !color.isValid() ) 
                    color=w->dc()["fillClr"].value<QColor>();
                //- Check fill image -
		QBrush brsh;
		QImage img;
		string backimg = w->resGet(fl_img);
		if( !backimg.empty() && img.loadFromData((const uchar*)backimg.c_str(),backimg.size()) )
		    brsh.setTextureImage(img);
                //- Make elements -
                if(fl_pnts.size()>=2)
                    inundationItems.push_back(InundationItem(newPath,color,brsh, fl_pnts));
            }
        }
        for (int i=0; i<shapeItems_temp.size(); i++)
        {
            Drop_Point(shapeItems_temp[i].n1,shapeItems.size()+1,shapeItems,pnts);
            Drop_Point(shapeItems_temp[i].n2,shapeItems.size()+1,shapeItems,pnts);
            Drop_Point(shapeItems_temp[i].n3,shapeItems.size()+1,shapeItems,pnts);
            Drop_Point(shapeItems_temp[i].n4,shapeItems.size()+1,shapeItems,pnts);
            Drop_Point(shapeItems_temp[i].n5,shapeItems.size()+1,shapeItems,pnts);
        }
        shapeItems_temp.clear();
        up = true;
    }	
    if( up && !w->allAttrLoad( ) )
    {
        //-Repainting all shapes by calling moveItemTo to each shape
        QVector<int> inundation_fig_num;
        bool flag_push_back;
        for (int i=0; i<shapeItems.size(); i++)
        {
            if (shapeItems[i].type==2)
            {
                count_moveItemTo=1;
                count_Shapes=1;
                flag_ctrl_move=false;
                flag_ctrl=true;
                offset=QPointF(0,0);
                index=i;
                itemInMotion=&shapeItems[index];
                moveItemTo(QPointF(0,0),shapeItems,pnts,w);
            }
        }
        for (int i=0; i<shapeItems.size(); i++)
        {
            if (shapeItems[i].type!=2)
            {
                count_moveItemTo=1;
                count_Shapes=1;
                flag_ctrl_move=false;
                flag_ctrl=true;
                offset=QPointF(0,0);
                index=i;
                itemInMotion=&shapeItems[index];
                moveItemTo(QPointF(0,0),shapeItems,pnts,w);
            }
        }
        if(shapeItems.size())
            for(int i=0; i<inundationItems.size(); i++)
            {
                if(inundationItems[i].number_shape.size()>2)
                {
                    for(int k=0; k<inundationItems[i].number_shape.size()-1; k++)
                        for(int j=0; j<shapeItems.size(); j++)
                            if((shapeItems[j].n1==inundationItems[i].number_shape[k] && shapeItems[j].n2==inundationItems[i].number_shape[k+1]) ||
                                (shapeItems[j].n1==inundationItems[i].number_shape[k+1] && shapeItems[j].n2==inundationItems[i].number_shape[k]) )
                            {
                                flag_push_back=true;
                                for(int p=0; p<inundation_fig_num.size(); p++)
                                    if((shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n2) ||
                                        (shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n1))
                                    {
                                        flag_push_back=false;
                                        if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                        if((shapeItems[inundation_fig_num[p]].type==3 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                        if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==3) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                    }
                                if (flag_push_back)
                                inundation_fig_num.push_back(j);
                            }
                        for(int j=0; j<shapeItems.size(); j++)
                            if((shapeItems[j].n1==inundationItems[i].number_shape[inundationItems[i].number_shape.size()-1] && shapeItems[j].n2==inundationItems[i].number_shape[0]) ||
                                (shapeItems[j].n1==inundationItems[i].number_shape[0] && shapeItems[j].n2==inundationItems[i].number_shape[inundationItems[i].number_shape.size()-1]) )
                            {
                                flag_push_back=true;
                                for(int p=0; p<inundation_fig_num.size(); p++)
                                    if((shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n2) ||
                                        (shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n1))
                                    {
                                        flag_push_back=false;
                                        if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                        if((shapeItems[inundation_fig_num[p]].type==3 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                        if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==3) && (inundation_fig_num[p]!=j))
                                            inundation_fig_num[p]=j;
                                    }
                                if (flag_push_back)
                                    inundation_fig_num.push_back(j);
                            }
                    QPainterPath temp_path=Create_Inundation_Path(inundation_fig_num, shapeItems, pnts,w);
                    inundation_fig_num=Inundation_sort(temp_path, inundation_fig_num, shapeItems, pnts, w);
                    inundationItems[i].path=Create_Inundation_Path(inundation_fig_num, shapeItems, pnts,w);
                    inundationItems[i].number_shape=inundation_fig_num;
                    if(inundation_fig_num.size()>inundationItems[inundationItems.size()-1].number_shape.size()) 
                        inundationItems[inundationItems.size()-1].path=newPath;
                }
                if(inundationItems[i].number_shape.size()==2)
                {
                    for(int j=0; j<shapeItems.size(); j++)
                    {
                        if((shapeItems[j].n1 == inundationItems[i].number_shape[0] && shapeItems[j].n2 == inundationItems[i].number_shape[1]) ||
                            (shapeItems[j].n1 == inundationItems[i].number_shape[1] && shapeItems[j].n2 == inundationItems[i].number_shape[0]))
                            inundation_fig_num.push_back(j);
                        if (inundation_fig_num.size()==2) break;
                    }
                    inundationItems[i].path=Create_Inundation_Path(inundation_fig_num, shapeItems, pnts,w);
                    inundationItems[i].number_shape=inundation_fig_num;
                }
                inundation_fig_num.clear();
            }
        itemInMotion=0;
        index=-1;
        if(rectItems.size())
            rectItems.clear();
        flag_ctrl=false;
        w->repaint();
    }    
    return up;
}
//-Saving shapes' attributes to data model-
bool ShapeElFigure::shapeSave( WdgView *w )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    QVector <ShapeItem> &shapeItems = *(QVector <ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    QVector <InundationItem> &inundationItems = *(QVector <InundationItem> *)w->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    string elList;
    //- Building attributes for all el_figures and fills -
    //--for el_figures--
    for( int i_s = 0; i_s < shapeItems.size(); i_s++ )
       switch( shapeItems[i_s].type )
	{
             case 1:
                 elList+="line:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+
                                ":"+((shapeItems[i_s].width==w->dc()["lineWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].width))+
                                ":"+((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString())?"":shapeItems[i_s].brush.color().name().toAscii().data())+
                                ":"+((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].border_width))+
                                ":"+((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"])?"":shapeItems[i_s].pen.color().name().toAscii().data())+"\n";

		break;
	    case 2:
                    elList+="arc:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+":"+
                            TSYS::int2str(shapeItems[i_s].n3)+":"+TSYS::int2str(shapeItems[i_s].n4)+":"+
                            TSYS::int2str(shapeItems[i_s].n5)+
                            ":"+((shapeItems[i_s].width==w->dc()["lineWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].width))+
                            ":"+((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString())?"":shapeItems[i_s].brush.color().name().toAscii().data())+
                            ":"+((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].border_width))+
                            ":"+((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"])?"":shapeItems[i_s].pen.color().name().toAscii().data())+"\n";

		break;
	    case 3:
                 elList+="bezier:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+":"+
                            TSYS::int2str(shapeItems[i_s].n3)+":"+TSYS::int2str(shapeItems[i_s].n4)+
                            ":"+((shapeItems[i_s].width==w->dc()["lineWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].width))+
                            ":"+((shapeItems[i_s].brush.color().name() == w->dc()["lineClr"].toString())?"":shapeItems[i_s].brush.color().name().toAscii().data())+
                            ":"+((shapeItems[i_s].border_width == w->dc()["bordWdth"].toInt())?"":TSYS::int2str((int)shapeItems[i_s].border_width))+
                            ":"+((shapeItems[i_s].pen.color().name() == w->dc()["bordClr"])?"":shapeItems[i_s].pen.color().name().toAscii().data())+"\n";

		break;
	}
        //--for fills--
        for(int i=0; i<inundationItems.size(); i++)
        {
            bool flag_n1=false;
            bool flag_n2=false;
            elList+="fill:";
            for(int k=0; k<inundationItems[i].number_shape.size()-1; k++)
            {
                if(inundationItems[i].number_shape.size()>2)
                    if(k==0)
                    {
                        elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
                        elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
                    }
                    else
                        if(shapeItems[inundationItems[i].number_shape[k]].n1==shapeItems[inundationItems[i].number_shape[k-1]].n2)
                            elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n2)+":";
                        else
                            elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[k]].n1)+":";
            }
            if(inundationItems[i].number_shape.size()<=2)
            {
                elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n1)+":";
                elList+=TSYS::int2str(shapeItems[inundationItems[i].number_shape[0]].n2)+":";
            }
            elList+=((inundationItems[i].brush.color().name() == w->dc()["fillClr"].value<QColor>().name())?"":inundationItems[i].brush.color().name().toAscii().data());
            elList+="\n";

        }
        w->attrSet( "elLst", elList );

    //- Write shapes points to data model -
    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
    {
        w->attrSet("p"+TSYS::int2str(pi.key())+"x",TSYS::int2str((int)pi.value().x()));
        w->attrSet("p"+TSYS::int2str(pi.key())+"y",TSYS::int2str((int)pi.value().y()));
    }
    devW->setSelect(true);
}

void ShapeElFigure::editEnter( WdgView *view )
{    
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(true);
    
    connect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)),
               this, SLOT(toolAct(QAction*)) );
    //- Init actions' address -
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(true);
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText(TSYS::addr2str(view).c_str());
    }
    populateScene();
}

void ShapeElFigure::editExit( WdgView *view )
{
    disconnect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)),
                  this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(false);
    //- Clear action;s address -
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
    {
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText("");
	((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setEnabled(false);
    }
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
            if(((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->objectName() != "hold")
                ((VisDevelop *)w->mainWin())->elFigTool->actions().at(i_a)->setChecked(false);
        }
    if( act->objectName() == "line" )
    {
        act->setChecked(true);
        shapeType=1;
        status=true;
    }
    if( act->objectName() == "arc" )
    {
        act->setChecked(true);
        shapeType=2;
        status=true;
    }
    if( act->objectName() == "besier" )
    {
        act->setChecked(true);
        shapeType=3;
        status=true;
    }
    if( act->objectName() == "hold" )
    {        
        if(flag_hold_checked)
        {
            act->setChecked(false);
            flag_hold_checked=false;
        }
        else
        {
            act->setCheckable(true);
            act->setChecked(true);
            flag_hold_checked=true;
        }
        status_hold=act->isChecked();
        rectItems.clear();
        w->update();
    }
    if( act->objectName() == "cursor" )
    {
        act->setChecked(true);
        w->unsetCursor();
        status=false;
        w->update();
    }
}

bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    QVector <ShapeItem> &shapeItems = *(QVector <ShapeItem> *)view->dc().value("shapeItems",(void*)0).value< void* >();
    QVector <InundationItem> &inundationItems = *(QVector <InundationItem> *)view->dc().value("inundationItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)view->dc().value("shapePnts",(void*)0).value< void* >();
    bool flag_hold_rect;
    flag_hold_rect=false;
    switch(event->type())
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
            //if (runW) pnt.setRenderHint(QPainter::Antialiasing, true);
            //else pnt.setRenderHint(QPainter::Antialiasing, false);
            
            //- Drawing all fills(inundations) -
            for(int i=0; i<inundationItems.size(); i++)
            {
                pnt.setBrush(inundationItems[i].brush);
                pnt.setPen(Qt::NoPen);
                pnt.drawPath(inundationItems[i].path);
                pnt.setBrush(inundationItems[i].brush_img);
                pnt.drawPath(inundationItems[i].path);
            }
            //- Drawing all el_figures -
            for (int k=0; k<=shapeItems.size() - 1; k++)
            {
                pnt.setBrush(shapeItems[k].brush);
                pnt.setPen(shapeItems[k].pen);
                pnt.drawPath(shapeItems[k].path);
                pnt.setPen(shapeItems[k].penSimple);
                //printf("K=%i\n", k);
                //printf("Color=%s\n", shapeItems[k].penSimple.color().name().toAscii().data());
                pnt.setBrush(Qt::NoBrush);
                pnt.drawPath(shapeItems[k].pathSimple);
            }
            //- Drawing all rects for choosen el_figures -
            pnt.setRenderHint(QPainter::Antialiasing, false);
            for (int k=0; k<=rectItems.size() - 1; k++)
            {
                pnt.setBrush(rectItems[k].brush);
                pnt.setPen(rectItems[k].pen);
                pnt.drawPath(rectItems[k].path);
            }
            pnt.setPen(QColor(0,0,0,255));
            pnt.drawPath(ellipse_draw_startPath);
            pnt.drawPath(ellipse_draw_endPath);
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
                for(int i=0; i<inundationItems.size(); i++)
                    if (inundationItems[i].path.contains(ev->pos()))
                    { sev="ws_Fig"+TSYS::int2str(i); }
                if( !sev.empty() )
		{
		    if( !runW->hasFocus() ) runW->setFocus(Qt::MouseFocusReason);
		    if( ev->buttons() & Qt::LeftButton )	sev += "Left";
		    if( ev->buttons() & Qt::RightButton ) 	sev += "Right";
		    if( ev->buttons() & Qt::MidButton )      	sev += "Midle";
		    view->attrSet("event",sev);
		    return true;
		}
            }	    
            else if (devW)
            {
                if (flag_down==0 && flag_up==0 && flag_left==0 && flag_right==0)
                {
                    if ((ev->button() == Qt::LeftButton) && (status==false))
                    {
                        //- initialization for holds - 
                        current_ss=-1;
                        current_se=-1;
                        current_es=-1;
                        current_ee=-1;
                        flag_holds=true;
                        // - getting the index of the figure -
                        index = itemAt(ev->pos(),shapeItems,view);
                        index_temp = index;
                        previousPosition_all = ev->pos();
                        previousPosition = ev->pos();
                        count_holds=0;
                        // - getting figures or rect number for moveItemTo -
                        if (index!= -1)
                        {
                            itemInMotion = &shapeItems[index];
                            if ((flag_ctrl && !status_hold) || (flag_ctrl && flag_A) )
                            {
                                bool fn=false;
                                for (int i=0; i<count_Shapes; i++)
                                    if (index_array[i]==index)
                                        fn=true;
                                if (!fn)
                                {
                                    index_array[count_Shapes]=index;
                                    count_Shapes+=1;
                                    itemInMotion=&shapeItems[index];
                                    flag_ctrl_move=1;
                                    offset=QPointF(0,0);
                                    moveItemTo(previousPosition, shapeItems,pnts,view);
                                }
                             }
                            if (status_hold && !flag_A)
                            {
                                Holds(shapeItems,pnts);
                                if (count_holds)
                                {
                                    if (rect_num!= -1)
                                    {
                                        int rect_num_temp=rect_num;
                                        rect_num=Real_rect_num(rect_num,shapeItems,pnts);
                                        itemInMotion = &shapeItems[index];
                                        if ((rect_num==2 || rect_num==3) && shapeItems[index].type==3)
                                            flag_rect=false;
                                        if ( rect_num==0 || rect_num==1)
                                            Rect_num_0_1(shapeItems,rect_num_temp,pnts, view);
                                        if ((rect_num==3 ||rect_num==4) && shapeItems[index].type==2)
                                        {
                                            Prev_pos_1=scale_rotate((*pnts)[shapeItems[index].n1], view, true);
                                            Prev_pos_2=scale_rotate((*pnts)[shapeItems[index].n2], view, true);
                                            Rect_num_3_4(shapeItems,pnts);
                                        }
                                    }
                                    if ( rect_num==-1 )
                                    {
                                        count_moveItemTo=0;
                                        num_vector.clear();
                                        offset=QPointF(0,0);
                                        for (int i=0; i<count_holds; i++)
                                        {
                                            count_moveItemTo+=1;
                                            flag_ctrl_move=0;
                                            flag_ctrl=1;
                                            itemInMotion=&shapeItems[index_array[i]];
                                            index=index_array[i];
                                            moveItemTo(ev->pos(),shapeItems,pnts, view);
                                            view->repaint();
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if(flag_A)
                            {
                                flag_ctrl=false;
                                flag_A=false;
                                index_array.clear();
                                itemInMotion=0;
                                count_Shapes=0;
                            }
                            rectItems.clear();
                            view->repaint();
                        }
                    }
                    // - getting start point for drawing-
                    if ((ev->button() == Qt::LeftButton) && (status==true))
                        StartLine=ev->pos();
                    // - repainting figures by mouse click -
                    if ((ev->button() == Qt::LeftButton) && (itemInMotion || (rect_num!=-1)) && (status==false) && flag_ctrl!=1 && count_holds==0)
                    {
                        count_Shapes=1;
                        count_moveItemTo=1;
                        offset=QPointF(0,0);
                        moveItemTo(ev->pos(),shapeItems,pnts, view);
                        view->repaint();
                    }
                }
        	return true;		
            }
	    break;
        }
        case QEvent::MouseButtonDblClick:
        {
            bool flag_arc_inund=false, flag_break_move, fl_brk;
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if (devW)
            {
                if (!flag_down && !flag_up && !flag_left && !flag_right)
                {
                    index = itemAt(ev->pos(),shapeItems,view);
                    // - getting fill by double click -
                    if( ev->button() == Qt::LeftButton && shapeItems.size() && index==-1 && status_hold )
                    {
                        QBrush fill_brush(QColor(0,0,0,0),Qt::SolidPattern), fill_img_brush;
                        fill_brush.setColor(view->dc()["fillClr"].value<QColor>());
                        fill_img_brush=view->dc()["fillImg"].value<QBrush>();
                        if(!Inundation_1_2(ev->pos(), shapeItems, inundationItems, pnts, view))
                        {
                            if(Build_Matrix(shapeItems)!=2)
                            {
                                fl_brk=false;
                                Inundation (ev->pos(), shapeItems, pnts, vect, Build_Matrix(shapeItems), view);
                                if(InundationPath!=newPath)
                                {
                                    for(int i=0; i<inundationItems.size(); i++)
                                        if (inundationItems[i].path == InundationPath)
                                        {
                                            fl_brk=true;
                                            break;
                                        }
                                    if(!fl_brk)
                                        inundationItems.push_back(InundationItem(InundationPath,fill_brush,fill_img_brush, Inundation_vector));
                                }
                            }
                            Inundation_vector.clear();
                            vect.clear();
                            map_matrix.clear();
                        }
                    }
                    // - deleting the figure -
                    if( ev->button() == Qt::LeftButton && shapeItems.size() && index!=-1 )
                    {

                        switch( shapeItems[index].type )
                        {
                            case 1:
                                Drop_Point (shapeItems[index].n1,index, shapeItems,pnts);
                                Drop_Point (shapeItems[index].n2,index,shapeItems,pnts);
                                break;
                            case 2:
                                Drop_Point (shapeItems[index].n1,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n2,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n3,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n4,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n5,index,shapeItems,pnts);
                                break;
                            case 3:
                                Drop_Point (shapeItems[index].n1,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n2,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n3,index,shapeItems,pnts);
                                Drop_Point (shapeItems[index].n4,index,shapeItems,pnts);
                                break;
                        }
                        // - deleting fill if deleted figure was in the fill's pfth -
                        for (int i=0; i<inundationItems.size(); i++)
                            if (shapeItems[index].type==2 && inundationItems[i].number_shape.size()==1 &&
                                inundationItems[i].number_shape[0]==index)
                        {
                            inundationItems.remove(i);
                            flag_arc_inund=true;
                        }
                        if(!flag_arc_inund)
                        {
                            for(int i=0; i<inundationItems.size(); i++)
                                for(int j=0; j<inundationItems[i].number_shape.size(); j++)
                            {
                                flag_break_move=false;
                                if(inundationItems[i].number_shape[j]==index)
                                {
                                    inundationItems.remove(i);
                                    flag_break_move=true;
                                    break;
                                }
                                if (flag_break_move) break;
                            }
                        }
                        for(int i=0; i<inundationItems.size(); i++)
                            for(int j=0; j<inundationItems[i].number_shape.size(); j++)
                                if(inundationItems[i].number_shape[j]>index)
                                    inundationItems[i].number_shape[j]-=1;
                        shapeItems.remove(index);
                        shapeSave(view);
                        rectItems.clear();
                    }
                    view->repaint();
                }
                shapeSave(view);
            }
            
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if (devW)
            {
                if (!flag_down && !flag_up && !flag_left && !flag_right)
                {
                    flag_cursor=false;
                    if (ev->button() == Qt::LeftButton && itemInMotion && (status==false))
                    {
                        flag_inund_break=false;
                        itemInMotion = &shapeItems[index];
                        index_temp=index;
                        // - connecting the figures -
                        if(status_hold)
                        {
                            if( current_ss!=-1 )
                            {
                                if( itemInMotion->type==2 )
                                {
                                    for (int i=0; i<shapeItems.size(); i++)
                                    {
                                        if ((shapeItems[current_ss].n1==shapeItems[i].n1) && (i!=index))
                                        {
                                            shapeItems[i].n1 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if ((shapeItems[current_ss].n1==shapeItems[i].n2) && (i!=index))
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_ss].n1==itemInMotion->n2) )
                                    {
                                        Drop_Point (itemInMotion->n1,index, shapeItems,pnts);
                                        itemInMotion->n1 = shapeItems[current_ss].n1;
                                        count_moveItemTo=1;
                                        flag_ctrl_move=0;
                                        flag_ctrl=1;
                                        count_Shapes=1;
                                        offset=QPointF(0,0);
                                        moveItemTo(ev->pos(),shapeItems,pnts,view);
                                    }
                            }
                            if(current_se!=-1)
                            {
                                if( itemInMotion->type==2 )
                                {
                                    for (int i=0; i<shapeItems.size(); i++)
                                    {
                                        if ((shapeItems[current_se].n1==shapeItems[i].n1) && (i!=index))
                                        {
                                            shapeItems[i].n1 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if ((shapeItems[current_se].n1==shapeItems[i].n2) && (i!=index))
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                {
                                    if( !(itemInMotion->type==3 && shapeItems[current_se].n1==itemInMotion->n1) )
                                    {
                                        Drop_Point (itemInMotion->n2,index, shapeItems,pnts);
                                        itemInMotion->n2 = shapeItems[current_se].n1;
                                        count_moveItemTo=1;
                                        flag_ctrl_move=0;
                                        flag_ctrl=1;
                                        count_Shapes=1;
                                        offset=QPointF(0,0);
                                        moveItemTo(ev->pos(),shapeItems,pnts,view);
                                    }
                                }
                            }
                            if(current_es!=-1)
                            {
                                if( itemInMotion->type == 2 )
                                {
                                    for (int i=0; i<shapeItems.size(); i++)
                                    {
                                        if ((shapeItems[current_es].n2==shapeItems[i].n1) && (i!=index) )
                                        {
                                            shapeItems[i].n1 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if ((shapeItems[current_es].n2==shapeItems[i].n2) && (i!=index))
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n1;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_es].n2==itemInMotion->n2) )
                                    {
                                        Drop_Point (itemInMotion->n1,index, shapeItems,pnts);
                                        itemInMotion->n1 = shapeItems[current_es].n2;
                                        count_moveItemTo=1;
                                        flag_ctrl_move=0;
                                        flag_ctrl=1;
                                        count_Shapes=1;
                                        offset=QPointF(0,0);
                                        moveItemTo(ev->pos(),shapeItems,pnts, view);
                                    }
                            }
                            if(current_ee!=-1)
                            {
                                if( itemInMotion->type == 2 )
                                {
                                    for (int i=0; i<shapeItems.size(); i++)
                                    {
                                        if ((shapeItems[current_ee].n2==shapeItems[i].n1) && (i!=index))
                                        {
                                            shapeItems[i].n1 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                        if ((shapeItems[current_ee].n2==shapeItems[i].n2) && (i!=index))
                                        {
                                            shapeItems[i].n2 = shapeItems[index].n2;
                                            index = i;
                                            itemInMotion = &shapeItems[index];
                                            count_moveItemTo=1;
                                            count_Shapes=1;
                                            offset=QPointF(0,0);
                                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                                            index=index_temp;
                                            itemInMotion = &shapeItems[index];
                                        } 
                                    }
                                }
                                else
                                    if( !(itemInMotion->type==3 && shapeItems[current_ee].n2==itemInMotion->n1) )
                                    {
                                        Drop_Point (itemInMotion->n2,index, shapeItems,pnts);
                                        itemInMotion->n2 = shapeItems[current_ee].n2;
                                        count_moveItemTo=1;
                                        flag_ctrl_move=0;
                                        flag_ctrl=1;
                                        count_Shapes=1;
                                        offset=QPointF(0,0);
                                        moveItemTo(ev->pos(),shapeItems,pnts,view);
                                    }
                            }
                        }
                        if( itemInMotion->type != 2 )
                        {
                            // - if simple figure and status_hold -
                            if ((!flag_ctrl && status_hold))
                            {
                                count_Shapes=1;
                                count_moveItemTo=1;
                                offset=QPointF(0,0);
                                moveItemTo(ev->pos(),shapeItems,pnts,view);
                            }
                            view->repaint(); 
                        }
                        ellipse_draw_startPath=newPath;//
                        ellipse_draw_endPath=newPath;
                        // - calling moveItemTo for figures, connected with the arc, if there was moving 3 or 4 rexts of the arc -
                        if( count_holds && (flag_arc_rect_3_4 || (flag_rect && shapeItems[index_array[0]].type==2)))
                        {
                            count_moveItemTo=0;
                            flag_ctrl_move=false;
                            flag_ctrl=true;
                            flag_hold_arc=true;
                            offset=QPointF(0,0);
                            Move_all(ev->pos(),shapeItems,pnts, inundationItems,view);
                        }
                        shapeSave( view );
                        itemInMotion = 0;
                        if (status_hold && !flag_A)
                        {
                            count_moveItemTo=0;
                            flag_hold_move=false;
                            flag_ctrl=false;
                            count_Shapes=0;
                            count_holds=0;
                            index_array.clear();
                            if (count_rects && !flag_arc_rect_3_4 && flag_rect)
                                rect_array.clear();
                            if (count_rects && flag_arc_rect_3_4)
                            {
                                arc_rect_array.clear();
                                fig_rect_array.clear();
                            }
                            flag_rect=false;
                            flag_arc_release=false;
                            count_rects=0;
                            flag_hold_arc=false;
                            rect_num_arc=-1;
                            flag_arc_rect_3_4=false;
                        }
                        
                            flag_m=false;

                    }
                    double ang;
                    // - getting the end point of the figure and building it -
                    if ((ev->button() == Qt::LeftButton) && (status==true))
                    { 
                        float Wdth=20.0;
                        QLineF line1,line2;
                        EndLine=ev->pos();
                        if(EndLine==StartLine) break;
                        QPainterPath circlePath, bigPath;
                        //-- if line --
                        if (shapeType==1)
                        {
                            //--- if orto ---
                            if(flag_key)
                            {
                                if ((EndLine.y()-StartLine.y())>20 || (StartLine.y()-EndLine.y())>20)
                                {
                                    if (StartLine.y()<=EndLine.y()) ang=270;
                                    else ang=90;
                                    EndLine=QPointF(StartLine.x(), EndLine.y());
                                }
                                else
                                {
                                    if (StartLine.x()<=EndLine.x()) ang=0;
                                    else ang=180;
                                    EndLine=QPointF(EndLine.x(), StartLine.y());
                                }
                                if ( view->dc()["bordWdth"].toInt()>0)
                                {
                                    circlePath = painter_path(view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, ang, StartLine,EndLine);
                                    shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0),QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                            view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                                }
                                else
                                {
                                    circlePath = painter_path_simple(1, ang, StartLine,EndLine);
                                    QPainterPath bigPath = painter_path(view->dc()["lineWdth"].toInt()+1, view->dc()["bordWdth"].toInt(), 1, ang, StartLine,EndLine);
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                            view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(),1) );
                                }
                                StartLine=unscale_unrotate(StartLine, view, true);
                                EndLine=unscale_unrotate(EndLine, view, true);
                                shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems,pnts);
                                shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                                shapeSave( view );
                                view->repaint();
                            }
                            //--- if !orto ---
                            else
                            {
                                line2=QLineF(StartLine,QPointF(StartLine.x()+10,StartLine.y()));
                                line1=QLineF(StartLine,EndLine);
                                if (StartLine.y()<=EndLine.y())
                                    ang=360-line1.angle(line2);
                                else
                                    ang=line1.angle(line2);
                               
                                if ( view->dc()["bordWdth"].toInt()>0)
                                {
                                    circlePath = painter_path(view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(),1, ang, StartLine,EndLine);
                                    shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                            view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                                }
                                else
                                {
                                    circlePath = painter_path_simple(1, ang, StartLine,EndLine);
                                    QPainterPath bigPath = painter_path(view->dc()["lineWdth"].toInt()+1,view->dc()["bordWdth"].toInt(),1, ang, StartLine,EndLine);
                                    shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                            QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                    QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                            view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                                }
                                StartLine=unscale_unrotate(StartLine, view, true);
                                EndLine=unscale_unrotate(EndLine, view, true);
                                shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems,pnts);
                                shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                                shapeSave( view );
                                view->repaint();
                            }
                        }
                        //-- if bezier --
                        if (shapeType==3)
                        {
                            QPointF CtrlPos_1,CtrlPos_2,EndLine_temp;
                            CtrlPos_1=QPointF(Length(EndLine,StartLine)/3,0);
                            CtrlPos_2=QPointF(2*Length(EndLine,StartLine)/3,0);
                            line2=QLineF(StartLine,QPointF(StartLine.x()+10,StartLine.y()));
                            line1=QLineF(StartLine,EndLine);
                            if (StartLine.y()<=EndLine.y())
                                ang=360-line1.angle(line2);
                            else
                                ang=line1.angle(line2);
                            CtrlPos_1=QPointF(StartLine.x()+ROTATE(CtrlPos_1,ang).x(),StartLine.y()-ROTATE(CtrlPos_1,ang).y());
                            CtrlPos_2=QPointF(StartLine.x()+ROTATE(CtrlPos_2,ang).x(),StartLine.y()-ROTATE(CtrlPos_2,ang).y());
                            if ( view->dc()["bordWdth"].toInt()>0)
                            {
                                circlePath = painter_path(view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(),3, ang, StartLine,EndLine,CtrlPos_1,CtrlPos_2);
                                shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                      view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(), 3) );
                            }
                            else
                            {
                                bigPath = painter_path(view->dc()["lineWdth"].toInt()+1,view->dc()["bordWdth"].toInt(),3, ang, StartLine,EndLine, CtrlPos_1,CtrlPos_2);
                                circlePath = painter_path_simple(3, ang, StartLine,EndLine,CtrlPos_1,CtrlPos_2);
                                shapeItems.push_back( ShapeItem(bigPath,circlePath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 3) );
                            }
                            StartLine=unscale_unrotate(StartLine, view, true);
                            EndLine=unscale_unrotate(EndLine, view, true);
                            CtrlPos_1=unscale_unrotate(CtrlPos_1, view, true);
                            CtrlPos_2=unscale_unrotate(CtrlPos_2, view, true);
                            
                            shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems,pnts);
                            shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n3 = Append_Point(CtrlPos_1,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n4 = Append_Point(CtrlPos_2,shapeItems, pnts);
                            shapeSave( view );
                            view->repaint();
                        }
                        //-- if arc --
                        if (shapeType==2)
                        {
                            QPointF CtrlPos_1,CtrlPos_2,CtrlPos_3,CtrlPos_4,Temp,StartLine_small,EndLine_small,pnt;
                            double a,a_small,b,b_small;
                            double t;
                            CtrlPos_1=QPointF(StartLine.x()+(EndLine.x()-StartLine.x())/2,
                                              StartLine.y()+(EndLine.y()-StartLine.y())/2);
                            a=Length(EndLine,StartLine)/2;
                            b=a+50;
                            line2=QLineF( CtrlPos_1,QPointF(CtrlPos_1.x()+10,CtrlPos_1.y()));
                            line1=QLineF( CtrlPos_1,StartLine);
                            if (StartLine.y()<=EndLine.y()) ang=line1.angle(line2);
                            else ang=360-line1.angle(line2);
                            CtrlPos_2=QPointF(CtrlPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                              CtrlPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
                            CtrlPos_3=StartLine;
                            CtrlPos_4=QPointF(0,0.5);

                            if ( view->dc()["bordWdth"].toInt()>0)
                            {
                                circlePath = painter_path(view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(),2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2,  CtrlPos_3, CtrlPos_4);
                                shapeItems.push_back( ShapeItem(circlePath, newPath, -1,-1,-1,-1, -1,CtrlPos_4,QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2) );
                            }
                            else
                            {
                                QPainterPath bigPath = painter_path( view->dc()["lineWdth"].toInt()+1,view->dc()["bordWdth"].toInt(), 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                circlePath = painter_path_simple( 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                                shapeItems.push_back( ShapeItem(bigPath,circlePath,-1,-1,-1,-1, -1,CtrlPos_4, QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin),
                                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2) );
                            }
                            StartLine=unscale_unrotate(StartLine, view, true);
                            EndLine=unscale_unrotate(EndLine, view, true);
                            CtrlPos_1=unscale_unrotate(CtrlPos_1, view, true);
                            CtrlPos_2=unscale_unrotate(CtrlPos_2, view, true);
                            CtrlPos_3=unscale_unrotate(CtrlPos_3, view, true);
                            shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems,pnts);
                            shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n3 = Append_Point(CtrlPos_1,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n4 = Append_Point(CtrlPos_2,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n5 = Append_Point(CtrlPos_3,shapeItems, pnts);
                            shapeSave( view );
                            view->repaint();
                        }
                        flag_key=0;
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
            bool flag_break_move, flag_arc_inund=false;
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            DevelWdgView *devW = qobject_cast<DevelWdgView*>(view);
            RunWdgView   *runW = qobject_cast<RunWdgView*>(view);
            if (devW)
               {
                Mouse_pos=ev->pos();

                if ((ev->buttons() && Qt::LeftButton) && itemInMotion && !status)
                {
                    flag_m=true;
                    if (count_holds)
                    {
                        count_Shapes=count_holds+1; 
                        flag_ctrl=true;
                        flag_hold_move=true;
                    }
                    //- calling moveItemTo for all connected figures -
                    if (flag_ctrl && count_Shapes && ((rect_num==-1 && rect_num_arc==-1) || flag_rect || flag_arc_rect_3_4))
                    {
                        offset=ev->pos()-previousPosition_all;
                        count_moveItemTo=0;
                        if (flag_rect || flag_arc_rect_3_4)
                            count_Shapes=count_rects;
                        flag_hold_arc=false;
                        if (shapeItems[index_array[0]].type==2)
                            flag_hold_arc=true;
                        Move_all(ev->pos(),shapeItems,pnts,inundationItems,view);
                        view->repaint();
                    }
                    // - moving the rect, which belongs only for one figure -
                    else
                    {
                        if (count_holds)
                        {
                            offset=ev->pos()-previousPosition_all;
                            flag_ctrl=true;
                            flag_rect=false;
                            count_moveItemTo=1;
                            flag_ctrl_move=false;
                            count_Shapes=1;
                            itemInMotion=&shapeItems[index];
                            if (rect_num==2 && itemInMotion->type==2)
                            {
                                offset=QPointF(0,0);
                            }
                            if (itemInMotion->type==2 && rect_num!=2) rect_num=rect_num_arc;
                            num_vector.clear();
                            moveItemTo(ev->pos(),shapeItems,pnts,view);
                            if(inundationItems.size())
                            {
                                for(int i=0; i<inundationItems.size(); i++)
                                    for(int j=0; j<inundationItems[i].number_shape.size(); j++)
                                {
                                    flag_break_move=false;
                                        if(inundationItems[i].number_shape[j]==index)
                                    {
                                        InundationPath=Create_Inundation_Path(inundationItems[i].number_shape,shapeItems,pnts, view);
                                        inundationItems[i].path=InundationPath;
                                        flag_break_move=true;
                                        break;
                                    }
                                    if (flag_break_move) break;
                                }
                            }
                            flag_hold_rect=true;
                            view->repaint();
                        }
                        else
                        {
                            if(!flag_ctrl)
                            {
                                for (int i=0; i<shapeItems.size(); i++)
                                {
                                    if (i!=index)
                                    {
                                        if (shapeItems[index].type==2)
                                        {
                                            if ( (shapeItems[index].n5==shapeItems[i].n1) || 
                                                  (shapeItems[index].n5==shapeItems[i].n2) ||
                                                  (shapeItems[index].n5==shapeItems[i].n3) || 
                                                  (shapeItems[index].n5==shapeItems[i].n4) ||
                                                  (shapeItems[index].n5==shapeItems[i].n5) 
                                               )
                                            {
                                                QPointF Temp=QPointF((*pnts)[shapeItems[index].n5].x(),(*pnts)[shapeItems[index].n5].y());
                                                shapeItems[index].n5 = Append_Point(Temp,shapeItems,pnts);
                                            }
                                        }
                                        if (shapeItems[index].type==2 || shapeItems[index].type==3)
                                        {
                                            if ( (shapeItems[index].n4==shapeItems[i].n1) || 
                                                  (shapeItems[index].n4==shapeItems[i].n2) ||
                                                  (shapeItems[index].n4==shapeItems[i].n3) || 
                                                  (shapeItems[index].n4==shapeItems[i].n4) ||
                                                  (shapeItems[index].n4==shapeItems[i].n5) 
                                               )
                                            {
                                                QPointF Temp=QPointF((*pnts)[shapeItems[index].n4].x(),(*pnts)[shapeItems[index].n4].y());
                                                shapeItems[index].n4 = Append_Point(Temp,shapeItems,pnts);
                                            }
                                            if ( (shapeItems[index].n3==shapeItems[i].n1) || 
                                                  (shapeItems[index].n3==shapeItems[i].n2) ||
                                                  (shapeItems[index].n3==shapeItems[i].n3) || 
                                                  (shapeItems[index].n3==shapeItems[i].n4) ||
                                                  (shapeItems[index].n3==shapeItems[i].n5) 
                                               )
                                            {
                                                QPointF Temp=QPointF((*pnts)[shapeItems[index].n3].x(),(*pnts)[shapeItems[index].n3].y());
                                                shapeItems[index].n3 = Append_Point(Temp,shapeItems,pnts);
                                            }
                                        }
                                        if( (shapeItems[index].n1==shapeItems[i].n1) || 
                                             (shapeItems[index].n1==shapeItems[i].n2) ||
                                             (shapeItems[index].n1==shapeItems[i].n3) || 
                                             (shapeItems[index].n1==shapeItems[i].n4) ||
                                             (shapeItems[index].n1==shapeItems[i].n5) 
                                          )
                                        {
                                            QPointF Temp=QPointF((*pnts)[shapeItems[index].n1].x(),(*pnts)[shapeItems[index].n1].y());
                                            shapeItems[index].n1 = Append_Point(Temp,shapeItems,pnts);
                                        }
                                        if( (shapeItems[index].n2==shapeItems[i].n1) || 
                                             (shapeItems[index].n2==shapeItems[i].n2) ||
                                             (shapeItems[index].n2==shapeItems[i].n3) ||
                                             (shapeItems[index].n2==shapeItems[i].n4) ||
                                             (shapeItems[index].n2==shapeItems[i].n5) )
                                        {
                                            QPointF Temp=QPointF((*pnts)[shapeItems[index].n2].x(),(*pnts)[shapeItems[index].n2].y());
                                            shapeItems[index].n2 = Append_Point(Temp,shapeItems,pnts);
                                        }
                                    }
                                }
                                count_Shapes=1;
                                count_moveItemTo=1;
                                offset=ev->pos()-previousPosition_all;
                                itemInMotion=&shapeItems[index];
                                moveItemTo(ev->pos(),shapeItems,pnts,view);
                                //- deleting fill's -
                                if(inundationItems.size())
                                    for (int i=0; i<inundationItems.size(); i++)
                                    {
                                        if (shapeItems[index].type==2 && inundationItems[i].number_shape.size()==1 &&
                                            inundationItems[i].number_shape[0]==index)
                                        {
                                            if(rect_num==0 || rect_num==1)
                                            {
                                                inundationItems.remove(i);
                                                shapeSave(view);
                                                flag_arc_inund=true;
                                            }
                                            else
                                            {
                                                inundationItems[i].path=Create_Inundation_Path(inundationItems[i].number_shape,shapeItems,pnts,view);
                                                flag_arc_inund=true;
                                            }
                                        }
                                    }
                                if(inundationItems.size() && !flag_inund_break && !flag_arc_inund)
                                {
                                    for(int i=0; i<inundationItems.size(); i++)
                                        for(int j=0; j<inundationItems[i].number_shape.size(); j++)
                                    {
                                        flag_break_move=false;
                                        if(inundationItems[i].number_shape[j]==index)
                                        {
                                            inundationItems.remove(i);
                                            shapeSave(view);
                                            flag_break_move=true;
                                            break;
                                        }
                                        if (flag_break_move) break;
                                    }
                                    flag_inund_break=true;
                                }
                                flag_arc_inund=false;
                                view->repaint();
                            }
                        }
                        if (rect_num!=-1)
                            temp=Real_rect_num (rect_num,shapeItems,pnts);
                        //- if the figure or it's rect is not connected to other one -
                        if (status_hold &&(rect_num==-1||((temp==0 || temp==1) && !flag_rect)))
                        {
                            current_se=-1;
                            current_ss=-1;
                            current_ee=-1;
                            current_es=-1;
                            ellipse_draw_startPath=newPath;
                            ellipse_draw_endPath=newPath;
                            itemInMotion=&shapeItems[index];
                            //- drawing the ellipse for connecting points -
                            for (int i=0;i<=shapeItems.size()-1;i++)
                            {
                                ellipse_startPath=newPath;
                                ellipse_endPath=newPath;
                                if (i!=index)
                                {
                                    ellipse_startPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n1], view, true).x()-8, scale_rotate((*pnts)[shapeItems[i].n1], view, true).y()-8,16,16);
                                    if( ellipse_startPath.contains(scale_rotate((*pnts)[shapeItems[index].n1], view, true)))
                                    { 
                                        if (temp==0 || rect_num==-1)
                                        {
                                            if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                            ellipse_draw_startPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n1], view, true).x()-8,scale_rotate((*pnts)[shapeItems[i].n1], view, true).y()-8,16,16);
                                            current_ss=i;
                                        }
                                    }
                                    if( ellipse_startPath.contains(scale_rotate((*pnts)[shapeItems[index].n2], view, true)))
                                    {
                                        if (temp==1 || rect_num==-1)
                                        {
                                            if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                            ellipse_draw_startPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n1], view, true).x()-8,scale_rotate((*pnts)[shapeItems[i].n1], view, true).y()-8,16,16);
                                            current_se=i;
                                        }
                                    }
                                    ellipse_endPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n2], view, true).x()-8, scale_rotate((*pnts)[shapeItems[i].n2], view, true).y()-8,16,16);
                                    if( ellipse_endPath.contains(scale_rotate((*pnts)[shapeItems[index].n2], view, true)) )
                                    {
                                        if (temp==1 || rect_num==-1)
                                        {
                                            if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                            ellipse_draw_endPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n2], view, true).x()-8,scale_rotate((*pnts)[shapeItems[i].n2], view, true).y()-8,16,16);
                                            current_ee=i;
                                        }
                                    }
                                    if( ellipse_endPath.contains(scale_rotate((*pnts)[shapeItems[index].n1], view, true)) )
                                    {
                                        if (temp==0 || rect_num==-1)
                                        {
                                            if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                            ellipse_draw_endPath.addEllipse(scale_rotate((*pnts)[shapeItems[i].n2], view, true).x()-8,scale_rotate((*pnts)[shapeItems[i].n2], view, true).y()-8,16,16);
                                            current_es=i;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (flag_cursor==0)
                {
                    if (flag_down || flag_left || flag_right || flag_up) break;

                    if (flag_first_move && !flag_A)
                    {
                        shapeSave( view );
                        flag_inund_break=false;
                        offset=QPointF(0,0);
                        if (flag_ctrl && status_hold)
                        {
                            count_moveItemTo=0;
                            flag_hold_move=false;
                            if (flag_rect)
                            {
                                offset=QPointF(0,0);
                                rect_array.clear();
                            }
                            if (flag_arc_rect_3_4)
                            {
                                offset=QPointF(0,0);
                                arc_rect_array.clear();
                                fig_rect_array.clear();
                            }
                            count_Shapes=0;
                            count_holds=0;
                            count_rects=0;
                            rect_num_arc=-1;
                            flag_ctrl=false;
                            flag_rect=false;
                            flag_arc_release=false;
                            flag_arc_rect_3_4=false;
                            flag_hold_rect=false;
                            index_array.clear();
                        }
                        flag_first_move=false;  
                    }
                    if(!flag_m)
                    {
                        view->unsetCursor();
                        fl = itemAt(ev->pos(),shapeItems,view);
                        if ((fl!= -1)&&(rect_num ==-1)&&(status==false))
                            view->setCursor(Qt::SizeAllCursor);
                        if ((status==false)&&(rect_num!=-1))
                            view->setCursor(Qt::SizeHorCursor);
                        if (status==true)
                            view->setCursor(Qt::CrossCursor);
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
            if (devW)
            {
                if(flag_m)  break;
                if (ev->key() == Qt::Key_Shift) flag_key=true;
                if (ev->key() == Qt::Key_Control)
                {
                    if (status_hold) break;
                    flag_ctrl=true;
                    for(int i=0; i<shapeItems.size(); i++)
                        index_array.push_back(-1);
                    count_Shapes=0;
                }
                if ((ev->key() == Qt::Key_A) && !status)
                {
                    if( !(QApplication::keyboardModifiers()&Qt::ControlModifier) )	break;
                    flag_A=true;
                    flag_ctrl_move=1;
                    if(index_array.size())
                        index_array.clear();
                    for(int i=0; i<shapeItems.size(); i++)
                        index_array.push_back(i);
                    count_Shapes=shapeItems.size();
                    Move_all(QPointF(0,0),shapeItems,pnts,inundationItems,view);
                    view->repaint();
                }
                if ((ev->key() == Qt::Key_Up) && (index_temp!=-1 || flag_A))
                {
                    if ((flag_down || flag_left || flag_right || index<0 || index>shapeItems.size()-1) && !flag_A)
                    {
                        break;
                    }
                    flag_up=true;
                    offset=QPointF(0,-1);
                    Move_UP_DOWN(shapeItems,pnts, inundationItems,view);
                    view->repaint();
                }
                if ((ev->key() == Qt::Key_Down) && (index_temp!=-1 || flag_A))
                {
                    if ((flag_up || flag_left || flag_right || index<0|| index>shapeItems.size()-1) && !flag_A) break;
                    flag_down=true;
                    offset=QPointF(0,1);
                    Move_UP_DOWN(shapeItems,pnts, inundationItems,view);
                    view->repaint();
                }

                if ((ev->key() == Qt::Key_Left) && (index_temp!=-1 || flag_A))
                {
                    if ((flag_down || flag_up || flag_right || index<0 || index>shapeItems.size()-1) && !flag_A) break;
                    flag_left=true;
                    offset=QPointF(-1,0);
                    Move_UP_DOWN(shapeItems,pnts, inundationItems,view);
                    view->repaint();
                }
                if ((ev->key() == Qt::Key_Right) && (index_temp!=-1 || flag_A))
                {
                    if ((flag_down || flag_left || flag_up || index<0 || index>shapeItems.size()-1) && !flag_A) break;
                    flag_right=true;
                    offset=QPointF(1,0);
                    Move_UP_DOWN(shapeItems,pnts, inundationItems,view);
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
            if (devW)
            {
                if (ev->key() == Qt::Key_Shift) flag_key=false;
                if (ev->key() == Qt::Key_Control)
                {
                    if(status_hold || flag_A) break;
                    if(count_Shapes)
                    {
                        count_Shapes=0;
                        rectItems.clear();
                        index_array.clear();
                        view->repaint();
                        itemInMotion=0;
                        index_temp=-1;
                        flag_ctrl=false;
                    }
                    else
                        flag_ctrl=false;
                }
                if (ev->key() == Qt::Key_Up)
                {
                    if (flag_down || flag_left || flag_right) break;
                    flag_up=false;
                    itemInMotion =0;
                    flag_first_move=true;

                }
                if (ev->key() == Qt::Key_Down)
                {
                    if (flag_up || flag_left || flag_right) break;
                    flag_down=false;
                    itemInMotion =0;
                    flag_first_move=true;
                }
                if (ev->key() == Qt::Key_Left)
                {
                    if (flag_down || flag_up || flag_right) break;
                    flag_left=false;
                    itemInMotion =0;
                    flag_first_move=true;
                }
                if (ev->key() == Qt::Key_Right)
                {
                    if (flag_down || flag_left || flag_up) break;
                    flag_right=false;
                    itemInMotion =0;
                    flag_first_move=true;
                }
        	return true;		
            }
        }
	break;
    }

    return false;
} 

void ShapeElFigure::populateScene()
{
    scene = new QGraphicsScene;
}

//- rotation of the point -
QPointF ShapeElFigure::ROTATE(const QPointF &pnt, double alpha)
{
    QPointF rotate_pnt;
    rotate_pnt=QPointF (pnt.x()*cos((alpha*M_PI)/180)-pnt.y()*sin((alpha*M_PI)/180),
                        pnt.x()*sin((alpha*M_PI)/180)+pnt.y()*cos((alpha*M_PI)/180));
    return  rotate_pnt;
}

//- unrotation of the point -
QPointF ShapeElFigure::UNROTATE(const QPointF &pnt, double alpha, double a, double b)
{
    QPointF unrotate_pnt;
    unrotate_pnt=QPointF ((pnt.x()-a)*cos((alpha*M_PI)/180)-(pnt.y()-b)*sin((alpha*M_PI)/180),
                           -(pnt.x()-a)*sin((alpha*M_PI)/180)-(pnt.y()-b)*cos((alpha*M_PI)/180));
    return  unrotate_pnt;
}

//- getting the point of the arc, using t as parameter and a,b as radiuses -
QPointF ShapeElFigure::ARC(double t,double a,double b)
{
    QPointF arc_pnt;  
    arc_pnt=QPointF(a*cos(t*M_PI*2),-b*sin(t*M_PI*2));
    return arc_pnt;
}

//- getting the rotation angle -
double ShapeElFigure::Angle(const QLineF &l,const QLineF &l1)
{
    if (l.isNull() || l1.isNull())  return 0;
    double cos_line = (l.dx()*l1.dx() + l.dy()*l1.dy()) / (l.length()*l1.length());
    double rad = 0;
    if (cos_line >= -1.0 && cos_line <= 1.0) rad = acos( cos_line );
    return rad * 180 / M_PI;
}

//- detecting the length between two points -
double ShapeElFigure::Length(const QPointF pt1, const QPointF pt2)
{
    double x = pt2.x() - pt1.x();
    double y = pt2.y() - pt1.y();
    return sqrt(x*x + y*y);
}
//- moving the figure -
void ShapeElFigure::moveItemTo(const QPointF &pos,QVector <ShapeItem> &shapeItems, PntMap *pnts, WdgView *view)
{
    ShapeItem temp_shape;
    double ang_t;
    double a,a_small,b,b_small;
    double ang;
    QPointF StartMotionPos;
    QPointF EndMotionPos;
    QPointF CtrlMotionPos_1;
    QPointF CtrlMotionPos_2;
    QPointF CtrlMotionPos_3;
    QPointF CtrlMotionPos_4;
    int MotionNum_1=itemInMotion->n1;
    int MotionNum_2=itemInMotion->n2;
    int MotionNum_3=itemInMotion->n3;
    int MotionNum_4=itemInMotion->n4;
    int MotionNum_5=itemInMotion->n5;
    shapeType= itemInMotion->type;
    QBrush MotionBrush=itemInMotion->brush;
    QPen MotionPen=itemInMotion->pen;
    QPen MotionPenSimple=itemInMotion->penSimple;
    float MotionWidth=itemInMotion->width;
    float MotionBorderWidth=itemInMotion->border_width;
    QLineF line1,line2;
    QPointF Temp,EndMotionPos_temp,CtrlMotionPos_1_temp,CtrlMotionPos_2_temp;
    bool flag_MotionNum_1=false;
    bool flag_MotionNum_2=false;
    bool flag_MotionNum_3=false;
    bool flag_MotionNum_4=false;
    bool flag_MotionNum_5=false;
    for (int i=0; i<num_vector.size(); i++)
    {
        if (num_vector[i]==MotionNum_1) flag_MotionNum_1=true;
        if (num_vector[i]==MotionNum_2) flag_MotionNum_2=true;
        if (num_vector[i]==MotionNum_3) flag_MotionNum_3=true;
        if (num_vector[i]==MotionNum_4) flag_MotionNum_4=true;
        if (num_vector[i]==MotionNum_5) flag_MotionNum_5=true;
    }
    //- moving the whole figure -
    if (rect_num==-1)

    {
        if ((status_hold && count_holds>1) || (flag_ctrl && count_Shapes>1))
        {
            if (!flag_MotionNum_1 )// moving this point in the first time
            {
                StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
                StartMotionPos+=offset;
                num_vector.append(MotionNum_1);//adding this point in the vector of common points
            }
            else StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);// don't change point's coordinates
            if (!flag_MotionNum_2)
            {
                EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
                EndMotionPos+=offset;
                num_vector.append(MotionNum_2);
            }
            else EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
            if (!flag_MotionNum_3)
            {
                CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                CtrlMotionPos_1+=offset;
                num_vector.append(MotionNum_1);
            }
            else    CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            if (!flag_MotionNum_4)
            {
                CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
                CtrlMotionPos_2+=offset;
                num_vector.append(MotionNum_2);
            }
            else   CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            if (!flag_MotionNum_5)
            {
                CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
                CtrlMotionPos_3+=offset;
                num_vector.append(MotionNum_2);
            }
            else   CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
        }
        else
        {
            StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
            EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
            StartMotionPos+=offset;
            EndMotionPos+=offset; 
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_1+=offset;
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_2+=offset;
        }  
        if (shapeType==2)
        {
            CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
            CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
            CtrlMotionPos_3+=offset;
            a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_1,CtrlMotionPos_2);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_3);
            if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            t_start=CtrlMotionPos_4.x();
            t_end=CtrlMotionPos_4.y();
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
        }
    }
    //- moving the start point of the figure -
    if (rect_num==0)
    {
        EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);

        if (shapeType==2)
        {
            StartMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
                StartMotionPos=(*pnts)[itemInMotion->n1]+offset;
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
            CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
            a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            t_end=CtrlMotionPos_4.y();
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+100,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_3);
            if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            StartMotionPos=UNROTATE(StartMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            if (StartMotionPos.x()>=a)
            {
                StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*a);
                StartMotionPos.setX(a);
            }
            if (StartMotionPos.x()<-a)
            {
                StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*(-a));
                StartMotionPos.setX(-a);
            }
            if(StartMotionPos.y()<=0)
                t_start=acos(StartMotionPos.x()/a)/(2*M_PI);
            else
                t_start=1-acos(StartMotionPos.x()/a)/(2*M_PI);
            if (t_start<0) t_start=1+t_start;  
            if (t_start>t_end) t_end+=1;
            if ((t_end-1)>t_start) t_end-=1;
            if (t_start==t_end) t_end+=1;
            if (t_end>t_start && t_start>=1 && t_end>1)
            {
                t_start-=1;
                t_end-=1;
            }
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
        }
        else
        {
        if (!flag_hold_arc && !flag_arc_rect_3_4 )// if the figure is not connected to the arc
            {
                if (status_hold && count_holds && flag_rect)
                    if (!flag_MotionNum_1)
                    {
                        StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
                        StartMotionPos+=offset;
                        num_vector.append(MotionNum_1);
                    }
                    else  StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
                else 
                {
                    StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
                    StartMotionPos+=offset;
                }
                CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            }
            if (flag_hold_arc || flag_arc_rect_3_4)// if the figure is connected to the arc
            {
                if (arc_rect==0)
                {
                    StartMotionPos=scale_rotate((*pnts)[shapeItems[index_array[0]].n1], view, true);
                    CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                    CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
                }
                if (arc_rect==1)
                {
                    StartMotionPos=scale_rotate((*pnts)[shapeItems[index_array[0]].n2], view, true);
                    CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                    CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
                }
            }
        }
    }
    //- moving the end point of the figure -
    if (rect_num==1)
    {
        StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
        if (shapeType==2)
        {
            EndMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
            {
                EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
                EndMotionPos+=offset;
            }
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
            CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_3);
            if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            EndMotionPos=UNROTATE(EndMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            if (EndMotionPos.x()<=-a)
            {
                EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(-a));
                EndMotionPos.setX(-a);
            }
            if (EndMotionPos.x()>a)
            {
                EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(a));
                EndMotionPos.setX(a);
            }
            if(EndMotionPos.y()<=0)
                t_end=acos(EndMotionPos.x()/a)/(2*M_PI);
            else
                t_end=1-acos(EndMotionPos.x()/a)/(2*M_PI);
            if (t_start>t_end) t_end+=1;
            if ((t_end-1)>t_start) t_end-=1;
            if (t_start==t_end) t_end+=1;
            if (t_end>t_start && t_start>=1 && t_end>1)
            {
                t_start-=1;
                t_end-=1;
            }
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
        }
        else
        {
            if (!flag_hold_arc && !flag_arc_rect_3_4)
            { 
                if (status_hold && count_holds && flag_rect)
                    if (!flag_MotionNum_2)
                    {
                        EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
                        EndMotionPos+=offset;
                        num_vector.append(MotionNum_2);
                    }
                    else EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
                else
                {
                    EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
                    EndMotionPos+=offset;
                }
                CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            }
            if (flag_hold_arc || flag_arc_rect_3_4)
            {
                if (arc_rect==0)
                {
                    EndMotionPos=scale_rotate((*pnts)[shapeItems[index_array[0]].n1], view, true);
                    CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                    CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
                }
                if (arc_rect==1)
                {
                    EndMotionPos=scale_rotate((*pnts)[shapeItems[index_array[0]].n2], view, true);
                    CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
                    CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
                }
            }

        }
    }
    //- moving the first control point of the figure -
    if (rect_num==2)
    {
        StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
        EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
        if (shapeType==2)
        {
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
            CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_3);
            if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            t_start=CtrlMotionPos_4.x();
            t_end=CtrlMotionPos_4.y();
        }
        else
        {
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_1+=offset;
        }
    }
    //- moving the second control point of the figure -
    if (rect_num==3)
    {
        StartMotionPos=scale_rotate((*pnts)[itemInMotion->n1], view, true);
        EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
        if (shapeType==2)
        {
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_2+=offset;
            CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
            CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
            EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
            a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            t_start=CtrlMotionPos_4.x();
            t_end=CtrlMotionPos_4.y();
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF( CtrlMotionPos_1,CtrlMotionPos_3);
            if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2); 
            else ang=360-Angle(line1,line2);
            CtrlMotionPos_3=UNROTATE(CtrlMotionPos_3,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            CtrlMotionPos_2=UNROTATE(CtrlMotionPos_2,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            line2=QLineF(QPointF(0,0),QPointF(-100,0));
            line1=QLineF(QPointF(0,0),CtrlMotionPos_2);
            ang_t=Angle(line1,line2)-90;
            ang=ang+ang_t;
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
        }
        else
        {
            CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
            CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
            CtrlMotionPos_2+=offset;
        }
    }
    //- moving the third control point of the figure -
    if (rect_num==4)
    {
        CtrlMotionPos_1=scale_rotate((*pnts)[itemInMotion->n3], view, true);
        CtrlMotionPos_2=scale_rotate((*pnts)[itemInMotion->n4], view, true);
        CtrlMotionPos_3=scale_rotate((*pnts)[itemInMotion->n5], view, true);
        CtrlMotionPos_3+=offset;
        CtrlMotionPos_4=QPointF(itemInMotion->ctrlPos4.x(),itemInMotion->ctrlPos4.y());
        EndMotionPos=scale_rotate((*pnts)[itemInMotion->n2], view, true);
        a=Length(CtrlMotionPos_3,CtrlMotionPos_1);
        b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
        t_start=CtrlMotionPos_4.x();
        t_end=CtrlMotionPos_4.y();
        line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+100,CtrlMotionPos_1.y()));
        line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_3);
        if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
        else ang=360-Angle(line1,line2);
        StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                               CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
        EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                             CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
    }
    shapeItems.remove(index);
    if (!flag_ctrl || (!flag_ctrl_move && count_Shapes==count_moveItemTo+count_Shapes-1))
        rectItems.clear();
    //- building the line -
    if (shapeType==1)
    {
        line2=QLineF(StartMotionPos,QPointF(StartMotionPos.x()+10,StartMotionPos.y()));
        line1=QLineF(StartMotionPos,EndMotionPos);
        if (StartMotionPos.y()<=EndMotionPos.y()) ang=360-Angle(line1,line2);
        else ang=Angle(line1,line2);
        if(MotionWidth==1 && MotionBorderWidth==0)
            shapeItems.append( ShapeItem(painter_path(MotionWidth+1,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos), painter_path_simple(1, ang, StartMotionPos,EndMotionPos),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1) );
        else
            shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos), painter_path_simple(1, ang, StartMotionPos,EndMotionPos),
                               MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1) );
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
	
        rectItems.append( RectItem(rectPath ,MotionNum_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        StartMotionPos=unscale_unrotate(StartMotionPos, view, true);
        EndMotionPos=unscale_unrotate(EndMotionPos, view, true);
        (*pnts).insert(MotionNum_1,StartMotionPos);
        (*pnts).insert(MotionNum_2,EndMotionPos);
    }
    //- building the arc -
    if (shapeType==2)
    {
        double t;
        CtrlMotionPos_2=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                CtrlMotionPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
        CtrlMotionPos_4=QPointF(t_start,t_end);
        CtrlMotionPos_3=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                CtrlMotionPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
        if(MotionWidth==1 && MotionBorderWidth==0)
            shapeItems.append( ShapeItem(painter_path(MotionWidth+1,MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                               painter_path_simple(2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                                       MotionNum_1,MotionNum_2,MotionNum_3,MotionNum_4,MotionNum_5,CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2) );
        else
            shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                               painter_path_simple(2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                                               MotionNum_1,MotionNum_2,MotionNum_3,MotionNum_4,MotionNum_5,CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2) );
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath,MotionNum_1,QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF( CtrlMotionPos_1.toPoint(),QSize(8,8)));
        rectItems.append( RectItem(rectPath , MotionNum_3, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(CtrlMotionPos_2.x()-4,CtrlMotionPos_2.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath,MotionNum_4, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        Temp=QPointF(CtrlMotionPos_3.x()-20,CtrlMotionPos_3.y()-4);
        rectPath.addRect(QRectF(Temp,QSize(8,8)));
        rectItems.append( RectItem(rectPath, MotionNum_5, QBrush(QColor(0,0,0,255),Qt::SolidPattern), 
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin)) );
        rectPath=newPath;
        
        StartMotionPos=unscale_unrotate(StartMotionPos, view, true);
        EndMotionPos=unscale_unrotate(EndMotionPos, view, true);
        CtrlMotionPos_1=unscale_unrotate(CtrlMotionPos_1, view, true);
        CtrlMotionPos_2=unscale_unrotate(CtrlMotionPos_2, view, true);
        CtrlMotionPos_3=unscale_unrotate(CtrlMotionPos_3, view, true);
        (*pnts).insert(MotionNum_1,StartMotionPos);
        (*pnts).insert(MotionNum_2,EndMotionPos);
        (*pnts).insert(MotionNum_3,CtrlMotionPos_1);
        (*pnts).insert(MotionNum_4,CtrlMotionPos_2);
        (*pnts).insert(MotionNum_5,CtrlMotionPos_3);
    }
    //- building the bezier curve -
    if (shapeType==3)
    {
        line2=QLineF(StartMotionPos,QPointF(StartMotionPos.x()+10,StartMotionPos.y()));
        line1=QLineF(StartMotionPos,EndMotionPos);
        if (StartMotionPos.y()<=EndMotionPos.y())
            ang=360-line1.angle(line2);
        else
            ang=line1.angle(line2);
        if(MotionWidth==1 && MotionBorderWidth==0)
            shapeItems.append( ShapeItem(painter_path(MotionWidth+1,MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                               painter_path_simple(3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                                       MotionNum_1,MotionNum_2,MotionNum_3, MotionNum_4,-1,QPointF(0,0),MotionBrush,MotionPen,MotionPenSimple,MotionWidth, MotionBorderWidth, 3) );
        else
            shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                               painter_path_simple(3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                                               MotionNum_1,MotionNum_2,MotionNum_3, MotionNum_4,-1,QPointF(0,0),MotionBrush,MotionPen,MotionPenSimple,MotionWidth, MotionBorderWidth, 3) );
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath ,MotionNum_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF( CtrlMotionPos_1.toPoint(),QSize(8,8)));
        rectItems.append( RectItem(rectPath, MotionNum_3, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF( CtrlMotionPos_2.toPoint(),QSize(8,8)));
        rectItems.append( RectItem(rectPath,MotionNum_4, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::RoundJoin)) );
        rectPath=newPath;
        StartMotionPos=unscale_unrotate(StartMotionPos, view, true);
        EndMotionPos=unscale_unrotate(EndMotionPos, view, true);
        CtrlMotionPos_1=unscale_unrotate(CtrlMotionPos_1, view, true);
        CtrlMotionPos_2=unscale_unrotate(CtrlMotionPos_2, view, true);
        (*pnts).insert(MotionNum_1,StartMotionPos);
        (*pnts).insert(MotionNum_2,EndMotionPos);
        (*pnts).insert(MotionNum_3,CtrlMotionPos_1);
        (*pnts).insert(MotionNum_4,CtrlMotionPos_2);
    }

    for (int i=shapeItems.size()-1; i>index; i--)
    {
        temp_shape=shapeItems[i-1];
        shapeItems[i-1]=shapeItems[i];
        shapeItems[i]=temp_shape;
    }
    if (count_moveItemTo==count_Shapes) 
        previousPosition_all=pos;
}

//- detecting connected figures -
bool ShapeElFigure::Holds(QVector <ShapeItem> &shapeItems, PntMap *pnts)
{
    int num,index_hold;
    bool flag_equal;
    if(index_array.size())
        index_array.clear();
    for(int i=0; i<shapeItems.size(); i++)
        index_array.push_back(-1);
    index_array[0]=index;
    num=0;
    do
    {
        index_hold=index_array[num];
        for (int i=0; i<shapeItems.size(); i++)
            if( i!=index_hold && ((shapeItems[index_hold].n1==shapeItems[i].n1) ||
			(shapeItems[index_hold].n2==shapeItems[i].n2) ||
			(shapeItems[index_hold].n1==shapeItems[i].n2) ||
			(shapeItems[index_hold].n2==shapeItems[i].n1)) &&
			ellipse_draw_startPath==newPath && ellipse_draw_endPath==newPath )
	    {
                    flag_equal=0;
                    for (int j=0; j<=count_holds; j++)
                        if( index_array[j]==i )	flag_equal=1;
                    if (flag_equal==0)
                    {
                        count_holds++;
                        index_array[count_holds]=i;
                    }
            }
        num++;
    }
    while(num!=count_holds+1);
    if (count_holds>0) return true;
    else return false;
}

//- detecting the figures(figure), or their points to move by key(up, down, left, rught) events -
void ShapeElFigure::Move_UP_DOWN(QVector <ShapeItem> &shapeItems, PntMap *pnts, QVector <InundationItem> &inundationItems, WdgView *view)
{
    int rect_num_temp;
    count_moveItemTo=0;
    bool flag_break_move;
    if (flag_ctrl && count_Shapes)
    {
        Move_all(QPointF(0,0),shapeItems,pnts,inundationItems,view);
    }
    else 
        if (!flag_first_move)
        {
            count_holds=0;
            flag_rect=false;
            if (status_hold)
                Holds(shapeItems,pnts);//calling "holds" to detect is the figure connected with other or not.
            if (count_holds)// if the Ctrl is pressed and there are connected figures
            {
                count_Shapes=count_holds+1;//getting the number of figures to move
                if (rect_num!=-1)// if ther is rect under the mouse pointer
                {
                    rect_num_temp=rect_num;
                    rect_num=Real_rect_num (rect_num,shapeItems,pnts);// detecting the number of the rect for moveItemTo
                    if( (rect_num==2 || rect_num==3) && shapeItems[index].type==3 )// if there is rect of unconnected point of bexier curve
                        flag_rect=false;
                    if( rect_num==0 || rect_num==1 )// if there is the rect of start or end point of the figure
                    {
                        Rect_num_0_1(shapeItems,rect_num_temp,pnts, view);
                    }
                    if( (rect_num==3 ||rect_num==4) && shapeItems[index].type==2 )
                        Rect_num_3_4(shapeItems,pnts);
                }
            }
        if (flag_rect || flag_arc_rect_3_4)// if there is the rect number of figures to move becomes equal to the number of figures, connected with this rect
            count_Shapes=count_rects;
        }
    if (flag_rect || flag_arc_rect_3_4 || (rect_num==-1 && count_holds))
        Move_all(QPointF(0,0),shapeItems,pnts,inundationItems,view);
    if ((!flag_ctrl  || (!flag_rect && rect_num!=-1)) && !flag_arc_rect_3_4)// if there is simple figure or one of its rects
        if (index!=-1)
        {
            num_vector.clear();
            flag_ctrl=true;
            flag_ctrl_move=false;
            count_moveItemTo=1;
            count_Shapes=1;
            if (!status_hold)
                for (int i=0; i<shapeItems.size(); i++)
                {
                    if (i!=index)
                    {
                        if (shapeItems[index].type==2)
                        {
                            if ( (shapeItems[index].n5==shapeItems[i].n1) || 
                                (shapeItems[index].n5==shapeItems[i].n2) ||
                                (shapeItems[index].n5==shapeItems[i].n3) || 
                                (shapeItems[index].n5==shapeItems[i].n4) ||
                                (shapeItems[index].n5==shapeItems[i].n5) )
                            {
                                QPointF Temp=(*pnts)[shapeItems[index].n5];
                                shapeItems[index].n5 = Append_Point(Temp,shapeItems,pnts);
                            }
                        }
                        if (shapeItems[index].type==2 || shapeItems[index].type==3)
                        {
                            if ( (shapeItems[index].n4==shapeItems[i].n1) || 
                                (shapeItems[index].n4==shapeItems[i].n2) ||
                                (shapeItems[index].n4==shapeItems[i].n3) || 
                                (shapeItems[index].n4==shapeItems[i].n4) ||
                                (shapeItems[index].n4==shapeItems[i].n5) )
                            {
                                QPointF Temp=(*pnts)[shapeItems[index].n4];
                                shapeItems[index].n4 = Append_Point(Temp,shapeItems,pnts);
                            }
                            if ( (shapeItems[index].n3==shapeItems[i].n1) || 
                                (shapeItems[index].n3==shapeItems[i].n2) ||
                                (shapeItems[index].n3==shapeItems[i].n3) || 
                                (shapeItems[index].n3==shapeItems[i].n4) ||
                                (shapeItems[index].n3==shapeItems[i].n5) )
                            {
                                QPointF Temp=(*pnts)[shapeItems[index].n3];
                                shapeItems[index].n3 = Append_Point(Temp,shapeItems,pnts);
                            }
                        }
                        if( (shapeItems[index].n1==shapeItems[i].n1) || 
                            (shapeItems[index].n1==shapeItems[i].n2) ||
                            (shapeItems[index].n1==shapeItems[i].n3) || 
                            (shapeItems[index].n1==shapeItems[i].n4) ||
                            (shapeItems[index].n1==shapeItems[i].n5) )
                        {
                            QPointF Temp=(*pnts)[shapeItems[index].n1];
                            shapeItems[index].n1 = Append_Point(Temp,shapeItems,pnts);
                        }
                        if( (shapeItems[index].n2==shapeItems[i].n1) || 
                            (shapeItems[index].n2==shapeItems[i].n2) ||
                            (shapeItems[index].n2==shapeItems[i].n3) ||
                            (shapeItems[index].n2==shapeItems[i].n4) ||
                            (shapeItems[index].n2==shapeItems[i].n5) )
                        {
                            QPointF Temp=(*pnts)[shapeItems[index].n2];
                            shapeItems[index].n2 = Append_Point(Temp,shapeItems,pnts);
                        }
                    }
                }
            itemInMotion=&shapeItems[index];
            moveItemTo((*pnts)[itemInMotion->n1],shapeItems,pnts,view);
            if(inundationItems.size())
                for(int i=0; i<inundationItems.size(); i++)
                    for(int j=0; j<inundationItems[i].number_shape.size(); j++)
                    {
                        flag_break_move=false;
                        if(inundationItems[i].number_shape[j]==index)
                        {
                            InundationPath=Create_Inundation_Path(inundationItems[i].number_shape,shapeItems,pnts,view);
                            inundationItems[i].path=InundationPath;
                            flag_break_move=true;
                            break;
                        }
                        if (flag_break_move) break;
                    }
            flag_ctrl=false;
        }
        else
            itemInMotion=0;
    if(inundationItems.size() && !flag_inund_break && !status_hold)
        for(int i=0; i<inundationItems.size(); i++)
            for(int j=0; j<inundationItems[i].number_shape.size(); j++)
            {
                flag_break_move=false;
                if(inundationItems[i].number_shape[j]==index)
                {
                    inundationItems.remove(i);
                    shapeSave(view);
                    flag_break_move=true;
                    break;
                }
                if (flag_break_move) break;
            }
    flag_inund_break=true;
}

int ShapeElFigure::Real_rect_num(int rect_num_old,QVector <ShapeItem> &shapeItems, PntMap *pnts)
{
    int rect_num_new;
    //- detecting the correct index of the figure -
    for (int i=0; i<=shapeItems.size()-1; i++)
	switch( shapeItems[i].type )
	{
	    case 1:
		if( (rectItems[rect_num].num==shapeItems[i].n1) ||
			(rectItems[rect_num].num==shapeItems[i].n2) )
		    index=i;
		break;
	    case 2:
		if( (rectItems[rect_num].num==shapeItems[i].n1) ||
			(rectItems[rect_num].num==shapeItems[i].n2) ||
			(rectItems[rect_num].num==shapeItems[i].n3) ||
			(rectItems[rect_num].num==shapeItems[i].n4) ||
			(rectItems[rect_num].num==shapeItems[i].n5) )
		    index=i;
		break;
	    case 3:
		if( (rectItems[rect_num].num==shapeItems[i].n1) ||
			(rectItems[rect_num].num==shapeItems[i].n2) ||
			(rectItems[rect_num].num==shapeItems[i].n3) ||
			(rectItems[rect_num].num==shapeItems[i].n4) )
		    index=i;
		break;
	}
    // - detecting the number of the rect for moveItemTo -
    switch( shapeItems[index].type )
    {
	case 1:
	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new=0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new=1;
	    break;
	case 2:
	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new=0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new=1;
	    if( rectItems[rect_num_old].num == shapeItems[index].n3 )	rect_num_new=2;
	    if( rectItems[rect_num_old].num == shapeItems[index].n4 )	rect_num_new=3;
	    if( rectItems[rect_num_old].num == shapeItems[index].n5 )	rect_num_new=4;
	    break;
	case 3:
    	    if( rectItems[rect_num_old].num == shapeItems[index].n1 )	rect_num_new=0;
	    if( rectItems[rect_num_old].num == shapeItems[index].n2 )	rect_num_new=1;
	    if( rectItems[rect_num_old].num == shapeItems[index].n3 )	rect_num_new=2;
	    if( rectItems[rect_num_old].num == shapeItems[index].n4 )	rect_num_new=3;
	    break;
    }
    return rect_num_new;
}

void ShapeElFigure::Rect_num_0_1(QVector <ShapeItem> &shapeItems,int rect_num_temp, PntMap *pnts, WdgView *view)
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
void ShapeElFigure::Rect_num_3_4(QVector <ShapeItem> &shapeItems,PntMap *pnts)
{
    flag_arc_rect_3_4=true;
    QVector <int> index_array_temp;
    for(int i=0; i<count_holds+5; i++)
    {
        fig_rect_array.push_back(0);
        arc_rect_array.push_back(0);
        index_array_temp.push_back(-1);
    }
    flag_rect=false;
    index_array_temp[0]=index;
    if (rect_num==3)
    {
        arc_rect_array[0]=3;
        fig_rect_array[0]=3;
    }
    if (rect_num==4)
    {
        arc_rect_array[0]=4;
        fig_rect_array[0]=4;
    }
    count_rects=1;
    for( int i=0; i<=count_holds; i++ )
    {
        if( index_array[i]!=index )
        {
            if( shapeItems[index].n1 == shapeItems[index_array[i]].n1 )
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=0;
                fig_rect_array[count_rects]=0;
                count_rects++;
            }
            if( shapeItems[index].n1 == shapeItems[index_array[i]].n2 )
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=0;
                fig_rect_array[count_rects]=1;
                count_rects++;
            }
            if( shapeItems[index].n2 == shapeItems[index_array[i]].n1 )
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=1;
                fig_rect_array[count_rects]=0;
                count_rects++;
            }
            if( shapeItems[index].n2 == shapeItems[index_array[i]].n2 )
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=1;
                fig_rect_array[count_rects]=1;
                count_rects++;
            }
        }
    }
    index_array.clear();
    for (int i=0; i<count_rects; i++)
        index_array.push_back(-1);
    for (int i=0; i<count_rects; i++)
        index_array[i]=index_array_temp[i];
    index_array_temp.clear();
}

//- moving all connected figures -
void ShapeElFigure::Move_all(QPointF pos,QVector <ShapeItem> &shapeItems,PntMap *pnts, QVector <InundationItem> &inundationItems, WdgView *view)
{
    num_vector.clear();
    int rect_num_temp;
    bool flag_break;
    for (int i=0; i<count_Shapes; i++)
    {
        count_moveItemTo+=1;
        flag_ctrl_move=false;
        flag_ctrl=true;
        itemInMotion=&shapeItems[index_array[i]];
        if (flag_rect)
        {
            rect_num=rect_array[i];
            arc_rect=rect_array[0];
        }
        if (flag_arc_rect_3_4)
        {
            if (i==0 && !flag_down && !flag_up && !flag_right && !flag_left) offset=pos-previousPosition_all;
            if (i>0)
                if (arc_rect_array[i]==0) 
                    offset=scale_rotate((*pnts)[shapeItems[index_array[0]].n1], view, true)-Prev_pos_1;
                else 
                    offset=scale_rotate((*pnts)[shapeItems[index_array[0]].n2], view, true)-Prev_pos_2;
            rect_num=fig_rect_array[i];
            arc_rect=arc_rect_array[i];
        }
        index=index_array[i];
        moveItemTo(pos,shapeItems,pnts,view);
        if (i==0 && flag_arc_rect_3_4)
        {
            Prev_pos_1=scale_rotate((*pnts)[shapeItems[index_array[0]].n1], view, true);
            Prev_pos_2=scale_rotate((*pnts)[shapeItems[index_array[0]].n2], view, true);
        }
    }
    if(inundationItems.size())
    {
        for(int i=0; i<inundationItems.size(); i++)
            for(int j=0; j<inundationItems[i].number_shape.size(); j++)
            {
                flag_break=false;
                for(int k=0; k<index_array.size(); k++)
                    if(inundationItems[i].number_shape[j]==index_array[k])
                    {
                        InundationPath=Create_Inundation_Path(inundationItems[i].number_shape,shapeItems,pnts,view);
                        inundationItems[i].path=InundationPath;
                        flag_break=true;
                        break;
                    }
                    if (flag_break) break;
            }
    }
}
//detecting the figure or one of its control points under mouse cursor
int ShapeElFigure::itemAt(const QPointF &pos, QVector <ShapeItem> &shapeItems,WdgView *w)
{
    QPointF point;
    rect_num=-1;
    bool flag_break=false;
    for (int j =0; j <=rectItems.size()-1; j++) 
    {
        const RectItem &item1 = rectItems[j];
        if (item1.path.contains(pos)) rect_num=j;
    }
    index=-1;
    if (rect_num!= -1)
        for (int i=0; i<=shapeItems.size()-1; i++)
            switch( shapeItems[i].type )
    {
        case 1:
            if( (rectItems[rect_num].num==shapeItems[i].n1) ||
                 (rectItems[rect_num].num==shapeItems[i].n2) )
                index=i;
            break;
        case 2:
            if( (rectItems[rect_num].num==shapeItems[i].n1) ||
                 (rectItems[rect_num].num==shapeItems[i].n2) ||
                 (rectItems[rect_num].num==shapeItems[i].n3) ||
                 (rectItems[rect_num].num==shapeItems[i].n4) ||
                 (rectItems[rect_num].num==shapeItems[i].n5) )
                index=i;
            break;
        case 3:
            if( (rectItems[rect_num].num==shapeItems[i].n1) ||
                 (rectItems[rect_num].num==shapeItems[i].n2) ||
                 (rectItems[rect_num].num==shapeItems[i].n3) ||
                 (rectItems[rect_num].num==shapeItems[i].n4) )
                index=i;
            break;
    }
    if (rect_num==-1)
    for (int i =0; i<shapeItems.size(); i++) 
    {
        const ShapeItem &item = shapeItems[i];
        if (item.path.contains(pos))  
        {
            index=i;
            flag_break=true;
        }
        if (flag_break) break;
        for(int j=3; j>0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if (item.path.contains(pos+point)) 
            {
               index=i;
               flag_break=true;
            }
        }
        if (flag_break) break;
        for(int j=3; j>0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if (item.path.contains(pos-point)) 
            {
                index=i;
                flag_break=true;
            }
        }
        if (flag_break) break;
    }
   return index;
}

//- Building the path for the current figure -
QPainterPath ShapeElFigure::painter_path(float el_width, float el_border_width, int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6)
{
    double arc_a_small, arc_b_small, t, t_start, t_end;
    QPointF  CtrlMotionPos_1_temp, CtrlMotionPos_2_temp, EndMotionPos_temp;
    QPainterPath circlePath;
    double arc_a, arc_b;
    circlePath=newPath;
    //-- if line --
    if(el_type==1)
    {
        el_border_width=el_border_width/2;
        circlePath.moveTo(TSYS::realRound(el_p1.x() + ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).y(), 2, true));
        circlePath.lineTo(TSYS::realRound(el_p1.x()+ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).y(), 2, true));
        circlePath.lineTo(TSYS::realRound(el_p1.x()+ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).y(), 2, true));
        circlePath.lineTo(TSYS::realRound(el_p1.x()+ROTATE(QPointF(-el_border_width,(el_width/2+el_border_width)),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(-el_border_width,(el_width/2+el_border_width)),el_ang).y(), 2, true));
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    //-- if arc --
    if(el_type==2)
    {
        arc_a=Length(QPointF(TSYS::realRound(el_p5.x(),2,true),TSYS::realRound(el_p5.y(),2,true)),QPointF(TSYS::realRound(el_p3.x(),2,true),TSYS::realRound(el_p3.y(),2,true)))
                +el_width/2+el_border_width;
        arc_b=Length(QPointF(TSYS::realRound(el_p3.x(),2,true),TSYS::realRound(el_p3.y(),2,true)),QPointF(TSYS::realRound(el_p4.x(),2,true),TSYS::realRound(el_p4.y(),2,true)))+el_width/2+el_border_width;
        arc_a_small=arc_a-el_width-el_border_width;
        arc_b_small=arc_b-el_width-el_border_width;
        t_start=el_p6.x();
        t_end=el_p6.y();
        circlePath.moveTo(TSYS::realRound(el_p3.x()+ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).x(), 2, true),
                          TSYS::realRound(el_p3.y()-ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).y(), 2, true));
        circlePath.lineTo(TSYS::realRound(el_p3.x()+ROTATE(ARC(t_start,arc_a,arc_b),el_ang).x(), 2, true),
                          TSYS::realRound(el_p3.y()-ROTATE(ARC(t_start,arc_a,arc_b),el_ang).y(), 2, true));
        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
            circlePath.lineTo(QPointF(TSYS::realRound(el_p3.x()+ROTATE(ARC(t,arc_a,arc_b),el_ang).x(), 2, true),
                                      TSYS::realRound(el_p3.y()-ROTATE(ARC(t,arc_a,arc_b),el_ang).y(), 2, true)));
        circlePath.lineTo(TSYS::realRound(el_p3.x()+ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).x(), 2, true),
                          TSYS::realRound(el_p3.y()-ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).y(), 2, true));
        for (t=t_end; t>t_start; t-=0.00277777777778) 
            circlePath.lineTo(QPointF(TSYS::realRound(el_p3.x()+ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).x(), 2, true),
                                      TSYS::realRound(el_p3.y()-ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).y(), 2, true)));
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    //-- if bezier --
    if(el_type==3)
    {

        el_border_width=el_border_width/2;
        CtrlMotionPos_1_temp=UNROTATE(el_p3,el_ang,el_p1.x(),el_p1.y());
        CtrlMotionPos_2_temp=UNROTATE(el_p4,el_ang,el_p1.x(),el_p1.y());
        EndMotionPos_temp=QPointF(Length(el_p2,el_p1)+el_border_width,0);
        circlePath.moveTo(TSYS::realRound(el_p1.x()+ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).y(), 2, true));
        circlePath.cubicTo(QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-(el_width/2+el_border_width)),el_ang).y(), 2, true)),
                           QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-(el_width/2+el_border_width)),el_ang).y(), 2, true)),
                           QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-(el_width/2+el_border_width)),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-(el_width/2+el_border_width)),el_ang).y(), 2, true)));
        circlePath.lineTo(TSYS::realRound(el_p1.x()+ROTATE(QPointF(EndMotionPos_temp.x(),el_width/2+el_border_width),el_ang).x(), 2, true),
                          TSYS::realRound(el_p1.y()-ROTATE(QPointF(EndMotionPos_temp.x(),el_width/2+el_border_width),el_ang).y(), 2, true));
        circlePath.cubicTo(QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+el_width/2+el_border_width),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+el_width/2+el_border_width),el_ang).y(), 2, true)),
                           QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+el_width/2+el_border_width),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+el_width/2+el_border_width),el_ang).y(), 2, true)),
                           QPointF(TSYS::realRound(el_p1.x()+ROTATE(QPointF(-el_border_width,el_width/2+el_border_width),el_ang).x(), 2, true),
                                   TSYS::realRound(el_p1.y()-ROTATE(QPointF(-el_border_width,el_width/2+el_border_width),el_ang).y(), 2, true)));
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    return circlePath;
}

//- building path for the figure(if its border's width=0) -
QPainterPath ShapeElFigure::painter_path_simple(int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6)
{
    QPainterPath circlePath;
    double t;
    circlePath=newPath;
    double arc_a, arc_b;
    if(el_type==1)
    {
        circlePath.moveTo(TSYS::realRound(el_p1.x(),2,true),TSYS::realRound(el_p1.y(),2,true));
        circlePath.lineTo(TSYS::realRound(el_p2.x(),2,true),TSYS::realRound(el_p2.y(),2,true));
    }
    if(el_type==2)
    {
        arc_a=Length(QPointF(TSYS::realRound(el_p5.x(),2,true),TSYS::realRound(el_p5.y(),2,true)),QPointF(TSYS::realRound(el_p3.x(),2,true),TSYS::realRound(el_p3.y(),2,true)));
        arc_b=Length(QPointF(TSYS::realRound(el_p3.x(),2,true),TSYS::realRound(el_p3.y(),2,true)),QPointF(TSYS::realRound(el_p4.x(),2,true),TSYS::realRound(el_p4.y(),2,true)));
        t_start=el_p6.x();
        t_end=el_p6.y();
        circlePath.moveTo(TSYS::realRound(el_p3.x()+ROTATE(ARC(t_start,arc_a,arc_b),el_ang).x(),2,true),
                          TSYS::realRound(el_p3.y()-ROTATE(ARC(t_start,arc_a,arc_b),el_ang).y(),2,true));
        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
            circlePath.lineTo(TSYS::realRound(el_p3.x()+ROTATE(ARC(t,arc_a,arc_b),el_ang).x(),2,true),
                              TSYS::realRound(el_p3.y()-ROTATE(ARC(t,arc_a,arc_b),el_ang).y(),2,true));
    }
    if(el_type==3)
    {
        circlePath.moveTo(TSYS::realRound(el_p1.x(),2,true),TSYS::realRound(el_p1.y(),2,true));
        circlePath.cubicTo(TSYS::realRound(el_p3.x(),2,true),TSYS::realRound(el_p3.y(),2,true), 
                           TSYS::realRound(el_p4.x(),2,true),TSYS::realRound(el_p4.y(),2,true), 
                           TSYS::realRound(el_p2.x(),2,true),TSYS::realRound(el_p2.y(),2,true));
    }
    return circlePath;
}

//- adding the point to data model -
int ShapeElFigure::Append_Point( QPointF &pos, QVector <ShapeItem> &shapeItems,PntMap *pnts )
{
    int i = 1;
    while( (*pnts).contains(i) ) i++;
    (*pnts).insert(i,pos);
    return i;
}

//- deleting the point from data model -
void ShapeElFigure::Drop_Point (int num, int num_shape, QVector <ShapeItem> &shapeItems, PntMap *pnts)
{
    bool equal=false;
    for(int i=0; i<shapeItems.size(); i++)
        if(i!=num_shape && (num==shapeItems[i].n1 || num==shapeItems[i].n2 || num==shapeItems[i].n3 || num==shapeItems[i].n4
           || num==shapeItems[i].n5) )
        {
            equal=true;
            break;
        }
    if(!equal)
        (*pnts).remove(num);
}
//- Building the contiguity matrix using start and end points of all figures -
int ShapeElFigure::Build_Matrix(QVector <ShapeItem> &shapeItems)
{
    int N;
    for (int j=0; j<2*shapeItems.size()+1; j++)
        vect.push_back(0);
    int j=1;
    bool flag_vect_n1, flag_vect_n2;
    for (int i=0; i<shapeItems.size(); i++)
    {
        flag_vect_n1=false;
        flag_vect_n2=false;
        for (int k=1; k<j; k++)
        {
            if(vect[k]==shapeItems[i].n1)
                flag_vect_n1=true;
            if(vect[k]==shapeItems[i].n2)
                flag_vect_n2=true;
        }
        if(!flag_vect_n1)
        {
            vect[j]=shapeItems[i].n1;
            j++;
        }
        if(!flag_vect_n2)
        {
            vect[j]=shapeItems[i].n2;
            j++;
        }
    }
    for(int i=0; i<j; i++)
    {
        QVector<int> el;
        for(int k=0; k<j; k++)  el.push_back(0);
        map_matrix.push_back(el);
    }
    N=j-1;
    for (int v=1; v<j; v++)
        for(int i=0; i<shapeItems.size(); i++)
    {
        if(shapeItems[i].n1==vect[v])
            for(int p=1; p<j; p++)
                if(vect[p]==shapeItems[i].n2)
                {
                    map_matrix[v][p]=1;
                    map_matrix[p][v]=1;
                }
        if(shapeItems[i].n2==vect[v])
            for(int p=1; p<j; p++)
                if(vect[p]==shapeItems[i].n1)
                {
                    map_matrix[v][p]=1;
                    map_matrix[p][v]=1;
                }
    }
    return N;
}



void ShapeElFigure::step(int s,int f, int p, QVector <int> vect, int N)
{
    int c;
    if (s==f && p>4) 
    {
        if(len>0 && len>clen)
            found--;
        len=clen;
        found++;
        minroad[found][0]=len;
        for (int k=1;k<p;k++)
            minroad[found][k]=road[k];
    }
    else
    {
        for(c=1;c<=N;c++)
            if (map_matrix[s][c] && !incl[c]&& ( len == 0 || clen+map_matrix[s][c]<=len))
            {
                road[p]=c; incl[c]=1;clen=clen+map_matrix[s][c];
                step (c,f,p+1, vect, N);
                incl[c]=0; road[p]=0;clen=clen-map_matrix[s][c];
            }
    }
}

//- detecting the figures for fill -
bool ShapeElFigure::Inundation (QPointF point, QVector <ShapeItem> &shapeItems,  PntMap *pnts, QVector <int> vect, int N, WdgView *view)
{
    found=false;
    InundationPath=newPath;
    bool flag_break=false;
    bool flag_push_back;
    QVector<int> work_sort;
    QVector<int> inundation_fig_num;
    for(int i=0; i < 2*shapeItems.size()+1; i++)
        work_sort.push_back(0);
    int i,j;
    i=1;
    do
    {
        found=0;
        minroad.clear();
        road.clear();
        incl.clear();
        for(int k=0; k<2*shapeItems.size()+1; k++)
        {
            QVector<int> el;
            for(int z=0; z<2*shapeItems.size()+1; z++)  el.push_back(0);
            minroad.push_back(el);
        }
        for (int k=0;k< 2*shapeItems.size()+1; k++)
        {
            road.push_back(0);
            incl.push_back(0);
        }
        road[1]=i; incl[i]=0;
        len=0; clen=0;
        step(i,i,2, vect, N);
        for (int i_found=1; i_found<=found; i_found++)
        {
            InundationPath=newPath;
            for(int k=1; k<=minroad[i_found][0]+1; k++)
                for(int j=0; j<shapeItems.size(); j++)
                    if((shapeItems[j].n1==vect[minroad[i_found][k]] && shapeItems[j].n2==vect[minroad[i_found][k+1]]) ||
                        (shapeItems[j].n1==vect[minroad[i_found][k+1]] && shapeItems[j].n2==vect[minroad[i_found][k]]) )
                    {
                        inundation_fig_num.push_back(j);
                        break;
                    }
            InundationPath=Create_Inundation_Path(inundation_fig_num,shapeItems,pnts,view);
            inundation_fig_num.clear();
            if (InundationPath.contains(point))
            {
                for(int i_m=1; i_m<=minroad[i_found][0]+1; i_m++)
                    if(work_sort[0]>minroad[i_found][0] || !work_sort[0])
                        for(int i_p=0; i_p<=minroad[i_found][0]+1; i_p++)
                            work_sort[i_p]=minroad[i_found][i_p];
            }
        }
        i++;
    }
    while(i<=N);
    InundationPath=newPath;
    for(int k=1; k<=work_sort[0]; k++)

        for(int j=0; j<shapeItems.size(); j++)
        if((shapeItems[j].n1==vect[work_sort[k]] && shapeItems[j].n2==vect[work_sort[k+1]]) ||
            (shapeItems[j].n1==vect[work_sort[k+1]] && shapeItems[j].n2==vect[work_sort[k]]) )
        {
                flag_push_back=true;
                for(int p=0; p<inundation_fig_num.size(); p++)
                    if((shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n2) ||
                        (shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n1))
                        {
                            flag_push_back=false;
                            if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                inundation_fig_num[p]=j;
                            if((shapeItems[inundation_fig_num[p]].type==3 && shapeItems[j].type==1) && (inundation_fig_num[p]!=j))
                                inundation_fig_num[p]=j;
                            if((shapeItems[inundation_fig_num[p]].type==2 && shapeItems[j].type==3) && (inundation_fig_num[p]!=j))
                                inundation_fig_num[p]=j;
                        }
                if (flag_push_back)
            inundation_fig_num.push_back(j);
        }
    work_sort.clear();
    if(inundation_fig_num.size()>0)
    {
        InundationPath=Create_Inundation_Path(inundation_fig_num, shapeItems, pnts,view);
        inundation_fig_num=Inundation_sort(InundationPath, inundation_fig_num, shapeItems, pnts, view);
        InundationPath=Create_Inundation_Path(inundation_fig_num, shapeItems, pnts,view);
        for(int i=0; i<inundation_fig_num.size(); i++)
            Inundation_vector.push_back(0);
        Inundation_vector=inundation_fig_num;
    }
    return true;
}

QVector <int> ShapeElFigure::Inundation_sort(QPainterPath InundationPath, QVector<int> inundation_fig_num, QVector <ShapeItem> &shapeItems, PntMap *pnts,  WdgView *view)
{
    for(int j=0; j<shapeItems.size(); j++)
        for(int p=0; p<inundation_fig_num.size(); p++)
            if((shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n1 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n2) ||
                (shapeItems[inundation_fig_num[p]].n1==shapeItems[j].n2 && shapeItems[inundation_fig_num[p]].n2==shapeItems[j].n1))
            {
                if(shapeItems[j].type==2  && p!=j)
                    if(InundationPath.contains(scale_rotate((*pnts)[shapeItems[j].n4], view, true)))
                        inundation_fig_num[p]=j;
                if(shapeItems[j].type==3 && p!=j && shapeItems[inundation_fig_num[p]].type!=2)
                    if(InundationPath.contains(scale_rotate((*pnts)[shapeItems[j].n4], view, true)) && 
                        InundationPath.contains(scale_rotate((*pnts)[shapeItems[j].n3], view, true)))
                        inundation_fig_num[p]=j;
            }
    return inundation_fig_num;
}

//- detecting the figures, which count <=2 for fill -
bool ShapeElFigure::Inundation_1_2(QPointF point, QVector <ShapeItem> &shapeItems, QVector <InundationItem> &inundationItems,  PntMap *pnts, WdgView *view)
{
    QPainterPath InundationPath_1_2;
    QVector <int> in_fig_num;
    bool flag_break;
    flag_break=false;
    QBrush fill_brush(QColor(0,0,0,0),Qt::SolidPattern), fill_img_brush;
    fill_brush.setColor(view->dc()["fillClr"].value<QColor>());
    fill_img_brush=view->dc()["fillImg"].value<QBrush>();
    for(int i=0; i<shapeItems.size(); i++)
    {
        if(shapeItems[i].type==2 )
            if(scale_rotate((*pnts)[shapeItems[i].n1], view, true).toPoint()==scale_rotate((*pnts)[shapeItems[i].n2], view, true).toPoint())
        {
            if(in_fig_num.size())
                in_fig_num.clear();
            in_fig_num.push_back(i);
            InundationPath_1_2=newPath;
            InundationPath_1_2=Create_Inundation_Path(in_fig_num, shapeItems, pnts,view);
            if(InundationPath_1_2.contains(point))
            {
                inundationItems.push_back(InundationItem(InundationPath_1_2,fill_brush,fill_img_brush, in_fig_num));
                flag_break=true;
            }
        }
        if(flag_break)
            return true;
        if(shapeItems[i].type==2 || shapeItems[i].type==3 && status_hold)
        {
            for(int j=0; j<shapeItems.size(); j++)
            {
                InundationPath_1_2=newPath;
                if(in_fig_num.size())
                    in_fig_num.clear();
                if(i!=j)
                    if((shapeItems[i].n1 == shapeItems[j].n1 && shapeItems[i].n2 == shapeItems[j].n2) ||
                        (shapeItems[i].n1 == shapeItems[j].n2 && shapeItems[i].n2 == shapeItems[j].n1))
                {
                    in_fig_num.push_back(i);
                    in_fig_num.push_back(j);
                    InundationPath_1_2=Create_Inundation_Path(in_fig_num, shapeItems, pnts,view);
                    if(InundationPath_1_2.contains(point))
                    {
                        inundationItems.push_back(InundationItem(InundationPath_1_2,fill_brush,fill_img_brush, in_fig_num));
                        flag_break=true;
                    }
                }
                if(flag_break)
                    return true;
            }
        }
    }
    return false;
}

 QPointF ShapeElFigure::scale_rotate(QPointF point, WdgView *view, bool flag_scale)
{
    QPointF center= QPointF((view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true))).toPoint();
    point=point-center;
    point=ROTATE(point, view->dc()["orient"].toDouble());
    point=point+center;
    if(flag_scale)
        point=QPointF(point.x()*view->xScale(true), point.y()*view->yScale(true));
    return point;
}

QPointF ShapeElFigure::unscale_unrotate(QPointF point, WdgView *view, bool flag_scale)
{
    QPointF center= QPointF((view->sizeF().width())/(2*view->xScale(true)), (view->sizeF().height())/(2*view->yScale(true))).toPoint();
    if(flag_scale)
        point=QPointF(point.x()/view->xScale(true), point.y()/view->yScale(true));
    point=point-center;
    point=ROTATE(point, 360-view->dc()["orient"].toDouble());
    point=point+center;
    return point;
}

//- Building the path for fill -
QPainterPath ShapeElFigure::Create_Inundation_Path (QVector <int> in_fig_num, QVector <ShapeItem> &shapeItems, PntMap *pnts,WdgView *view )
{
    QPainterPath path;
    int flag, in_index;
    bool flag_remove=false;
    bool flag_n1, flag_n2, flag_break;
    double arc_a,arc_b,t_start,t_end,t,ang;
    QLineF line1,line2;
    path=newPath;
    if(shapeItems[in_fig_num[0]].n1<shapeItems[in_fig_num[0]].n2)
    {
        path.moveTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).x(), 2, true),
                    TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).y(), 2, true));
        switch(shapeItems[in_fig_num[0]].type)
        {
            case 1:
                path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).x()),
                            TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).y()));
                break;
            case 2:
                line2=QLineF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true) + 10, 
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true));
                line1=QLineF( TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true));
                if (TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true)<=TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)) 
                    ang=line1.angle(line2);
                else ang=360-line1.angle(line2);
                arc_a=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)),
                             QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true)));
                arc_b=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)),
                             QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).y(), 2, true)));
                t_start=shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end=shapeItems[in_fig_num[0]].ctrlPos4.y();
                for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
                    path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x()+ROTATE(ARC(t,arc_a,arc_b),ang).x(), 2, true),
                                TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y()-ROTATE(ARC(t,arc_a,arc_b),ang).y(), 2, true)); 
                break;
            case 3:
                path.cubicTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).y(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).y(), 2, true));
    
                break;
        }
        flag_n2=true;
        flag_n1=false;
    }
    else
    {
        path.moveTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).x(), 2, true),
                    TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n2], view, true).y(), 2, true));
        switch(shapeItems[in_fig_num[0]].type)
        {
            case 1:
                path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).x(), 2, true),
                            TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).y(), 2, true));
                break;
            case 2:    
                line2=QLineF( TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true) + 10, 
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true) );
                line1=QLineF( TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                              TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true) ) ;
                if (TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true)<=TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)) 
                    ang=line1.angle(line2);
                else ang=360-line1.angle(line2);
                arc_a=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)),
                             QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n5], view, true).y(), 2, true)));
                arc_b=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true)),
                             QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).y(), 2, true)));
                t_start=shapeItems[in_fig_num[0]].ctrlPos4.x();
                t_end=shapeItems[in_fig_num[0]].ctrlPos4.y();
                for (t=t_end; t>t_start+0.00277777777778; t-=0.00277777777778) 
                    path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x()+ROTATE(ARC(t,arc_a,arc_b),ang).x(), 2, true),
                                TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y()-ROTATE(ARC(t,arc_a,arc_b),ang).y(), 2, true)); 
                break;
            case 3:
                path.cubicTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n4], view, true).y(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n3], view, true).y(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).x(), 2, true),
                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_fig_num[0]].n1], view, true).y(), 2, true));
                break;
        }
        flag_n2=false;
        flag_n1=true;
    }
    int k=0;
    for(int i=0; i<in_fig_num.size()-1; i++)
    {
        if(flag_n2)
        {
            flag_break=false;
            for (int j=0; j<in_fig_num.size(); j++)
            {
                if((k!=j) && (shapeItems[in_fig_num[k]].n2==shapeItems[in_fig_num[j]].n1))
                {
                    flag=1;
                    in_index=in_fig_num[j];
                    k=j;
                    flag_break=true;
                }
                if (flag_break) break;
                if((k!=j) && (shapeItems[in_fig_num[k]].n2==shapeItems[in_fig_num[j]].n2))
                {
                    flag=2;
                    in_index=in_fig_num[j];
                    k=j;
                    flag_break=true;
                }
                if (flag_break) break;
            }
        }
        if(flag_n1)
        {
            flag_break=false;
            for (int j=0; j<in_fig_num.size(); j++)
            {
                if((k!=j) && (shapeItems[in_fig_num[k]].n1==shapeItems[in_fig_num[j]].n1))
                {
                    flag=1;
                    in_index=in_fig_num[j];
                    k=j;
                    flag_break=true;
                }
                if (flag_break) break;
                if((k!=j) && (shapeItems[in_fig_num[k]].n1==shapeItems[in_fig_num[j]].n2))
                {
                    flag=2;
                    in_index=in_fig_num[j];
                    k=j;
                    flag_break=true;
                }
                if (flag_break) break;
            }
        }
        switch(flag)
        {
            case 1:
                switch(shapeItems[in_index].type)
                {
                    case 1:
                        path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n2], view, true).x(), 2, true),
                                    TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n2], view, true).y(), 2, true));
                        break;
                    case 2:
                        line2=QLineF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true)+10,
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true) );
                        line1=QLineF( TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true), 
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true) );
                        if (TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true)<=
                            TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)) 
                            ang=line1.angle(line2);
                        else ang=360-line1.angle(line2);
                        arc_a=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)),
                                     QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true)));
                        arc_b=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)),
                                     QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).y(), 2, true)));
                        t_start=shapeItems[in_index].ctrlPos4.x();
                        t_end=shapeItems[in_index].ctrlPos4.y();
                        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
                            path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x()+ROTATE(ARC(t,arc_a,arc_b),ang).x(), 2, true),
                                        TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y()-ROTATE(ARC(t,arc_a,arc_b),ang).y(), 2, true)); 
                        break;
                
                    case 3:
                        path.cubicTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n2], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n2], view, true).y(), 2, true));
                        break;
                }
                flag_n2=true;
                flag_n1=false;
                break;
            case 2:
                switch(shapeItems[in_index].type)
                {
                    case 1:
                        path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n1], view, true).x(), 2, true),
                                    TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n1], view, true).y(), 2, true));
                        break;
                    case 2:
                        line2=QLineF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true)+10,
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true) );
                        line1=QLineF( TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true), 
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                      TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true) );
                        if (TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true)<=
                            TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)) 
                            ang=line1.angle(line2);
                        else ang=360-line1.angle(line2);
                        arc_a=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)),
                                     QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n5], view, true).y(), 2, true)));
                        arc_b=Length(QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true)),
                                     QPointF(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).x(), 2, true),
                                             TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).y(), 2, true)));
                        t_start=shapeItems[in_index].ctrlPos4.x();
                        t_end=shapeItems[in_index].ctrlPos4.y();
                        for (t=t_end; t>t_start+0.00277777777778; t-=0.00277777777778) 
                            path.lineTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x()+ROTATE(ARC(t,arc_a,arc_b),ang).x(), 2, true),
                                        TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y()-ROTATE(ARC(t,arc_a,arc_b),ang).y(), 2, true)); 
                        break;
                    case 3:
                        path.cubicTo(TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n4], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n3], view, true).y(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n1], view, true).x(), 2, true),
                                     TSYS::realRound(scale_rotate((*pnts)[shapeItems[in_index].n1], view, true).y(), 2, true));

                        break;
                }
                flag_n2=false;
                flag_n1=true;
                break;
        }
    }
    return path;
}

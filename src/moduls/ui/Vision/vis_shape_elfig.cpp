
//OpenSCADA system module UI.VISION file: vis_shape_elfig.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Lysenko Maxim (mlisenko@ukr.net)
 *   			by Yashina Kseniya (mlisenko@ukr.net) 
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
    WdgShape("ElFigure"), itemInMotion(0), flag_down(false), flag_up(false), flag_left(false), flag_right(false), flag_ctrl(false), 
    flag_rect(false), flag_hold_move(false), flag_m(false), flag_hold_arc(false), flag_arc_rect_3_4(false), flag_first_move(false), Flag(false), 
    current_ss(-1), current_se(-1), current_es(-1), current_ee(-1), count_Shapes(0), count_holds(0), count_rects(0), rect_num_arc(-1)
{
    newPath.addEllipse(QRect(0, 0, 0, 0));	//Задание нулевого пути;
}

void ShapeElFigure::init( WdgView *w )
{
    QList<ShapeItem> *shapeItems = new QList<ShapeItem>();
    w->dc()["shapeItems"].setValue( (void*)shapeItems );
    PntMap *pnts = new PntMap;
    w->dc()["shapePnts"].setValue( (void*)pnts );
}

void ShapeElFigure::destroy( WdgView *w )
{
    delete (QList<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    delete (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    rectItems.clear();
}

bool ShapeElFigure::attrSet( WdgView *w, int uiPrmPos, const string &val )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);
        
    bool rel_list=false;
    bool up=false;
    QPointF abc;
    QList<ShapeItem> &shapeItems = *(QList<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
   // PntMap *pnts_temp = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    QLineF line1, line2;
    double ang;
    QPointF StartMotionPos;//Первая контрольная точка(StartLine)
    QPointF EndMotionPos;//Вторая контрольная точка(EndLine)
    QPointF CtrlMotionPos_1;//Третья контрольная точка
    QPointF CtrlMotionPos_2;//Четвёртая контрольная точка
    QPointF CtrlMotionPos_3;//Пятая контрольная точка(для дуги)
    QPointF CtrlMotionPos_4;//Шестая контрольная точка(для дуги)
    double t_start, t_end, a, b, ang_t;
    float MotionWidth;
    QPainterPath circlePath;
   // rectItems.clear();
  
    switch( uiPrmPos )
    {
	case -1:	//load
	    rel_list = true;
	    break;
        case 5:		//en
            if(runW)    w->dc()["en"] = (bool)atoi(val.c_str());
            break;
        case 6:		//active
            if(runW)    w->dc()["active"] = (bool)atoi(val.c_str());
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
        case 25:	//backgClr
            w->dc()["backgClr"] = QColor(val.c_str());
            up=true;
            break;
        case 26:	//backgImg
            w->dc()["backgImg"] = val.c_str();
            up=true;
            break;
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
                QPoint pnt_ = (*pnts)[pnt];
                if( patr == 0 ) pnt_.setX(pval);
                else pnt_.setY(pval);
                (*pnts)[pnt] = pnt_;
                rel_list = true;
                
            }
    }
	
    if( rel_list && !w->allAttrLoad( ) )
    {
        shapeItems.clear();
            //- Parse last attributes list and make point list -
        string sel;
        int p[5];
        QPoint ip[5];
        for( int off = 0; (sel=TSYS::strSepParse(w->dc()["elLst"].toString().toAscii().data(),0,'\n',&off)).size(); )
        {
            //PntMap::iterator ip;
            if(sscanf(sel.c_str(),"line:%d:%d",&p[0],&p[1]) == 2 )
            {
                for( int i_p = 0; i_p < 2; i_p++ )
                    ip[i_p] = (*pnts)[p[i_p]];
                    
                if ( w->dc()["bordWdth"].toInt()>0)
                {
                    printf("Hard\n");
                    line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                    line1=QLineF(ip[0],ip[1]);
                    if( ip[0].y()<=ip[1].y() )	ang=360-line1.angle(line2);
                    else ang=line1.angle(line2);
                    circlePath = painter_path(w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 1, ang, ip[0],ip[1]);
                    shapeItems.append( ShapeItem(circlePath, newPath, p[0],p[1],-1,-1,-1,QPointF(0,0), QBrush(w->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
		           QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                 QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin), w->dc()["lineWdth"].toInt(),  w->dc()["bordWdth"].toInt(), 1) );
                }
                else
                {
                    printf("Simple\n");
                    line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                    line1=QLineF(ip[0],ip[1]);
                    if (ip[0].y()<=ip[1].y())
                        ang=360-line1.angle(line2);
                    else
                        ang=line1.angle(line2);
                    circlePath = painter_path_simple( 1, ang, ip[0],ip[1] );
                    QPainterPath bigPath = painter_path( w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 1, ang, ip[0], ip[1] );
                    
		    shapeItems.append( ShapeItem(bigPath,circlePath,p[0],p[1],-1,-1,-1,QPointF(0,0), QBrush(w->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                       QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                             QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin), w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(),1) );
                }
            }
            else if(sscanf(sel.c_str(),"arc:%d:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3],&p[4]) == 5)
            {
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = (*pnts)[p[i_p]];
                
                //////////////////////////////////////////////////////////////////////////////////
                StartMotionPos=ip[0];
                EndMotionPos=ip[1];
                CtrlMotionPos_1=ip[2];
                CtrlMotionPos_2=ip[3];
                CtrlMotionPos_3=ip[4];
               // CtrlMotionPos_4=ip[4];
                MotionWidth=w->dc()["lineWdth"].toInt();
                
                printf("StartMotionPos_beg=(%f,%f)\n", StartMotionPos.x(), StartMotionPos.y());
                printf("EndMotionPos_beg=(%f,%f)\n", EndMotionPos.x(), EndMotionPos.y());
                printf("CtrlMotionPos_1_beg=(%f,%f)\n", CtrlMotionPos_1.x(), CtrlMotionPos_1.y());
                printf("CtrlMotionPos_2_beg=(%f,%f)\n", CtrlMotionPos_2.x(), CtrlMotionPos_2.y());
                printf("CtrlMotionPos_4_beg=(%f,%f)\n", CtrlMotionPos_4.x(), CtrlMotionPos_4.y());
                
                line2=QLineF(CtrlMotionPos_1 ,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
                line1=QLineF( CtrlMotionPos_1,CtrlMotionPos_3);
                if (CtrlMotionPos_3.y()<=CtrlMotionPos_1.y()) ang=line1.angle(line2);
                else ang=360-line1.angle(line2);
                
                a=Length(CtrlMotionPos_3, CtrlMotionPos_1);
                b=Length(CtrlMotionPos_2, CtrlMotionPos_1);
                
                
                CtrlMotionPos_2=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                        CtrlMotionPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
                //printf("CtrlMotionPos_2_beg_per=(%f,%f)\n", CtrlMotionPos_2.x(), CtrlMotionPos_2.y());
               //CtrlMotionPos_4=UNROTATE(CtrlMotionPos_4,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
                
               
                StartMotionPos=UNROTATE(StartMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
                QPoint StartMotionPos_i=StartMotionPos.toPoint();
                //printf("StartMotionPos_beg_unrot=(%f,%f)\n", StartMotionPos.x(), StartMotionPos.y());
                //printf("StartMotionPos_i_unrot=(%i,%i)\n", StartMotionPos_i.x(), StartMotionPos_i.y());
                /*line1=QLineF(QPointF(0,0),CtrlMotionPos_4);
                line2=QLineF(QPointF(0,0),StartMotionPos);
                if (StartMotionPos.y()<0)
                    ang_t=Angle(line1,line2);
                            
                else ang_t=360-Angle(line1,line2);
                
                t_start=ang_t/360;
               /* if (t_start>t_end) t_end+=1;
                if ((t_end-1)>t_start) t_end-=1;*/
                ///////////////////////////////////////////
                
                if (StartMotionPos.x()>=a)
                {
                    //printf(">=a\n");
                    StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*a);
                    StartMotionPos.setX(a);
                }
                if (StartMotionPos.x()<-a)
                {
                    //printf("<a\n");
                    StartMotionPos.setY((StartMotionPos.y()/StartMotionPos.x())*(-a));
                    StartMotionPos.setX(-a);
                }
                if(StartMotionPos.y()<=0)
                {
                    //printf("StartMotionPos.y()<=0\n");
                    t_start=acos(StartMotionPos.x()/a)/(2*M_PI);
                }
                else
                {
                    //printf ("else\n");
                    t_start=1-acos(StartMotionPos.x()/a)/(2*M_PI);
                }
                /*if (t_start<0) t_start=1+t_start;  
                if (t_start>t_end) t_end+=1;
                if ((t_end-1)>t_start) t_end-=1;
                if (t_start==t_end) t_end+=1;
                if (t_end>t_start && t_start>=1 && t_end>1)
                {
                    t_start-=1;
                    t_end-=1;
                }*/
                            
               // EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                    // CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y());
                //////////Tstart//////////////////////
                ///////////////////////////////////
               
                EndMotionPos=UNROTATE(EndMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
                printf("EndMotionPos_unrot=(%f,%f)\n", EndMotionPos.x(), EndMotionPos.y());
                if (EndMotionPos.x()<-a)
                {
                    printf("<=-a\n");
                    EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(-a));
                    EndMotionPos.setX(-a);
                }
                if (EndMotionPos.x()>=a)
                {
                    printf(">a\n");
                    EndMotionPos.setY((EndMotionPos.y()/EndMotionPos.x())*(a));
                    EndMotionPos.setX(a);
                }
                if(EndMotionPos.y()<=0)
                {
                    printf ("else\n");
                    t_end=acos(EndMotionPos.x()/a)/(2*M_PI);
                }
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
                     // line1=QLineF(QPointF(0,0),CtrlMotionPos_4);
                /*line2=QLineF(QPointF(0,0),EndMotionPos);
                if (EndMotionPos.y()<0)
                    ang_t=Angle(line1,line2);
                else ang_t=360-Angle(line1,line2);
                if (t_start==t_end)
                {
                    t_start=0;
                    t_end=0;
                }
                t_end=ang_t/360;
                  
                if (t_start>t_end) t_end+=1;
                if ((t_end-1)>t_start) t_end-=1;
                ///////////////////////////////////////////
                CtrlMotionPos_4=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                        CtrlMotionPos_1.y()-ROTATE(ARC(0,a,b),ang).y());*/
                    StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                                           CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());
                    EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a,b),ang).x(),
                                         CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a,b),ang).y());
                CtrlMotionPos_4=QPointF(t_start, t_end);
                printf("a=%f\n",a);
                printf("b=%f\n",b);
                printf("T_start=%f\n",t_start);
                printf("T_end=%f\n",t_end);
                printf("StartMotionPos_end=(%f,%f)\n", StartMotionPos.x(), StartMotionPos.y());
                printf("EndMotionPos_end=(%f,%f)\n", EndMotionPos.x(), EndMotionPos.y());
                printf("CtrlMotionPos_1_end=(%f,%f)\n", CtrlMotionPos_1.x(), CtrlMotionPos_1.y());
                printf("CtrlMotionPos_2_end=(%f,%f)\n", CtrlMotionPos_2.x(), CtrlMotionPos_2.y());
                printf("CtrlMotionPos_4_end=(%f,%f)\n", CtrlMotionPos_4.x(), CtrlMotionPos_4.y());
                //////////////////////////////////////////////////////////////////////////////////
                
                                
                if ( w->dc()["bordWdth"].toInt()>0)
                {
                    circlePath = painter_path( w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 2, ang, StartMotionPos, EndMotionPos,
                                              CtrlMotionPos_1, CtrlMotionPos_2,  CtrlMotionPos_3, CtrlMotionPos_4 );
		    shapeItems.append( ShapeItem(circlePath, newPath, p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,QBrush(w->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                       QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                             QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin), w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 2) );
                }
                else
                {
                    QPainterPath bigPath = painter_path( w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 2, ang, StartMotionPos, EndMotionPos,
                            CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4 );
                    circlePath = painter_path_simple(2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2,  CtrlMotionPos_3, CtrlMotionPos_4 );
		    shapeItems.append( ShapeItem(bigPath,circlePath,p[0],p[1],p[2],p[3], p[4],CtrlMotionPos_4, QBrush(w->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                       QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                             QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin), w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(),2) );
                }
            }
            else if( sscanf(sel.c_str(),"bezier:%d:%d:%d:%d",&p[0],&p[1],&p[2],&p[3]) == 4 )
            {
                for( int i_p = 0; i_p < 4; i_p++ )
                    ip[i_p] = (*pnts)[p[i_p]];
                
                if ( w->dc()["bordWdth"].toInt()>0)
                {
                    line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                    line1=QLineF(ip[0],ip[1]);
                    if (ip[0].y()<=ip[1].y())
                        ang=360-line1.angle(line2);
                    else
                        ang=line1.angle(line2);
                    circlePath = painter_path( w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 3, ang, ip[0], ip[1], ip[2], ip[3] );
                    
		    shapeItems.append( ShapeItem(circlePath, newPath, p[0], p[1], p[2], p[3],-1,QPointF(0,0), QBrush(w->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
			QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin),
                              QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 3) );
                }
                else
                {
                    line2=QLineF(ip[0],QPointF(ip[0].x()+10,ip[0].y()));
                    line1=QLineF(ip[0],ip[1]);
                    if (ip[0].y()<=ip[1].y())
                        ang=360-line1.angle(line2);
                    else
                        ang=line1.angle(line2);
                    QPainterPath bigPath = painter_path(w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(), 3, ang, ip[0],ip[1],ip[2],ip[3]);w->dc()["bordWdth"].toInt(),
                    circlePath = painter_path_simple(3, ang, ip[0], ip[1], ip[2], ip[3]);
		    shapeItems.append( ShapeItem(bigPath,circlePath, p[0], p[1], p[2], p[3], -1,QPointF(0,0), QBrush(w->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
			QPen( w->dc()["bordClr"].value<QColor>(), w->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::FlatCap, Qt::MiterJoin),
                              QPen( w->dc()["lineClr"].value<QColor>(), w->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin), w->dc()["lineWdth"].toInt(), w->dc()["bordWdth"].toInt(),3) );
                }
            }
        }
        up = true;
    }	
        
    if( up && !w->allAttrLoad( ) ) w->update();
    
    return up;
}

bool ShapeElFigure::shapeSave( WdgView *w )
{
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    QList<ShapeItem> &shapeItems = *(QList<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)w->dc().value("shapePnts",(void*)0).value< void* >();
    QPointF Prev_p_1, Prev_p_2, Prev_p_3, Prev_p_4, Prev_p_5;
    string elList;
    for( int i_s = 0; i_s < shapeItems.size(); i_s++ )
       switch( shapeItems[i_s].type )
	{
	    case 1:
		elList+="line:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+"\n";
		break;
	    case 2:
		elList+="arc:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+":"+
		    TSYS::int2str(shapeItems[i_s].n3)+":"+TSYS::int2str(shapeItems[i_s].n4)+":"+
		    TSYS::int2str(shapeItems[i_s].n5)+"\n";
                
                Prev_p_1=(*pnts)[shapeItems[i_s].n1];
                Prev_p_2=(*pnts)[shapeItems[i_s].n2];
                Prev_p_3=(*pnts)[shapeItems[i_s].n3];
                Prev_p_4=(*pnts)[shapeItems[i_s].n4];
                Prev_p_5=(*pnts)[shapeItems[i_s].n5];
                printf("StartMotionPos_save=(%f,%f)\n", Prev_p_1.x(), Prev_p_1.y());
                printf("EndMotionPos_save=(%f,%f)\n", Prev_p_2.x(), Prev_p_2.y());
                printf("CtrlMotionPos_1_save=(%f,%f)\n", Prev_p_3.x(), Prev_p_3.y());
                printf("CtrlMotionPos_2_save=(%f,%f)\n", Prev_p_4.x(), Prev_p_4.y());
                printf("CtrlMotionPos_3_save=(%f,%f)\n", Prev_p_5.x(), Prev_p_5.y());
		break;
	    case 3:
		elList+="bezier:"+TSYS::int2str(shapeItems[i_s].n1)+":"+TSYS::int2str(shapeItems[i_s].n2)+":"+
		    TSYS::int2str(shapeItems[i_s].n3)+":"+TSYS::int2str(shapeItems[i_s].n4)+"\n";
		break;
	}
    w->attrSet( "elLst", elList );

    //Write shapes points to data model
    for( PntMap::iterator pi = pnts->begin(); pi != pnts->end(); pi++ )
    {
        w->attrSet("p"+TSYS::int2str(pi.key())+"x",TSYS::int2str(pi.value().x()));
        w->attrSet("p"+TSYS::int2str(pi.key())+"y",TSYS::int2str(pi.value().y()));
    }
    //emit devW->selected(w->id());
    devW->setSelect(true);
}

void ShapeElFigure::editEnter( WdgView *view )
{
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(true);
    connect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)),
               this, SLOT(toolAct(QAction*)) );
    //-- Init actions' address --
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText(TSYS::addr2str(view).c_str());
    populateScene();
}
					
void ShapeElFigure::editExit( WdgView *view )
{
    disconnect( ((VisDevelop *)view->mainWin())->elFigTool, SIGNAL(actionTriggered(QAction*)),
                  this, SLOT(toolAct(QAction*)) );
    ((VisDevelop *)view->mainWin())->elFigTool->setVisible(false);
    //-- Clear action;s address --
    for( int i_a = 0; i_a < ((VisDevelop *)view->mainWin())->elFigTool->actions().size(); i_a++ )
        ((VisDevelop *)view->mainWin())->elFigTool->actions().at(i_a)->setIconText("");
}
								
void ShapeElFigure::toolAct( QAction *act )
{
    bool ptr_line,ptr_arc,ptr_bezier;
    //rectItems.clear();
    WdgView *w= (WdgView*)TSYS::str2addr(act->iconText().toAscii().data());
    if (act->toolTip()=="Add line to elementary figure")
    {
        shapeType=1;
        status=true;
    }
    if (act->toolTip()=="Add arc to elementary figure")
    {
        shapeType=2;
        status=true;
    }
    if (act->toolTip()=="Add Besier curve to elementary figure")
    {
        shapeType=3;
        status=true;
    }
    if (act->toolTip()=="Enable holds")
    {
        status_hold=act->isChecked();
        rectItems.clear();
        w->update();
    }
}

bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    QList<ShapeItem> &shapeItems = *(QList<ShapeItem> *)view->dc().value("shapeItems",(void*)0).value< void* >();
    PntMap *pnts = (PntMap*)view->dc().value("shapePnts",(void*)0).value< void* >();
    bool flag_hold_rect;
    flag_hold_rect=false;//Дополнительная переменная, служащая флагом для рисования кружочка при привязках
    switch(event->type())
    { 
       
        case QEvent::Paint:
        {
            QPainter pnt( view );
            //- Prepare draw area -
            int margin = view->dc()["geomMargin"].toInt();
            QRect draw_area = view->rect().adjusted(0,0,-2*margin,-2*margin);	    
            pnt.setWindow(draw_area);
            pnt.setViewport(view->rect().adjusted(margin,margin,-margin,-margin));
            
            //- Draw decoration -
            QColor bkcol = view->dc()["backgClr"].value<QColor>();
            if( bkcol.isValid() ) pnt.fillRect(draw_area,bkcol);
            QBrush bkbrsh = view->dc()["backgImg"].value<QBrush>();
            if( bkbrsh.style() != Qt::NoBrush ) pnt.fillRect(draw_area,bkbrsh);
            //Рисование всех экземпляров класса ShapeItem
            /////////////////////////////////////
            for (int k=0; k<=shapeItems.size() - 1; k++)
            {
               // printf("StartMotionPos_paint=(%f,%f)\n", shapeItems[k].startposition().x(), shapeItems[k].startposition().y());
                //printf("EndMotionPos_paint=(%f,%f)\n", shapeItems[k].endposition().x(), shapeItems[k].endposition().y());
                pnt.setBrush(shapeItems[k].brush);
                pnt.setPen(shapeItems[k].pen);
                pnt.drawPath(shapeItems[k].path);
                pnt.setPen(shapeItems[k].penSimple);
                pnt.setBrush(Qt::NoBrush);
                pnt.drawPath(shapeItems[k].pathSimple);
            }
            /////////////////////////////////////
            //Рисование всех экземпляров класса RectItem
            /////////////////////////////////////
            for (int k=0; k<=rectItems.size() - 1; k++)
            {
                pnt.setBrush(rectItems[k].brush);
                pnt.setPen(rectItems[k].pen);
                pnt.drawPath(rectItems[k].path);
            }
            pnt.setPen(QColor(0,0,0,255));
            pnt.drawPath(ellipse_draw_startPath);
            pnt.drawPath(ellipse_draw_endPath);
            /////////////////////////////////////
            return true;
        }
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            if (flag_down==0 && flag_up==0 && flag_left==0 && flag_right==0) //Условие ненажатия управляющих клавиш
            {
                if (ev->button() == Qt::RightButton)//Прекращение режима рисования
                {
                    view->unsetCursor();
                    status=false;
                }
               //Получение индекса перемещаемой фигуры и начальной позиции, от которой будет производиться перемещение
                if (ev->button() == Qt::LeftButton && (status==false))
                {
                  //Инициализация перемненных, отвечающих за то, какую комбинацию из двух точек связывать
                    /////////////////////
                    current_ss=-1;
                    current_se=-1;
                    current_es=-1;
                    current_ee=-1;
                    flag_holds=true;
                    /////////////////////
                    index = itemAt(ev->pos(),shapeItems);//Определение того, по какой фигуре клацнули
                    index_temp = index;//Определение индекса для управляющих клавиш(он не меняется в зависимости от перемещения)
                    previousPosition_all = ev->pos();//Определение начальной позиции для нескольких фигур(выделенных по CTRL или связанных по привязкам)
                    previousPosition = ev->pos();
                    if (index!= -1)//Проврка на то, что попали по фигуре
                    {
                        itemInMotion = &shapeItems[index];//Объявление указателя на выделенную фигуру
                        //previousPosition = ev->pos();//Определение начальной позиции для одной, не связанной ни с чем, фигуры
                        if (flag_ctrl && !status_hold) //Проверка на то, что нажат  Сtrl
                        {
                            bool fn=false;//Объявление и инициализация переменной, которая показывает клацали с CTRL по данной фигуре или клацают впервые
                            if (rectItems.size() && !count_Shapes)// Проверка на то, что была выделена фигура до нажатия Сtrl
                            {
                                index_array[0]=shapeItems.size()-1;//Запись этой выделенной фигуры в нулевой элемент массива фигур, выделяемых с CTRL
                                count_Shapes=1;
                            }
                            for (int i=0; i<count_Shapes; i++)
                                if (index_array[i]==index)
                                    fn=true;// Проверка на то, что фигура выделяется впервые
                            if (!fn)//То есть по фигуре клацнули впервые
                            {
                                index_array[count_Shapes]=index; //Добавление индекса фигуры в массив индексов выделенных с CTRL фигур
                                count_Shapes+=1;//Приращение колва фигур
                                itemInMotion=&shapeItems[index];
                                flag_ctrl_move=1;//Переменная показывае, что мы выделяем фигуру с CTRL и не начинаем её перемещать
                                moveItemTo(previousPosition, shapeItems,pnts);
                            }
                        }
                        if (status_hold)
                        {
                            count_holds=0;//Обнуление числа связанных фигур
                            //////////////////////////////////////////////////
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
                            //////////////////////////////////////////////////
                            Holds(shapeItems,pnts);
                            printf ("rect_num=%i\n",rect_num);
                            printf ("count_holds=%i\n",count_holds);
                           //Если есть связанные фигуры
                            ////////////////////////////
                            if (count_holds)
                            {
                                if (rect_num!= -1)
                                {
                                    int rect_num_temp=rect_num;//Запоминание истинного номера квадратика в контейнере rectitems
                                    rect_num=Real_rect_num(rect_num,shapeItems,pnts);
                                    printf ("Real_rect_num=%i\n",rect_num);
                                    itemInMotion = &shapeItems[index];
                                   //Проверка на то, что квадратик не startposition и не endposition
                                    ////////////////////////////////////////////////
                                    if ((rect_num==2 || rect_num==3) && shapeItems[index].type==3)                                  
                                        flag_rect=false;
                                  
                                    ////////////////////////////////////////////////
                                   //Проверка на то,  что квадратик или startposition или endposition
                                    /////////////////////////////////////////////
                                    if ( rect_num==0 || rect_num==1)
                                        Rect_num_0_1(shapeItems,rect_num_temp,pnts);
                                    /////////////////////////////////////////////
                                   //Если клацнули по третьему или четвертому квадратику дуги
                                    //////////////////////////////////////////////////////////
                                    if ((rect_num==3 ||rect_num==4) && shapeItems[index].type==2)
                                    {
                                       //(*pnts)[shapeItems[index].num_1()]
                                        Prev_pos_1=(*pnts)[shapeItems[index].n1];
                                        Prev_pos_2=(*pnts)[shapeItems[index].n2];
                                        printf("Prev_pos_1_before=(%f,%f)\n", Prev_pos_1.x(), Prev_pos_1.y());
                                        printf("Prev_pos_2_before=(%f,%f)\n", Prev_pos_2.x(), Prev_pos_2.y());
                                        Rect_num_3_4(shapeItems,pnts);
                                    }
                                    ////////////////////////////////////////////////////////////
                                }
                                ////////////////////////////
                                if ( rect_num==-1 )//Проврка на включение привязок
                                {
                                    offset_all=QPointF(0,0);//Определение нулевого смещения для связанных фигур
                                    count_moveItemTo=0;//Обнулени количества вызова процедуры moveItemTo для связанных фигур
                                   
                                  
                                    num_vector.clear();
                                    for (int i=0; i<=count_holds; i++)//Вызов moveItemTo для всех связанных фигур
                                    {
                                        count_moveItemTo+=1;
                                        flag_ctrl_move=0;//Переменная показывает, что мы перемещаем фигуры, выделенные по привязкам(по CTRL)
                                        flag_ctrl=1;//Возведение флага CTRL в 1(для привязок)
                                        itemInMotion=&shapeItems[index_array[i]];
                                        index=index_array[i];
                                        printf ("Dohli do moveItemTo\n");
                                        moveItemTo((*pnts)[itemInMotion->n1],shapeItems,pnts);
                                        printf ("Prohli do moveItemTo\n");
                                        view->repaint();
                                    }
                                }
                            }
                        }
                    }
                    else // Т.е не попали не по какой фигуре
                    {
                        rectItems.clear();
                        view->repaint();
                    }
                }
              //Получение начальной точки для рисования фигуры
                if ((ev->button() == Qt::LeftButton) && (status==true))
                    StartLine=ev->pos();
               //Перерисовка фигуры при её актвизации(не работает для связанных фигур)
                ///////////////////////////////////////
                if ((ev->buttons() && Qt::LeftButton) && (itemInMotion || (rect_num!=-1)) && (status==false) && flag_ctrl!=1 && count_holds==0)
                {
                    moveItemTo(ev->pos(),shapeItems,pnts);
                    view->repaint();
                    flag_cursor=1;
                }
            }

            return true;
        }

        case QEvent::MouseButtonDblClick:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            if (!flag_down && !flag_up && !flag_left && !flag_right) //Условие на ненажате управляющих клавиш
            {
                index = itemAt(ev->pos(),shapeItems);
                //Удаление фигуры из контейнера фигур и чистка контейнера квадратиков
                ///////////////////////////////////
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
		    shapeItems.removeAt(index);
                    rectItems.clear();
                }
                ///////////////////////////////////
                view->repaint();
            }
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            if (!flag_down && !flag_up && !flag_left && !flag_right) //Условие на ненажате управляющих клавиш
            {
                flag_cursor=false;//Сбрасывание флага для изменения вида курсора
                //Получение окончательной позиции для перемещения в неё фигуры
                /////////////////////////////////////
                if (ev->button() == Qt::LeftButton && itemInMotion && (status==false))
                {
                    //Привязываие одной фигуры к другой
                    ////////////////////////////////////////////
                    itemInMotion = &shapeItems[index_temp];
                    if( current_ss!=-1 )
		    {
                        if( itemInMotion->type==2 )//Если стыкует дугу, то к ней привязываем линию или Безье
                        {
                            shapeItems[current_ss].n1 = itemInMotion->n1;
                            itemInMotion = &shapeItems[current_ss];
                            index = current_ss;
                            moveItemTo(ev->pos(),shapeItems,pnts);
                            itemInMotion = &shapeItems[index_temp];
                            index = index_temp;
                        }
                        else
                            itemInMotion->n1 = shapeItems[current_ss].n1;
                    }
                    if(current_se!=-1)
                    {
                        if( itemInMotion->type==2 )//Если стыкует дугу, то к ней привязываем линию или Безье
                        {
                            shapeItems[current_se].n1 = itemInMotion->n2;
                            itemInMotion = &shapeItems[current_se];
                            index=current_se;
                            moveItemTo(ev->pos(),shapeItems,pnts);
                            itemInMotion = &shapeItems[index_temp];
                            index = index_temp;
                        }
                        else
                            itemInMotion->n2 = shapeItems[current_se].n1;
                    }
                    if(current_es!=-1)
                    {
                        if( itemInMotion->type == 2 )//Если стыкует дугу, то к ней привязываем линию или Безье
                        {
                            shapeItems[current_es].n2 = itemInMotion->n1;
                            itemInMotion = &shapeItems[current_es];
                            index=current_es;
                            moveItemTo(ev->pos(),shapeItems,pnts);
                            itemInMotion = &shapeItems[index_temp];
                            index = index_temp;
                        }
                        else
                            itemInMotion->n1 = shapeItems[current_es].n2;
                    }
                    if(current_ee!=-1)
                    {
                        if( itemInMotion->type == 2 )//Если стыкует дугу, то к ней привязываем линию или Безье
                        {
                            shapeItems[current_ee].n2 = itemInMotion->n2;
                            itemInMotion = &shapeItems[current_ee];
                            index = current_ee;
                            moveItemTo(ev->pos(),shapeItems,pnts);
                            itemInMotion = &shapeItems[index_temp];
                            index = index_temp;
                        }
                        else
                            itemInMotion->n2 = shapeItems[current_ee].n2;
                    }
                    //////////////////////////////////////////////
                    if( itemInMotion->type != 2 )
                    {
                        if ((!flag_ctrl && status_hold))//Проверка на то, что перемещаем одну фигуру с включёнными привязками
                        {
                            moveItemTo(ev->pos(),shapeItems,pnts);
                        }
                    }
                    ellipse_draw_startPath=newPath;//Обнуление путей кружочков привязок
                    ellipse_draw_endPath=newPath;//Обнуление путей кружочков привязок
                    view->repaint();
                    //Если двигали какой-то из квадратиков дуги, связанной с другими фигурами, то при Release вызываем для всех связанных 
                    //фигур moveItemTo, чтобы не потерять связь
                    if( count_holds && (flag_arc_rect_3_4 || (flag_rect && shapeItems[index_array[0]].type==2)))
                    {
                        count_moveItemTo=0;
                        flag_ctrl_move=false;
                        flag_ctrl=true;
                        flag_hold_arc=true;
                        offset_all=QPointF(0,0);
                        Move_all(ev->pos(),shapeItems,pnts);
                        view->repaint();
                    }
                    shapeSave( view );
                    itemInMotion = 0;//Обнуление указателя на выделенную фигуру
                    if (flag_ctrl && status_hold)//Проверка на то, что при включённых привязках, было связано несколько фигур
                    {
                        count_moveItemTo=0;
                        flag_hold_move=false;
                        flag_ctrl=false;
                        count_Shapes=0;
                        count_holds=0;
                        delete [] index_array;
                        if (count_rects && !flag_arc_rect_3_4 && flag_rect)
                            delete [] rect_array;
                        if (count_rects && flag_arc_rect_3_4)
                        {
                            delete [] arc_rect_array;
                            delete [] fig_rect_array;
                        }
                        flag_rect=false;
                        flag_arc_release=false;
                        count_rects=0;
                        flag_hold_arc=false;
                        rect_num_arc=-1;
                        flag_arc_rect_3_4=false;
                    }
                    if(!flag_ctrl)//Если не нажат CTRL и соответственно нет связанных фигур
                        flag_m=false;//Обнуление флага, сигнализирующего движение(то, что был MouseMove и не было MouseButtonRelease)

                }
                /////////////////////////////////////
                //Определение конечной точки и создание фигуры
                double ang;
                if ((ev->button() == Qt::LeftButton) && (status==true))
                { 
                    float Wdth=20.0;//Ширина фигуры
                    QLineF line1,line2;
                    EndLine=ev->pos();
                    if(EndLine==StartLine) break;
                    QPainterPath circlePath, bigPath;
                    //Если фигура линия
                    /////////////////////////////////////
                    if (shapeType==1)
                    {
                        //Если нажат Shift, то включить орто-режим
                        /////////////////////////////////////
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
				shapeItems.append( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0),QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                          view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                            }
                            else
                            {
                               
                                circlePath = painter_path_simple(1, ang, StartLine,EndLine);
                                QPainterPath bigPath = painter_path(view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1, ang, StartLine,EndLine);
				
				shapeItems.append( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                          view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(),1) );
                            }
                        
                            
                            shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems,pnts);
                            shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                            shapeSave( view );
                            view->repaint();
                        }
                        
                        /////////////////////////////////////
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
				shapeItems.append( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                          view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                            }
                            else
                            {
                               
                                circlePath = painter_path_simple(1, ang, StartLine,EndLine);
                                QPainterPath bigPath = painter_path(view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(),1, ang, StartLine,EndLine);
                    
				shapeItems.append( ShapeItem(bigPath,circlePath,-1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                        QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                              QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                          view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 1) );
                            }
                            shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine,shapeItems, pnts);
                            shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                            shapeSave( view );
                            view->repaint();
                        }
                    }
                    /////////////////////////////////////
                     //Если фигура кривая Безье
                    /////////////////////////////////////
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
                    
			    shapeItems.append( ShapeItem(circlePath, newPath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                               QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                                       QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                      view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(), 3) );
                        }
                        else
                        {
                            
                            bigPath = painter_path(view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(),3, ang, StartLine,EndLine, CtrlPos_1,CtrlPos_2);
                            circlePath = painter_path_simple(3, ang, StartLine,EndLine,CtrlPos_1,CtrlPos_2);
			    shapeItems.append( ShapeItem(bigPath,circlePath, -1,-1,-1,-1,-1,QPointF(0,0), QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                               QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                                       QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 3) );
                        }
                        
                        
                        shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine, shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n3 = Append_Point(CtrlPos_1,shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n4 = Append_Point(CtrlPos_2,shapeItems, pnts);
                        shapeSave( view );
                         view->repaint();
                    }
                    /////////////////////////////////////
                    //Если фигура дуга
                    /////////////////////////////////////
                    if (shapeType==2)
                    {
                        QPointF CtrlPos_1,CtrlPos_2,CtrlPos_3,CtrlPos_4,Temp,StartLine_small,EndLine_small,pnt;
                        double a,a_small,b,b_small;
                        double t;
                        CtrlPos_1=QPointF(StartLine.x()+(EndLine.x()-StartLine.x())/2,
                                          StartLine.y()+(EndLine.y()-StartLine.y())/2);
                        a=Length(EndLine,StartLine)/2/*+Wdth/2*/;
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
			    shapeItems.append( ShapeItem(circlePath, newPath, -1,-1,-1,-1, -1,CtrlPos_4,QBrush(view->dc()["lineClr"].value<QColor>(),Qt::SolidPattern),
                                               QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                                       QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2) );
                        }
                        else
                        {
                            QPainterPath bigPath = painter_path( view->dc()["lineWdth"].toInt(),view->dc()["bordWdth"].toInt(), 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
                            circlePath = painter_path_simple( 2, ang, StartLine, EndLine, CtrlPos_1, CtrlPos_2, CtrlPos_3, CtrlPos_4 );
			    shapeItems.append( ShapeItem(bigPath,circlePath,-1,-1,-1,-1, -1,CtrlPos_4, QBrush(view->dc()["lineClr"].value<QColor>(),Qt::NoBrush),
                                               QPen( view->dc()["bordClr"].value<QColor>(), view->dc()["bordWdth"].toInt(), Qt::NoPen,Qt::RoundCap, Qt::RoundJoin),
                                                       QPen( view->dc()["lineClr"].value<QColor>(), view->dc()["lineWdth"].toInt(), Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin),
                                      view->dc()["lineWdth"].toInt(), view->dc()["bordWdth"].toInt(), 2) );
                        }
                        
                        shapeItems[shapeItems.size()-1].n1 = Append_Point(StartLine, shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n2 = Append_Point(EndLine,shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n3 = Append_Point(CtrlPos_1,shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n4 = Append_Point(CtrlPos_2,shapeItems, pnts);
                        shapeItems[shapeItems.size()-1].n5 = Append_Point(CtrlPos_3,shapeItems, pnts);
                        shapeSave( view );
                        view->repaint();
                    }
                    /////////////////////////////////////
                    flag_key=0;
                }
            } 
            return true;
        }
        case QEvent::MouseMove:
        {
            int fl;//Вспомогательная переменная для игры с куrect_num=1
            int index_arc;
            int temp;
            QMouseEvent *ev = static_cast<QMouseEvent*>(event); 
            Mouse_pos=ev->pos();
            if ((ev->buttons() && Qt::LeftButton) && itemInMotion && !status)
            {
                flag_m=true;//Возведение флага, сигнализирующего движение
                if (count_holds)//Проврка на то, что существуют привязанные фигуры
                {
                    count_Shapes=count_holds+1; 
                    flag_ctrl=true;//Есть связанные фигуры
                    flag_hold_move=true;//Есть движение
                }
                //Проверка на нажатие  Сtrl и на наличие выделенных фигур, и на то, что попали по квадратику, общему для нескольких связанных фигур
                ///////////////////////////////////////////////////
                if (flag_ctrl && count_Shapes && ((rect_num==-1 && rect_num_arc==-1) || flag_rect || flag_arc_rect_3_4))
                {
                    offset_all=ev->pos()-previousPosition_all;//Определение смещения для всех фигур по тому на сколько смещается фигура, за которую
                                                                //тянут
                    count_moveItemTo=0;
                    //Вызов moveItemTo для всех выделенных фигур
                    ///////////////////////////////////////////
                    if (flag_rect || flag_arc_rect_3_4)//Если клацнули по квадратику, то число перемещаемых фигур равно числу фигур связаных с этим квадратиком
                        count_Shapes=count_rects;
                    flag_hold_arc=false;
                    if (shapeItems[index_array[0]].type==2)//Если среди связаных фигур есть дуга
                        flag_hold_arc=true;
                  //Перемещение всех связанных(выделенных с Ctrl) фигур
                    ////////////////////
                    Move_all(ev->pos(),shapeItems,pnts);
                    view->repaint();
                    //////////////////////
                    ///////////////////////////////////////////
                }
                ///////////////////////////////////////////////////
                else
                {
                    printf("1_Ctrl_popali\n", rect_num);
                    printf("1_rect_num=%i\n", rect_num);
                    //Если есть связанные фигуры(перемещение квадратика, не являющегося общим для нескольких связанных фигур)
                    ////////////////////////////////////
                    if (count_holds)
                    {
                        printf("3_Ctrl_popali\n", rect_num);
                        printf("3_rect_num=%i\n", rect_num);
                        offset_all=ev->pos()-previousPosition_all;
                        //Для того, чтобы фигура становилась на своё место в контейнере shapeItems
                        ////////////////////////////
                        flag_ctrl=true;
                        flag_rect=false;
                        count_moveItemTo=1;
                        flag_ctrl_move=false;
                        count_Shapes=1;
                        ////////////////////////////
                        itemInMotion=&shapeItems[index];
                        if (rect_num==2 && itemInMotion->type==2)
                        {
                            offset_all=QPointF(0,0);
                        }
                        if (itemInMotion->type==2 && rect_num!=2) rect_num=rect_num_arc;
                        num_vector.clear();
                        moveItemTo(ev->pos(),shapeItems,pnts);//Перемещение одной, ни с чем не связанной фигуры
                        flag_hold_rect=true;
                        view->repaint();
                    }
                    ////////////////////////////////////
                    else
                    {
                        if(!flag_ctrl)
                        {
                            printf("2_Ctrl_popali\n", rect_num);
                            printf("2_rect_num=%i\n", rect_num);
                            for (int i=0; i<shapeItems.size(); i++)
                            {
                            
                                if (i!=index)
                                {
                                    if( (shapeItems[index].n1==shapeItems[i].n1) || 
                                         (shapeItems[index].n1==shapeItems[i].n2) )
                                    {
                                        QPointF Temp=QPointF((*pnts)[shapeItems[index].n1].x(),(*pnts)[shapeItems[index].n1].y());
                                        shapeItems[index].n1 = Append_Point(Temp,shapeItems,pnts);
                                    }
                                    if( (shapeItems[index].n2==shapeItems[i].n1) || 
                                         (shapeItems[index].n2==shapeItems[i].n2) )
                                    {
                                        QPointF Temp=QPointF((*pnts)[shapeItems[index].n2].x(),(*pnts)[shapeItems[index].n2].y());
                                        shapeItems[index].n2 = Append_Point(Temp,shapeItems,pnts);  
                                    }
                                }
                            }
                            moveItemTo(ev->pos(),shapeItems,pnts);
                            view->repaint();
                        }
                    }
                    if (rect_num!=-1)
                        temp=Real_rect_num (rect_num,shapeItems,pnts);
                    if (status_hold &&(rect_num==-1||((temp==0 || temp==1) && !flag_rect)))//Если включены привязки и перемещается квадратик у несвязанной вигуры либо сама фигура
                    {
                        current_se=-1;
                        current_ss=-1;
                        current_ee=-1;
                        current_es=-1;
                        ellipse_draw_startPath=newPath;
                        ellipse_draw_endPath=newPath;
                        itemInMotion=&shapeItems[index];
                        for (int i=0;i<=shapeItems.size()-1;i++)
                        {
                            ellipse_startPath=newPath;
                            ellipse_endPath=newPath;
                            if (i!=index)
                            {
				ellipse_startPath.addEllipse((*pnts)[shapeItems[i].n1].x()-8,(*pnts)[shapeItems[i].n1].y()-8,16,16);
				if( ellipse_startPath.contains((*pnts)[shapeItems[index].n1]) )
                                { 
                                    if (temp==0 || rect_num==-1)
                                    {
                                        if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                        ellipse_draw_startPath.addEllipse((*pnts)[shapeItems[i].n1].x()-8,(*pnts)[shapeItems[i].n1].y()-8,16,16);
                                        current_ss=i;
                                    }
                                }
                                if( ellipse_startPath.contains((*pnts)[shapeItems[index].n2]) )
                                {
                                    if (temp==1 || rect_num==-1)
                                    {
                                        if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                        ellipse_draw_startPath.addEllipse((*pnts)[shapeItems[i].n1].x()-8,(*pnts)[shapeItems[i].n1].y()-8,16,16);
                                        current_se=i;
                                    }
                                }
                                ellipse_endPath.addEllipse((*pnts)[shapeItems[i].n2].x()-8,(*pnts)[shapeItems[i].n2].y()-8,16,16);
                                if( ellipse_endPath.contains((*pnts)[shapeItems[index].n2]) )
                                {
                                    if (temp==1 || rect_num==-1)
                                    {
                                        if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                        ellipse_draw_endPath.addEllipse((*pnts)[shapeItems[i].n2].x()-8,(*pnts)[shapeItems[i].n2].y()-8,16,16);
                                        current_ee=i;
                                    }
                                }
                                if( ellipse_endPath.contains((*pnts)[shapeItems[index].n1]) )
                                {
                                    if (temp==0 || rect_num==-1)
                                    {
                                        if(itemInMotion->type==2 && shapeItems[i].type==2) break;
                                        ellipse_draw_endPath.addEllipse((*pnts)[shapeItems[i].n2].x()-8,(*pnts)[shapeItems[i].n2].y()-8,16,16);
                                        current_es=i;
                                    }
                                }
                            }
                        }
                    }
                }
            }
             //Определяем вид курсора
            /////////////////////////////////////
            if (flag_cursor==0)
            {
                if (flag_down || flag_left || flag_right || flag_up) break;
                if (flag_first_move)//Если пред этим двигали фигуру с помощью управляющих клавиш обнуляем
                {
                    shapeSave( view );
                    if (flag_ctrl && status_hold)
                    {
                        flag_hold_move=false;
                        if (flag_rect)
                        {
                            offset_all=QPointF(0,0);
                            delete [] rect_array;
                        }
                        if (flag_arc_rect_3_4)
                        {
                            offset_all=QPointF(0,0);
                            delete [] arc_rect_array;
                            delete [] fig_rect_array;
                        }
                        count_Shapes=0;
                        count_holds=0;
                        count_rects=0;
                        flag_ctrl=false;
                        flag_rect=false;
                        flag_arc_rect_3_4=false;
                        flag_hold_rect=false;
                        delete [] index_array;
                        ;
                    }
                    flag_first_move=false;  
                }
                view->unsetCursor();
                fl = itemAt(ev->pos(),shapeItems);
                if ((fl!= -1)&&(rect_num ==-1)&&(status==false))
                    view->setCursor(Qt::SizeAllCursor);
                if ((status==false)&&(rect_num!=-1))
                    view->setCursor(Qt::SizeHorCursor);
                if (status==true)
                    view->setCursor(Qt::CrossCursor);
            }
            /////////////////////////////////////
            return true;
        }
        case QEvent::KeyPress:
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            if(flag_m)  break;
            if (ev->key() == Qt::Key_Shift) flag_key=true;
            if (ev->key() == Qt::Key_Alt)
                Holds(shapeItems,pnts);
            if (ev->key() == Qt::Key_Control)
            {
                if (status_hold) break;
                flag_ctrl=true;
                index_array=new int[shapeItems.size()];
                count_Shapes=0;
            }
            if ((ev->key() == Qt::Key_Up) && index_temp!=-1)
            {
                if (flag_down || flag_left || flag_right || index<0 || index>shapeItems.size()-1)
                {
                    break;
                }
                flag_up=true;
                offset_all=QPointF(0,-1);
                Move_UP_DOWN(shapeItems,pnts);
                view->repaint();
            }
            if ((ev->key() == Qt::Key_Down) && index_temp!=-1)
            {
                if (flag_up || flag_left || flag_right || index<0|| index>shapeItems.size()-1) break;
                flag_down=true;
                offset_all=QPointF(0,1);
                Move_UP_DOWN(shapeItems,pnts);
                view->repaint();
            }

            if ((ev->key() == Qt::Key_Left) && index_temp!=-1)
            {
                if (flag_down || flag_up || flag_right || index<0 || index>shapeItems.size()-1) break;
                flag_left=true;
                offset_all=QPointF(-1,0);
                Move_UP_DOWN(shapeItems,pnts);
                view->repaint();
            }
            if ((ev->key() == Qt::Key_Right) && index_temp!=-1)
            {
                if (flag_down || flag_left || flag_up || index<0 || index>shapeItems.size()-1) break;
                flag_right=true;
                offset_all=QPointF(1,0);
                Move_UP_DOWN(shapeItems,pnts);
                view->repaint();
            }
            return true;
        }
        case QEvent::KeyRelease:
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            if (ev->key() == Qt::Key_Shift) flag_key=false;
            if (ev->key() == Qt::Key_Control)
            {
                if(status_hold ) break;
                if(count_Shapes)
                {
                    count_Shapes=0;
                    rectItems.clear();
                    delete [] index_array;
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


    return false;
} 

void ShapeElFigure::populateScene()
{
    scene = new QGraphicsScene;//Создание Move_allэкземпляра класса QGraphicsScene
}

QPointF ShapeElFigure::ROTATE(const QPointF &pnt, double alpha)
{
    QPointF rotate_pnt;
    rotate_pnt=QPointF (pnt.x()*cos((alpha*M_PI)/180)-pnt.y()*sin((alpha*M_PI)/180),
                        pnt.x()*sin((alpha*M_PI)/180)+pnt.y()*cos((alpha*M_PI)/180));
    //printf("pnt_rotate=(%f,%f)\n", rotate_pnt.x(), rotate_pnt.y());
    //printf("EndMotionPos__rotate=(%f,%f)\n", EndMotionPos.x(), EndMotionPos.y());
    return  rotate_pnt;
}


QPointF ShapeElFigure::UNROTATE(const QPointF &pnt, double alpha, double a, double b)
{
    QPointF unrotate_pnt;
    unrotate_pnt=QPointF ((pnt.x()-a)*cos((alpha*M_PI)/180)-(pnt.y()-b)*sin((alpha*M_PI)/180),
                           -(pnt.x()-a)*sin((alpha*M_PI)/180)-(pnt.y()-b)*cos((alpha*M_PI)/180));
    return  unrotate_pnt;
}

QPointF ShapeElFigure::ARC(double t,double a,double b)
{
    QPointF arc_pnt;  
    arc_pnt=QPointF(a*cos(t*M_PI*2),-b*sin(t*M_PI*2));
    //printf("arc\n");
    return arc_pnt;
}


double ShapeElFigure::Angle(const QLineF &l,const QLineF &l1)
{
    if (l.isNull() || l1.isNull())  return 0;
    double cos_line = (l.dx()*l1.dx() + l.dy()*l1.dy()) / (l.length()*l1.length());
    double rad = 0;
    if (cos_line >= -1.0 && cos_line <= 1.0) rad = acos( cos_line );
    return rad * 180 / M_PI;
}


double ShapeElFigure::Length(const QPointF pt1, const QPointF pt2)
{
    double x = pt2.x() - pt1.x();
    double y = pt2.y() - pt1.y();
    return sqrt(x*x + y*y);
}


//Процедура передвижения фигуры(изменения её размеров)
void ShapeElFigure::moveItemTo(const QPointF &pos,QList<ShapeItem> &shapeItems, PntMap *pnts)
{
    ShapeItem temp_shape;
    double ang_t;//Вспомогательный угол для определения t_start и t_end
    double a,a_small,b,b_small;//Два радиуса большой и маленькой дуг
    double ang;//Угол поворота фигуры относительно начала координат
    QPointF StartMotionPos;//Первая контрольная точка(StartLine)
    QPointF EndMotionPos;//Вторая контрольная точка(EndLine)
    QPointF CtrlMotionPos_1;//Третья контрольная точка
    QPointF CtrlMotionPos_2;//Четвёртая контрольная точка
    QPointF CtrlMotionPos_3;//Пятая контрольная точка(для дуги)
    QPointF CtrlMotionPos_4;//Шестая контрольная точка(для дуги)
    QPointF offset = pos - previousPosition;//Определение смещения перемещаемой точки
    int MotionNum_1=itemInMotion->n1;
    int MotionNum_2=itemInMotion->n2;
    int MotionNum_3=itemInMotion->n3;
    int MotionNum_4=itemInMotion->n4;
    int MotionNum_5=itemInMotion->n5;
    shapeType= itemInMotion->type;//Определение типа активной фигуры
    QBrush MotionBrush=itemInMotion->brush;//Определение заливки активной фигуры
    QPen MotionPen=itemInMotion->pen;//Определение пера активной фигуры
    QPen MotionPenSimple=itemInMotion->penSimple;
    float MotionWidth=itemInMotion->width;//Определение толщины активной фигуры
    float MotionBorderWidth=itemInMotion->border_width;
    QLineF line1,line2;//Линии, использующиеся для определения ang или ang_t
    QPointF Temp,EndMotionPos_temp,CtrlMotionPos_1_temp,CtrlMotionPos_2_temp;//Вспомогательные переменные для выполнения UNROTATE
    //Определение единичного смещения в зависимости от того, какая клавиша нажата
    /////////////////////////////////////////////////////////////////////////////
    if (flag_up) offset=QPointF(0,-1);
    if (flag_down) offset=QPointF(0,1);
    if (flag_left) offset=QPointF(-1,0);
    if (flag_right) offset=QPointF(1,0);
    if (flag_ctrl && !flag_ctrl_move)
        offset=offset_all;//Задание offset при одновременном перемещении нескольких фигур
    /////////////////////////////////////////////////////////////////////////////
    //printf("shapeType=%i\n",shapeType);
    //printf("rect_num_1=%i\n",rect_num);
    
    bool flag_MotionNum_1=false;
    bool flag_MotionNum_2=false;
    printf ("num_vector.size()=%i\n",num_vector.size());
    for (int i=0; i<num_vector.size(); i++)
    {
        if (num_vector[i]==MotionNum_1) flag_MotionNum_1=true;
        if (num_vector[i]==MotionNum_2) flag_MotionNum_2=true;
    }

    if (rect_num==-1)//Указывает на то, что перемещать всю фигуру целиком

    {
        if (status_hold && count_holds)
        {
        if (!flag_MotionNum_1 )
        {
            printf ("Popali !flag_MotionNum_1\n");
            printf ("MotionNum_1=%i\n",MotionNum_1);
        StartMotionPos=(*pnts)[itemInMotion->n1] + offset;
        num_vector.append(MotionNum_1);
        }
        else  StartMotionPos=(*pnts)[itemInMotion->n1];
        if (!flag_MotionNum_2)
        {
            printf ("Popali !flag_MotionNum_2\n");
            printf ("MotionNum_2=%i\n",MotionNum_2);
        EndMotionPos= (*pnts)[itemInMotion->n2]+offset;
        num_vector.append(MotionNum_2);
        }
        else EndMotionPos= (*pnts)[itemInMotion->n2];
        }
        else
        {
        StartMotionPos=(*pnts)[itemInMotion->n1] + offset;
        EndMotionPos= (*pnts)[itemInMotion->n2]+offset; 
        }  
        CtrlMotionPos_1=(*pnts)[itemInMotion->n3] + offset;
        CtrlMotionPos_2= (*pnts)[itemInMotion->n4] + offset;
        
       if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            CtrlMotionPos_4=itemInMotion->ctrlPos4;
            CtrlMotionPos_3=(*pnts)[itemInMotion->n5]+offset;
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
    if (rect_num==0)//Указывает на то, что перемещать StartLine, выбранной фигуры
    {
        EndMotionPos= (*pnts)[itemInMotion->n2];

        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            StartMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
                StartMotionPos=(*pnts)[itemInMotion->n1]+offset;
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4];
            CtrlMotionPos_3=(*pnts)[itemInMotion->n5];
            CtrlMotionPos_4=itemInMotion->ctrlPos4;
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
            if (!flag_hold_arc && !flag_arc_rect_3_4 )//Если фигура не связана с дугой
            {
                if (status_hold && count_holds && flag_rect)
                if (!flag_MotionNum_1)
                {
                    printf ("Popali !flag_MotionNum_1\n");
                    printf ("MotionNum_1=%i\n",MotionNum_1);
                    StartMotionPos=(*pnts)[itemInMotion->n1] + offset;
                    num_vector.append(MotionNum_1);
                }
                else  StartMotionPos=(*pnts)[itemInMotion->n1];
                else
                StartMotionPos= (*pnts)[itemInMotion->n1] +offset;
                CtrlMotionPos_1= (*pnts)[itemInMotion->n3]+offset;
                CtrlMotionPos_2= (*pnts)[itemInMotion->n4]+offset;
            }
            if (flag_hold_arc || flag_arc_rect_3_4)//Если фигура связана с дугой
            {
                if (arc_rect==0)
                {
                    StartMotionPos=(*pnts)[shapeItems[index_array[0]].n1];
                    CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
                    CtrlMotionPos_2=(*pnts)[itemInMotion->n4]+offset;
                }
                if (arc_rect==1)
                {
                    StartMotionPos=(*pnts)[shapeItems[index_array[0]].n2];
                    CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
                    CtrlMotionPos_2=(*pnts)[itemInMotion->n4]+offset;
                }
            }
        }
    }
    if (rect_num==1)//Указывает на то, что перемещать EndLine, выбранной фигуры
    {
        StartMotionPos=(*pnts)[itemInMotion->n1];
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            EndMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
                EndMotionPos=(*pnts)[itemInMotion->n2]+offset;
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4];
            CtrlMotionPos_3=(*pnts)[itemInMotion->n5];
            CtrlMotionPos_4=itemInMotion->ctrlPos4;
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
            if (!flag_hold_arc && !flag_arc_rect_3_4)//Если фигура не связана с дугой
            { 
                if (status_hold && count_holds && flag_rect)
                if (!flag_MotionNum_2)
                {
                    printf ("Popali !flag_MotionNum_2\n");
                    printf ("MotionNum_2=%i\n",MotionNum_2);
                    EndMotionPos= (*pnts)[itemInMotion->n2]+offset;
                    num_vector.append(MotionNum_2);
                }
                else EndMotionPos= (*pnts)[itemInMotion->n2];
                else
                EndMotionPos= (*pnts)[itemInMotion->n2]+offset;
                CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
                CtrlMotionPos_2= (*pnts)[itemInMotion->n4]+offset;
            }
            if (flag_hold_arc || flag_arc_rect_3_4)//Если фигура связана с дугой
            {
                printf ("Popali flag_hold_arc || flag_arc_rect_3_4\n");
                if (arc_rect==0)
                {
                    EndMotionPos=(*pnts)[shapeItems[index_array[0]].n1];
                    CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
                    CtrlMotionPos_2=(*pnts)[itemInMotion->n4]+offset;
                }
                if (arc_rect==1)
                {
                    EndMotionPos=(*pnts)[shapeItems[index_array[0]].n2];
                    CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
                    CtrlMotionPos_2=(*pnts)[itemInMotion->n4]+offset;
                }
            }

        }
    }
    if (rect_num==2)//Указывает на то, что перемещать третью контрольную точку, выбранной фигуры
    {
        StartMotionPos=(*pnts)[itemInMotion->n1];
        EndMotionPos=(*pnts)[itemInMotion->n2]; 
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            StartMotionPos=(*pnts)[itemInMotion->n1];
            EndMotionPos=(*pnts)[itemInMotion->n2];
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4];
            CtrlMotionPos_3=(*pnts)[itemInMotion->n5];
            CtrlMotionPos_4=itemInMotion->ctrlPos4;
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
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4];
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3]+offset;
        }
    }
    if (rect_num==3)//Указывает на то, что перемещать четвёртую контрольную точку, выбранной фигуры
    {
        StartMotionPos=(*pnts)[itemInMotion->n1];
        EndMotionPos=(*pnts)[itemInMotion->n2];
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4]+offset;
            CtrlMotionPos_3=(*pnts)[itemInMotion->n5];
            CtrlMotionPos_4=itemInMotion->ctrlPos4;
            EndMotionPos=(*pnts)[itemInMotion->n2];
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
            CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
            CtrlMotionPos_2=(*pnts)[itemInMotion->n4] + offset;
        }
    }
    if (rect_num==4)//Указывает на то, что перемещать пятую контрольную точку, выбранной фигуры
    {
        CtrlMotionPos_1=(*pnts)[itemInMotion->n3];
        CtrlMotionPos_2=(*pnts)[itemInMotion->n4];
        CtrlMotionPos_3=(*pnts)[itemInMotion->n5]+offset;
        CtrlMotionPos_4=itemInMotion->ctrlPos4;
        EndMotionPos=(*pnts)[itemInMotion->n2];
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
    shapeItems.removeAt(index);
    if (!flag_ctrl || (!flag_ctrl_move && count_Shapes==count_moveItemTo+count_Shapes-1))
        //Проверка на то, что не нажат Сtrl или нажат Сtrl,
        //выполняется движение и оно прошло один цикл
        rectItems.clear();//Очищает контейнер кваflag_arc_release=false;дратиков
    if (shapeType==1)//Если выбранная фигура линия
    {
        line2=QLineF(StartMotionPos,QPointF(StartMotionPos.x()+10,StartMotionPos.y()));
        line1=QLineF(StartMotionPos,EndMotionPos);
        if (StartMotionPos.y()<=EndMotionPos.y()) ang=360-Angle(line1,line2);
        else ang=Angle(line1,line2);
	shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 1, ang, StartMotionPos, EndMotionPos), painter_path_simple(1, ang, StartMotionPos,EndMotionPos),
                        MotionNum_1, MotionNum_2, -1, -1, -1, QPointF(0,0), MotionBrush, MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 1) );
        printf ("Dohli do pnts_temp\n ");
       
        (*pnts).insert(MotionNum_1,StartMotionPos.toPoint ());
        (*pnts).insert(MotionNum_2,EndMotionPos.toPoint ());
              
        /////////////////////////////////////////////////////
        //Определение центра квадратика_2
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
	
	rectItems.append( RectItem(rectPath ,MotionNum_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;//Обнуление пути
        //Определение центра квадратика_1
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;//Обнуление пути
    }
    if (shapeType==2)//Если выбранная фигура дуга
    {
       double t;
        CtrlMotionPos_2=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                CtrlMotionPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
        CtrlMotionPos_4=QPointF(t_start,t_end);
        CtrlMotionPos_3=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                CtrlMotionPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
        shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                        painter_path_simple(2, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4),
                                            MotionNum_1,MotionNum_2,MotionNum_3,MotionNum_4,MotionNum_5,CtrlMotionPos_4, MotionBrush,MotionPen, MotionPenSimple, MotionWidth, MotionBorderWidth, 2) );
        
        (*pnts).insert(MotionNum_1,StartMotionPos.toPoint ());
        (*pnts).insert(MotionNum_2,EndMotionPos.toPoint ());
        (*pnts).insert(MotionNum_3,CtrlMotionPos_1.toPoint ());
        (*pnts).insert(MotionNum_4,CtrlMotionPos_2.toPoint ());
        (*pnts).insert(MotionNum_5,CtrlMotionPos_3.toPoint ());
        
 //       circlePath=newPath;//Обнуление пути
        //Рисование пяти квадратиков в четырёх контрольных точках
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath,MotionNum_1,QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF( CtrlMotionPos_1.toPoint(),QSize(8,8)));
	rectItems.append( RectItem(rectPath , MotionNum_3, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(CtrlMotionPos_2.x()-4,CtrlMotionPos_2.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath,MotionNum_4, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;
        Temp=QPointF(CtrlMotionPos_3.x()-20,CtrlMotionPos_3.y()-4);
        rectPath.addRect(QRectF(Temp,QSize(8,8)));
	rectItems.append( RectItem(rectPath, MotionNum_5, QBrush(QColor(0,0,0,255),Qt::SolidPattern), 
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin)) );
        //////////////////////////////////////////////////////
        rectPath=newPath;
    }
    if (shapeType==3)//Если выбранная фигура кривая Безье
    {
        line2=QLineF(StartMotionPos,QPointF(StartMotionPos.x()+10,StartMotionPos.y()));
        line1=QLineF(StartMotionPos,EndMotionPos);
        if (StartMotionPos.y()<=EndMotionPos.y())
            ang=360-line1.angle(line2);
        else
            ang=line1.angle(line2);
        shapeItems.append( ShapeItem(painter_path(MotionWidth,MotionBorderWidth, 3, ang, StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                        painter_path_simple(3, ang,StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2),
                                            MotionNum_1,MotionNum_2,MotionNum_3, MotionNum_4,-1,QPointF(0,0),MotionBrush,MotionPen,MotionPenSimple,MotionWidth, MotionBorderWidth, 3) );
        (*pnts).insert(MotionNum_1,StartMotionPos.toPoint ());
        (*pnts).insert(MotionNum_2,EndMotionPos.toPoint ());
        (*pnts).insert(MotionNum_3,CtrlMotionPos_1.toPoint ());
        (*pnts).insert(MotionNum_4,CtrlMotionPos_2.toPoint ());
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath ,MotionNum_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;//Обнуление пути
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
	rectItems.append( RectItem(rectPath ,MotionNum_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        rectPath=newPath;//Обнуление пути
        //Построение квадратика_3
        ////////////////////////////////////////////////
        rectPath.addRect(QRectF( CtrlMotionPos_1.toPoint(),QSize(8,8)));
	rectItems.append( RectItem(rectPath, MotionNum_3, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        ////////////////////////////////////////////////
        rectPath=newPath;//Обнуление пути
        //Построение квадратика_4
        ////////////////////////////////////////////////
        rectPath.addRect(QRectF( CtrlMotionPos_2.toPoint(),QSize(8,8)));
	rectItems.append( RectItem(rectPath,MotionNum_4, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                          QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin)) );
        ////////////////////////////////////////////////
        rectPath=newPath;//Обнуление пути
    }

 //   circlePath=newPath;//Обнуление пути
    if (!flag_ctrl)// Если не нажат Сtrl, фигура добавляется в конец контейнера
    {
        index=shapeItems.size()-1;
        index_temp=index;
        itemInMotion =&shapeItems[shapeItems.size()-1];
        previousPosition = pos;//Определение предыдущей позиции фигуры
    }
    if (flag_ctrl)//Если  нажат Сtrl, фигура добавляется в  контейнер на свое место
        for (int i=shapeItems.size()-1; i>index; i--)
    {
        temp_shape=shapeItems[i-1];
        shapeItems[i-1]=shapeItems[i];
        shapeItems[i]=temp_shape;
    }

    if (flag_ctrl && flag_ctrl_move) itemInMotion = &shapeItems[index];//Если нажат Сtrl и нет движения
    if (count_moveItemTo==count_Shapes && !flag_ctrl_move && flag_ctrl) 
    {
        previousPosition_all=pos;//Если нажат Сtrl, есть движение и цикл выполнился один раз для всех перемещаемых фигур
    }
    
}

bool ShapeElFigure::Holds(QList<ShapeItem> &shapeItems, PntMap *pnts)
{
    printf ("Holds\n");
    int num,index_hold;
    bool flag_equal;
    index_array=new int[shapeItems.size()];
   
   
    for (int l=0; l<shapeItems.size(); l++)
        index_array[l]=-1;
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
    printf ("Prohli\n");
    if (count_holds>0) return true;
    else return false;
}

void ShapeElFigure::Move_UP_DOWN(QList<ShapeItem> &shapeItems, PntMap *pnts)
{
    int rect_num_temp;
    count_moveItemTo=0;
    if (flag_ctrl && count_Shapes)
    {
        Move_all(QPointF(0,0),shapeItems,pnts);
    }
    else 
        if (!flag_first_move)
    {
        count_holds=0;
        flag_rect=false;

        Holds(shapeItems,pnts);//Вызов Holds для определения связана фигура с другими или нет
        if (count_holds)//Проверка на нажатие  Сtrl и на наличие выделенных фигур
        {
            count_Shapes=count_holds+1;//Числу перемещаемых фигур присваиваем число всязаных
            if (rect_num!=-1)//Если попали по квадратику
            {
                rect_num_temp=rect_num;
                rect_num=Real_rect_num (rect_num,shapeItems,pnts);//Определяем номер квадратика, кот. нужен moveItemTo
                if( (rect_num==2 || rect_num==3) && shapeItems[index].type==3 )//Если попали по несвязанному квадратику Бизье
                    flag_rect=false;
                if( rect_num==0 || rect_num==1 )//Если попали по 1,2 квадратику
                {
                    Rect_num_0_1(shapeItems,rect_num_temp,pnts);
                }
                if( (rect_num==3 ||rect_num==4) && shapeItems[index].type==2 )//Если попали по 3,4 квадратику дуги
                    Rect_num_3_4(shapeItems,pnts);
            }
        }
        if (flag_rect || flag_arc_rect_3_4) //Если клацнули по квадратику, то число перемещаемых фигур равно числу фигур связаных с этим квадратиком
        {
            count_Shapes=count_rects;
        }
    }
    if (flag_rect || flag_arc_rect_3_4 || (rect_num==-1 && count_holds))//Вызов moveItemTo для всех выделенных фигур(если тяним за квадртик общий для нескольких фигур,
                                                                       //3,4 квадратик связанной с другими фигурами дуги, просто связанные фигуры)
    {
        Move_all(QPointF(0,0),shapeItems,pnts);
    }
    if ((!flag_ctrl  || (!flag_rect && rect_num!=-1)) && !flag_arc_rect_3_4)//Если двигаем несвязанную фигуру или ее квадратик
        if (index!=-1)
    {
        flag_ctrl=true;
        flag_ctrl_move=false;
        count_moveItemTo=1;
        count_Shapes=1;
        itemInMotion=&shapeItems[index];
        moveItemTo((*pnts)[itemInMotion->n1],shapeItems,pnts);
        flag_ctrl=false;
    }
    else
        itemInMotion=0;
}

int ShapeElFigure::Real_rect_num(int rect_num_old,QList<ShapeItem> &shapeItems, PntMap *pnts)
{
    int rect_num_new;
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
    printf ("index=%i\n",index);
    printf ("rect_num_old=%i\n",rect_num_old);
    printf ("rectItems[rect_num_old].num()=%i\n",rectItems[rect_num_old].num);                           
    //Определение номера квадратика, который нужно передать в moveItemTo
    ///////////////////////////////////////////
    switch( shapeItems[index].type )
    {
	case 1:
	    printf("shapeItems[index].type()==1\n");
	    printf("rectItems[rect_num_old].num()=%i\n",rectItems[rect_num_old].num);
	    printf("shapeItems[index].num_1()=%i\n",shapeItems[index].n1);
	    printf("shapeItems[index].num_2()=%i\n",shapeItems[index].n2);
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

void ShapeElFigure::Rect_num_0_1(QList<ShapeItem> &shapeItems,int rect_num_temp, PntMap *pnts)
{
    flag_rect=true;
    int* index_array_temp=new int[count_holds+1];//Выделение памяти под массив, в котором хранятся индексы фигур, у которых есть общие точки
    rect_array=new int[count_holds+1];//Выделение памяти под массив, в которм хранятся индексы(для moveItemTo) квадратиков для фигур, у которых есть общие точки
    count_rects=0;//Обнуление количества квадратиков в массиве|
    //Перебор всех связанных фигур, чтобы определить, у каких из них есть общие точки
    /////////////////////////////////////////////////////////////////////////////////
    for (int i=0; i<=count_holds; i++)
    {
        if ((*pnts)[rectItems[rect_num_temp].num]==(*pnts)[shapeItems[index_array[i]].n1])
        {
            index_array_temp[count_rects]=index_array[i];//Заполняем массив связанными фигурами с общими точками
            rect_array[count_rects]=0;//Заполняем массив номерми квадратиков для moveItemTo
            count_rects++;
            flag_rect=true;//Указывает на то, что мы щёлкнули по квадратику, который является общим для нескольких фигур
        }
        if ((*pnts)[rectItems[rect_num_temp].num]==(*pnts)[shapeItems[index_array[i]].n2])
        {
            index_array_temp[count_rects]=index_array[i];
            rect_array[count_rects]=1;
            count_rects++;
            flag_rect=true;//Указывает на то, что мы щёлкнули по квадратику, который является общим для нескольких фигур
        }
    }
    /////////////////////////////////////////////
    delete [] index_array;//Обнуление массива связанных фигур
    index_array=new int[count_rects+1];
    //Переформирование массива связанных фигур
    /////////////////////////////////////////////
    for (int i=0; i<count_rects; i++)
        index_array[i]=index_array_temp[i];
    /////////////////////////////////////////////
    // Если в index_array есть дуга ставим ее на первое место
    ////////////////////////////////////////////////////////
    int num_arc=-1;
    for (int i=0; i<count_rects; i++)
        if (shapeItems[index_array[i]].type==2)
    {
        flag_hold_arc=true;// flag_hold_arc для moveItemTo
        num_arc=i;//Номер дуги в index_array
    }
    if (num_arc!=-1)//Дуга есть
    {
        int index_0=index_array[0];
        int rect_0=rect_array[0];
        index_array[0]=index_array[num_arc];
        rect_array[0]=rect_array[num_arc];
        index_array[num_arc]=index_0;
        rect_array[num_arc]=rect_0;
    }
    ////////////////////////////////////////////////////////
    // count_rects==1-фигура несвязана с другими
    if (count_rects==1)
    {
        flag_rect=false;
        if (shapeItems[index_array[0]].type==2)
            rect_num_arc=rect_num;
    }
    /////////////////////////////////
    delete [] index_array_temp;

}
void ShapeElFigure::Rect_num_3_4(QList<ShapeItem> &shapeItems,PntMap *pnts)
{
    printf ("POPALI Rect_num_3_4\n");
    flag_arc_rect_3_4=true;
    //Выделение памяти под index_array_temp,arc_rect_array,fig_rect_array
    ////////////////////////////////////////////////////////////////////
    int* index_array_temp=new int[count_holds+1];
    arc_rect_array=new int[count_holds+1];
    fig_rect_array=new int[count_holds+1];
    ////////////////////////////////////////////////////////////////////
    flag_rect=false;
    index_array_temp[0]=index;//на первое место в массиве связанных фигур ставится фигура, по кот. клацнули в данном случае дуга
    //Нулевые позиции   flag_hold_arc=false;в массивах arc_rect_array и fig_rect_array занимает номер квадратика дуги, по которому клацули (третий или четвертый)
    /////////////////////////////////////////////////////////////////////
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
    
    /////////////////////////////////////////////////////////////////////
    count_rects=1;
  //Для заполнения массивов index_array_temp, fig_rect_array,arc_rect_array находим какие фигуры,
 //какими точками связаны с какими точками дуги соответственно
    /////////////////////////////////////////////////////////////////
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
    /////////////////////////////////////////////////////////////////
    delete [] index_array;//Обнуление массива связанных фигур
    //Переформирование массива связанных фигур
    /////////////////////////////////////////////
    index_array=new int[count_rects];
    for (int i=0; i<count_rects; i++)
        index_array[i]=index_array_temp[i];
    ////////////////////////////////////////////
    delete [] index_array_temp;
}

void ShapeElFigure::Move_all(QPointF pos,QList<ShapeItem> &shapeItems,PntMap *pnts)
{
    printf("Pop v Move_all");
   
    num_vector.clear();
    for (int i=0; i<count_Shapes; i++)
    {
        count_moveItemTo+=1;
        flag_ctrl_move=false;
        flag_ctrl=true;
        itemInMotion=&shapeItems[index_array[i]];
        if (flag_rect)
        {
            rect_num=rect_array[i]; //Номер перемещаемого квадратика
            arc_rect=rect_array[0]; //С каким квадратико дуги связан перемещаемый квадратик фигуры
        }
        if (flag_arc_rect_3_4)
        {
            if (i==0) offset_all=pos-previousPosition_all;
            else
                if (arc_rect_array[i]==0) 
            {
                offset_all=(*pnts)[shapeItems[index_array[0]].n1]-Prev_pos_1;
                printf("Prev_pos_1\n");
            }
            else 
            {
                offset_all=(*pnts)[shapeItems[index_array[0]].n2]-Prev_pos_2;
                printf("Prev_pos_1\n");
            }
            rect_num=fig_rect_array[i];//Номер перемещаемого квадратика
//            printf ("rect_num=%i\n",rect_num);
            arc_rect=arc_rect_array[i];//С каким квадратико дуги связан перемещаемый квадратик фигуры
        }
        index=index_array[i];
        moveItemTo(pos,shapeItems,pnts);
        if (i==0 && flag_arc_rect_3_4)
        {
            Prev_pos_1=(*pnts)[shapeItems[index_array[0]].n1];
            Prev_pos_2=(*pnts)[shapeItems[index_array[0]].n2];
        }
    }
    printf("Prev_pos_1=(%f,%f)\n", Prev_pos_1.x(), Prev_pos_1.y());
    printf("Prev_pos_2=(%f,%f)\n", Prev_pos_2.x(), Prev_pos_2.y());
    printf("Prohli  Move_all\n");
    
}


//Процедура обнаружения фигуры или квадратика в данной точкеcount_rects+=1;
int ShapeElFigure::itemAt(const QPointF &pos, QList<ShapeItem> &shapeItems)
{
    QPointF point;
    rect_num=-1;
    int* index_array_temp;
    for (int j =0; j <=rectItems.size()-1; j++) 
    {
        const RectItem &item1 = rectItems[j];
        if (item1.path.contains(pos)) rect_num=j;
    }
    for (int i =shapeItems.size()-1; i >=0; i--) 
    {
        const ShapeItem &item = shapeItems[i];
        if (item.path.contains(pos)) return i;
        for(int j=3; j>0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if (item.path.contains(pos+point)) return i;
        }
        for(int j=3; j>0; j-- )
        {
            point.setY(j);
            point.setX(j);
            if (item.path.contains(pos-point)) return i;
        }
    }
    if(rect_num==-1) return -1;
    else return shapeItems.size()-1;
}

 QPainterPath ShapeElFigure::painter_path(float el_width, float el_border_width, int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6)
{
    double arc_a_small, arc_b_small, t, t_start, t_end;
    QPointF  CtrlMotionPos_1_temp, CtrlMotionPos_2_temp, EndMotionPos_temp;
    //QLineF line1,line2;
    QPainterPath circlePath;//Путь для создания экземнпляра класса ShapeItem
    double arc_a, arc_b;
    
    circlePath=newPath;
    if(el_type==1)
    {
       // el_width=el_width;
        printf ("el_width/2=%f\n",el_width);
        printf ("el_width/2+el_border_width=%f\n",el_width/2+el_border_width);
        el_border_width=el_border_width/2;
        circlePath.moveTo(el_p1.x()+ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).y());
        circlePath.lineTo(el_p1.x()+ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).y());
        circlePath.lineTo(el_p1.x()+ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).y());
        circlePath.lineTo(el_p1.x()+ROTATE(QPointF(-el_border_width,(el_width/2+el_border_width)),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(-el_border_width,(el_width/2+el_border_width)),el_ang).y());
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    if(el_type==2)
    {
        arc_a=Length(el_p5,el_p3)+el_width/2+el_border_width;
        arc_b=Length(el_p3,el_p4)+el_width/2+el_border_width;
        arc_a_small=arc_a-el_width-el_border_width;
        arc_b_small=arc_b-el_width-el_border_width;
        t_start=el_p6.x();
        t_end=el_p6.y();
        circlePath.moveTo(el_p3.x()+ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).x(),
                          el_p3.y()-ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).y());
        circlePath.lineTo(el_p3.x()+ROTATE(ARC(t_start,arc_a,arc_b),el_ang).x(),
                          el_p3.y()-ROTATE(ARC(t_start,arc_a,arc_b),el_ang).y());//QPointF(50,50));
        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
            circlePath.lineTo(QPointF(el_p3.x()+ROTATE(ARC(t,arc_a,arc_b),el_ang).x(),
                              el_p3.y()-ROTATE(ARC(t,arc_a,arc_b),el_ang).y()));
        circlePath.lineTo(el_p3.x()+ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).x(),
                          el_p3.y()-ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).y());
        for (t=t_end; t>t_start; t-=0.00277777777778) 
            circlePath.lineTo(QPointF(el_p3.x()+ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).x(),
                              el_p3.y()-ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).y()));
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    if(el_type==3)
    {
        
        el_border_width=el_border_width/2;
        CtrlMotionPos_1_temp=UNROTATE(el_p3,el_ang,el_p1.x(),el_p1.y());
        CtrlMotionPos_2_temp=UNROTATE(el_p4,el_ang,el_p1.x(),el_p1.y());
        EndMotionPos_temp=QPointF(Length(el_p2,el_p1)+el_border_width,0);
        circlePath.moveTo(el_p1.x()+ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(-el_border_width,-(el_width/2+el_border_width)),el_ang).y());
        circlePath.cubicTo(QPointF(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-(el_width/2+el_border_width)),el_ang).x(),
                           el_p1.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-(el_width/2+el_border_width)),el_ang).y()),
                                   QPointF(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-(el_width/2+el_border_width)),el_ang).x(),
                                           el_p1.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-(el_width/2+el_border_width)),el_ang).y()),
                                           QPointF(el_p1.x()+ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-(el_width/2+el_border_width)),el_ang).x(),
                                                   el_p1.y()-ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-(el_width/2+el_border_width)),el_ang).y()));
        circlePath.lineTo(el_p1.x()+ROTATE(QPointF(EndMotionPos_temp.x(),el_width/2+el_border_width),el_ang).x(),
                          el_p1.y()-ROTATE(QPointF(EndMotionPos_temp.x(),el_width/2+el_border_width),el_ang).y());
        circlePath.cubicTo(QPointF(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+el_width/2+el_border_width),el_ang).x(),
                           el_p1.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+el_width/2+el_border_width),el_ang).y()),
                                   QPointF(el_p1.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+el_width/2+el_border_width),el_ang).x(),
                                           el_p1.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+el_width/2+el_border_width),el_ang).y()),
                                           QPointF(el_p1.x()+ROTATE(QPointF(-el_border_width,el_width/2+el_border_width),el_ang).x(),
                                                   el_p1.y()-ROTATE(QPointF(-el_border_width,el_width/2+el_border_width),el_ang).y()));
        circlePath.closeSubpath();
        circlePath.setFillRule ( Qt::WindingFill );
    }
    return circlePath;
}
 
 QPainterPath ShapeElFigure::painter_path_simple(int el_type, double el_ang, QPointF el_p1, QPointF el_p2, QPointF el_p3, QPointF el_p4, QPointF el_p5, QPointF el_p6)
{
    QPainterPath circlePath;//Путь для создания экземнпляра класса ShapeItem
    double t;
    circlePath=newPath;
    double arc_a, arc_b;
    if(el_type==1)
    {
        circlePath.moveTo(el_p1);
        circlePath.lineTo(el_p2);
    }
    if(el_type==2)
    {
        arc_a=Length(el_p5,el_p3);
        arc_b=Length(el_p3,el_p4);
        t_start=el_p6.x();
        t_end=el_p6.y();
        circlePath.moveTo(el_p3.x()+ROTATE(ARC(t_start,arc_a,arc_b),el_ang).x(),
                          el_p3.y()-ROTATE(ARC(t_start,arc_a,arc_b),el_ang).y());
        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
            circlePath.lineTo(QPointF(el_p3.x()+ROTATE(ARC(t,arc_a,arc_b),el_ang).x(),
                              el_p3.y()-ROTATE(ARC(t,arc_a,arc_b),el_ang).y()));
    }
    if(el_type==3)
    {
        circlePath.moveTo(el_p1);
        circlePath.cubicTo(el_p3, el_p4, el_p2);
    }
    return circlePath;
}

int ShapeElFigure::Append_Point( QPointF &pos, QList<ShapeItem> &shapeItems,PntMap *pnts )
{
    int i = 0;
    while( (*pnts).contains(i) ) i++;
    (*pnts).insert(i,pos.toPoint());
    return i;
}

void ShapeElFigure::Drop_Point (int num, int num_shape, QList<ShapeItem> &shapeItems, PntMap *pnts)
{
    bool equal=false;
    for(int i=0; i<shapeItems.size(); i++)
        if(i!=num_shape && (num==shapeItems[i].n1 || num==shapeItems[i].n2) )
        {
            equal=true;
            break;
        }
    if(!equal) (*pnts).remove(num);
}

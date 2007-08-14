
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

using namespace VISION;

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
QPainterPath ShapeItem::path() const
{
    return myPath;
}

QPointF ShapeItem::startposition() const
{
    return myStartPosition;
}

QPointF ShapeItem::endposition() const
{
    return myEndPosition;
}

QPointF ShapeItem::ctrlposition_1() const
{
    return myCtrlPosition_1;
}

QPointF ShapeItem::ctrlposition_2() const
{
    return myCtrlPosition_2;
}

QPointF ShapeItem::ctrlposition_3() const
{
    return myCtrlPosition_3;
}

QPointF ShapeItem::ctrlposition_4() const
{
    return myCtrlPosition_4;
}


QBrush ShapeItem::brush() const
{
    return myBrush;
}

QString ShapeItem::toolTip() const
{
    return myToolTip;
}

QPen ShapeItem::pen() const
{
    return myPen;
}

int ShapeItem::width() const
{
    return myWidth;
}

int ShapeItem::type() const
{
    return myType;
}


void ShapeItem::setPath(const QPainterPath &path)
{
    myPath = path;
}

void ShapeItem::setToolTip(const QString &toolTip)
{
    myToolTip = toolTip;
}

void ShapeItem::setStartPosition(const QPointF &startposition)
{
    myStartPosition = startposition;
}

void ShapeItem::setEndPosition(const QPointF &endposition)
{
    myEndPosition = endposition;
}

void ShapeItem::setCtrlPosition_1(const QPointF &ctrlposition_1)
{
    myCtrlPosition_1 = ctrlposition_1;
}

void ShapeItem::setCtrlPosition_2(const QPointF &ctrlposition_2)
{
    myCtrlPosition_2 = ctrlposition_2;
}

void ShapeItem::setCtrlPosition_3(const QPointF &ctrlposition_3)
{
    myCtrlPosition_3 = ctrlposition_3;
}

void ShapeItem::setCtrlPosition_4(const QPointF &ctrlposition_4)
{
    myCtrlPosition_4 = ctrlposition_4;
}


void ShapeItem::setBrush(const QBrush &brush)
{
    myBrush = brush;
}

void ShapeItem::setPen(const QPen &pen)
{
    myPen = pen;
}

void ShapeItem::setWidth(int width)
{
    myWidth = width;
}

void ShapeItem::setType(int type)
{
    myType = type;
}

ShapeItem &ShapeItem::operator=(const ShapeItem &item)
{
    setToolTip(item.toolTip());
    setStartPosition(item.startposition());
    setEndPosition(item.endposition());
    setCtrlPosition_1(item.ctrlposition_1());
    setCtrlPosition_2(item.ctrlposition_2());
    setCtrlPosition_3(item.ctrlposition_3());
    setCtrlPosition_4(item.ctrlposition_4());
    setBrush(item.brush());
    setPen(item.pen());
    setWidth(item.width());
    setType(item.type());
    setPath(item.path());
    return *this;
}
/////////////////////////////////////////////////////////////////////////////
//RectItem.cpp
/////////////////////////////////////////////////////////////////////////////
QPainterPath RectItem::path() const
{
    return myPath;
}

QPointF RectItem::startposition() const
{
    return myStartPosition;
}


QBrush RectItem::brush() const
{
    return myBrush;
}


QPen RectItem::pen() const
{
    return myPen;
}


void RectItem::setPath(const QPainterPath &path)
{
    myPath = path;
}


void RectItem::setStartPosition(const QPointF &startposition)
{
    myStartPosition = startposition;
}


void RectItem::setBrush(const QBrush &brush)
{
    myBrush = brush;
}

void RectItem::setPen(const QPen &pen)
{
    myPen = pen;
}


ShapeElFigure::ShapeElFigure() : WdgShape("ElFigure") 
{
    itemInMotion = 0;//Обнуление активной фигуры
    //Обнуление флагов для работы с клавишами
    ////////////////////////////////////////
    flag_down=false;
    flag_up=false;
    flag_left=false;
    flag_right=false;
    flag_ctrl=false;
    flag_rect=false;
    flag_hold_move=false;
    flag_m=false;
    flag_hold_arc=false;
    flag_arc_rect_3_4=false;
    flag_first_move=false;
    Flag=false;
    current_ss=-1;
    current_se=-1;
    current_es=-1;
    current_ee=-1;
    count_Shapes=0;
    count_holds=0;
    count_rects=0;
    rect_num_arc=-1;
    ////////////////////////////////////////
    newPath.addEllipse(QRect(0, 0, 0, 0));//Задание нулевого пути
}

void ShapeElFigure::init( WdgView *w )
{
    QList<ShapeItem> *shapeItems = new QList<ShapeItem>();
    w->dc()["shapeItems"].setValue((void*)shapeItems);
}

void ShapeElFigure::destroy( WdgView *w )
{
    QList<ShapeItem> *shapeItems = (QList<ShapeItem> *)w->dc().value("shapeItems",(void*)0).value< void* >();
    if( shapeItems ) delete shapeItems;
    rectItems.clear();
}

bool ShapeElFigure::attrSet( WdgView *view, int uiPrmPos, const string &val )
{
    return false;
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
    }
}

bool ShapeElFigure::event( WdgView *view, QEvent *event )
{
    QList<ShapeItem> &shapeItems = *(QList<ShapeItem> *)view->dc().value("shapeItems",(void*)0).value< void* >();
   
    bool flag_hold_rect;
    flag_hold_rect=false;//Дополнительная переменная, служащая флагом для рисования кружочка при привязках
    switch(event->type())
    { 
       
        case QEvent::Paint:
        {
    	    QPainter pnt( view );
            //Рисование всех экземпляров класса ShapeItem
            /////////////////////////////////////
            for (int k=0; k<=shapeItems.size() - 1; k++)
            {
                pnt.setBrush(shapeItems[k].brush());
                pnt.setPen(shapeItems[k].pen());
                pnt.drawPath(shapeItems[k].path());
            }
            /////////////////////////////////////
            //Рисование всех экземпляров класса RectItem
           /////////////////////////////////////
            for (int k=0; k<=rectItems.size() - 1; k++)
            {
                pnt.setBrush(rectItems[k].brush());
                pnt.setPen(rectItems[k].pen());
                pnt.drawPath(rectItems[k].path());
            }
            pnt.setPen(QColor(0,0,0,255));
            pnt.drawPath(ellipse_draw_startPath);
            pnt.drawPath(ellipse_draw_endPath);
           /////////////////////////////////////
            event->accept();
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
                              moveItemTo(previousPosition, shapeItems);
                          }
                      }
                      if (status_hold)
                      {
                          count_holds=0;//Обнуление числа связанных фигур
                          Holds(shapeItems);
                           //Если есть связанные фигуры
                          ////////////////////////////
                          if (count_holds)
                          {
                              if (rect_num!= -1)
                              {
                                  int rect_num_temp=rect_num;//Запоминание истинного номера квадратика в контейнере rectitems
                                  rect_num=Real_rect_num(rect_num,shapeItems);
                                   //Проверка на то, что квадратик не startposition и не endposition
                                  ////////////////////////////////////////////////
                                  if ((rect_num==2 || rect_num==3) && shapeItems[index].type()==3)
                                      flag_rect=false;
                                  ////////////////////////////////////////////////
                                   //Проверка на то,  что квадратик или startposition или endposition
                                  /////////////////////////////////////////////
                                  if ( rect_num==0 || rect_num==1)
                                      Rect_num_0_1(shapeItems,rect_num_temp);
                                  /////////////////////////////////////////////
                                   //Если клацнули по третьему или четвертому квадратику дуги
                                  //////////////////////////////////////////////////////////
                                  if ((rect_num==3 ||rect_num==4) && shapeItems[index].type()==2)
                                      Rect_num_3_4(shapeItems);
                                 ////////////////////////////////////////////////////////////
                              }
                              ////////////////////////////
                              if ( rect_num==-1 )//Проврка на включение привязок
                              {
                                  offset_all=QPointF(0,0);//Определение нулевого смещения для связанных фигур
                                  count_moveItemTo=0;//Обнулени количества вызова процедуры moveItemTo для связанных фигур
                                  for (int i=0; i<=count_holds; i++)//Вызов moveItemTo для всех связанных фигур
                                  {
                                      count_moveItemTo+=1;
                                      flag_ctrl_move=0;//Переменная показывает, что мы перемещаем фигуры, выделенные по привязкам(по CTRL)
                                      flag_ctrl=1;//Возведение флага CTRL в 1(для привязок)
                                      itemInMotion=&shapeItems[index_array[i]];
                                      index=index_array[i];
                                      moveItemTo(itemInMotion->startposition(),shapeItems);
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
                  moveItemTo(ev->pos(),shapeItems);
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
                if (ev->button() == Qt::LeftButton )
                { 
                    removeShapeItem(index,shapeItems);
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
                    itemInMotion=&shapeItems[index_temp];
                    if(current_ss!=-1)
                    {

                        if (itemInMotion->type()==2)//Если стыкует дугу, то к ней привязываем линию или Безье 
                        shapeItems[current_ss].setStartPosition(itemInMotion->startposition());
                        else
                        itemInMotion->setStartPosition(shapeItems[current_ss].startposition());
                    }
                    if(current_se!=-1)
                    {
                        if (itemInMotion->type()==2)//Если стыкует дугу, то к ней привязываем линию или Безье 
                        shapeItems[current_se].setStartPosition(itemInMotion->endposition());
                        else
                        itemInMotion->setEndPosition(shapeItems[current_se].startposition());
                    }
                    if(current_es!=-1)
                    {
                        if (itemInMotion->type()==2)//Если стыкует дугу, то к ней привязываем линию или Безье  
                        shapeItems[current_es].setEndPosition(itemInMotion->startposition());
                        else
                        itemInMotion->setStartPosition(shapeItems[current_es].endposition());
                    }
                    if(current_ee!=-1)
                    {
                        if (itemInMotion->type()==2)//Если стыкует дугу, то к ней привязываем линию или Безье  
                        shapeItems[current_ee].setEndPosition(itemInMotion->endposition());
                        else
                        itemInMotion->setEndPosition(shapeItems[current_ee].endposition());
                    }
                    //////////////////////////////////////////////
                    if (!flag_ctrl && status_hold && rect_num==-1)//Проверка на то, что перемещаем одну фигуру с включёнными привязками
                        moveItemTo(ev->pos(),shapeItems);
                    ellipse_draw_startPath=newPath;//Обнуление путей кружочков привязок
                    ellipse_draw_endPath=newPath;//Обнуление путей кружочков привязок
                    view->repaint();
                    //Если двигали какой-то из квадратиков дуги, связанной с другими фигурами, то при Release вызываем для всех связанных 
                    //фигур moveItemTo, чтобы не потерять связь
                   if (count_holds && (flag_arc_rect_3_4 || (flag_rect && shapeItems[index_array[0]].type()==2)))
                    {
                        count_moveItemTo=0;
                        flag_ctrl_move=false;
                        flag_ctrl=true;
                        flag_hold_arc=true;
                        offset_all=QPointF(0,0);
                        Move_all(ev->pos(),shapeItems);
                        view->repaint();
                    }
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
                   int Wdth=20;//Ширина фигуры
                    QLineF line1,line2;
                    EndLine=ev->pos();
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
                            circlePath.moveTo(StartLine.x()+ROTATE(QPointF(0,-Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(0,-Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(Length(EndLine,StartLine),-Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(Length(EndLine,StartLine),-Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(Length(EndLine,StartLine),Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(Length(EndLine,StartLine),Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(0,Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(0,Wdth/2),ang).y());
                            circlePath.closeSubpath();
                            createShapeItem(circlePath, _("Circle <%1>"),
                                            StartLine,EndLine,StartLine,EndLine,StartLine,EndLine,QBrush(QColor(0,0,255,255),Qt::SolidPattern),
                                            QPen(QColor(255, 0, 0), 0, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin),Wdth,1,shapeItems);
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
                            circlePath.moveTo(StartLine.x()+ROTATE(QPointF(0,-Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(0,-Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(Length(EndLine,StartLine),-Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(Length(EndLine,StartLine),-Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(Length(EndLine,StartLine),Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(Length(EndLine,StartLine),Wdth/2),ang).y());
                            circlePath.lineTo(StartLine.x()+ROTATE(QPointF(0,Wdth/2),ang).x(),
                                              StartLine.y()-ROTATE(QPointF(0,Wdth/2),ang).y());
                            circlePath.closeSubpath();
                            createShapeItem(circlePath , _("Circle <%1>"),
                                            StartLine,EndLine,StartLine,EndLine,StartLine,EndLine,QBrush(QColor(0,0,255,255),Qt::SolidPattern),
                                            QPen(QColor(255, 0, 0), 0, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin),Wdth,1,shapeItems);
                            view->repaint();
                        }
                        circlePath=newPath;
                    }
                     /////////////////////////////////////
                     //Если фигура кривая Безье
                     /////////////////////////////////////
                    if (shapeType==3)
                    {
                        QPointF CtrlPos,CtrlPos1,EndLine_temp;
                        CtrlPos=QPointF(Length(EndLine,StartLine)/3,0);
                        CtrlPos1=QPointF(2*Length(EndLine,StartLine)/3,0);
                        line2=QLineF(StartLine,QPointF(StartLine.x()+10,StartLine.y()));
                        line1=QLineF(StartLine,EndLine);
                        if (StartLine.y()<=EndLine.y())
                            ang=360-line1.angle(line2);
                        else
                            ang=line1.angle(line2);
                        EndLine_temp=QPointF(Length(EndLine,StartLine),0);
                        circlePath.moveTo(StartLine.x()+ROTATE(QPointF(0,-Wdth/2),ang).x(),
                                          StartLine.y()-ROTATE(QPointF(0,-Wdth/2),ang).y());
                        circlePath.cubicTo(QPointF(StartLine.x()+ROTATE(QPointF(CtrlPos.x(),CtrlPos.y()-Wdth/2),ang).x(),
                            StartLine.y()-ROTATE(QPointF(CtrlPos.x(),CtrlPos.y()-Wdth/2),ang).y()),
                                           QPointF(StartLine.x()+ROTATE(QPointF(CtrlPos1.x(),CtrlPos1.y()-Wdth/2),ang).x(),
                                               StartLine.y()-ROTATE(QPointF(CtrlPos1.x(),CtrlPos1.y()-Wdth/2),ang).y()),
                                           QPointF(StartLine.x()+ROTATE(QPointF(EndLine_temp.x(),EndLine_temp.y()-Wdth/2),ang).x(),
                                               StartLine.y()-ROTATE(QPointF(EndLine_temp.x(),EndLine_temp.y()-Wdth/2),ang).y()));
                        circlePath.lineTo(StartLine.x()+ROTATE(QPointF(EndLine_temp.x(),Wdth/2),ang).x(),
                                          StartLine.y()-ROTATE(QPointF(EndLine_temp.x(),Wdth/2),ang).y());
                        circlePath.cubicTo(QPointF(StartLine.x()+ROTATE(QPointF(CtrlPos1.x(),CtrlPos1.y()+Wdth/2),ang).x(),
                            StartLine.y()-ROTATE(QPointF(CtrlPos1.x(),CtrlPos1.y()+Wdth/2),ang).y()),
                                           QPointF(StartLine.x()+ROTATE(QPointF(CtrlPos.x(),CtrlPos.y()+Wdth/2),ang).x(),
                                               StartLine.y()-ROTATE(QPointF(CtrlPos.x(),CtrlPos.y()+Wdth/2),ang).y()),
                                           QPointF(StartLine.x()+ROTATE(QPointF(0,Wdth/2),ang).x(),
                                               StartLine.y()-ROTATE(QPointF(0,Wdth/2),ang).y()));
                        circlePath.closeSubpath();
                        CtrlPos=QPointF(StartLine.x()+ROTATE(CtrlPos,ang).x(),StartLine.y()-ROTATE(CtrlPos,ang).y());
                        CtrlPos1=QPointF(StartLine.x()+ROTATE(CtrlPos1,ang).x(),StartLine.y()-ROTATE(CtrlPos1,ang).y());
                        createShapeItem(circlePath , tr("Circle <%1>"),
                                        StartLine,EndLine, CtrlPos, CtrlPos1,StartLine,StartLine,QBrush(QColor(0,0,255,255),Qt::SolidPattern),
                                        QPen(QColor(255, 0, 0), 1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin),Wdth,3,shapeItems);
                        circlePath=newPath;
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
                        a=Length(EndLine,StartLine)/2+Wdth/2;
                        b=a+50;
                        a_small=a-Wdth;
                        b_small=a_small+50;
                        line2=QLineF( CtrlPos_1,QPointF(CtrlPos_1.x()+10,CtrlPos_1.y()));
                        line1=QLineF( CtrlPos_1,StartLine);
                        if (StartLine.y()<=EndLine.y()) ang=line1.angle(line2);
                        else ang=360-line1.angle(line2);
                        circlePath.moveTo(CtrlPos_1.x()+ROTATE(ARC(0,a_small,b_small),ang).x(),
                                          CtrlPos_1.y()-ROTATE(ARC(0,a_small,b_small),ang).y());
                        circlePath.lineTo(CtrlPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                          CtrlPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
                        for (t=0; t<0.50277777777778; t+=0.00277777777778) 
                            circlePath.lineTo(QPointF(CtrlPos_1.x()+ROTATE(ARC(t,a,b),ang).x(),
                                CtrlPos_1.y()-ROTATE(ARC(t,a,b),ang).y()));
                        circlePath.lineTo(CtrlPos_1.x()+ROTATE(ARC(0.5,a_small,b_small),ang).x(),
                                          CtrlPos_1.y()-ROTATE(ARC(0.5,a_small,b_small),ang).y());
                        for (t=0.5; t>=0; t-=0.00277777777778) 
                            circlePath.lineTo(QPointF(CtrlPos_1.x()+ROTATE(ARC(t,a_small,b_small),ang).x(),
                                CtrlPos_1.y()-ROTATE(ARC(t,a_small,b_small),ang).y()));
                        circlePath.closeSubpath();
                        CtrlPos_2=QPointF(CtrlPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                          CtrlPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());
                        CtrlPos_4=QPointF(CtrlPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                          CtrlPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
                        CtrlPos_3=QPointF(0,0.5);
                        createShapeItem(circlePath , tr("Circle <%1>"),
                                        StartLine,EndLine,CtrlPos_1,CtrlPos_2,CtrlPos_3,CtrlPos_4,QBrush(QColor(0,0,255,255),Qt::SolidPattern),
                                        QPen(QColor(255, 0, 0), 1, Qt::SolidLine,Qt::SquareCap,Qt::RoundJoin),Wdth,2,shapeItems);
                        circlePath=newPath;
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
                    if (shapeItems[index_array[0]].type()==2)//Если среди связаных фигур есть дуга
                        flag_hold_arc=true;
                  //Перемещение всех связанных(выделенных с Ctrl) фигур
                  ////////////////////
                    Move_all(ev->pos(),shapeItems);
                    view->repaint();
                    //////////////////////
                    ///////////////////////////////////////////
                }
                ///////////////////////////////////////////////////
                else
                {
                    //Если есть связанные фигуры(перемещение квадратика, не являющегося общим для нескольких связанных фигур)
                    ////////////////////////////////////
                    if (count_holds)
                    {
                       offset_all=ev->pos()-previousPosition_all;
                        //Для того, чтобы фигура становилась на своё место в контейнере shapeItems
                        ////////////////////////////
                        flag_ctrl=true;
                        count_moveItemTo=1;
                        flag_ctrl_move=false;
                        count_Shapes=1;
                        ////////////////////////////
                        itemInMotion=&shapeItems[index];
                        if (itemInMotion->type()==2) rect_num=rect_num_arc; 
                        moveItemTo(ev->pos(),shapeItems);//Перемещение одной, ни с чем не связанной фигуры
                        flag_hold_rect=true;
                    }
                    ////////////////////////////////////
                    else moveItemTo(ev->pos(),shapeItems);
                    if (status_hold && !flag_rect && !flag_hold_rect)//Если включены привязки и перемещается квадратик у несвязанной вигуры либо сама фигура
                    {
                        current_se=-1;
                        current_ss=-1;
                        current_ss=-1;
                        current_es=-1;
                        ellipse_draw_startPath=newPath;
                        ellipse_draw_endPath=newPath;
                        for (int i=0;i<=shapeItems.size()-1;i++)
                        {
                            ellipse_startPath=newPath;
                            ellipse_endPath=newPath;
                            if (i!=index)
                            { 
                                ellipse_startPath.addEllipse(shapeItems[i].startposition().x()-8,shapeItems[i].startposition().y()-8,16,16);
                                if (ellipse_startPath.contains(itemInMotion->startposition()))
                                { 
                                 ellipse_draw_startPath.addEllipse(shapeItems[i].startposition().x()-8,shapeItems[i].startposition().y()-8,16,16);
                                    current_ss=i;
                                }
                                if (ellipse_startPath.contains(itemInMotion->endposition()))
                                {  rectItems.clear();
                                    ellipse_draw_startPath.addEllipse(shapeItems[i].startposition().x()-8,shapeItems[i].startposition().y()-8,16,16);
                                    current_se=i;
                                }
                                ellipse_endPath.addEllipse(shapeItems[i].endposition().x()-8,shapeItems[i].endposition().y()-8,16,16);
                                if (ellipse_endPath.contains(itemInMotion->endposition()))
                               {
                                   ellipse_draw_endPath.addEllipse(shapeItems[i].endposition().x()-8,shapeItems[i].endposition().y()-8,16,16);
                                    current_ee=i;
                                }
                                if (ellipse_endPath.contains(itemInMotion->startposition()))
                                {
                                    ellipse_draw_endPath.addEllipse(shapeItems[i].endposition().x()-8,shapeItems[i].endposition().y()-8,16,16);
                                    current_es=i;
                                }
                            }
                            view->repaint();
                        }
                    }
                    view->repaint();
                }
            }
             //Определяем вид курсора
            /////////////////////////////////////
            if (flag_cursor==0)
            {
               if (flag_first_move)//Если пред этим двигали фигуру с помощью управляющих клавиш обнуляем
                {
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
                Holds(shapeItems);
            if (ev->key() == Qt::Key_Control)
            {
                if (status_hold) break;
                flag_ctrl=true;
                index_array=new int[shapeItems.size()];
            }
            if ((ev->key() == Qt::Key_Up) && index_temp!=-1)
            {
                if (flag_down || flag_left || flag_right || index<0 || index>shapeItems.size()-1) break;
                flag_up=true;
                offset_all=QPointF(0,-1);
                Move_UP_DOWN(shapeItems);
                view->repaint();
            }
            if ((ev->key() == Qt::Key_Down) && index_temp!=-1)
            {
                if (flag_up || flag_left || flag_right || index<0|| index>shapeItems.size()-1) break;
                flag_down=true;
                offset_all=QPointF(0,1);
                Move_UP_DOWN(shapeItems);
                view->repaint();
            }

            if ((ev->key() == Qt::Key_Left) && index_temp!=-1)
            {
                if (flag_down || flag_up || flag_right || index<0 || index>shapeItems.size()-1) break;
                flag_left=true;
                offset_all=QPointF(-1,0);
                Move_UP_DOWN(shapeItems);
                view->repaint();
            }
            if ((ev->key() == Qt::Key_Right) && index_temp!=-1)
            {
                if (flag_down || flag_left || flag_up || index<0 || index>shapeItems.size()-1) break;
                flag_right=true;
                offset_all=QPointF(1,0);
                Move_UP_DOWN(shapeItems);
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
void ShapeElFigure::moveItemTo(const QPointF &pos,QList<ShapeItem> &shapeItems)
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
    //QPointF Middle;//Середина прямой, соединяющей StartMotionPos и EndMotionPos выделенной дуги
    //QPointF Top1,Top2,Top3,Top4;//Четыре вершины для рисования квадратика
    QPointF offset = pos - previousPosition;//Определение смещения перемещаемой точки
    shapeType= itemInMotion->type();//Определение типа активной фигуры
    QBrush MotionBrush=itemInMotion->brush();//Определение заливки активной фигуры
    QPen MotionPen=itemInMotion->pen();//Определение пера активной фигуры
    int MotionWidth=itemInMotion->width();//Определение толщины активной фигуры
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
   if (rect_num==-1)//Указывает на то, что перемещать всю фигуру целиком

    {
        StartMotionPos=itemInMotion->startposition() + offset;
        EndMotionPos=itemInMotion->endposition()+offset;
        CtrlMotionPos_1=itemInMotion->ctrlposition_1() + offset;
        CtrlMotionPos_2=itemInMotion->ctrlposition_2() + offset;
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            CtrlMotionPos_3=itemInMotion->ctrlposition_3();
            CtrlMotionPos_4=itemInMotion->ctrlposition_4()+offset;
            a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_1,CtrlMotionPos_2);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_4);
            if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            t_start=CtrlMotionPos_3.x();
            t_end=CtrlMotionPos_3.y();
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y()); 
         }
    }
    if (rect_num==0)//Указывает на то, что перемещать StartLine, выбранной фигуры
    {
        EndMotionPos=itemInMotion->endposition();

        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            StartMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
                StartMotionPos=itemInMotion->startposition()+offset;
            CtrlMotionPos_1=itemInMotion->ctrlposition_1();
            CtrlMotionPos_2=itemInMotion->ctrlposition_2();
            CtrlMotionPos_3=itemInMotion->ctrlposition_3();
            CtrlMotionPos_4=itemInMotion->ctrlposition_4();
            a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            t_end=CtrlMotionPos_3.y();
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+100,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_4);
            if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            CtrlMotionPos_4=UNROTATE(CtrlMotionPos_4,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            StartMotionPos=UNROTATE(StartMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            line1=QLineF(QPointF(0,0),CtrlMotionPos_4);
            line2=QLineF(QPointF(0,0),StartMotionPos);
            if (StartMotionPos.y()<0)
                ang_t=Angle(line1,line2);
            else ang_t=360-Angle(line1,line2);
            t_start=ang_t/360;
            if (t_start>t_end) t_end+=1;
            if ((t_end-1)>t_start) t_end-=1;
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y());
        }
        else
        {
            if (!flag_hold_arc && !flag_arc_rect_3_4)//Если фигура не связана с дугой
            {
            StartMotionPos=itemInMotion->startposition() +offset;
            CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
            CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
            }
            if (flag_hold_arc || flag_arc_rect_3_4)//Если фигура связана с дугой
            {
                if (arc_rect==0)
                {
                    StartMotionPos=shapeItems[index_array[0]].startposition();
                    CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
                    CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
                }
                if (arc_rect==1)
                {
                    StartMotionPos=shapeItems[index_array[0]].endposition();
                    CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
                    CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
                }
            }
        }
    }
    if (rect_num==1)//Указывает на то, что перемещать EndLine, выбранной фигуры
    {
        StartMotionPos=itemInMotion->startposition();
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            EndMotionPos=Mouse_pos;
            if (flag_up || flag_down || flag_right || flag_left)
                EndMotionPos=itemInMotion->endposition()+offset;
            CtrlMotionPos_1=itemInMotion->ctrlposition_1();
            CtrlMotionPos_2=itemInMotion->ctrlposition_2();
            CtrlMotionPos_3=itemInMotion->ctrlposition_3();
            CtrlMotionPos_4=itemInMotion->ctrlposition_4();
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_4);
            if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            CtrlMotionPos_4=UNROTATE(CtrlMotionPos_4,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            EndMotionPos=UNROTATE(EndMotionPos,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            line1=QLineF(QPointF(0,0),CtrlMotionPos_4);
            line2=QLineF(QPointF(0,0),EndMotionPos);
            if (EndMotionPos.y()<0)
                ang_t=Angle(line1,line2);
            else ang_t=360-Angle(line1,line2);
            t_end=ang_t/360;
            t_start=CtrlMotionPos_3.x();
            if (t_start>t_end) t_end+=1;
            if ((t_end-1)>t_start) t_end-=1;
            CtrlMotionPos_4=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                    CtrlMotionPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y());
        }
        else
        {
            if (!flag_hold_arc && !flag_arc_rect_3_4)//Если фигура не связана с дугой
            { 
            EndMotionPos=itemInMotion->endposition()+offset;
            CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
            CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
            }
             if (flag_hold_arc || flag_arc_rect_3_4)//Если фигура связана с дугой
            {
             
            if (arc_rect==0)
             {
              EndMotionPos=shapeItems[index_array[0]].startposition();
              CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
              CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
              }
            if (arc_rect==1)
             {
              EndMotionPos=shapeItems[index_array[0]].endposition();
              CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
              CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
              }
            } 

        }
    }
     if (rect_num==2)//Указывает на то, что перемещать третью контрольную точку, выбранной фигуры
    {
        StartMotionPos=itemInMotion->startposition();
        EndMotionPos=itemInMotion->endposition(); 
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            StartMotionPos=itemInMotion->startposition();
            EndMotionPos=itemInMotion->endposition();
            CtrlMotionPos_1=itemInMotion->ctrlposition_1();
            CtrlMotionPos_2=itemInMotion->ctrlposition_2();
            CtrlMotionPos_3=itemInMotion->ctrlposition_3();
            CtrlMotionPos_4=itemInMotion->ctrlposition_4();
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_4);
            if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
            else ang=360-Angle(line1,line2);
            t_start=CtrlMotionPos_3.x();
            t_end=CtrlMotionPos_3.y();
        }
        else
        {
            CtrlMotionPos_2=itemInMotion->ctrlposition_2();
            CtrlMotionPos_1=itemInMotion->ctrlposition_1()+offset;
        }
     }
    if (rect_num==3)//Указывает на то, что перемещать четвёртую контрольную точку, выбранной фигуры
    {
        StartMotionPos=itemInMotion->startposition();
        EndMotionPos=itemInMotion->endposition();
        if (shapeType==2)//Переопределяет данную операцию для дуги
        {
            CtrlMotionPos_1=itemInMotion->ctrlposition_1();
            CtrlMotionPos_2=itemInMotion->ctrlposition_2()+offset;
            CtrlMotionPos_3=itemInMotion->ctrlposition_3();
            CtrlMotionPos_4=itemInMotion->ctrlposition_4();
            EndMotionPos=itemInMotion->endposition();
            a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
            b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
            t_start=CtrlMotionPos_3.x();
            t_end=CtrlMotionPos_3.y();
            line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+10,CtrlMotionPos_1.y()));
            line1=QLineF( CtrlMotionPos_1,CtrlMotionPos_4);
            if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2); 
            else ang=360-Angle(line1,line2);
            CtrlMotionPos_4=UNROTATE(CtrlMotionPos_4,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            CtrlMotionPos_2=UNROTATE(CtrlMotionPos_2,ang,CtrlMotionPos_1.x(),CtrlMotionPos_1.y());
            line2=QLineF(QPointF(0,0),QPointF(-100,0));
            line1=QLineF(QPointF(0,0),CtrlMotionPos_2);
            ang_t=Angle(line1,line2)-90;
            ang=ang+ang_t;
            StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                   CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).y());
            EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                                 CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y());
        }
        else
        {
            CtrlMotionPos_1=itemInMotion->ctrlposition_1();
            CtrlMotionPos_2=itemInMotion->ctrlposition_2() + offset;
        }
    }
    if (rect_num==4)//Указывает на то, что перемещать пятую контрольную точку, выбранной фигуры
    {
        CtrlMotionPos_1=itemInMotion->ctrlposition_1();
        CtrlMotionPos_2=itemInMotion->ctrlposition_2();
        CtrlMotionPos_3=itemInMotion->ctrlposition_3();
        CtrlMotionPos_4=itemInMotion->ctrlposition_4()+offset;
        EndMotionPos=itemInMotion->endposition();
        a=Length(CtrlMotionPos_4,CtrlMotionPos_1);
        b=Length(CtrlMotionPos_2,CtrlMotionPos_1);
        t_start=CtrlMotionPos_3.x();
        t_end=CtrlMotionPos_3.y();
        line2=QLineF(CtrlMotionPos_1,QPointF(CtrlMotionPos_1.x()+100,CtrlMotionPos_1.y()));
        line1=QLineF(CtrlMotionPos_1,CtrlMotionPos_4);
        if (CtrlMotionPos_4.y()<=CtrlMotionPos_1.y()) ang=Angle(line1,line2);
        else ang=360-Angle(line1,line2);
        StartMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                               CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a-MotionWidth/2,b-MotionWidth/2),ang).y());
        EndMotionPos=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).x(),
                             CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a-MotionWidth/2,b-MotionWidth/2),ang).y());
    }
    removeShapeItem(index,shapeItems);//Удаляет перемещаемую фигуру
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
        circlePath.moveTo(StartMotionPos.x()+ROTATE(QPointF(0,MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(0,MotionWidth/2),ang).y());
        circlePath.lineTo(StartMotionPos.x()+ROTATE(QPointF(Length(EndMotionPos,StartMotionPos),MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(Length(EndMotionPos,StartMotionPos),MotionWidth/2),ang).y());
        circlePath.lineTo(StartMotionPos.x()+ROTATE(QPointF(Length(EndMotionPos,StartMotionPos),-MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(Length(EndMotionPos,StartMotionPos),-MotionWidth/2),ang).y());
        circlePath.lineTo(StartMotionPos.x()+ROTATE(QPointF(0,-MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(0,-MotionWidth/2),ang).y());
        circlePath.closeSubpath();
        createShapeItem(circlePath , _("Circle <%1>"),
                        StartMotionPos,EndMotionPos,StartMotionPos,EndMotionPos,StartMotionPos,StartMotionPos,MotionBrush,MotionPen,MotionWidth,1,shapeItems);
        circlePath=newPath;
/////////////////////////////////////////////////////
        //Определение центра квадратика_2
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath ,StartMotionPos, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;//Обнуление пути
        //Определение центра квадратика_1
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath ,EndMotionPos, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;//Обнуление пути
    }
    if (shapeType==2)//Если выбранная фигура дуга
    {
        double t;
        a_small=a-MotionWidth;
        b_small=b-MotionWidth;
        circlePath.moveTo(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a_small,b_small),ang).x(),
                          CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a_small,b_small),ang).y());
        circlePath.lineTo(CtrlMotionPos_1.x()+ROTATE(ARC(t_start,a,b),ang).x(),
                          CtrlMotionPos_1.y()-ROTATE(ARC(t_start,a,b),ang).y());//QPointF(50,50));
        for (t=t_start; t<t_end+0.00277777777778; t+=0.00277777777778) 
            circlePath.lineTo(QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t,a,b),ang).x(),
                                      CtrlMotionPos_1.y()-ROTATE(ARC(t,a,b),ang).y()));
        circlePath.lineTo(CtrlMotionPos_1.x()+ROTATE(ARC(t_end,a_small,b_small),ang).x(),
                          CtrlMotionPos_1.y()-ROTATE(ARC(t_end,a_small,b_small),ang).y());
        for (t=t_end; t>t_start; t-=0.00277777777778) 
            circlePath.lineTo(QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(t,a_small,b_small),ang).x(),
                                      CtrlMotionPos_1.y()-ROTATE(ARC(t,a_small,b_small),ang).y()));
        circlePath.closeSubpath();
        CtrlMotionPos_2=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0.25,a,b),ang).x(),
                                    CtrlMotionPos_1.y()-ROTATE(ARC(0.25,a,b),ang).y());

        CtrlMotionPos_3=QPointF(t_start,t_end);
        CtrlMotionPos_4=QPointF(CtrlMotionPos_1.x()+ROTATE(ARC(0,a,b),ang).x(),
                                CtrlMotionPos_1.y()-ROTATE(ARC(0,a,b),ang).y());
        createShapeItem(circlePath , _("Circle <%1>"),
                        StartMotionPos,EndMotionPos,CtrlMotionPos_1,CtrlMotionPos_2,CtrlMotionPos_3,CtrlMotionPos_4,MotionBrush,MotionPen,MotionWidth,2,shapeItems);
        circlePath=newPath;//Обнуление пути
        //Рисование пяти квадратиков в четырёх контрольных точках
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath,StartMotionPos,QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath ,EndMotionPos, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;
        rectPath.addRect(QRectF( CtrlMotionPos_1,QSize(8,8)));
        createRectItem(rectPath , CtrlMotionPos_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;
        rectPath.addRect(QRectF(QPointF(CtrlMotionPos_2.x()-4,CtrlMotionPos_2.y()-4),QSize(8,8)));
        createRectItem(rectPath,CtrlMotionPos_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern), 
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;
        if (MotionWidth<=18) Temp=QPointF(CtrlMotionPos_4.x()-14,CtrlMotionPos_4.y()-4);
        else Temp=QPointF(CtrlMotionPos_4.x()-4,CtrlMotionPos_4.y()-4);
        rectPath.addRect(QRectF(Temp,QSize(8,8)));
        createRectItem(rectPath ,CtrlMotionPos_4, QBrush(QColor(0,0,0,255),Qt::SolidPattern), 
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
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
        CtrlMotionPos_1_temp=UNROTATE(CtrlMotionPos_1,ang,StartMotionPos.x(),StartMotionPos.y());
        CtrlMotionPos_2_temp=UNROTATE(CtrlMotionPos_2,ang,StartMotionPos.x(),StartMotionPos.y());
        EndMotionPos_temp=QPointF(Length(EndMotionPos,StartMotionPos),0);
        circlePath.moveTo(StartMotionPos.x()+ROTATE(QPointF(0,-MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(0,-MotionWidth/2),ang).y());
        circlePath.cubicTo(QPointF(StartMotionPos.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()-MotionWidth/2),ang).y()),
                           QPointF(StartMotionPos.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()-MotionWidth/2),ang).y()),
                           QPointF(StartMotionPos.x()+ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(EndMotionPos_temp.x(),EndMotionPos_temp.y()-MotionWidth/2),ang).y()));
        circlePath.lineTo(StartMotionPos.x()+ROTATE(QPointF(EndMotionPos_temp.x(),MotionWidth/2),ang).x(),
                          StartMotionPos.y()-ROTATE(QPointF(EndMotionPos_temp.x(),MotionWidth/2),ang).y());
        circlePath.cubicTo(QPointF(StartMotionPos.x()+ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(CtrlMotionPos_2_temp.x(),CtrlMotionPos_2_temp.y()+MotionWidth/2),ang).y()),
                           QPointF(StartMotionPos.x()+ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(CtrlMotionPos_1_temp.x(),CtrlMotionPos_1_temp.y()+MotionWidth/2),ang).y()),
                           QPointF(StartMotionPos.x()+ROTATE(QPointF(0,MotionWidth/2),ang).x(),
                                   StartMotionPos.y()-ROTATE(QPointF(0,MotionWidth/2),ang).y()));
        circlePath.closeSubpath();
        CtrlMotionPos_1=QPointF(StartMotionPos.x()+ROTATE(CtrlMotionPos_1_temp,ang).x(),StartMotionPos.y()-ROTATE(CtrlMotionPos_1_temp,ang).y());
        CtrlMotionPos_2=QPointF(StartMotionPos.x()+ROTATE(CtrlMotionPos_2_temp,ang).x(),StartMotionPos.y()-ROTATE(CtrlMotionPos_2_temp,ang).y());
        createShapeItem(circlePath , tr("Circle <%1>"),
                        StartMotionPos,EndMotionPos,CtrlMotionPos_1, CtrlMotionPos_2,StartMotionPos,StartMotionPos,MotionBrush,
                        MotionPen,MotionWidth,3,shapeItems);
        rectPath.addRect(QRectF(QPointF(StartMotionPos.x()-4,StartMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath ,StartMotionPos, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;//Обнуление пути
        rectPath.addRect(QRectF(QPointF(EndMotionPos.x()-4,EndMotionPos.y()-4),QSize(8,8)));
        createRectItem(rectPath ,EndMotionPos, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        rectPath=newPath;//Обнуление пути
        //Построение квадратика_3
        ////////////////////////////////////////////////
        rectPath.addRect(QRectF( CtrlMotionPos_1,QSize(8,8)));
        createRectItem(rectPath ,CtrlMotionPos_1, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        ////////////////////////////////////////////////
        rectPath=newPath;//Обнуление пути
        //Построение квадратика_4
        ////////////////////////////////////////////////
        rectPath.addRect(QRectF( CtrlMotionPos_2,QSize(8,8)));
        createRectItem(rectPath,CtrlMotionPos_2, QBrush(QColor(127,127,127,128),Qt::SolidPattern),
                       QPen(QColor(0, 0, 0),1, Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin));
        ////////////////////////////////////////////////
        rectPath=newPath;//Обнуление пути
    }

    circlePath=newPath;//Обнуление пути
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
        previousPosition_all=pos;//Если нажат Сtrl, есть движение и цикл выполнился один раз для всех перемещаемых фигур
}

//Процедура удаления фигуры
void ShapeElFigure::removeShapeItem(int num, QList<ShapeItem> &shapeItems)
{ 
    shapeItems.removeAt(num);//Удаляет фигуру из контейнера по заданному индексу
}

//Процедура удаления квадратика
void ShapeElFigure::removeRectItem(int num)
{ 
    rectItems.removeAt(num);//Удаляет фигуру из контейнера по заданному индексу
}

bool ShapeElFigure::Holds(QList<ShapeItem> &shapeItems)
{
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
        {
            if (i!=index_hold)
            {
                                if ((((int(shapeItems[index_hold].startposition().x())==(int)shapeItems[i].startposition().x()) &&
                                     (int(shapeItems[index_hold].startposition().y())==int(shapeItems[i].startposition().y()))) ||
                                    ((int(shapeItems[index_hold].endposition().x())==int(shapeItems[i].startposition().x())) &&
                                     (int(shapeItems[index_hold].endposition().y())==int(shapeItems[i].startposition().y()))) ||
                                    ((int(shapeItems[index_hold].endposition().x())==int(shapeItems[i].endposition().x())) &&
                                     (int(shapeItems[index_hold].endposition().y())==int(shapeItems[i].endposition().y()))) ||
                                    ((int(shapeItems[index_hold].startposition().x())==int(shapeItems[i].endposition().x())) &&
                                     (int(shapeItems[index_hold].startposition().y())==int(shapeItems[i].endposition().y())))))
                                    if (ellipse_draw_startPath==newPath && ellipse_draw_endPath==newPath)
                                {
                                    flag_equal=0;
                                    for (int j=0; j<=count_holds; j++)
                                        if (index_array[j]==i) flag_equal=1;
                                    if (flag_equal==0)
                                    {
                                        count_holds++;
                                        index_array[count_holds]=i;
                                    }
                                }
                            }
                        }
                        num++;
                    }
                    while(num!=count_holds+1);
    if (count_holds>0) return true;
    else return false;
}

void ShapeElFigure::Move_UP_DOWN(QList<ShapeItem> &shapeItems)
{
    int rect_num_temp;
    if (!flag_first_move)
    {
        count_holds=0;
        flag_rect=false;
        Holds(shapeItems);//Вызов Holds для определения связана фигура с другими или нет
        count_Shapes=count_holds+1;//Числу перемещаемых фигур присваиваем число всязаных
        if ((flag_ctrl && count_Shapes) || count_holds)//Проверка на нажатие  Сtrl и на наличие выделенных фигур
            if (rect_num!=-1)//Если попали по квадратику
        {
            rect_num=Real_rect_num (rect_num,shapeItems);//Определяем номер квадратика, кот. нужен moveItemTo
            if ((rect_num==2 || rect_num==3) && shapeItems[index].type()==3)//Если попали по несвязанному квадратику Бизье
                flag_rect=false;
            if ( rect_num==0 || rect_num==1)//Если попали по 1,2 квадратику
                Rect_num_0_1(shapeItems,rect_num);
            if ((rect_num==3 ||rect_num==4) && shapeItems[index].type()==2)//Если попали по 3,4 квадратику дуги
                Rect_num_3_4(shapeItems);
        }
        if (flag_rect || flag_arc_rect_3_4) //Если клацнули по квадратику, то число перемещаемых фигур равно числу фигур связаных с этим квадратиком
            count_Shapes=count_rects;
    }
    count_moveItemTo=0;
    if (flag_rect || flag_arc_rect_3_4 || (rect_num==-1 && count_holds))//Вызов moveItemTo для всех выделенных фигур(если тяним за квадртик общий для нескольких фигур,
                                                                       //3,4 квадратик связанной с другими фигурами дуги, просто связанные фигуры)
        Move_all(QPointF(0,0),shapeItems);
    if ((!flag_ctrl  || (!flag_rect && rect_num!=-1)) && !flag_arc_rect_3_4)//Если двигаем несвязанную фигуру или ее квадратик
       if (index!=-1)
        {
            flag_ctrl=true;
            flag_ctrl_move=false;
            count_moveItemTo=1;
            count_Shapes=1;
            itemInMotion=&shapeItems[index];
            moveItemTo(itemInMotion->startposition(),shapeItems);
            flag_ctrl=false;
        }
        else
            itemInMotion=0;
}

int ShapeElFigure::Real_rect_num(int rect_num_old,QList<ShapeItem> &shapeItems)
{
    int rect_num_new;
    for (int i=0; i<=shapeItems.size()-1; i++)
        if((rectItems[rect_num_old].startposition()==shapeItems[i].startposition()) ||
            (rectItems[rect_num_old].startposition()==shapeItems[i].endposition()) ||
            (rectItems[rect_num_old].startposition()==shapeItems[i].ctrlposition_1()) ||
            (rectItems[rect_num_old].startposition()==shapeItems[i].ctrlposition_2()) ||
            (rectItems[rect_num_old].startposition()==shapeItems[i].ctrlposition_3()) ||
            (rectItems[rect_num_old].startposition()==shapeItems[i].ctrlposition_4()))
            index=i;

    //Определение номера квадратика, который нужно передать в moveItemTo
    ///////////////////////////////////////////
    if (shapeItems[index].type()==1)
    {
        if (rectItems[rect_num_old].startposition()==shapeItems[index].startposition()) rect_num_new=0;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].endposition()) rect_num_new=1;
    }
    if (shapeItems[index].type()==2)
    {
        if (rectItems[rect_num_old].startposition()==shapeItems[index].startposition()) rect_num_new=0;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].endposition()) rect_num_new=1;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].ctrlposition_1()) rect_num_new=2;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].ctrlposition_2()) rect_num_new=3;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].ctrlposition_4()) rect_num_new=4;
    }
    if (shapeItems[index].type()==3)
    {
        if (rectItems[rect_num_old].startposition()==shapeItems[index].startposition()) rect_num_new=0;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].endposition()) rect_num_new=1;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].ctrlposition_1()) rect_num_new=2;
        if (rectItems[rect_num_old].startposition()==shapeItems[index].ctrlposition_2()) rect_num_new=3;
    }
    return rect_num_new;
}

void ShapeElFigure::Rect_num_0_1(QList<ShapeItem> &shapeItems,int rect_num_temp)
{
    flag_rect=true;
    int* index_array_temp=new int[count_holds+1];//Выделение памяти под массив, в котором хранятся индексы фигур, у которых есть общие точки
    rect_array=new int[count_holds+1];//Выделение памяти под массив, в которм хранятся индексы(для moveItemTo) квадратиков для фигур, у которых есть общие точки
    count_rects=0;//Обнуление количества квадратиков в массиве|
    //Перебор всех связанных фигур, чтобы определить, у каких из них есть общие точки
    /////////////////////////////////////////////////////////////////////////////////
    for (int i=0; i<=count_holds; i++)
    {
        if (rectItems[rect_num_temp].startposition()==shapeItems[index_array[i]].startposition())
        {
            index_array_temp[count_rects]=index_array[i];//Заполняем массив связанными фигурами с общими точками
            rect_array[count_rects]=0;//Заполняем массив номерми квадратиков для moveItemTo
            count_rects++;
            flag_rect=true;//Указывает на то, что мы щёлкнули по квадратику, который является общим для нескольких фигур
        }
        if (rectItems[rect_num_temp].startposition()==shapeItems[index_array[i]].endposition() )
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
        if (shapeItems[index_array[i]].type()==2)
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
      if (shapeItems[index_array[0]].type()==2)
          rect_num_arc=rect_num;
    }
    /////////////////////////////////
    delete [] index_array_temp;

}
void ShapeElFigure::Rect_num_3_4(QList<ShapeItem> &shapeItems)
{
                                     
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
   for (int i=0; i<=count_holds;i++)
    {
        if (index_array[i]!=index)
        {
            if (shapeItems[index].startposition()==shapeItems[index_array[i]].startposition())
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=0;
                fig_rect_array[count_rects]=0;
                count_rects++;
            }
            if (shapeItems[index].startposition()==shapeItems[index_array[i]].endposition())
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=0;
                fig_rect_array[count_rects]=1;
                count_rects++;
            }
            if (shapeItems[index].endposition()==shapeItems[index_array[i]].startposition())
            {
                index_array_temp[count_rects]=index_array[i];
                arc_rect_array[count_rects]=1;
                fig_rect_array[count_rects]=0;
                count_rects++;
            }
            if (shapeItems[index].endposition()==shapeItems[index_array[i]].endposition())
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

void ShapeElFigure::Move_all(QPointF pos,QList<ShapeItem> &shapeItems)
{
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
            rect_num=fig_rect_array[i];//Номер перемещаемого квадратика
            arc_rect=arc_rect_array[i];//С каким квадратико дуги связан перемещаемый квадратик фигуры
        }
        index=index_array[i];
        moveItemTo(pos,shapeItems);
    }
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
        if (item1.path().contains(pos)) rect_num=j;
    }
    for (int i =shapeItems.size()-1; i >=0; i--) 
       {
           const ShapeItem &item = shapeItems[i];
           if (item.path().contains(pos)) return i;
           for(int j=3; j>0; j-- )
            {
                point.setY(j);
                point.setX(j);
                if (item.path().contains(pos+point)) return i;
            }
             for(int j=3; j>0; j-- )
            {
                point.setY(j);
                point.setX(j);
                if (item.path().contains(pos-point)) return i;
            }
         }
    if(rect_num==-1) return -1;
    else return shapeItems.size()-1;
 }

//Процедура создания фигуры
void  ShapeElFigure::createShapeItem(const QPainterPath &path,
                                const QString &toolTip, const QPointF &startpos,const QPointF &endpos,
                                const QPointF &ctrlpos_1, const QPointF &ctrlpos_2,const QPointF &ctrlpos_3,
                                const QPointF &ctrlpos_4, const QBrush &brush, const QPen &pen,
                                const int width, const int type, QList<ShapeItem> &shapeItems)
{
    ShapeItem shapeItem;
    shapeItem.setToolTip(toolTip);
    shapeItem.setStartPosition(startpos);
    shapeItem.setEndPosition(endpos);
    shapeItem.setCtrlPosition_1(ctrlpos_1);
    shapeItem.setCtrlPosition_2(ctrlpos_2);
    shapeItem.setCtrlPosition_3(ctrlpos_3);
    shapeItem.setCtrlPosition_4(ctrlpos_4);
    shapeItem.setBrush(brush);
    shapeItem.setPen(pen);
    shapeItem.setWidth(width);
    shapeItem.setType(type);
    shapeItem.setPath(path);
    shapeItems.append(shapeItem);

}
//Процедура создания квадратика
void  ShapeElFigure::createRectItem(const QPainterPath &path, const QPointF &startpos, const QBrush &brush, const QPen &pen)
{
    RectItem rectItem; 
    rectItem.setStartPosition(startpos);
    rectItem.setBrush(brush);
    rectItem.setPen(pen);
    rectItem.setPath(path);
    rectItems.append(rectItem);

}



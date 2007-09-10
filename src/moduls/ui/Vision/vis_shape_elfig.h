
//OpenSCADA system module UI.VISION file: vis_shape_elfig.h
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

#ifndef VIS_SHAPE_ELFIG_H
#define VIS_SHAPE_ELFIG_H

#include <math.h>

#include <QGraphicsScene>
#include <QLineF>
#include <QColor>
#include <QPainterPath>
#include <QPoint>
#include <QPen>
#include <QBrush>
#include <QPainter>

#include "vis_shapes.h"
#include "vis_widgs.h"

class QGraphicsScene;
class QLabel;
class QAction;
class QPainter;
class QPen;
class QBrush;
class QGradient;

namespace VISION
{

class ShapeItem 
{
    public:
        void setPath(const QPainterPath &path) {myPath = path;}
        void setToolTip(const QString &toolTip){ myToolTip = toolTip;}
        void setStartPosition(const QPointF &startposition){myStartPosition = startposition;}
        void setEndPosition(const QPointF &endposition){myEndPosition = endposition;}
        void setCtrlPosition_1(const QPointF &ctrlposition_1){myCtrlPosition_1 = ctrlposition_1;}
        void setCtrlPosition_2(const QPointF &ctrlposition_2){myCtrlPosition_2 = ctrlposition_2;}
        void setCtrlPosition_3(const QPointF &ctrlposition_3){myCtrlPosition_3 = ctrlposition_3;}
        void setCtrlPosition_4(const QPointF &ctrlposition_4){myCtrlPosition_4 = ctrlposition_4;}
        void setBrush(const QBrush &brush){ myBrush = brush;}
        void setPen(const QPen &pen){myPen = pen;}
        void setWidth(int width){myWidth = width;}
        void setType (int type){myType = type;}
	ShapeItem &operator=(const ShapeItem &other);
    
        QPainterPath path() const {return myPath;}
        QPointF startposition() const {return myStartPosition;}
        QPointF endposition() const {return myEndPosition;}
        QPointF ctrlposition_1() const {return myCtrlPosition_1;}
        QPointF ctrlposition_2() const {return myCtrlPosition_2;}
        QPointF ctrlposition_3() const {return myCtrlPosition_3;}
        QPointF ctrlposition_4() const {return myCtrlPosition_4;}
        QBrush brush() const{return myBrush;}
        QString toolTip() const{return myToolTip;}
        QPen pen() const{return myPen;}
        int width() const{return myWidth;}
        int type() const{return myType;}
    
    private:
	QPainterPath myPath;
	QPointF myStartPosition;
	QPointF myEndPosition;
	QPointF myCtrlPosition_1;
	QPointF myCtrlPosition_2;
	QPointF myCtrlPosition_3;
	QPointF myCtrlPosition_4;
	QBrush myBrush;
	QString myToolTip;
	QPen myPen;
	int myWidth;
	int myType;
       
    
};

class RectItem 
{
    public:
        void setPath(const QPainterPath &path){ myPath = path;}
        void setStartPosition(const QPointF &startposition){ myStartPosition = startposition;}
        void setBrush(const QBrush &brush){myBrush = brush;}
        void setPen(const QPen &pen){myPen = pen;}
    
        QPainterPath path() const{return myPath;}
        QPointF startposition() const{return myStartPosition;}
        QBrush brush() const{return myBrush;}
        QPen pen() const{return myPen;}
    
    private:
	QPainterPath myPath;
	QPointF myStartPosition;
	QBrush myBrush;
	QPen myPen;
};

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
class ShapeElFigure : public WdgShape 
{    
    Q_OBJECT    
    public:
	ShapeElFigure();
    
        bool isEditable( )	{ return true; }

	void init( WdgView *view );
        void destroy( WdgView *w );
    
        void editEnter( WdgView *view );
        void editExit( WdgView *view );
	
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
    
	bool event( WdgView *view, QEvent *event );
    
	//Процедура создания экземнпляра класса ShapeItem
	////////////////////////////////////////////////
        void createShapeItem(const QPainterPath &path, const QString &toolTip,
			    const QPointF &startpos,  const QPointF &endpos, const QPointF &ctrlpos_1, 
			    const QPointF &ctrlpos_2,const QPointF &ctrlpos_3, const QPointF &ctrlpos_4, const QBrush &brush, 
                             const QPen &pen, const int width, const int type, QList<ShapeItem> &shapeItems);
	////////////////////////////////////////////////
    
	//Процедура создания экземнпляра класса RectItem
	////////////////////////////////////////////////
	void createRectItem(const QPainterPath &path, const QPointF &startpos, const QBrush &brush, 
			    const QPen &pen);
	////////////////////////////////////////////////
    
        void removeShapeItem(int num, QList<ShapeItem> &shapeItems);//Процедура удаления экземнпляра класса ShapeItem из контейнера всех экземпляров
	void removeRectItem(int num);//Процедура удаления экземнпляра класса RectItem из контейнера всех экземпляров
    
	QList<RectItem> rectItems;////Контейнер для экземпляров класса RectItem
	QPainterPath circlePath;//Путь для создания экземнпляра класса ShapeItem
	QPainterPath newPath;//Нулевой путь
	QPainterPath rectPath;//Путь для создания экземнпляра класса RectItem
        QPainterPath ellipse_startPath,ellipse_endPath;
        
        QPainterPath ellipse_draw_startPath,ellipse_draw_endPath;
    
	QString userFriendlyCurrentFile();
	QString currentFile() { return curFile; }
    
	bool status; //Определяет нажата ли какая-либо кнопка рисования примитива 
	int shapeType; //Определяет тип выделенной фигуры
    
    
    private slots:
	void toolAct( QAction * );
    
    private:   
     	QPointF StartLine; //Начальная точка при создании фигуры(точка, из которой будет производится рисование фигуры)
	QPointF EndLine;//Конечная точка при создании фигуры(точка, в которую будет производится рисование фигуры)
    
	QPointF previousPosition,previousPosition_all;//Предыдущая позиция точки, за которую тянем, при перемещении фигуры
	ShapeItem *itemInMotion;//Выделенная(перемещаемая) фигура
       
        int itemAt(const QPointF &pos, QList<ShapeItem> &shapeItems);//Процедура определения фигуры, соответствующей положению курсора
        void moveItemTo(const QPointF &pos,QList<ShapeItem> &shapeItems);//Процедура перемещения фигуры
    
	/*bool maybeSave();*/
	QGraphicsScene *scene;
	void populateScene();
	void setCurrentFile(const QString &fileName);
	QString strippedName(const QString &fullFileName);
	QPointF ROTATE(const QPointF &pnt,double alpha);//Процедура поворота данной точки на данный угол относительно начала координат
	QPointF UNROTATE(const QPointF &pnt, double alpha,double a, double b);//Процедура, обратная к ROTATE
	QPointF ARC(double t,double a,double b);//Процедура вычисления координат точки дуги с центром в точке [0,0], соответствующей параметру  t
	double Angle(const QLineF &l,const QLineF &l1);//Процедура определения угла между двумя прямыми
	double Length(const QPointF pt1, const QPointF pt2);//Процедура определения расстояния между двумя точками
        bool Holds(QList<ShapeItem> &shapeItems);//Процедура определения привязанных к данной фигуре других фигур
        void Move_UP_DOWN (QList<ShapeItem> &shapeItems);//Процедура перемещения фигуры с помощью управляющих клавиш
        int Real_rect_num(int rect_num_old,QList<ShapeItem> &shapeItems);//Определение истиного номера квадратика
        void Rect_num_0_1(QList<ShapeItem> &shapeItems,int rect_num_temp);//Что делать если попали по 1,2 квадратику
        void Rect_num_3_4(QList<ShapeItem> &shapeItems);//Что делать если попали по 2,4 квадратику дуги
        void Move_all(QPointF pos,QList<ShapeItem> &shapeItems);//Одновременное передвижение нескольних фигур(квадратиков)
    
	QString curFile;
	bool isUntitled;
        bool status_hold;//Статус нажатия кнопки привязок
    
	int* index_array;//Массив, содержащий индексы фигур, которые выделяются с Сtrl
        int* rect_array;
        
	int count_Shapes,count_moveItemTo;//Количество фигур, выделенных с Сtrl и количество вызовов процедуры moveItemTo для этих фигур соответственно
	int index,index_temp;//Индекс фигуры в контейнере
	int rect_num;//Номер выделенного квадратика(0..3)
	bool flag_cursor,flag_key,flag_up,flag_down,flag_left,flag_right,flag_ctrl,flag,flag_ctrl_move,flag_m,flag_hold_arc;//Флаги для вида курсора и 
					     							    //для управляющих клавиш соответственно
        bool flag_rect, flag_arc_rect_3_4,flag_arc_release,flag_first_move,Flag;
        int count_rects,rect_num_arc,arc_rect;
        bool flag_holds;//Вспомогательная переменная для того, чтобы вызывать Holds() только один раз в начале движения
        bool flag_hold_move;//Флаг передвижения привязанных фигур
        double t_start,t_end;//Начальное и конечное значения параметра для рисования дуги
	QPointF Mouse_pos,offset_all;//Текущая позиция мышки
        int current_ss,current_se,current_ee,current_es;//Указывает, какую комбинацию из двух точек связывать 
        int count_holds;//Количество привязанных фигур
        int* arc_rect_array;//Содержит номера квадратиков дуги,связанные с другими фигурами
        int* fig_rect_array;//Содержит номера квадратиков фигур,связанных с дугой 
        int* index_array_all;
       };

}

#endif //VIS_SHAPE_ELFIG_H


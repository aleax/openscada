
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

typedef QMap<int,QPoint> PntMap;

class QGraphicsScene;
class QLabel;
class QAction;
class QPainter;
class QPen;
class QBrush;
class QGradient;

namespace VISION
{

//************************************************
//* ShapeItem                                    *
//************************************************
class ShapeItem 
{
    public:
	ShapeItem( )	{ }
	ShapeItem( const QPainterPath &ipath, const QPainterPath &path_simple, const int num_1,const int num_2, const int num_3, const int num_4,const int num_5,
		    const QPointF &ctrlpos_4, const QBrush &brush, const QPen &ipen, const QPen &pen_simple, const float iwidth, const int itype ) : 
    	    ctrlPos4(ctrlpos_4), n1(num_1), n2(num_2), n3(num_3), n4(num_4), n5(num_5), brush(brush),
	    pen(ipen), penSimple(pen_simple), width(iwidth), type(itype), path(ipath), pathSimple(path_simple)
	{ };								

	QPainterPath 	path, 
		        pathSimple;
	QPointF		ctrlPos4;
        int 		n1, n2, n3, n4, n5;
	QBrush 		brush;
	QPen 		pen, 
			penSimple;
	float 		width;
	int 		type;
};

//************************************************
//* RectItem                                     *
//************************************************
class RectItem 
{
    public:
	RectItem( )	{ }
        RectItem( const QPainterPath &ipath, const int inum, const QBrush &ibrush, const QPen &ipen ) :
	    num(inum), brush(ibrush), pen(ipen), path(ipath)	{  }

	QPainterPath 	path;
	int 		num;
	QBrush 		brush;
	QPen 		pen;
};

//*************************************************
//* Elementary figures shape widget               *
//*************************************************
class ShapeElFigure : public WdgShape 
{    
    Q_OBJECT    
    public:
	//Public methods
	ShapeElFigure( );
    
        bool isEditable( )		{ return true; }

	void init( WdgView *view );
        void destroy( WdgView *w );
    
        void editEnter( WdgView *view );
        void editExit( WdgView *view );
	
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
        bool shapeSave( WdgView *view );
     
	bool event( WdgView *view, QEvent *event );
    
 	QString userFriendlyCurrentFile( );
	QString currentFile( ) 		{ return curFile; }

	//Public attributes
	QList<RectItem> rectItems;		//RectItem's container
	QPainterPath 	newPath,		//Null path
			rectPath,		//Path fo RectItem creation
        		ellipse_startPath, ellipse_endPath,
        		ellipse_draw_startPath, ellipse_draw_endPath;
    
	bool status; 				//Check fo any primitive paint key pressed
	int shapeType; 				//Selected figure type
        int kr;    
    
    private slots:
	void toolAct( QAction * );
    
    private:
    	//Methods
        int itemAt( const QPointF &pos, QList<ShapeItem> &shapeItems );			//Check for figure type under cursor
        void moveItemTo(const QPointF &pos,QList<ShapeItem> &shapeItems, PntMap *pnts);	//Move figure procedure
    
	QGraphicsScene *scene;
	void populateScene();
	void setCurrentFile( const QString &fileName );
	QString strippedName( const QString &fullFileName );
	QPointF ROTATE( const QPointF &pnt, double alpha );				//Процедура поворота данной точки на данный угол относительно начала координат
	QPointF UNROTATE( const QPointF &pnt, double alpha, double a, double b );	//Процедура, обратная к ROTATE
	QPointF ARC( double t, double a, double b);					//Процедура вычисления координат точки дуги с центром в точке [0,0], соответствующей параметру  t
	double Angle( const QLineF &l, const QLineF &l1 );				//Процедура определения угла между двумя прямыми
	double Length( const QPointF pt1, const QPointF pt2 );				//Процедура определения расстояния между двумя точками
        bool Holds( QList<ShapeItem> &shapeItems, PntMap *pnts );			//Процедура определения привязанных к данной фигуре других фигур
        void Move_UP_DOWN ( QList<ShapeItem> &shapeItems, PntMap *pnts );		//Процедура перемещения фигуры с помощью управляющих клавиш
        int Real_rect_num( int rect_num_old, QList<ShapeItem> &shapeItems, PntMap *pnts );//Определение истиного номера квадратика
        void Rect_num_0_1( QList<ShapeItem> &shapeItems, int rect_num_temp, PntMap *pnts);//Что делать если попали по 1,2 квадратику
        void Rect_num_3_4( QList<ShapeItem> &shapeItems, PntMap *pnts );		//Что делать если попали по 2,4 квадратику дуги
        void Move_all( QPointF pos, QList<ShapeItem> &shapeItems, PntMap *pnts );	//Одновременное передвижение нескольних фигур(квадратиков)
        QPainterPath painter_path( float el_width, int el_type, double el_ang, 
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) );
        QPainterPath painter_path_simple( int el_type, double el_ang, 
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) ) ;
        int Append_Point( QPointF &pos, QList<ShapeItem> &shapeItems, PntMap *pnts );
        void Drop_Point ( int num, int num_shape, QList<ShapeItem> &shapeItems, PntMap *pnts );
      	
	//Attributes
	QPointF StartLine, EndLine,		//Start and end points for paint created figure    
		previousPosition, previousPosition_all;	//Previous position for drag point by figure moving
	ShapeItem *itemInMotion;		//Selected (moving) figure 
    
	QString curFile;
	bool isUntitled;
        bool status_hold;			//Статус нажатия кнопки привязок
    
	int* index_array;			//Массив, содержащий индексы фигур, которые выделяются с Сtrl
        
        int* rect_array;
      
      
        
        
	int count_Shapes, count_moveItemTo,	//Количество фигур, выделенных с Сtrl и количество вызовов процедуры moveItemTo для этих фигур соответственно
	    index, index_temp,			//Индекс фигуры в контейнере
	    rect_num;				//Номер выделенного квадратика(0..3)
	bool flag_cursor, flag_key, flag_up, flag_down, flag_left, flag_right, 
	     flag_ctrl, flag,flag_ctrl_move, flag_m, flag_hold_arc;	//Флаги для вида курсора и 
					    	//для управляющих клавиш соответственно
        bool flag_rect, flag_arc_rect_3_4, flag_arc_release, flag_first_move, Flag;
        int count_rects, rect_num_arc, arc_rect;
        bool flag_holds;			//Вспомогательная переменная для того, чтобы вызывать Holds() только один раз в начале движения
        bool flag_hold_move;			//Флаг передвижения привязанных фигур
        double t_start, t_end;			//Начальное и конечное значения параметра для рисования дуги
	QPointF Mouse_pos, offset_all;		//Текущая позиция мышки
        int current_ss, current_se, current_ee, current_es;	//Указывает, какую комбинацию из двух точек связывать 
        int count_holds;			//Количество привязанных фигур
        int *arc_rect_array;			//Содержит номера квадратиков дуги,связанные с другими фигурами
        int *fig_rect_array;			//Содержит номера квадратиков фигур,связанных с дугой 
        int *index_array_all;
        
        QPointF Prev_pos_1, Prev_pos_2;
       
        QVector<int> num_vector;       
    };

}

#endif //VIS_SHAPE_ELFIG_H


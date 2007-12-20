
//OpenSCADA system module UI.VISION file: vis_shape_elfig.h
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
		    const QPointF &ctrlpos_4, const QBrush &brush, const QPen &ipen, const QPen &pen_simple, const float iwidth, const float bwidth, const int itype ) : 
    	    ctrlPos4(ctrlpos_4), n1(num_1), n2(num_2), n3(num_3), n4(num_4), n5(num_5), brush(brush),
	    pen(ipen), penSimple(pen_simple), width(iwidth), border_width(bwidth), type(itype), path(ipath), pathSimple(path_simple)
	{ };								

	QPainterPath 	path, 
		        pathSimple;
	QPointF		ctrlPos4;
        int 		n1, n2, n3, n4, n5;
	QBrush 		brush;
	QPen 		pen, 
			penSimple;
	float 		width;
        float           border_width;
	int 		type;
};

//************************************************
//* InundationItem                                    *
//************************************************
class InundationItem
{
    public:
        //Methods
        InundationItem( )	{ }
        InundationItem( const QPainterPath &ipath, const QBrush &ibrush, QBrush &ibrush_img, const QVector <int> inumber_shape ) : 
                        brush(ibrush),brush_img(ibrush_img), path(ipath), number_shape(inumber_shape)
        { };

        //Attributes
        QPainterPath   path; 
        QBrush         brush,brush_img;
        QVector <int>  number_shape;
        QString        img_nm;
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
        //Data
        enum FigType { Line, Arc, Besier };
	//Public methods
	ShapeElFigure( );
    
        bool isEditable( )		{ return true; }

	void init( WdgView *view );
        void destroy( WdgView *w );
    
        void editEnter( WdgView *view );
        void editExit( WdgView *view );
	void wdgPopup( WdgView *view, QMenu &menu );
	
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
        bool shapeSave( WdgView *view );
     
	bool event( WdgView *view, QEvent *event );
    
 	QString userFriendlyCurrentFile( );
	QString currentFile( ) 		{ return curFile; }

	//Public attributes
	QVector <RectItem> rectItems;		//RectItem's container
	QPainterPath 	newPath,		//Null path
			rectPath,		//Path fo RectItem creation
        		ellipse_startPath, ellipse_endPath,
        		ellipse_draw_startPath, ellipse_draw_endPath,
                        InundationPath;
    
	bool status; 				//Check fo any primitive paint key pressed
	int shapeType; 				//Selected figure type
        int kr;    
    
    private slots:
	void toolAct( QAction * );
    
    private:
    	//Methods
        int itemAt( const QPointF &pos, QVector <ShapeItem> &shapeItems,WdgView *w);//Check for figure type under cursor
        void itemAtRun();
        void moveItemTo(const QPointF &pos,QVector <ShapeItem> &shapeItems, PntMap *pnts, WdgView *view);	//Move figure procedure
    
	QGraphicsScene *scene;
	void populateScene();
	void setCurrentFile( const QString &fileName );
	QString strippedName( const QString &fullFileName );
	QPointF ROTATE( const QPointF &pnt, double alpha );				
	QPointF UNROTATE( const QPointF &pnt, double alpha, double a, double b );	
	QPointF ARC( double t, double a, double b);					
	double Angle( const QLineF &l, const QLineF &l1 );				
	double Length( const QPointF pt1, const QPointF pt2 );				
        bool Holds( QVector <ShapeItem> &shapeItems, PntMap *pnts );			
        void Move_UP_DOWN ( QVector <ShapeItem> &shapeItems, PntMap *pnts, QVector <InundationItem> &inundationItems, WdgView *view );		
        int Real_rect_num( int rect_num_old, QVector <ShapeItem> &shapeItems, PntMap *pnts );
        void Rect_num_0_1( QVector <ShapeItem> &shapeItems, int rect_num_temp, PntMap *pnts, WdgView *view);
        void Rect_num_3_4( QVector <ShapeItem> &shapeItems, PntMap *pnts );		
        void Move_all( QPointF pos, QVector <ShapeItem> &shapeItems, PntMap *pnts, QVector <InundationItem> &inundationItems, WdgView *view);	
        QPainterPath painter_path( float el_width, float el_border_width, int el_type, double el_ang, 
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) );
        QPainterPath painter_path_simple( int el_type, double el_ang, 
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) ) ;
        int Append_Point( QPointF &pos, QVector <ShapeItem> &shapeItems, PntMap *pnts );
        void Drop_Point ( int num, int num_shape, QVector <ShapeItem> &shapeItems, PntMap *pnts );
        void step(int s,int f, int p, QVector <int> vect, int N);
        bool Inundation (QPointF point,QVector <ShapeItem> &shapeItems,  PntMap *pnts, QVector <int> vect, int N, WdgView *view);
        bool Inundation_1_2(QPointF point, QVector <ShapeItem> &shapeItems, QVector <InundationItem> &inundationItems,  PntMap *pnts, WdgView *view);
        int Build_Matrix(QVector <ShapeItem> &shapeItems);
        QPainterPath Create_Inundation_Path (QVector <int> in_fig_num, QVector <ShapeItem> &shapeItems, PntMap *pnts, WdgView *view);
        QVector <int> Inundation_sort(QPainterPath InundationPath, QVector <int> inundation_fig_num, QVector <ShapeItem> &shapeItems, PntMap *pnts,  WdgView *view);
      	
	//Attributes
	QPointF StartLine, EndLine,		//Start and end points for paint created figure    
		previousPosition, previousPosition_all;	//Previous position for drag point by figure moving
	ShapeItem *itemInMotion;		//Selected (moving) figure 
    
	QString curFile;
	bool isUntitled;
        bool status_hold;			
    
        QVector <int> index_array;
        QVector <int> rect_array;
            
	int count_Shapes, count_moveItemTo,	
	    index, index_temp,index_inund,			
	    rect_num;				
	bool flag_cursor, flag_key, flag_up, flag_down, flag_left, flag_right, 
	     flag_ctrl, flag,flag_ctrl_move, flag_m, flag_hold_arc;
					    	
        bool flag_rect, flag_arc_rect_3_4, flag_arc_release, flag_first_move, Flag;
        QPointF Start_offset, End_offset, CtrlPos1_offset,CtrlPos2_offset,CtrlPos3_offset,CtrlPos4_offset;
        int count_rects, rect_num_arc, arc_rect;
        bool flag_holds;			
        bool flag_hold_move;			
        bool flag_inund_break;
        double t_start, t_end;			
	QPointF Mouse_pos, offset;		
        int current_ss, current_se, current_ee, current_es;
        int count_holds;			
        QVector <int> arc_rect_array;
        QVector <int> fig_rect_array;
        QVector <int> vect;
        QVector< QVector<int> > map_matrix;
        QVector< QVector<int> > minroad;
        QVector< QVector<QPointF> > scale_offset;
        int counter_start, counter_end;
        int len,clen;
        int found;
        QVector<int> road;
        QVector <int> incl;
        QVector <int> Inundation_vector;
        QPointF Prev_pos_1, Prev_pos_2;
        QVector<int> num_vector;       
        int rect_num_move;
    };

}

#endif //VIS_SHAPE_ELFIG_H


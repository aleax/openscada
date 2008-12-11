
//OpenSCADA system module UI.VISION file: vis_shape_elfig.h
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

#ifndef VIS_SHAPE_ELFIG_H
#define VIS_SHAPE_ELFIG_H

#include <math.h>
#include <map>
#include <QPen>

#include "vis_shapes.h"
#include "vis_widgs.h"

using std::map;
typedef map<int,QPointF> PntMap;
typedef map<int,float> WidthMap;
typedef map<int,QColor> ColorMap;
typedef map<int,string> ImageMap;
typedef map<int,Qt::PenStyle> StyleMap;

namespace VISION
{

//************************************************
//* ShapeItem                                    *
//************************************************
class ShapeItem 
{
    public:
	ShapeItem( )	{ }
        ShapeItem( const QPainterPath &ipath, const QPainterPath &path_simple, short num_1, short num_2, short num_3, short num_4, short num_5,
		    const QPointF &ctrlpos_4, const short &ilineColor, const short &iborderColor, const short &istyle , short iwidth, short bwidth, short itype, double iangle_temp ) : 
    	    ctrlPos4(ctrlpos_4), n1(num_1), n2(num_2), n3(num_3), n4(num_4), n5(num_5), lineColor(ilineColor),
	    borderColor(iborderColor), style(istyle), width(iwidth), border_width(bwidth), type(itype), path(ipath), pathSimple(path_simple), angle_temp(iangle_temp)
	{ };								

	QPainterPath 	path, 
		        pathSimple;
	QPointF		ctrlPos4;
        short 		n1, n2, n3, n4, n5;
        short           lineColor, borderColor;
        short           style;
	short 		width;
        short           border_width;
	short 		type :3;
        double 		angle_temp;
};

//************************************************
//* inundationItem                               *
//************************************************
class inundationItem
{
    public:
        //Methods
        inundationItem( )	{ }
        inundationItem( const QPainterPath &ipath, const short &ibrush, const short &ibrush_img,
                        const QVector<int> &inumber_shape, const QVector<int> &inumber_point ) : 
                        brush(ibrush), brush_img(ibrush_img), path(ipath), number_shape(inumber_shape),
                       number_point(inumber_point)
        { };

        //Attributes
        QPainterPath   	path; 
        short         	brush, brush_img;
        QVector<int>  	number_shape;
        QVector<int>    number_point;
};


//************************************************
//* RectItem                                     *
//************************************************
class RectItem 
{
    public:
	RectItem( )	{ }
        RectItem( const QPainterPath &ipath, int inum, const QBrush &ibrush, const QPen &ipen ) :
	    num(inum), brush(ibrush), pen(ipen), path(ipath)	{  }

	QPainterPath 	path;
	int 		num;
	QBrush 		brush;
	QPen 		pen;
};

//*************************************************
//* Second sideo of shape widget               *
//*************************************************
class ElFigDt : public QObject
{
    Q_OBJECT
    public:
        //Methods
        ElFigDt( WdgView *wi ) : en(true), active(true), geomMargin(0), orient(0), w(wi)  { }
        //Attributes
        short	en          :1;
        short	active      :1;
        short   geomMargin  :8;
        double  orient;
        string  elLst;
        QVector<ShapeItem> shapeItems;
        QVector<inundationItem> inundationItems;
        PntMap  shapePnts;
        WidthMap shapeWidths;
        ColorMap shapeColors;
        ImageMap shapeImages;
        StyleMap shapeStyles;
        WdgView *w;
        QImage  pictObj;
    private slots:
        void dynamic();
};


//*************************************************
//* Elementary figures shape widget               *
//*************************************************
class ShapeElFigure : public WdgShape 
{    
    friend class ElFigDt;
    Q_OBJECT

    public:
	//Public methods
	ShapeElFigure( );
    
        bool isEditable( )		{ return true; }

	void init( WdgView *w );
        void destroy( WdgView *w );
    
        void editEnter( WdgView *w );
        void editExit( WdgView *w );
	void wdgPopup( WdgView *w, QMenu &menu );
	
	bool attrSet( WdgView *w, int uiPrmPos, const string &val );
        bool shapeSave( WdgView *view );
     
	bool event( WdgView *view, QEvent *event );
    
	//Public attributes
	QVector<RectItem> rectItems;		//RectItem's container
	QPainterPath 	newPath,		//Null path
			rectPath,		//Path fo RectItem creation
        		ellipse_startPath, ellipse_endPath,
        		ellipse_draw_startPath, ellipse_draw_endPath,
                        inundationPath,
                        dashedRectPath;
        QRect           dashedRect;
    
	bool status; 				//Check fo any primitive paint key pressed
	int shapeType; 				//Selected figure type
        int kr;    
    
    private slots:
        void toolAct( QAction* );
    
    private:
        //Data
    	//Methods
        int itemAt( const QPointF &pos, const QVector<ShapeItem> &shapeItems, WdgView *w );			//Check for figure type under cursor
        void moveItemTo( const QPointF &pos, QVector<ShapeItem> &shapeItems, PntMap *pnts, WdgView *w );	//Move figure procedure
    
	QPointF rotate( const QPointF &pnt, double alpha );
	QPointF unRotate( const QPointF &pnt, double alpha, double a, double b );
	QPointF arc( double t, double a, double b);	
	double angle( const QLineF &l, const QLineF &l1 );
	double length( const QPointF &pt1, const QPointF &pt2 );				
        bool holds( const QVector<ShapeItem> &shapeItems, PntMap *pnts );
        void moveUpDown( QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems, WdgView *w );
        int  realRectNum( int rect_num_old, const QVector<ShapeItem> &shapeItems );
        void rectNum0_1( const QVector<ShapeItem> &shapeItems, int rect_num_temp, PntMap *pnts, WdgView *w );
        void rectNum3_4( const QVector<ShapeItem> &shapeItems);
        void moveAll( const QPointF &pos, QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems, WdgView *w );
        void checkPoint_checkInundation( QVector<ShapeItem> &shapeItems, PntMap *pnts, QVector<inundationItem> &inundationItems );
        void paintImage( WdgView *view );
        QPainterPath painterPath( float el_width, float el_border_width, int el_type, double el_ang,
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) );
        QPainterPath painterPathSimple( int el_type, double el_ang, 
		QPointF el_p1 = QPointF(0,0), QPointF el_p2 = QPointF(0,0), QPointF el_p3 = QPointF(0,0), 
		QPointF el_p4 = QPointF(0,0), QPointF el_p5 = QPointF(0,0), QPointF el_p6 = QPointF(0,0) );
        int appendPoint( const QPointF &pos, const QVector<ShapeItem> &shapeItems, PntMap *pnts, bool flag_down );
        void dropPoint( int num, int num_shape, const QVector<ShapeItem> &shapeItems, PntMap *pnts );
        void step( int s, int f, int p, const QVector<int> &vect, int N );
        bool inundation( const QPointF &point, const QVector<ShapeItem> &shapeItems, PntMap *pnts, const QVector<int> &vect, int N, WdgView *w );
        bool inundation1_2( const QPointF &point, const QVector<ShapeItem> &shapeItems, QVector<inundationItem> &inundationItems, PntMap *pnts, WdgView *w, int number );
        int  buildMatrix( const QVector<ShapeItem> &shapeItems );
        QPointF scaleRotate( const QPointF &point, WdgView *w, bool flag_scale, bool flag_rotate );
        QPointF unScaleRotate( const QPointF &point, WdgView *w, bool flag_scale, bool flag_rotate );
        QPainterPath createInundationPath( const QVector<int> &in_fig_num, const QVector<ShapeItem> &shapeItems, PntMap &pnts, WdgView *w );
        QVector<int> inundationSort( const QPainterPath &inundationPath, QVector<int> &inundation_fig_num, const QVector<ShapeItem> &shapeItems, PntMap *pnts, WdgView *w );
      	
	//Attributes
	QPointF StartLine, EndLine,				//Start and end points for paint created figure
		previousPosition, previousPosition_all;		//Previous position for drag point by figure moving
	ShapeItem *itemInMotion;				//Selected (moving) figure
    
        bool status_hold;			
    
        QVector<int> index_array;
        QVector<int> rect_array;
        QVector<int> copy_index, index_array_copy, index_array_copy_flag_A;
            
	int count_Shapes, count_moveItemTo,
	    index, fill_index, index_temp, index_del,index_inund,
	    rect_num;
	bool flag_cursor, flag_key, flag_up, flag_down, flag_left, flag_right, 
             flag_ctrl, flag_ctrl_move, flag_m, flag_hold_arc, flag_A, flag_copy, flag_check_pnt_inund;
					    	
        bool flag_rect, flag_arc_rect_3_4, flag_first_move, flag_move, flag_release;
        int count_rects, rect_num_arc, arc_rect;
        bool flag_hold_move;			
        bool flag_inund_break;
        bool flag_scale, flag_rotate;
        double t_start, t_end;			
	QPointF Mouse_pos, offset, pop_pos;	
        QPoint stPointDashedRect;	
        int current_ss, current_se, current_ee, current_es;
        int count_holds;			
        QVector<int> arc_rect_array, fig_rect_array, vect;
        QVector< QVector<int> > map_matrix;
        QVector< QVector<int> > minroad;
        int len, clen;
        int found;
        QVector<int> road;
        QVector<int> incl;
        QVector<int> inundation_vector;
        QPointF Prev_pos_1, Prev_pos_2;
        QVector<int> num_vector;
        double angle_temp;
        bool flag_angle_temp;
        int rect_dyn;
        QImage rect_img;
        PntMap  shapePnts_temp;
        WidthMap shapeWidths_temp;
        ColorMap shapeColors_temp;
        ImageMap shapeImages_temp;
        StyleMap shapeStyles_temp;

    };
}

#endif //VIS_SHAPE_ELFIG_H


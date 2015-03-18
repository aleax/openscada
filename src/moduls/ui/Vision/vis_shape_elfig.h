
//OpenSCADA system module UI.VISION file: vis_shape_elfig.h
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

#ifndef VIS_SHAPE_ELFIG_H
#define VIS_SHAPE_ELFIG_H

#include <math.h>
#include <map>

#include <QPainterPath>
#include <QPen>

#include "vis_shapes.h"
#include "vis_widgs.h"

using std::map;
typedef map<int,QPointF> PntMap;
typedef map<int,float> WidthMap;
typedef map<int,QColor> ColorMap;
typedef map<int,string> ImageMap;
typedef map<int,Qt::PenStyle> StyleMap;

#define ARC_STEP	1.0/360.0	// Arc build sted 1/360 = 1 degree
#define XY_MIN		-1000000	// Canva's XY coordinats value minimum
#define XY_MAX		1000000		// Canva's XY coordinats value maximum

namespace VISION
{

enum SpecIdx { SpI_NullIt = -1, SpI_DefLine = -5, SpI_DefBord = -6, SpI_DefFill = -7, SpI_DefFillImg = -5, SpI_StatIts = -10 };
enum ShapeType { ShT_Line = 1, ShT_Arc = 2, ShT_Bezier = 3, ShT_Fill = 4 };
enum PntName { PntNull = -1, PntStart = 0, PntEnd = 1, PntCntr1 = 2, PntCntr2 = 3, PntCntr3 = 4 };

//************************************************
//* ShapeItem					 *
//************************************************
class ShapeItem
{
    public:
	//Methods
	ShapeItem( )	{ }
	ShapeItem( const QPainterPath &ipath, const QPainterPath &ipathSimple, short in1, short in2, short in3,
		short in4, short in5, const QPointF &ictrlPos4, const short &ilineColor, const short &iborderColor,
		const short &istyle , short iwidth, short iborderWidth, short itype, double iangle_temp, double iang_t = 0 ) :
	    path(ipath), pathSimple(ipathSimple), n1(in1), n2(in2), n3(in3), n4(in4), n5(in5),
	    type(itype), width(iwidth), lineColor(ilineColor), borderWidth(iborderWidth), borderColor(iborderColor), style(istyle),
	    ctrlPos4(ictrlPos4), angle_temp(iangle_temp), ang_t(iang_t)
	{ };

	QPainterPath path, pathSimple;
	short	n1, n2, n3, n4, n5;
	short	type, width, lineColor, borderWidth, borderColor, style;

	// Specific for Arc precalculated
	QPointF	ctrlPos4;
	double	angle_temp, ang_t;
};

//************************************************
//* InundationItem				 *
//************************************************
class inundationItem
{
    public:
	//Methods
	inundationItem( )	{ }
	inundationItem( const QPainterPath &ipath, const QVector<int> &in, const short &ibrush, const short &ibrushImg ) :
	    path(ipath), n(in), brush(ibrush), brushImg(ibrushImg)
	{ };

	//Attributes
	QPainterPath	path;
	QVector<int>	n;
	short		brush, brushImg;
};

//************************************************
//* RectItem					 *
//************************************************
class RectItem
{
    public:
	RectItem( ) { }
	RectItem( int inum, const QPainterPath &ipath = QPainterPath(),
		const QBrush &ibrush = QBrush(QColor(0,255,0,128),Qt::SolidPattern),
		const QPen &ipen = QPen(QColor(0,0,0),1,Qt::SolidLine,Qt::FlatCap,Qt::RoundJoin) ) :
	    path(ipath), num(inum), brush(ibrush), pen(ipen)    { }

	QPainterPath path;
	int	num;
	QBrush	brush;
	QPen	pen;
};

//*************************************************
//* Second sideo of shape widget		  *
//*************************************************
class ElFigDt : public QObject
{
    Q_OBJECT
    public:
	//Methods
	ElFigDt( WdgView *wi ) : en(true), active(true), geomMargin(0), mirror(false), orient(0), w(wi)  { }

	int appendPoint( const QPointF &pos, bool flag_down );		//Append the new point to the points' map
	void dropPoint( int num, int num_shape );			//Drop the point from the points' map
	int appendWidth( const float &width, bool flag_down );		//Append the new width to the widths' map
	int appendStyle( const Qt::PenStyle &style, bool flag_down );	//Append the new style to the styles' map
	int appendColor( const QColor &color, bool flag_down );		//Append the new color to the colors' map
	int appendImage( const string &image, bool flag_down );		//Append the new image to the images' map

	//Attributes
	short	en		:1;
	short	active		:1;
	short	geomMargin	:8;
	short	mirror		:1;
	double	orient;
	string	elLst;
	QVector<ShapeItem>	shapeItems;		//Shape items
	QVector<inundationItem>	inundItems;		//Inundation items
	PntMap	shapePnts, shapePnts_temp;
	WidthMap shapeWidths, shapeWidths_temp;
	ColorMap shapeColors, shapeColors_temp;
	ImageMap shapeImages, shapeImages_temp;
	StyleMap shapeStyles, shapeStyles_temp;
	WdgView *w;
	QPixmap pictObj;
	//QImage pictObj;

    private slots:
	void dynamic();
	void properties();
};

//*************************************************
//* Elementary figures shape widget		  *
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

	void editEnter( DevelWdgView *w );
	void editExit( DevelWdgView *w );
	void wdgPopup( WdgView *w, QMenu &menu );

	bool attrSet( WdgView *w, int uiPrmPos, const string &val );
	void shapeSave( WdgView *w );				//Saving shapes' attributes to data model

	bool event( WdgView *w, QEvent *event );

    private slots:
	void toolAct( QAction* );

    private:
	//Methods
	// Global
	//  Compute the point of the arc due to the given parameter "t"
	QPointF arc( double t, double a, double b )	{ return QPointF(a*cos(t*M_PI*2), -b*sin(t*M_PI*2)); }
	//  Compute the angle between two lines
	double angle( const QLineF &l, const QLineF &l1 ) {
	    return (!l.isNull() && !l1.isNull()) ?
		acos(vmax(-1,vmin(1,(l.dx()*l1.dx()+l.dy()*l1.dy())/(l.length()*l1.length()))))*180/M_PI :
		0;
	}
	// Compute the lenght between two points
	double length( const QPointF &pt1, const QPointF &pt2 ) { return sqrt(pow(pt2.x()-pt1.x(),2) + pow(pt2.y()-pt1.y(),2)); }
	//  Rotate the point around the centre of the widget
	QPointF rotate( const QPointF &pnt, double alpha ) {
	    return QPointF(pnt.x()*cos((alpha*M_PI)/180)-pnt.y()*sin((alpha*M_PI)/180),
		pnt.x()*sin((alpha*M_PI)/180)+pnt.y()*cos((alpha*M_PI)/180));
	}
	//  Unrotate the point around the centre of the widget
	QPointF unRotate( const QPointF &pnt, double alpha, const QPointF &cntr ) {
	    return QPointF((pnt.x()-cntr.x())*cos((alpha*M_PI)/180)-(pnt.y()-cntr.y())*sin((alpha*M_PI)/180),
			  -(pnt.x()-cntr.x())*sin((alpha*M_PI)/180)-(pnt.y()-cntr.y())*cos((alpha*M_PI)/180));
	}
	void moveAll( const QPointF &pos, WdgView *w );
	QPointF scaleRotate( const QPointF &point, WdgView *w, int8_t toScale = -1, int8_t toTrans = -1 );	//Scale or/and rotate of the point
	QPointF unScaleRotate( const QPointF &point, WdgView *w, int8_t toScale = -1, int8_t toTrans = -1 );	//Redo the scale or/and rotate of the point
	QPainterPath painterPath( float width, float bWidth, int type, double ang, QPointF pBeg, QPointF pEnd,
		QPointF pCntr1 = QPointF(), QPointF pCntr2 = QPointF(), QPointF pCntr3 = QPointF(), QPointF aT = QPointF() );
	QPainterPath painterPathSimple( int type, double ang, QPointF pBeg, QPointF pEnd,
		QPointF pCntr1 = QPointF(), QPointF pCntr2 = QPointF(), QPointF pCntr3 = QPointF(), QPointF aT = QPointF() );
	void paintImage( WdgView *w );				//Creating the paths for the figures

	// Items-figures
	void initShapeItems( const QPointF &pos, QVector<int> &items_array, WdgView *w );
	int itemAt( const QPointF &pos, WdgView *w );		//Check for figure type under cursor
	void moveItemTo( const QPointF &pos, WdgView *w );	//Move figure procedure
	bool holds( WdgView *w );				//Compute the number of connected figures with the given one
	void moveUpDown( WdgView *w );				//Moving the figure(s) with the help of keyboard
	int  realRectNum( int rect_num_old, WdgView *w );	//Compute the real rect number of the figure when several figures are selected
	void rectNum0_1( int rect_num_temp, WdgView *w );
	void rectNum3_4( WdgView *w );
	//  Calculate the t_start and t_end for the arc
	QPointF getArcStartEnd( QPointF pBeg, QPointF pEnd, QPointF pCntr1, QPointF pCntr2, QPointF pCntr3 );

	// Inundations
	//  Checking if appending of the point and deleting of the fill(inundation) is needed
	//   when several(choosen with one of the methods) figures are moving
	//  Creation the filling(inundation) path from the 'fill' string of the attributes inspector
	QPainterPath createInundationPath( const QVector<int> &in_fig_num, PntMap &pnts, WdgView *w );
	void removeFill( QVector<int> ind_array, int count, WdgView *w );
	void checkPoint_checkInundation( WdgView *w );
	void step( int s, int f, int p, const QVector<int> &vect, int N );	//The function for the calculation of the minimum path to be filled
	bool inundation( const QPointF &point, int N, WdgView *w );		//Building the inundation(fill) if there are more then 2 figures in its path
	bool inundation1_2( const QPointF &point, int number, WdgView *w );	//Building the inundation(fill) if there are 2 pr 1 figures in its path
	QVector<int> inundationSort( const QPainterPath &inundationPath, QVector<int> &inundation_fig_num, WdgView *w );
	int  buildMatrix( WdgView *w );						//The function for the calculation of the minimum path to be filled

	//Attributes
	QPointF StartLine, EndLine,				//Start and end points for paint created figure
		previousPosition, previousPosition_all;		//Previous position for drag point by figure moving
	ShapeItem *itemInMotion;				//Selected (moving) figure

	QVector<int> index_array;				//Array of the selected figures
	QVector<int> rect_array;
	QVector<int> copy_index, index_array_copy, index_array_copy_flag_A;

	int count_Shapes, count_moveItemTo, index, fill_index, index_temp, index_del, rect_num, dyn_num;

	unsigned status_hold		:1;
	unsigned flag_up		:1;
	unsigned flag_down		:1;
	unsigned flag_left		:1;
	unsigned flag_right		:1;
	unsigned flag_ctrl		:1;
	unsigned flag_ctrl_move		:1;
	unsigned flag_m			:1;
	unsigned flag_hold_arc		:1;
	unsigned flag_A			:1;
	unsigned flag_copy		:1;
	unsigned flag_check_pnt_inund	:1;
	unsigned flag_check_point	:1;
	unsigned flag_rect		:1;
	unsigned flag_arc_rect_3_4	:1;
	unsigned flag_first_move	:1;
	unsigned flag_move		:1;
	unsigned flag_hold_move		:1;
	unsigned flag_inund_break	:1;
	unsigned fTransl		:1;	//Coordinates translation
	unsigned flag_angle_temp	:1;
	unsigned flag_geom		:1;
	unsigned flag_rect_items	:1;
	unsigned flag_def_stat		:1;
	unsigned flag_dyn_save		:1;
	unsigned fl_status_move		:1;	//Flag for the calling moveItemTo during the creation of the figure
	unsigned fl_orto_move		:1;	//Flag for using during the orthogonal line or bezier curve drawing
	unsigned fl_orto_disable	:1;	//Flag for disabling the orthogonal drawing in the moveItemTo function
	unsigned status			:1;	//Check fo any primitive paint key pressed
	//unsigned fMoveHoldMove		:1;	//Moving processing flag hysteresis

	int count_holds, count_rects, rect_num_arc, arc_rect;
	double t_start, t_end;			//Start and end values of the arc, global modified from moveItemTo()
	QPointF Mouse_pos, offset, pop_pos;
	QPoint stPointDashedRect, mousePress_pos;
	int current_ss, current_se, current_ee, current_es;
	QVector<int> arc_rect_array, fig_rect_array, vect;
	QVector< QVector<int> > map_matrix;
	QVector< QVector<int> > minroad;
	int len, clen, found;
	int geomH, geomW;
	QVector<int> road;
	QVector<int> incl;
	QVector<int> inundation_vector;
	QPointF Prev_pos_1, Prev_pos_2;
	QVector<int> num_vector;		//????
	double angle_temp;
	int rect_dyn;
	QPixmap rect_img;

	QVector<RectItem> rectItems;		//RectItem's container
	QPainterPath	newPath,		//Null path
			ellipse_startPath,
			ellipse_endPath,
			ellipse_draw_startPath,
			ellipse_draw_endPath,
			inundationPath,		//Path of the fill(inundation)
			dashedRectPath;		//Path of the rect to select figures with mouse
	QRect	dashedRect;			//Rect to select figures with mouse
	int	shapeType;			//Selected figure type
    };
}

#endif //VIS_SHAPE_ELFIG_H

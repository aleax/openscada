
//OpenSCADA system module UI.WebVision file: vca_sess.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Yashina Kseniya (ksu@oscada.org)	   *
 *		   2007-2012 by Lysenko Maxim (mlisenko@oscada.org)	   *
 *		   2007-2013 by Roman Savochenko (rom_as@oscada.org)	   *
 *									   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.	           *
 *									   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of	   *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	   *
 *   GNU General Public License for more details.			   *
 *									   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the			   *
 *   Free Software Foundation, Inc.,				           *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.	     	   *
 ***************************************************************************/

#ifndef VCA_SESS_H
#define VCA_SESS_H

#include <gd.h>

#if HAVE_FFTW3_H
#include <fftw3.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <math.h>

#include <tcntrnode.h>


class Point
{
    public:
	Point( ) : x(0), y(0) 				{ };
	Point( double ix, double iy) : x(ix), y(iy)	{ };
	double x;
	double y;
};

typedef map<int,Point> PntMap;
typedef map<int,int> WidthMap;
typedef map<int,int> ColorMap;
typedef map<int,string> ImageMap;
typedef map<int,int> StyleMap;

using std::string;
using std::vector;
using std::map;
using namespace OSCADA;

namespace WebVision
{

class SSess;

//*************************************************
//* VCAObj					  *
//*************************************************
class VCASess;

class VCAObj : public TCntrNode
{
    public:
	//Methods
	VCAObj( const string &iid );

	const char *nodeName( )	{ return mId.c_str(); }
	string path( )	{ return TSYS::sepstr2path(mId); }

	virtual void getReq( SSess &ses ) = 0;
	virtual void postReq( SSess &ses ) = 0;
	virtual void setAttrs( XMLNode &node, const string &user ) = 0;

	VCASess &owner( );

    protected:
	string imgDef;

    private:
	//Attributes
	const string	mId;
};


//*************************************************
//* ElFigure					  *
//*************************************************
class ShapeItem
{
    public:
	//Data
	enum Type { Line = 1, Arc = 2, Bezier = 3, Fill = 4 };
	enum SpecIdx { DefLine = -5, DefBord = -6, DefFill = -7, DefFillImg = -5, StatIts = -10 };

	//Methods
	ShapeItem( )	{ }
	ShapeItem(const int num_1,const int num_2, const int num_3, const int num_4,const int num_5,
		  const Point &ctrlpos_4, const double iang, const int color, const int bcolor, const int iwidth,
		  const int bwidth, const int itype, const int istyle, const bool iflag_brd ) :
	    ctrlPos4(ctrlpos_4), ang(iang), n1(num_1), n2(num_2), n3(num_3), n4(num_4), n5(num_5), style(istyle),
	    width(iwidth), border_width(bwidth), lineColor(color), borderColor(bcolor), type(itype), flag_brd(iflag_brd)
	{ }

	Point	ctrlPos4;
	double	ang;
	int 	n1, n2, n3, n4, n5,
		style,
		width,
		border_width,
		lineColor,
		borderColor,
		type;
	bool	flag_brd;
};

class InundationItem
{
    public:
	InundationItem( )	{ }
	InundationItem( vector<int> inumber_point, int color, int i_index_color, string P_imgFill ) : number_point(inumber_point), P_color(color), index_color(i_index_color), imgFill(P_imgFill)
	{ }

	vector<int>	number_point;
	int		P_color, index_color;
	string		imgFill;

};

class VCAElFigure : public VCAObj
{
    public:
	//Methods
	VCAElFigure( const string &iid );
	~VCAElFigure( );

	void getReq( SSess &ses );
	void postReq( SSess &ses );
	void setAttrs( XMLNode &node, const string &user );

	bool lineIntersect( double x1, double y1, double x2, double y2,
			    double x3, double y3, double x4, double y4 );
	//vector<Point> floodFill( gdImagePtr im, int xMin, int yMin, int xMax, int yMax, int x, int y, int border_color);
	double angle( const Point p1, const Point p2, const Point p3, const Point p4 );
	static double length( const Point pt1, const Point pt2 );
	Point arc( double t, double a, double b );
	Point unrotate(const Point pnt, double alpha, double a, double b);
	Point rotate( const Point pnt, double alpha );
	Point bezier( double t, Point p1, Point p2, Point p3, Point p4 );
	double bezierDeltaT( Point p1, Point p2, Point p3, Point p4 );
	static double ABS(double var);
	//bool isPaintable( ShapeItem item, double xScale, double yScale );
	void paintFigure( gdImagePtr im, ShapeItem item, double xScale, double yScale, bool flag_allocate, bool flag_style );
	void paintFigureBorders( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, int clr_el_line, double el_width, double el_border_width, int type, double xScale, double yScale );
	void dashDot( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, double el_width, int type, int style  );
	void dashDotFigureBorders( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, int clr_el_line, double el_width, double el_border_width, int type, double wdt, double wdt_1, double xScale, double yScale  );
	void paintFill( gdImagePtr im, Point pnt, InundationItem &in_item );
	Point unscaleUnrotate( Point point, double xScale, double yScale, bool flag_scale, bool flag_rotate );
	Point scaleRotate( Point point, double xScale, double yScale, bool flag_scale, bool flag_rotate );
	int drawElF( SSess &ses, double xSc, double ySc, Point clickPnt );
	//Attributes
	double	width,	  //Widget geometry
		height,
		geomX,
		geomY;
	int	geomMargin,     //Margin
		lineClr,	//The color of the line
		bordWdth,       //The border's width
		bordClr,	//The border's color
		lineWdth,       //The line's width
		fillClr,	//The fill's color
		lineStyle,      //The style of the line
		scaleHeight,    //The vertical scale
		scaleWidth;     //The horizontal scale
	string	elLst;
	double	orient;		//The orientation angle
	bool	active,		//Active diagram
		rel_list;
	gdImagePtr im;

	PntMap pnts;
	WidthMap widths;
	ColorMap colors;
	ImageMap images;
	StyleMap styles;

	vector<ShapeItem> shapeItems;
	vector<InundationItem> inundationItems;

	Res	mRes;
};

//*************************************************
//* VCAText					  *
//*************************************************
class VCAText : public VCAObj
{
    private:
	//Data
	//> Argument object's class
	class ArgObj
	{
	    public:
		//Methods
		ArgObj( )				{ };

		string &cfg( )				{ return m_cfg; }
		string &val( )				{ return m_val; }
		int type( )				{ return m_type; }

		void setCfg( const string &vl )		{ m_cfg = vl; }
		void setVal( const string &vl )		{ m_val = vl; }
		void setType( const int vl )		{ m_type = vl; }

	    private:
		//Attributes
		string		m_val;
		string  	m_cfg;
		int	     m_type;
	};

    public:
	//Methods
	VCAText( const string &iid );
	~VCAText( );

	void getReq( SSess &ses );
	void postReq( SSess &ses )  { }
	void setAttrs( XMLNode &node, const string &user );
	Point rot( const Point pnt, double alpha, const Point center );
	vector<int> textRotate( double ang, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4 );

	//Attributes
	gdImagePtr im;
	double	orient;
	bool	active;		//Active diagram

	double	width, height;  //Widget geometry
	int     scaleHeight,    //Vertical scale
		scaleWidth;     //Horizontal scale
	int     textFontSize,   //The font's size of the text
		textColor;      //The color of the text
	short   alignHor :4;    //Horizontal align
	short   alignVer :3;    //Vertical align
	bool    wordWrap,       //Word wrap flag
		underline,      //Underline text flag
		strikeout,      //Strikeout text flag
		bold;	   	//Bold text flag ( for calculating the underline and strikeout width )
	string  text,	   	//The result text
		text_tmpl,      //The text without the arguments
		textFont;       //The text's font
	vector<ArgObj>	args;   //The argument's vector

	Res	mRes;
};

//*************************************************
//* VCADiagram					  *
//*************************************************
class VCADiagram : public VCAObj
{
    public:
	//Methods
	VCADiagram( const string &iid );

	void getReq( SSess &ses );
	void postReq( SSess &ses );
	void setAttrs( XMLNode &node, const string &user );
	void makeImgPng( SSess &ses, gdImagePtr im );

	void makeTrendsPicture( SSess &ses );
	void makeSpectrumPicture( SSess &ses );

	//Attributes
	int	width, height,		//Widget geometry
		tArX, tArY, tArW, tArH;	//Trend area geometry
	short	active		:1;	//Active diagram
	short	type		:3;	//Diagram type
	short	bordWidth	:8;	//Border width
	short	geomMargin	:8;	//Margin
	short	tTimeCurent	:1;	//Curent time
	short	trcPer		:10;	//Tracing period
	short	sclHor		:4;	//Horisontal scale mode
	short	sclVer		:4;	//Vertical scale mode
	short	holdCur		:1;	//Hold cursor
	int64_t	tTime, curTime, tPict, sclHorPer;	//Trend time, trend cursor's time position and picture time
	float	tSize;			//Trend size (s)
	float	fftBeg, fftEnd;
	float	sclVerScl, sclVerSclOff;
	int	curColor, 		//Cursor line color
		sclColor,		//Scale grid color
		sclMarkColor,		//Scale markers color
		sclMarkFontSize;	//Scale markers font size
	string	sclMarkFont,		//Scale markers font
		valArch;		//Value archivator
	time_t	lstTrc;			//Last trace

    private:
	//Data
	//> Trend object's class
	class TrendObj
	{
	    friend class VCADiagram;
	    public:
		//Data
		class SHg
		{
		    public:
			SHg( int64_t itm, double ival ) : tm(itm), val(ival) { }
			int64_t	tm;
			double	val;
		};

		//Methods
		TrendObj( VCADiagram *owner );
		~TrendObj( );

		string	addr( )		{ return mAddr; }
		double	bordL( )	{ return mBordLow; }
		double	bordU( )	{ return mBordUp; }
		int	color( )	{ return mColor; }
		char	width( )	{ return mWidth; }
		int	scale( )	{ return mScale; }
		double	curVal( )	{ return mCurvl; }
		int	valTp( )	{ return val_tp; }
		int64_t	valBeg( );
		int64_t	valEnd( );
		int val( int64_t tm );
		deque<SHg> &val( )	{ return vals; }

		void setAddr( const string &vl );
		void setBordL( double vl )	{ mBordLow = vl; }
		void setBordU( double vl )	{ mBordUp  = vl; }
		void setColor( int vl )		{ mColor = vl; }
		void setWidth( char vl )	{ mWidth = vl; }
		void setScale( char vl )	{ mScale = vl; }
		void setCurVal( double vl )	{ mCurvl = vl; }

		void loadData( const string &user, bool full = false );
		void loadTrendsData( const string &user, bool full );
		void loadSpectrumData( const string &user, bool full );

		VCADiagram &owner( );

		//Attributes
		double	adjL, adjU;		//Adjusted lower and upper borders
		bool	isIndiv;		//Individual scale
		char	wScale;
#if HAVE_FFTW3_H
		//> FFT
		int	fftN;			//Spectrum samples number
		fftw_complex	*fftOut;	//Spectrum out buffer, size = fftN/2+1
#endif
	    private:
		//Attributes
		string		mAddr;		//A parameter or an archive item address
		double mBordLow, mBordUp;	//Borders
		double		mCurvl;		//Curent value
		int		mColor;		//Values line color
		char		mWidth;		//Line width
		char		mScale;		//Separted scale
		//> Archive
		int64_t		arh_per;	//Archive period
		int64_t		arh_beg;	//Archive begin time
		int64_t		arh_end;	//Archive end time
		//> Values
		int		val_tp;		//Values type
		deque<SHg>	vals;		//Values buffer

		VCADiagram	*m_owner;	//Owner object
	};

	//Methods
	void setCursor( int64_t tTime, const string& user );

	//Attributes
	vector<TrendObj> trnds;			//Trends container

	Res	mRes;
};

//*************************************************
//* VCADocument					  *
//*************************************************
class VCADocument : public VCAObj
{
    public:
	//Methods
	VCADocument( const string &iid );
	~VCADocument( )			{ }

	void getReq( SSess &ses )	{ }
	void postReq( SSess &ses )	{ }
	void setAttrs( XMLNode &node, const string &user );
};

//*************************************************
//* VCASess					  *
//*************************************************
class VCASess : public TCntrNode
{
    public:
	//Methods
	VCASess( const string &iid, bool isCreate );

	string id( )			{ return mId; }
	const string &sender( )		{ return mSender; }
	time_t lstReq( )		{ return lst_ses_req; }

	void senderSet(const string &val)	{ mSender = val; }

	void getReq( SSess &ses );
	void postReq( SSess &ses );

	//> Objects
	void objCheck( const string &rootId, const string &wPath );
	void objList( vector<string> &list )		{ chldList(id_objs,list); }
	bool objPresent( const string &name )		{ return chldPresent(id_objs,name); }
	void objAdd( VCAObj *obj );
	void objDel( const string &name )		{ chldDel(id_objs, name); }
	AutoHD<VCAObj> objAt( const string &name )	{ return chldAt(id_objs,name); }

	string resGet( const string &res, const string &path, const string &user, string *mime = NULL );

	string cacheResGet( const string &res, string *mime = NULL );
	void cacheResSet( const string &res, const string &val, const string &mime );

	Res &nodeRes( )			{ return nRes; }

    private:
	//Data
	class CacheEl
	{
	    public:
	    CacheEl( time_t itm, const string &ival, const string &imime ) : tm(itm), val(ival), mime(imime)	{ }
	    CacheEl( )	{ }

	    time_t	tm;
	    string	val;
	    string	mime;
	};

	//Methods
	const char *nodeName( )	{ return mId.c_str(); }
	void postDisable( int flag );

	//Attributes
	const string		mId;
	int			id_objs;	//Primitive object's container identifier
	time_t			lst_ses_req;
	string			mSender;
	bool			mIsCreate;
	map<string,CacheEl>	mCacheRes;	//Resources cache

	Res	nRes;
};

}
#endif //VCA_SESS_H

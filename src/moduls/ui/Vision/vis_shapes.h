
//OpenSCADA system module UI.Vision file: vis_shapes.h
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko
 *   rom_as@diyaorg.dp.ua
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

#ifndef VIS_SHAPES_H
#define VIS_SHAPES_H

#include <string>
#include <deque>
#include <vector>

#include <fftw3.h>

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QMenu>
#include <QPen>
#include <QLabel>
#include <QPicture>
#include <QTableWidget>
#include <QScrollArea>
#include <QTextBrowser>

#include <tmess.h>
#include <xml.h>

using std::string;
using std::deque;
using std::vector;

class QEvent;
class QAction;
class QPainter;

namespace VISION
{

class WdgView;

//*************************************************
//* Widget shape abstract object                  *
//*************************************************
class WdgShape : public QObject
{
    Q_OBJECT

    public:
	//Methods
	WdgShape( const string &iid );

	string id( )									{ return m_id; }

	virtual bool isEditable( )							{ return false; }

	virtual void init( WdgView *view )						{ }
	virtual void destroy( WdgView *view )						{ }

	virtual void editEnter( WdgView *view )						{ }
	virtual void editExit( WdgView *view )						{ }
	virtual void wdgPopup( WdgView *view, QMenu &menu )				{ }

	virtual bool attrSet( WdgView *view, int uiPrmPos, const string &val );

	virtual bool event( WdgView *view, QEvent *event );
	virtual bool eventFilter( WdgView *view, QObject *object, QEvent *event )	{ }

	//Static methods
	static QFont getFont( const string &vl, float fsc = 1, bool pixSize = true );
	static QColor getColor( const string &vl );

    protected:
	//Methods
	void borderDraw( QPainter &pnt, QRect dA, QPen bpen, int bordStyle );

    private:
	//Attributes
	string m_id;
};

//============ Support widget's shapes ============

//*************************************************
//* Form element shape widget                     *
//*************************************************
class ShapeFormEl : public WdgShape
{
    Q_OBJECT

    public:
	//Methods
	ShapeFormEl( );

	void init( WdgView *view );
	void destroy( WdgView *view );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );
	bool eventFilter( WdgView *view, QObject *object, QEvent *event );

    public slots:
	//Public slots
	//>> Edit line events
	void lineAccept( );
	//>> Edit text events
	void textAccept( );
	//>> Check box events
	void checkChange(int);
	//-- Combo box and list events --
	void comboChange(const QString&);
	//-- List events --
	void listChange(int);
	//-- Button's events --
	void buttonPressed( );
	void buttonReleased( );
	void buttonToggled( bool val );
	//-- Sliders' events --
	void sliderMoved( int val );

    private:
	//Data
	//- Shape node date -
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), active(true), evLock(false), addrWdg(NULL), elType(-1), welType(-1) { }
		//Attributes
		short	en	:1;
		short	active	:1;
		short	evLock	:1;
		short	elType	:4;
		short	welType	:4;
		short	view	:4;
		short	wordWrap:1;
		short	checkable:1;
		QWidget	*addrWdg;
		QFont	font;
		string	name;
		string	value;
		string	img;
		string	items;
		string	cfg;
		string	color, colorText;
	};

	//Methods
	//Recursively widgets process for disable focusable and events filter set
	void eventFilterSet( WdgView *view, QWidget *wdg, bool en );
	void setFocus(WdgView *view, QWidget *wdg, bool en = false, bool devel = false );
};

//************************************************
//* Text element shape widget                    *
//************************************************
class ShapeText : public WdgShape
{
    public:
	//Methods
	ShapeText( );

	void init( WdgView *view );
	void destroy( WdgView *view );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );

    private:
	//Data
	//- Argument object's class -
	class ArgObj
	{
	    public:
		//Methods
		ArgObj( )				{ };

		string &cfg( )				{ return m_cfg; }
		QVariant val( )				{ return m_val; }

		void setCfg( const string &vl )		{ m_cfg = vl; }
		void setVal( const QVariant &vl )	{ m_val = vl; }

	    private:
		//Attributes
		QVariant	m_val;
		string		m_cfg;
	};
	//- Shape node date -
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), active(true), geomMargin(0), text_flg(0)	{ }
		//Attributes
		short	en		:1;
		short	active		:1;
		short	geomMargin	:8;
		short	bordStyle	:5;
		short int orient;
		short int text_flg;
		string	text_tmpl,
			text;
		QColor	color;
		QBrush	backGrnd;
		QFont	font;
		QPen	border;
		vector<ArgObj>	args;
	};
};

//************************************************
//* Media view shape widget                      *
//************************************************
class ShapeMedia : public WdgShape
{
    public:
	//Methods
	ShapeMedia( );

	void init( WdgView *view );
	void destroy( WdgView *view );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );

    private:
	//Data
	//- Map areas structure -
	class MapArea
	{
	    public:
		//Methods
		MapArea( ) : shp(-1)	{ }

		bool containsPoint( const QPoint & point );

		//Attributes
		int	shp;		//Area shape
		string	title;		//Area title
		QVector<QPoint>	pnts;	//Area points
	};
	//- Shape node date -
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), mediaType(-1), geomMargin(0)	{ }
		//Attributes
		short	en		:1;
		short	geomMargin	:8;
		short	bordStyle	:5;
		short	mediaType	:3;
		short	mediaFit	:1;
		short	mediaSpeed	:8;
		QBrush	backGrnd;
		QLabel	*labWdg;
		QPen	border;
		string	mediaSrc;
		vector<MapArea>	maps;
	};
};

//************************************************
//* Diagram view shape widget                    *
//************************************************
class ShapeDiagram : public WdgShape
{
    Q_OBJECT

    public:
	//Data
	//> Trend object's class
	class TrendObj
	{
	    public:
		//Data
		class SHg
		{
		    public:
			SHg( long long itm, double ival ) : tm(itm), val(ival) { }
			long long tm;
			double val;
		};
		//Methods
		TrendObj( WdgView *view );
		~TrendObj( );

		string	addr( )		{ return m_addr; }
		double	bordL( )	{ return m_bord_low; }
		double	bordU( )	{ return m_bord_up; }
		QColor	color( )	{ return m_color; }
		double	curVal( )	{ return m_curvl; }
		int	valTp( )	{ return val_tp; }
		long long valBeg( );
		long long valEnd( );
		int	val( long long tm );
		deque<SHg> &val( )	{ return vals; }

		void setAddr( const string &vl );
		void setBordL( double vl )	{ m_bord_low = vl; }
		void setBordU( double vl )	{ m_bord_up  = vl; }
		void setColor( const QColor &vl ){ m_color = vl; }
		void setCurVal( double vl )	{ m_curvl = vl; }

		void loadData( bool full = false );
		void loadTrendsData( bool full = false );
		void loadSpectrumData( bool full = false );

		//Attributes
		//> FFT
		int		fftN;		//Spectrum samples number
		fftw_complex	*fftOut;	//Spectrum out buffer, size = fftN/2+1

	    private:
		//Attributes
		string		m_addr;		//A parameter or an archive item address
		double m_bord_low, m_bord_up;	//Borders
		double		m_curvl;	//Curent value
		QColor		m_color;	//Values line color
		//> Archive
		int		arh_per;	//Archive period
		long long	arh_beg;	//Archive begin time
		long long	arh_end;	//Archive end time
		//> Values
		int		val_tp;		//Values type
		deque<SHg>	vals;		//Values buffer

		WdgView 	*view;
	};
	//> Shape node date
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), active(true), geomMargin(0), tTime(0), tPict(0), curTime(0), 
		    sclVerScl(100), sclVerSclOff(0), tTimeCurent(false), trcPer(0), holdCur(true)	{ }
		//Attributes
		short	en		:1;
		short	active		:1;
		short	type		:3;
		short	holdCur		:1;
		short	geomMargin	:8;
		short	bordStyle	:5;
		short	tTimeCurent	:1;
		short	trcPer		:10;
		short	sclHor		:4;
		short	sclVer		:4;
		QColor	curColor, sclColor, sclMarkColor;
		QBrush	backGrnd;
		QPen		border;
		QImage		pictObj;
		QRect		pictRect;
		QFont		sclMarkFont;
		QTimer 		*trcTimer;
		vector<TrendObj>	prms;
		long long	tTime, tPict, curTime;
		float		tSize;
		float		fftBeg, fftEnd;
		float		sclVerScl, sclVerSclOff;
		string		valArch;
	};

	//Methods
	ShapeDiagram( );

	void init( WdgView *view );
	void destroy( WdgView *view );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );

    private slots:
	//Private slots
	void tracing( );	//Trends tracing

    private:
	//Methods
	void makePicture( WdgView *view );
	void makeTrendsPicture( WdgView *view );
	void makeSpectrumPicture( WdgView *view );

	void loadData( WdgView *view, bool full = false );
	void setCursor( WdgView *view, long long itm );
};

//************************************************
//* Protocol view shape widget                   *
//************************************************
class ShapeProtocol : public WdgShape
{
    Q_OBJECT

    public:
	//Methods
	ShapeProtocol( );

	void init( WdgView *view );
	void destroy( WdgView *w );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );
	bool eventFilter( WdgView *view, QObject *object, QEvent *event );

    private slots:
	//Private slots
	void tracing( );	//Trends tracing

    private:
	//Data
	//> Shape node date
	class ShpDt
	{
	    public:
		//Data
		class ItProp
		{
		    public:
			ItProp( ) : lev(0)	{ }

			int lev;
			string tmpl;
			QColor clr;
			QFont font;
		};
		//Methods
		ShpDt( ) : active(true), time(0), tSize(60), timeCurent(false), arhBeg(0), arhEnd(0)	{ }
		//Attributes
		short	active		:1;
		short	timeCurent	:1;
		short	trcPer		:10;
		short	lev		:4;
		short	viewOrd		:4;
		QTimer 		*trcTimer;
		QTableWidget	*addrWdg;
		unsigned int	time, tSize;
		unsigned int	arhBeg, arhEnd;
		string		arch, tmpl, col;
		vector<ItProp>	itProps;
		deque<TMess::SRec>	messList;
	};
	//Private methods
	void loadData( WdgView *view, bool full = false );
	//> Recursively widgets process for disable focusable and events filter set
	void eventFilterSet( WdgView *view, QWidget *wdg, bool en );
	void setFocus(WdgView *view, QWidget *wdg, bool en = false, bool devel = false );
};

//************************************************
//* Document view shape widget                   *
//************************************************
class ShapeDocument : public WdgShape
{
    Q_OBJECT

    public:
	//Data
	//> Shape node date
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), active(true), tmpl(false), web(NULL) { }
		//Attributes
		short	en	:1;
		short	active	:1;
		short	view	:4;
		short	tmpl	:1;
		QTextBrowser	*web;
		string	style;
		string	doc;
	};

	//Methods
	ShapeDocument( );

	void init( WdgView *view );
	void destroy( WdgView *w );
	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );
	bool eventFilter( WdgView *view, QObject *object, QEvent *event );

    private:
	//Methods
	//Recursively widgets process for disable focusable and events filter set
	void eventFilterSet( WdgView *view, QWidget *wdg, bool en );
	void setFocus(WdgView *view, QWidget *wdg, bool en = false, bool devel = false );

	void nodeProcess( XMLNode *xcur, ShpDt *shD );

	//Attributes
	static const char *XHTML_entity;
};

//************************************************
//* User element shape widget                    *
//************************************************
class RunPageView;

class ShapeBox : public WdgShape
{
    public:
	//Data
	//> Shape node date
	class ShpDt
	{
	    public:
		//Methods
		ShpDt( ) : en(true), geomMargin(0), bordStyle(0), inclWidget(NULL), inclScrl(NULL)	{ }
		//Attributes
		short	en		:1;
		short	geomMargin	:8;
		short	bordStyle	:5;
		QPen		border;
		QBrush		backGrnd;
		RunPageView	*inclWidget;
		QScrollArea 	*inclScrl;
	};

	//Methods
	ShapeBox( );

	void init( WdgView *view );
	void destroy( WdgView *view );

	bool attrSet( WdgView *view, int uiPrmPos, const string &val);
	bool event( WdgView *view, QEvent *event );
};

//************************************************
//* User function shape widget                   *
//************************************************
class ShapeFunction : public WdgShape
{
    public:
	//Methods
	ShapeFunction( );
	//bool event( WdgView *view, QEvent *event );
};

}

#endif //VIS_SHAPES_H

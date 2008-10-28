
//OpenSCADA system module UI.WebVision file: vca_sess.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko (rom_as@fromru.com)       *
 *                           by Lysenko Maxim (mlisenko@ukr.net)           *
 *                           by Yashina Kseniya (sobacurka@ukr.net)        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string.h>

#include <tsys.h>

#include "web_vision.h"
#include "vca_sess.h"

using namespace WebVision;

//*************************************************
//* VCASess                                       *
//*************************************************
VCASess::VCASess( const string &iid, bool isCreate ) : m_id(iid), mIsCreate(isCreate)
{
    lst_ses_req	= time(NULL);
    id_objs	= grpAdd("obj_");
}

void VCASess::postDisable( int flag )
{
    TCntrNode::postDisable(flag);

    //- Disconnect/delete session -
    if( mIsCreate )
    {
	XMLNode req("disconnect");
	req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",id());
	mod->cntrIfCmd(req,"root");
    }
}

void VCASess::getReq( SSess &ses )
{
    //- Access time to session is updating -
    lst_ses_req = time(NULL);

    map< string, string >::iterator prmEl = ses.prm.find("com");
    string first_lev = TSYS::pathLev(ses.url,1);
    string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";
    if( wp_com.empty() )
    {
	string prjNm;
	//-- Get project's name --
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fobj%2fst%2fprj");
	if( !mod->cntrIfCmd(req,ses.user) )
	{
	    req.setAttr("path","/prj_"+req.text()+"/%2fobj%2fcfg%2fname");
	    if( !mod->cntrIfCmd(req,ses.user) )	prjNm = req.text();
	}

	ses.page = mod->pgHead("",prjNm)+"<SCRIPT>\n"+mod->VCAjs+"\n</SCRIPT>\n"+mod->pgTail();
	ses.page = mod->httpHead("200 OK",ses.page.size())+ses.page;
    }
    //- Session/projects icon -
    else if( wp_com == "ico" )
    {
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fico");
	mod->cntrIfCmd(req,ses.user);
	ses.page = TSYS::strDecode(req.text(),TSYS::base64);
	ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    }
    //- Get open pages list -
    else if( wp_com == "pgOpenList" && first_lev.empty() )
    {
	prmEl = ses.prm.find("tm");
	XMLNode req("openlist");
	req.setAttr("path",ses.url+"/%2fserv%2fpg")->
	    setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
    //- Close page command -
    //- Page and widget attributes request -
    else if( wp_com == "attrs" )
    {
	prmEl = ses.prm.find("tm");
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fserv%2fattr")->
	    setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	mod->cntrIfCmd(req,ses.user);
	if( objPresent(ses.url) ) objAt(ses.url).at().setAttrs(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
    //- Resources request (images and other files) -
    else if( wp_com == "res" )
    {
	prmEl = ses.prm.find("val");
	if( prmEl != ses.prm.end() )
	{
	    XMLNode req("get");
	    req.setAttr("path",ses.url+"/%2fwdg%2fres")->setAttr("id",prmEl->second);
	    mod->cntrIfCmd(req,ses.user);
	    ses.page = TSYS::strDecode(req.text(),TSYS::base64);
	    ses.page = mod->httpHead("200 OK",ses.page.size(),req.attr("mime"))+ses.page;
	} else ses.page = mod->httpHead("404 Not Found");
    }
    //- Page or widget child widgets request -
    else if( wp_com == "chlds" )
    {
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fwdg%2fcfg%2fpath")->setAttr("resLink","1");
	if( !mod->cntrIfCmd(req,ses.user) )
	{
	    req.clear()->setAttr("path",req.text()+"/%2finclwdg%2fwdg")->setAttr("chkUserPerm","1");
	    mod->cntrIfCmd(req,ses.user);
	}
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
    //- Widget root element identifier (primitive) -
    else if( wp_com == "root" )
    {
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fwdg%2fcfg%2froot");
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
    //- Request to primitive object. Used for data caching -
    else if( wp_com == "obj" )
    {
	if( !objPresent(ses.url) )
	{
	    //-- Request to widget type --
	    bool new_obj = false;
	    XMLNode req("get");
	    req.setAttr("path",ses.url+"/%2fwdg%2fcfg%2froot");
	    mod->cntrIfCmd(req,ses.user);
	    if( req.text() == "ElFigure" )	{ objAdd( new VCAElFigure(ses.url) ); new_obj = true; }
	    else if( req.text() == "Diagram" )	{ objAdd( new VCADiagram(ses.url) ); new_obj = true; }
	    if( new_obj )
	    {
		//-- Request new object's attributes --
		req.clear()->setAttr("path",ses.url+"/%2fserv%2fattr");
		mod->cntrIfCmd(req,ses.user);
		objAt(ses.url).at().setAttrs(req,ses.user);
	    }
	}
	if( objPresent(ses.url) ) objAt(ses.url).at().getReq(ses);
    }
    else
    {
	mess_warning(nodePath().c_str(),_("Unknown command: %s."),wp_com.c_str());
	ses.page = mod->pgHead()+"<center>Call page/widget '"+ses.url+"' command: '"+wp_com+"'</center>\n<br/>"+mod->pgTail();
	ses.page = mod->httpHead("200 OK",ses.page.size())+ses.page;
    }
}

void VCASess::postReq( SSess &ses )
{
    //- Commands process -
    map< string, string >::iterator cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //- Attributes set -
    if( wp_com == "attrs" )
    {
	XMLNode req("set");
	req.load(ses.content);
	req.setAttr("path",ses.url+"/%2fserv%2fattr");
	mod->cntrIfCmd(req,ses.user);
    }
    //- Open page command -
    else if( wp_com == "pgClose" || wp_com == "pgOpen" )
    {
	XMLNode req((wp_com=="pgOpen")?"open":"close");
	req.setAttr("path","/"+TSYS::pathLev(ses.url,0)+"/%2fserv%2fpg")->setAttr("pg",ses.url);
	mod->cntrIfCmd(req,ses.user);
    }
    else if( wp_com == "obj" && objPresent(ses.url) )
        objAt(ses.url).at().postReq(ses);
    ses.page = mod->httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
}

void VCASess::objAdd( VCAObj *obj )
{
    if( !obj )	return;
    if( objPresent(obj->id()) )	delete obj;
    else chldAdd( id_objs, obj );
}

//*************************************************
//* VCAObj                                        *
//*************************************************
VCAObj::VCAObj( const string &iid ) : m_id(iid) 
{

}

VCASess &VCAObj::owner( )
{
    return *(VCASess*)nodePrev( );
}


//*************************************************
//* ElFigure                                      *
//*************************************************
VCAElFigure::VCAElFigure( const string &iid ) : VCAObj(iid), im(NULL)
{

}

VCAElFigure::~VCAElFigure( )
{
    if( im ) gdImageDestroy(im);
}


#define SAME_SIGNS(a, b) ((a) * (b) >= 0)

// Line intersection algorithm, copied from Graphics Gems II
bool VCAElFigure::lineIntersect( double x1, double y1, double x2, double y2,
                                 double x3, double y3, double x4, double y4 )
{
    double a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    double r1, r2, r3, r4;         /* 'Sign' values */

    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;

    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;

    if ( r3 != 0 && r4 != 0 && SAME_SIGNS( r3, r4 ))
        return false;

    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;

    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;

    if ( r1 != 0 && r2 != 0 && SAME_SIGNS( r1, r2 ) )
        return false;

    return true;
}

/*Point VCAElFigure::lineIntersect_point( Point pt1, Point pt2, Point pt3, Point pt4 )
{
    double dx1 = ( pt2.x - pt1.x );
    double dx2 = ( pt4.x - pt3.x );
    double dy1 = ( pt2.y - pt1.y );
    double dy2 = ( pt4.y - pt3.y );
    Point isect;
     // For special case where one of the lines are vertical
    if ( dx1 == 0 && dx2 == 0) 
    {
        printf("No intersection\n");
        //type = NoIntersection;
    } else if ( dx1 == 0 ) 
    {
        double la = dy2 / dx2;
        isect = Point( pt1.x, la * pt1.x + pt3.y - la*pt3.x );
    } else if ( dx2 == 0 )
    {
        double ta = dy1 / dx1;
        isect = Point( pt3.x, ta * pt3.x + pt2.y - ta*pt2.x );
    } else {
        double ta = dy1 / dx1;
        double la = dy2 / dx2;
        if (ta == la) // no intersection
            printf("No Intersection\n");

        double x = ( - pt3.y + la * pt3.x + pt1.y - ta * pt1.x ) / (la - ta);
        isect = Point( x, ta*(x - pt1.x) + pt1.y );
    }
}*/

Point VCAElFigure::rotate( const Point pnt, double alpha )
{
    return Point( pnt.x*cos((alpha*M_PI)/180) - pnt.y*sin((alpha*M_PI)/180),
                  pnt.x*sin((alpha*M_PI)/180) + pnt.y*cos((alpha*M_PI)/180) );
}

//- unrotation of the point -
Point VCAElFigure::unrotate( const Point pnt, double alpha, double a, double b )
{
    return Point( (pnt.x-a)*cos((alpha*M_PI)/180) - (pnt.y-b)*sin((alpha*M_PI)/180),
                  -(pnt.x-a)*sin((alpha*M_PI)/180) - (pnt.y-b)*cos((alpha*M_PI)/180) );
}

//- getting the point of the arc, using t as parameter and a,b as radiuses -
Point VCAElFigure::arc( double t, double a, double b )
{
    return Point( a*cos(t*M_PI*2), -b*sin(t*M_PI*2) );
}

double VCAElFigure::ABS( double var )
{
    if( var < 0 ) return -var;
    else return var;
}

//- Computing the step in the bezier curve construction -
double VCAElFigure::bezierDeltaT( Point p1, Point p2, Point p3, Point p4 )
{
    double t = 0, dx_dt, dy_dt;
    double max = ABS( -3*pow((1-t),2)*p1.x-6*t*(1-t)*p2.x+6*t*p3.x+3*t*t*p4.x ); 
    do
    {
        dx_dt = ABS( -3*pow((1-t),2)*p1.x-6*t*(1-t)*p2.x+6*t*p3.x+3*t*t*p4.x );
        if( max < dx_dt ) max = dx_dt;
        dy_dt = ABS( -3*pow((1-t),2)*p1.y-6*t*(1-t)*p2.y+6*t*p3.y+3*t*t*p4.y );
        if(max < dy_dt) max = dy_dt;
        t += 0.1;
    }
    while(t < 1);
    return 1/max;
}

//- getting the point of the bezier curve, using t as parameter -
Point VCAElFigure::bezier(double t,Point p1,Point p2, Point p3, Point p4)
{
    return Point( pow((1-t),3)*p1.x+3*t*pow((1-t),2)*p2.x+3*t*t*(1-t)*p3.x+t*t*t*p4.x,
                  pow((1-t),3)*p1.y+3*t*pow((1-t),2)*p2.y+3*t*t*(1-t)*p3.y+t*t*t*p4.y );
}

//- Computing the angle between two lines -
double VCAElFigure::angle( const Point p1, const Point p2, const Point p3, const Point p4 )
{
    double dx1 = ( p2.x - p1.x );
    double dx2 = ( p4.x - p3.x );
    double dy1 = ( p2.y - p1.y );
    double dy2 = ( p4.y - p3.y );
    double cos_line = vmax( -1, vmin(1, (( dx1*dx2 + dy1*dy2 ) / ( length(p1,p2)*length(p3,p4)))) );
    double rad;
    rad = acos( cos_line );
    return rad * 180 / M_PI;
}

//- Computing the length between two points -
double VCAElFigure::length( const Point pt1, const Point pt2 )
{
    double x = pt2.x - pt1.x;
    double y = pt2.y - pt1.y;
    return sqrt( x*x + y*y );
}

//- Scaling and rotating the point -
Point VCAElFigure::scaleRotate( const Point point, double xScale, double yScale,  bool flag_scale, bool flag_rotate )
{
    Point rpnt = Point( point.x, point.y );
    Point center;
    if( flag_rotate )
    {
        if ( !flag_scale ) center = Point( TSYS::realRound( width*xScale/2 ), TSYS::realRound( height*yScale/2 ) );
        else center = Point( TSYS::realRound( width/2 ), TSYS::realRound( height/2 ) );
        rpnt.x = rpnt.x - center.x;
        rpnt.y = rpnt.y - center.y;
        rpnt = rotate( rpnt, orient);
        rpnt.x = rpnt.x + center.x;
        rpnt.y = rpnt.y + center.y;
    }
    if ( flag_scale ) rpnt = Point( rpnt.x*xScale, rpnt.y*yScale );
    
    return rpnt;
}

//- Unscaling and unrotating the point -
Point VCAElFigure::unscaleUnrotate( const Point point, double xScale, double yScale, bool flag_scale, bool flag_rotate )
{
    Point rpnt = Point( point.x, point.y );
    Point center;
    if( flag_scale ) rpnt = Point( rpnt.x/xScale, rpnt.y/yScale );
    if( flag_rotate )
    {
        if ( !flag_scale ) center = Point( TSYS::realRound( width*xScale/2 ), TSYS::realRound( height*yScale/ 2 ) );
        else center = Point( TSYS::realRound( width/2 ), TSYS::realRound( height/2 ) );
        rpnt.x = rpnt.x - center.x;
        rpnt.y = rpnt.y - center.y;
        rpnt = rotate( rpnt, 360 - orient );
        rpnt.x = rpnt.x + center.x;
        rpnt.y = rpnt.y + center.y;
    }
    return rpnt;
}

//- Painting existing inundationItems -
void VCAElFigure::paintFill( gdImagePtr im, Point pnt, InundationItem &in_item, int color )
{
    int fill_clr = gdImageColorAllocate( im, (ui8)(in_item.P_color>>16), (ui8)(in_item.P_color>>8), (ui8)in_item.P_color ); 
    in_item.index_color = fill_clr;
    gdImageFillToBorder( im, (int) TSYS::realRound(pnt.x), 
                             (int) TSYS::realRound(pnt.y), color,  fill_clr );
}

//- Detecting if any point of the figure is out of the borders of the image -
bool VCAElFigure::isPaintable( ShapeItem item, double xScale, double yScale )
{
    double s_width = width*xScale;
    double s_height = height*yScale;
    switch(item.type)
    {
        case 1:
            if(  (scaleRotate((pnts)[item.n1],xScale,yScale,true, true).x>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x<=(s_width)) &&
                 (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y<=(s_height)) &&
                 (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x<=(s_width)) &&
                 (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y<=(s_height))
              )
                return true;
            else
                return false;
        case 2:
            if(
               (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n5],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n5],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n5],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n5],xScale,yScale,true,true).y<=(s_height))
              )
                return true;
            else return false;
        case 3:
            if(
               (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y<=(s_height)) &&
               (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x>=0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x<=(s_width)) &&
               (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y>=0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y<=(s_height))
              )
                return true;
            else return false;
    }
    return false;
}

//- Drawing of dashed or dotted borders of the figure -
void VCAElFigure::dashDotFigureBorders( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, int clr_el_line, double el_width, double el_border_width, int type, double wdt, double wdt_1, double xScale, double yScale  )
{
    switch( type )
    {
        case 1:
        {
            double el_ang;
            Point un_p1, un_p2;
            if( el_p1.y <= el_p2.y ) el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            else el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            un_p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
            un_p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
            Point el_pb1, el_pb2;
            do
            {
                if( un_p1.x + wdt + 2*(el_width+2) <= un_p2.x )
                {
                    el_pb1 = Point ( el_p1.x + rotate( un_p1, el_ang ).x,  el_p1.y - rotate( un_p1, el_ang ).y );
                    el_pb2 = Point ( el_p1.x + rotate( Point( un_p1.x + wdt, un_p1.y ), el_ang ).x, 
                                     el_p1.y - rotate( Point ( un_p1.x + wdt, un_p1.y ), el_ang ).y ); 
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0),  Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, xScale, yScale  );
                    un_p1.x += wdt + 2*(el_width+2);
                }
                else
                {
                    el_pb1 = Point( el_p1.x + rotate( un_p1, el_ang ).x, el_p1.y - rotate( un_p1, el_ang ).y );
                    el_pb2 = Point ( el_p1.x + rotate( un_p2, el_ang ).x, el_p1.y - rotate( un_p2, el_ang ).y );
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0),  Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, xScale, yScale  );
                    break;
                }
            }
            while( true );
        }
            break;
        case 2:
        {
            double ang;
            if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            double arc_a = length( el_p5, el_p3 );
            double arc_b = length( el_p3, el_p4 );
            Point el_pb1, el_pb2, el_pb6;
            double t_start = el_p6.x;
            double t_end = el_p6.y;
            double len = length( Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x,
                                        el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y ),
                                 Point( el_p3.x + rotate( arc( t_start+0.00277777777778, arc_a, arc_b ), ang ).x,
                                        el_p3.y - rotate( arc( t_start+0.00277777777778, arc_a, arc_b ), ang ).y ) );
            int kol = (int)TSYS::realRound( wdt/len, 2, true);
            int kol_1 = (int)TSYS::realRound( wdt_1/len, 2, true);
            if( kol_1 < 1 ) kol_1 = 1;
            double t = t_start;
            do
            {
                if( t +  0.00277777777778*kol + 0.00277777777778*kol_1 <= t_end)
                {
                    el_pb1 = Point( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y  );
                    el_pb2 = Point( el_p3.x + rotate( arc( t + 0.00277777777778*kol, arc_a, arc_b ), ang ).x, el_p3.y - rotate( arc( t + 0.00277777777778*kol, arc_a, arc_b ), ang ).y  );
                    el_pb6 = Point ( t, t + 0.00277777777778*kol );
                    paintFigureBorders( im, el_pb1, el_pb2, el_p3, el_p4,  el_p5, el_pb6, clr_el, clr_el_line, el_width, el_border_width, 2, xScale, yScale );
                    t += 0.00277777777778*kol + 0.00277777777778*kol_1;
                }
                else
                {
                    el_pb1 = Point( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y  );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang ).x, el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y  );
                    el_pb6 = Point ( t, t_end );
                    paintFigureBorders( im, el_pb1, el_pb2, el_p3, el_p4,  el_p5, el_pb6, clr_el, clr_el_line, el_width, el_border_width, 2, xScale, yScale  );
                    break;
                }
            }
            while ( true );
        }
            break;
        case 3:
        {
            double delta = bezierDeltaT( el_p1, el_p3,
                                         el_p4, el_p2 );
            double len = length( Point( bezier( 0.0, el_p1, el_p3, el_p4, el_p2).x,
                                        bezier( 0.0, el_p1, el_p3, el_p4, el_p2).y ),
                                 Point( bezier( 0.0 + delta, el_p1, el_p3, el_p4, el_p2).x,
                                        bezier( 0.0 + delta, el_p1, el_p3, el_p4, el_p2).y ) );
            int kol = (int)TSYS::realRound( wdt/len, 2, true);
            int kol_1 = (int)TSYS::realRound( wdt_1/len, 2, true);
            if( kol_1 < 1 ) kol_1 = 1;

            double t = 0.0;
            do
            {
                if( t +  delta*kol + delta*kol_1 <= 1 )
                {
                    paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4,  Point(t,t + delta*kol), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, xScale, yScale );
                    t += delta*kol + delta*kol_1;
                }
                else
                {
                    paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4,  Point(t,1.0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, xScale, yScale );
                    break;
                }
            }
            while ( true );
        }
            break;
    }
}

//- Drawing the borders of the figure -
void VCAElFigure::paintFigureBorders( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, int clr_el_line, double el_width, double el_border_width, int type, double xScale, double yScale )
{
    double el_ang, x_center, y_center, t, t_start, t_end, ang, arc_a, arc_b, arc_a_small, arc_b_small, delta ;
    Point p_center, p1, p2, p3, p4, pb1, pb2 ;
    int s_width = (int) (width*xScale + 0.5);
    int s_height = (int) (height*yScale + 0.5);
    switch( type )
    {
        case 1:
            gdImageSetThickness( im, (int)(el_border_width*2) );
            if( el_p1.y <= el_p2.y ) el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            else el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            
            gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, - (el_width/2+el_border_width)), el_ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, 2, true ), clr_el );
            gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, ( el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).y, 2, true ), clr_el );
            gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, (el_width/2+el_border_width)), el_ang ).y, 2, true ), clr_el );
            gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, 2, true ), clr_el );
            x_center = (el_p1.x + el_p2.x)/2;
            y_center = (el_p1.y + el_p2.y)/2;
            if( (int)(x_center+0.5) < s_width && (int)(y_center+0.5) < s_height )
                gdImageFillToBorder( im, (int)(x_center+0.5), (int)(y_center+0.5), clr_el, clr_el_line );
            break;
        case 3:
            gdImageSetThickness( im, (int)(el_border_width) );
            delta = bezierDeltaT( el_p1, el_p3, el_p4, el_p2 );
            if( el_p1.y <= el_p2.y )
                el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            else
                el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
            p2 = unrotate( el_p3, el_ang, el_p1.x, el_p1.y );
            p3 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
            p4 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
            t_start = el_p5.x;
            t_end = el_p5.y;
            p_center = Point( (int)TSYS::realRound( el_p1.x + rotate( bezier( t_start + (t_end - t_start)/2, p1, p2, p3, p4 ), el_ang ).x, 2, true ),
                               (int)TSYS::realRound( el_p1.y  - rotate( bezier( t_start + (t_end - t_start)/2, p1, p2, p3, p4 ), el_ang ).y, 2, true ) );
            t = t_start;
            do
            {
                gdImageLine( im, (int)TSYS::realRound(el_p1.x + rotate( bezier( t,
                                 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                                 (int)TSYS::realRound(el_p1.y - rotate( bezier( t,
                                 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true ),
                                 (int)TSYS::realRound( el_p1.x + rotate( bezier( t+delta,
                                 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p1.y - rotate( bezier( t+delta,
                                 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                                 Point( p3.x,p3.y+(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true ), clr_el );
                t += delta;
            }
            while ( t < t_end );
            t = t_start;
            do
            {
                gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( bezier( t,
                                 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p1.y - rotate( bezier( t,
                                 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true ),
                                 (int)TSYS::realRound( el_p1.x + rotate( bezier( t+delta,
                                 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                                 (int)TSYS::realRound(el_p1.y - rotate( bezier( t+delta,
                                 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                                 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                                 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                                 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true ), clr_el );
                t += delta;
            }
            while ( t < t_end );
            pb1 = Point(
                    (int)TSYS::realRound( el_p1.x + rotate( bezier( t,
                    Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                    Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                    Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                    Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                    (int)TSYS::realRound(el_p1.y - rotate( bezier( t,
                    Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
                    Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
                    Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
                    Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true )
                  );
            pb2 = Point(
                    (int)TSYS::realRound( el_p1.x + rotate( bezier( t,
                    Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                    Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                    Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
                    Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, 2, true ),
                    (int)TSYS::realRound( el_p1.y - rotate( bezier( t,
                    Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
                    Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
                    Point( p3.x,p3.y+(el_width/2+el_border_width/2) ),
                    Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, 2, true )
                  );
            gdImageLine( im, (int)pb1.x, (int)pb1.y, (int)pb2.x, (int)pb2.y, clr_el );
            if( t_start > 0.0 || t_end < 1.0 )
                p1 = Point( bezier( t_start, p1, p2, p3, p4 ).x,
                            bezier( t_start, p1, p2, p3, p4 ).y );
            gdImageLine( im, (int)TSYS::realRound(el_p1.x +
                             rotate( Point( p1.x, p1.y+el_width/2+el_border_width/2 ), el_ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y -
                             rotate( Point( p1.x, p1.y+el_width/2+el_border_width/2 ), el_ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p1.x +
                             rotate( Point( p1.x, p1.y - (el_width/2+el_border_width/2) ), el_ang).x, 2, true ),
                             (int)TSYS::realRound( el_p1.y -
                             rotate( Point( p1.x, p1.y - (el_width/2+el_border_width/2) ), el_ang ).y, 2, true ), clr_el );
            if( (int)(p_center.x+0.5) < s_width && (int)(p_center.y+0.5) < s_height )
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el, clr_el_line );
            break;
        case 2:
            gdImageSetThickness( im, (int)(el_border_width) );
            arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width/2;
            arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width/2;
            arc_a_small = arc_a - el_width - el_border_width;
            arc_b_small = arc_b - el_width - el_border_width;
            if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            t_start = el_p6.x;
            t_end = el_p6.y;
            t = t_start;
            do
            {
                gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                 (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el );
                t += 0.00277777777778;
            }
            while ( t < t_end );
            gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang ).y, 2, true ),
                             (int)TSYS::realRound( el_p3.x + rotate( arc( t_end, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p3.y - rotate( arc( t_end, arc_a_small, arc_b_small ), ang ).y, 2, true ), clr_el );
            t = t_start;
            do
            {
                gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a_small, arc_b_small ), ang ).y, 2, true ),
                                 (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                 (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a_small, arc_b_small ), ang ).y, 2, true ),clr_el );
                t += 0.00277777777778;
            }
            while ( t < t_end );
            gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x, 2,
                             true ),
                             (int)TSYS::realRound( el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y, 2,
                              true ),
                             (int)TSYS::realRound( el_p3.x + rotate( arc( t_start, arc_a_small, arc_b_small ),
                              ang ).x, 2, true ),
                             (int)TSYS::realRound( el_p3.y - rotate( arc( t_start, arc_a_small, arc_b_small ),
                              ang ).y, 2, true ), clr_el );
            p_center = Point( TSYS::realRound( el_p3.x +rotate( arc( (t_end + t_start)/2, arc_a - el_width/2-1,                        arc_b - el_width/2-1 ), ang ).x, 2, true ),
                              TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a - el_width/2-1, arc_b - el_width/2-1 ), ang ).y, 2, true ) );
            if( (int)(p_center.x+0.5) < s_width && (int)(p_center.y+0.5) < s_height )
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el, clr_el_line );
            break;
    }
}

//- Drawing the dashed or dotted figure without borders -
void VCAElFigure::dashDot( gdImagePtr im, Point el_p1, Point el_p2, Point el_p3, Point el_p4, Point el_p5, Point el_p6, int  clr_el, double el_width, int type, int style )
{
    switch( type )
    {
        case 1:
        {
            Point p1, p2, un_p1, un_p2;
            int kol; 
            double el_ang;
            if( el_p1.y <= el_p2.y ) el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            else el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            un_p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
            un_p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
            gdImageSetThickness( im, (int)el_width );
            double wdt, wdt_1;
            if( style == 1 )
            {
                if( el_width < 3 )
                {
                    wdt = 5*el_width-1;
                    wdt_1 = el_width+1;
                }
                else
                {
                    wdt = 4*el_width-1;
                    wdt_1 = 2*el_width+1;
                }
            }
            else if( style == 2 )
            {
                if( el_width < 3 )
                {
                    wdt = 2*el_width-1;
                    wdt_1 = el_width+1;
                }
                else
                {
                    wdt = el_width-1;
                    wdt_1 = 2*el_width+1;
                }
            }
            do
            {
                if( un_p1.x + wdt + wdt_1 <= un_p2.x )
                {
                    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( un_p1, el_ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p1.y - rotate( un_p1, el_ang ).y, 2, true ),
                                     (int)TSYS::realRound( el_p1.x + rotate( Point( un_p1.x + wdt, un_p1.y ), el_ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p1.y - rotate( Point ( un_p1.x + wdt, un_p1.y ), el_ang ).y, 2, true ), clr_el );
                    un_p1.x += wdt + wdt_1;
                }
                else
                {
                    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( un_p1, el_ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p1.y - rotate( un_p1, el_ang ).y, 2, true ),
                                     (int)TSYS::realRound( el_p1.x + rotate( un_p2, el_ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p1.y - rotate( un_p2, el_ang ).y, 2, true ), clr_el );
                    break;
                }
            }
            while( true );
        }
            break;
        case 2:
        {
            double ang, arc_a, arc_b, t_start, t_end, t;
            if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
            arc_a = length( el_p5, el_p3 );
            arc_b = length( el_p3, el_p4 );
            t_start = el_p6.x;
            t_end = el_p6.y;
            gdImageSetThickness( im, (int)el_width );
            Point un_p1, un_p2, un_p3, un_p4, un_p5;
            double wdt, wdt_1;
            double len = length( Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x,
                                        el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y ),
                                 Point( el_p3.x + rotate( arc( t_start+0.00277777777778, arc_a, arc_b ), ang ).x,
                                        el_p3.y - rotate( arc( t_start+0.00277777777778, arc_a, arc_b ), ang ).y ) );
            if( style == 1 )
            {
                if( el_width < 3 )
                {
                    wdt = 5*el_width-1;
                    wdt_1 = el_width+1;
                }
                else
                {
                    wdt = 4*el_width-1;
                    wdt_1 = 2*el_width+1;
                }
            }
            else if( style == 2 )
            {
                if( el_width < 3 )
                {
                    wdt = 2*el_width;
                    wdt_1 = el_width+1;
                }
                else
                {
                    wdt = el_width-1;
                    wdt_1 = 2*el_width+1;
                }
            }
            int kol = (int)TSYS::realRound( wdt/len, 2, true);
            int kol_1 = (int)TSYS::realRound( wdt_1/len, 2, true);
            if( kol_1 < 1 ) kol_1 = 1;
            t = t_start;
            int calc=0;
            int calc_1=0;
            do
            {
                if( calc_1 == kol_1 ) { calc = 0; calc_1=0; }
                if( calc < kol )
                {
                    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                     (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                     (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el );
                    calc++;
                }
                else 
                {
                    calc_1++;
                }
                t += 0.00277777777778;
            }
            while ( t < t_end );
            break;
        }
        case 3:
        {
            double delta = bezierDeltaT( el_p1, el_p3, el_p4, el_p2 );
            Point un_p1, un_p2, un_p3, un_p4, un_p5;
            double wdt, wdt_1, t;
            double len = length( Point(bezier( 0.0, el_p1, el_p3, el_p4, el_p2 ).x,
                                       bezier( 0.0, el_p1, el_p3, el_p4, el_p2 ).y ),
                                 Point(bezier( 0.0+delta, el_p1, el_p3, el_p4, el_p2 ).x,
                                       bezier( 0.0+delta, el_p1, el_p3, el_p4, el_p2 ).y ) );
            if( style == 1 )
            {
                wdt = 4*el_width-1;
                wdt_1 = 2*el_width;
            }
            else if( style == 2 )
            {
                wdt_1 = 2*el_width;
                wdt = el_width;
            }
            int kol = (int)TSYS::realRound( wdt/len, 2, true);
            int kol_1 = (int)TSYS::realRound( wdt_1/len, 2, true);
            if( kol_1 < 1 ) kol_1 = 1;
            t = 0;
            int calc=0;
            int calc_1=0;
            gdImageSetThickness( im, (int)el_width );
            do
            {
                if( calc_1 == kol_1 ) { calc = 0; calc_1=0; }
                if( calc < kol )
                {
                    gdImageLine( im, (int)TSYS::realRound( bezier( t, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                     (int)TSYS::realRound( bezier( t, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ),
                                     (int)TSYS::realRound( bezier( t+delta, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                     (int)TSYS::realRound( bezier( t+delta, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ), clr_el );
                    calc++;
                }
                else 
                {
                    calc_1++;
                }
                t += delta;
            }
            while ( t < 1 );

        }
            break;
    }
}

//- Painting existing figures -
void VCAElFigure::paintFigure( gdImagePtr im, ShapeItem item, double xScale, double yScale, bool flag_allocate, bool flag_style, bool flag_clr_ln )
{
    int clr_el, clr_el_line;
    double t;
    double arc_a, arc_b, t_start, t_end,arc_a_small,arc_b_small, ang;
    double el_width, el_border_width;
    //-- Arc --
    if ( item.type == 2 )
        if( isPaintable( item, xScale, yScale ) )
            if( item.border_width == 0 )//--- Drawing the arc with borders' width == 0 ---
            {
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                Point el_p5 = scaleRotate( (pnts)[item.n5], xScale, yScale, true, true );
                Point el_p6 = item.ctrlPos4;
                if ( el_p5.y <= el_p3.y ) 
                    ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                arc_a = length( el_p5, el_p3 );
                arc_b = length( el_p3, el_p4 );
                t_start = item.ctrlPos4.x;
                t_end = item.ctrlPos4.y;
                if( flag_allocate )
                    clr_el = gdImageColorResolve( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
                else clr_el = item.lineColor;
                if( item.style != 0 && flag_style )//---- Drawing the dashed or dotted arc with borders' width == 0 ----
                    dashDot( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6, clr_el, item.width, 2, item.style );
                else//---- Drawing the solid arc with borders' width == 0 ----
                {
                    gdImageSetThickness( im, item.width );

                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el );
                        t += 0.00277777777778;
                    }
                    while ( t < t_end );
                }
                (pnts)[item.n1] = unscaleUnrotate( Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x,
                el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n2] = unscaleUnrotate( Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n4] = unscaleUnrotate( Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n5] = unscaleUnrotate( Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
            }
            else//--- Drawing the arc with borders' width > 0 ---
            {
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                Point el_p5 = scaleRotate( (pnts)[item.n5], xScale, yScale, true, true );
                Point el_p6 = item.ctrlPos4;
                el_width = item.width;
                el_border_width = item.border_width;
                gdImageSetThickness( im, item.border_width );
                if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                clr_el_line = gdImageColorAllocate( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
                if( flag_clr_ln ) clr_el = clr_el_line;
                else if( flag_allocate )
                    clr_el = gdImageColorAllocate( im, (ui8)(item.borderColor>>16), (ui8)(item.borderColor>>8), (ui8)item.borderColor );
                    else
                    {
                        clr_el = item.borderColor;
                        if( item.flag_brd ) clr_el_line = clr_el;
                    }
                if( item.border_width < 4 && item.style != 0 && item.flag_brd && flag_style )//---- Drawing the dashed or dotted arc with borders' width < 4 and flag_brd ----
                {
                    double wdt, wdt_1;
                    if( item.style == 1 )
                    {
                        wdt = 4*(item.width+2)-1; 
                        wdt_1 = 2*(item.width+2);
                    }
                    else if( item.style == 2 )
                    {
                        wdt = (item.width+2)-1;
                        wdt_1 = 2*(item.width+2);
                    }
                    dashDotFigureBorders( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6,  clr_el, clr_el_line, el_width, el_border_width, 2, wdt, wdt_1, xScale, yScale  );
                }
                if( item.border_width < 4 && item.style != 0 && !item.flag_brd && flag_style )//---- Drawing the dashed or dotted arc with borders' width < 4 and !flag_brd ----
                {
                    Point el_pb1, el_pb2, el_pb3, el_pb4, el_pb5;
                    t_start = item.ctrlPos4.x;
                    t_end = item.ctrlPos4.y;
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                    
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    el_pb3 = Point( el_p3.x, el_p3.y );
                    el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    dashDot( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, item.ctrlPos4, clr_el, item.border_width, 2, item.style );
                    
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    el_pb3 = Point( el_p3.x, el_p3.y );
                    el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    dashDot( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, item.ctrlPos4, clr_el, item.border_width, 2, item.style );
                }
                if( item.border_width < 4 && (item.style == 0 || !flag_style) )//---- Drawing the solid arc with borders' width <4 ----
                    paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6, clr_el, clr_el_line, el_width, el_border_width, 2, xScale, yScale  );
                //---- Drawing the dashed or dotted border of the arc with borders' width >= 4 ----//
                if( item.border_width > 4 && item.style != 0 && flag_style )
                {
                    double el_ang;
                    Point p1, p2, un_p1, un_p2;
                    double wdt, wdt_1, len;
                    int kol, kol_1;
                    if( item.style == 1 )
                    {
                        wdt = 4*item.border_width-1; 
                        wdt_1 = 2*item.border_width;
                    }
                    else if( item.style == 2 )
                    {
                        wdt = item.border_width-1;
                        wdt_1 = 2*item.border_width;
                    }
                    t_start = item.ctrlPos4.x;
                    t_end = item.ctrlPos4.y;
                    //----- Drawing the first line connecting two arcs -----
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    p1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    p2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    //----- Drawing the second line connecting two arcs -----
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    p1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    p2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    //----- Drawing the big arc -----
                    Point el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6;
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    el_pb3 = Point( el_p3.x, el_p3.y );
                    el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    el_pb6 = Point( t_start, t_end);
                    dashDotFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6,  clr_el, clr_el, el_border_width-2, 1, 2, wdt, wdt_1, xScale, yScale  );
                    //----- Drawing the small arc -----
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    el_pb3 = Point( el_p3.x, el_p3.y );
                    el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    el_pb6 = Point( t_start, t_end);
                    dashDotFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6,  clr_el, clr_el, el_border_width-2, 1, 2, wdt, wdt_1, xScale, yScale  );
                }
                //---- Drawing the solid arc with borders' width >=4 ----
                if( item.border_width >= 4 && ( item.style == 0 || !flag_style ) )
                {
                    if( flag_clr_ln ) el_border_width = el_border_width/2;
                    t_start = el_p6.x;
                    t_end = el_p6.y; 
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    Point el_pb1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    Point el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    Point el_pb3 = Point( el_p3.x, el_p3.y );
                    Point el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    Point el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    Point el_pb6 = Point( t_start, t_end);
                    paintFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6, clr_el, clr_el, el_border_width-2, 1, 2, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width/2;
                    el_pb1 = Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang  ).y );
                    el_pb3 = Point( el_p3.x, el_p3.y );
                    el_pb4 = Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang  ).y );
                    el_pb5 = Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang  ).y );
                    el_pb6 = Point( t_start, t_end);
                    paintFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6, clr_el, clr_el, el_border_width-2, 1, 2, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 );
                    arc_b = length( el_p3, el_p4 );
                    Point p_center = Point( TSYS::realRound( el_p3.x +
                                            rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, 2, true ),
                                            TSYS::realRound( el_p3.y -
                                            rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, 2, true ) );
                    gdImageFillToBorder( im, (int)TSYS::realRound( p_center.x ), (int)TSYS::realRound( p_center.y ), clr_el, clr_el_line );
                    
                }
                //---- Recalculating the points of the arc to make them really belonging to the arc ----
                el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                el_p5 = scaleRotate( (pnts)[item.n5], xScale, yScale, true, true );
                el_p6 = item.ctrlPos4;
                if ( el_p5.y <= el_p3.y ) 
                    ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                arc_a = length( el_p5, el_p3 );
                arc_b = length( el_p3, el_p4 );
                t_start = item.ctrlPos4.x;
                t_end = item.ctrlPos4.y;
                (pnts)[item.n1] = unscaleUnrotate( Point( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n2] = unscaleUnrotate( Point( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n4] = unscaleUnrotate( Point( el_p3.x + rotate( arc( 0.25, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( 0.25, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
                (pnts)[item.n5] = unscaleUnrotate( Point( el_p3.x + rotate( arc( 0, arc_a, arc_b ), ang ).x,
                 el_p3.y - rotate( arc( 0, arc_a, arc_b ), ang ).y ), xScale, yScale, true, true );
            }
    //-- bezier curve --
    if ( item.type == 3)
        if( isPaintable( item, xScale, yScale ) )
        //--- Drawing the bezier curve with borders' width == 0 ---
        if( item.border_width == 0 )
        {
            if( flag_allocate )
                clr_el = gdImageColorResolve( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
            else clr_el = item.lineColor;
            t_start = 0;
            t_end = 1;
            //---- Drawing the dashed or dotted bezier curve with borders' width == 0 ----
            if( item.style != 0 && flag_style )
            {
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                dashDot( im, el_p1, el_p2, el_p3, el_p4, Point(0,0), Point(0,0), clr_el, item.width, 3, item.style );
            }
            else//---- Drawing the solid bezier curve with borders' width == 0 ----
            {
                gdImageSetThickness( im, item.width );
                double delta = bezierDeltaT( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ), scaleRotate( (pnts)[item.n3], xScale, yScale, true, true ),
                                             scaleRotate( (pnts)[item.n4], xScale, yScale, true, true ), scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ) );
                t = t_start;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound( bezier(t,scaleRotate((pnts)[item.n1],xScale,yScale,true, true),
                    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n4],xScale,yScale,true, true),
                    scaleRotate((pnts)[item.n2],xScale,yScale,true, true)).x, 2, true ),
                    (int)TSYS::realRound(bezier(t,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).y, 2, true ),
                    (int)TSYS::realRound(bezier(t+delta,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).x, 2, true ),
                    (int)TSYS::realRound(bezier(t+delta,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
                    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).y, 2, true ),clr_el );
                    t += delta;
                }
                while ( t < t_end );
            }
        }
        else//---- Drawing the bezier curve wit borders' width > 0 ----
        {
            double el_width = item.width;
            double el_border_width = item.border_width;
            double el_ang, ang;
            Point un_p1, un_p2;
            Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
            Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
            Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
            Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
            double delta = bezierDeltaT( el_p1, el_p3, el_p4, el_p2 );
            if( el_p1.y <= el_p2.y )
                el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            else
                el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
            clr_el_line = gdImageColorAllocate( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
            if( flag_clr_ln ) clr_el = clr_el_line;
            else if( flag_allocate ) clr_el = gdImageColorAllocate( im, (ui8)(item.borderColor>>16), (ui8)(item.borderColor>>8), (ui8)item.borderColor );
            else 
            {
                clr_el = item.borderColor;
                if( item.flag_brd ) clr_el_line = clr_el;
            }
            if( item.border_width < 4 && item.style != 0 && item.flag_brd && flag_style )//---- Drawing the dashed or dotted bezier curve with borders' width < 4 and with flag_brd ----
            {
                double wdt, wdt_1;
                if( item.style == 1 )
                {
                    wdt = 4*(item.width+2)-1; 
                    wdt_1 = 2*(item.width+2);
                }
                else if( item.style == 2 )
                {
                    wdt = (item.width+2)-1; 
                    wdt_1 = 2*(item.width+2);
                }
                dashDotFigureBorders( im, el_p1, el_p2, el_p3, el_p4, Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, wdt, wdt_1, xScale, yScale  );
            }
            if( item.border_width < 4 && item.style != 0 && !item.flag_brd && flag_style )//---- Drawing the dashed or dotted bezier curve with borders' width < 4 and without flag_brd ----
            {
                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                Point el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                Point el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, item.border_width, 3, item.style );
                
                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, item.border_width, 3, item.style );
                
                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                
                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
            }
            if( item.border_width < 4 && ( item.style == 0 || !flag_style ) )//----- Drawing the solid bezier curve with borders' width < 4 -----
                paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4, Point(0.0,1.0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, xScale, yScale  );
            if( item.border_width >=4 && item.style != 0 && flag_style )//----- Drawing the dashed or dotted bezier curve with borders' width >= 4 -----
            {
                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                //------ Drawing the first bezier curve ------
                Point el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                Point el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                double wdt, wdt_1;
                if( item.style == 1 )
                {
                    wdt = 4*item.border_width-1; 
                    wdt_1 = 2*item.border_width;
                }
                else if( item.style == 2 )
                {
                    wdt = item.border_width-1; 
                    wdt_1 = 2*item.border_width;
                }
                dashDotFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, wdt, wdt_1, xScale, yScale  );
                //------ Drawing the second bezier curve ------
                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                dashDotFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3,  wdt, wdt_1, xScale, yScale   );
                //------ Drawing the first line, connecting two curves ------
                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).y );
                dashDotFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                //------ Drawing the second line, connecting two curves ------
                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                dashDotFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
            }
            if( item.border_width >=4 && ( item.style == 0 || !flag_style ) )//----- Drawing the solid bezier curve with borders' width >= 4 -----
            {
                if( flag_clr_ln ) el_border_width = el_border_width/2;
                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                Point el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                Point el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                Point el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                paintFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0.0,1.0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                      el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                paintFigureBorders( im, el_pb1, el_pb2, el_pb3, el_pb4, Point(0.0,1.0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1),                el_ang ).x, el_p1.y - rotate( Point( p1.x-el_border_width/2,el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1) ), el_ang ).y );
                paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale );

                Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, 2, true ) );
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el, clr_el_line );
            }
        }
    //-- Line --
    if( item.type == 1 )
        if( isPaintable( item, xScale, yScale ) )
            //--- Drawing the line with borders' width == 0 ---
            if( item.border_width == 0 )
            {
                if( flag_allocate )
                    clr_el = gdImageColorResolve( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
                else clr_el = item.lineColor;
                gdImageSetThickness( im, item.width );
                //---- Drawing the dashed line with borders' width == 0 ----
                if( item.style != 0 && flag_style )
                {
                    Point el_p1 = Point( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ) );
                    Point el_p2 = Point( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ) );
                    dashDot( im, el_p1, el_p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.width, 1, item.style );
                }
                else//---- Drawing the solid line with borders' width == 0 ----
                {
                    gdImageLine( im, (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x, 2, true ),
                    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y, 2, true ),
                    (int)TSYS::realRound( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x, 2, true ),
                    (int)TSYS::realRound( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y, 2, true ),    clr_el );
                }
            }
            else//--- Drawing the line with borders' width > 0 ---
            {
                clr_el_line = gdImageColorAllocate( im, (ui8)(item.lineColor>>16), (ui8)(item.lineColor>>8), (ui8)item.lineColor );
                if( flag_clr_ln ) clr_el = clr_el_line; 
                else if( flag_allocate ) clr_el = gdImageColorAllocate( im, (ui8)(item.borderColor>>16), (ui8)(item.borderColor>>8), (ui8)item.borderColor );
                else 
                {
                    clr_el = item.borderColor;
                    if( item.flag_brd )
                        clr_el_line = clr_el;
                }
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                double el_border_width = (double)item.border_width/2;
                double el_width = item.width;
                if( item.border_width < 4 && item.style != 0 && item.flag_brd && flag_style )//---- Drawing the dashed or dotted line with borders' width == 1(for lines with width > 3) ----
                {
                    double wdt; 
                    if( item.style == 1 )
                        wdt = 4*(item.width+2)-1;
                    else if( item.style == 2 )
                        wdt = item.width+1;
                    dashDotFigureBorders( im, el_p1, el_p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, wdt, 0.0, xScale, yScale  );
                }
                if( item.border_width < 4 && item.style != 0 && !item.flag_brd  && flag_style)//---- Drawing the dashed or dotted borders of the line (for borders with width < 4) ----
                {
                    double el_ang;
                    Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                    Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    double el_width = item.width;
                    double el_border_width = item.border_width;
                    Point el_pb1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    Point el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );

                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                    
                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );
                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                    
                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );                                              
                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                    
                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );                    
                    dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );
                }
                if(  item.border_width < 4  && ( item.style == 0 || !flag_style ) )//----- Drawing the solid line with borders' width == 1(for lines with width > 3) -----
                    paintFigureBorders( im, el_p1, el_p2, Point(0,0), Point(0,0),  Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, xScale, yScale  );
                if( item.border_width >= 4 && item.style != 0 && flag_style )//---- Drawing the dashed or dotted line with the borders' width >= 4 ----
                {
                    double el_ang;
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    double ang;
                    Point p1, p2, un_p1, un_p2;
                    int kol; 
                    p1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );
                    double wdt;
                    if( item.style == 1)
                        wdt = 4*item.border_width-1;
                    else if( item.style == 2 )
                        wdt = item.border_width-1;
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                        
                    p1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                        
                    p1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    p2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );
                    
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    p1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );
                    dashDotFigureBorders( im, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                }
                if( item.border_width >= 4 && ( item.style == 0 || !flag_style ) )//----- Drawing the solid borders of the line -----
                {
                    double el_ang;
                    if( flag_clr_ln )
                        el_border_width = el_border_width/2;
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    Point el_pb1, el_pb2;
                    el_pb1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );
                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );
                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );                                              
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );
                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );                    
                    paintFigureBorders( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );
                    double x_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x)/2;
                    double y_center=(scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y)/2;
                    gdImageFillToBorder( im, (int)(x_center+0.5), (int)(y_center+0.5), clr_el, clr_el_line);
                }
            }
}

void VCAElFigure::getReq( SSess &ses )
{
    ResAlloc res(mRes,true);

    //- Prepare picture -
    vector<int> shape_temp;
    vector<int> line_color_shape;
    vector<int> border_color_shape;
    vector<int> width_shape;
    vector<int> border_width_shape;
    map< string, string >::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    if( im ) gdImageDestroy(im);
    im = gdImageCreate( (int)TSYS::realRound(width*xSc), (int)TSYS::realRound(height*ySc) );
    gdImageFilledRectangle( im, 0, 0, (int)TSYS::realRound(width*xSc-1), (int)TSYS::realRound(height*ySc-1), gdImageColorAllocateAlpha(im,0,0,0,127) );
    double scale;
    double el_width;
    double border_width;
    bool flag_min;
    int s_width = (int) (width*xSc + 0.5);
    int s_height = (int) (height*ySc + 0.5);
    if( xSc < ySc ) scale = xSc;
    else scale = ySc;
    if( scale != 1.0 )
        for( int i=0; i<shapeItems.size(); i++ )
        {
            if( !shapeItems[i].flag_brd && shapeItems[i].border_width > 0 )
            {
                border_width = shapeItems[i].border_width;
                border_width = border_width*scale;
                shapeItems[i].border_width = (int)TSYS::realRound(border_width);
                if( shapeItems[i].border_width < 1 ) shapeItems[i].border_width = 1;
            }
            
            flag_min = false;
            if( shapeItems[i].flag_brd ) shapeItems[i].width += 2;
            el_width = shapeItems[i].width;
            el_width = el_width*scale;
            shapeItems[i].width = (int)TSYS::realRound(el_width);
            if( shapeItems[i].width > 3 && shapeItems[i].flag_brd )
            {
                shapeItems[i].width -= 2;
                flag_min = true;
            }
            if( shapeItems[i].width < 1 ) shapeItems[i].width = 1;
            if( shapeItems[i].width <= 3 && shapeItems[i].flag_brd )
            {
                if( flag_min ) shapeItems[i].width += 2;
                shapeItems[i].border_width = 0;
                shapeItems[i].flag_brd = false;
            }
        }
    for(int i = 0; i < inundationItems.size(); i++ )
    {
        //- Detecting which figures correspond the points of each fill -
        bool flag_fill = true;
        int num_pnt;
        int min_x, min_y, max_x, max_y;
        vector<int> fig;
        vector<int> point_num;
        double delta;
        double W1,W2;
        W1 = 0; W2 = 0;
        Point delta_point_1, delta_point_2, delta_point_center;
        int count_min_x,count_min_y,count_max_x,count_max_y;
        shape_temp.clear();
        line_color_shape.clear();
        border_color_shape.clear();
        width_shape.clear();
        border_width_shape.clear();
        int tmp_clr=gdImageColorAllocate(im,(ui8)(inundationItems[i].P_color>>16), (ui8)(inundationItems[i].P_color>>8), 
                                             (ui8)inundationItems[i].P_color);
        if( inundationItems[i].number_point.size() == 2 )
        {
            for( int k = 0; k < shapeItems.size(); k++ )
                if( ((inundationItems[i].number_point[1] == shapeItems[k].n1) && (inundationItems[i].number_point[0] == shapeItems[k].n2)) ||
                    ((inundationItems[i].number_point[1] == shapeItems[k].n2) && (inundationItems[i].number_point[0] == shapeItems[k].n1)))
                {
                    shape_temp.push_back(k);
                    width_shape.push_back( shapeItems[k].width );
                    border_width_shape.push_back( shapeItems[k].border_width );
                    line_color_shape.push_back( shapeItems[k].lineColor );
                    if( shapeItems[k].border_width != 0 ) border_color_shape.push_back( shapeItems[k].borderColor );
                    else border_color_shape.push_back( shapeItems[k].lineColor );
                }
        }
        else
        {
            for( int j = 0; j < inundationItems[i].number_point.size(); j++ )
                for( int k = 0; k < shapeItems.size(); k++ )
                    if( ((inundationItems[i].number_point[j] == shapeItems[k].n1) && (inundationItems[i].number_point[j+1] == shapeItems[k].n2)) ||
                        ((inundationItems[i].number_point[j] == shapeItems[k].n2) && (inundationItems[i].number_point[j+1] == shapeItems[k].n1)) )
                    {
                        shape_temp.push_back(k);
                        width_shape.push_back( shapeItems[k].width );
                        border_width_shape.push_back( shapeItems[k].border_width );
                        line_color_shape.push_back( shapeItems[k].lineColor );
                        if( shapeItems[k].border_width != 0 ) border_color_shape.push_back( shapeItems[k].borderColor );
                        else border_color_shape.push_back( shapeItems[k].lineColor );
                    }
            for( int k = 0; k < shapeItems.size(); k++ )
                if( ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n1) && (inundationItems[i].number_point[0] == shapeItems[k].n2)) ||
                    ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n2) && (inundationItems[i].number_point[0] == shapeItems[k].n1)) )
                {
                    shape_temp.push_back(k);
                    width_shape.push_back( shapeItems[k].width );
                    border_width_shape.push_back( shapeItems[k].border_width );
                    line_color_shape.push_back( shapeItems[k].lineColor );
                    if( shapeItems[k].border_width != 0 ) border_color_shape.push_back( shapeItems[k].borderColor );
                    else border_color_shape.push_back( shapeItems[k].lineColor );
                }
        }
        //- Changing the color of the figure for the same for all figures from which the each fill is consist and painting them -
        for(int j = 0; j < shape_temp.size(); j++ )
            if( !isPaintable( shapeItems[shape_temp[j]], xSc, ySc ) )
            {
                flag_fill = false;
                break;
            }
        if( flag_fill )
        {
        for( int j = 0; j < shape_temp.size(); j++ )
            if( shapeItems[shape_temp[j]].type == 2 )
            {
                shapeItems[shape_temp[j]].width = 1;
                shapeItems[shape_temp[j]].border_width = 0;
                shapeItems[shape_temp[j]].lineColor = tmp_clr;
                paintFigure( im, shapeItems[shape_temp[j]], xSc, ySc, false, false, false );
            }
        for( int j = 0; j < shape_temp.size(); j++ )
            if( shapeItems[shape_temp[j]].type != 2 )
            {
                shapeItems[shape_temp[j]].width = 1;
                shapeItems[shape_temp[j]].border_width = 0;
                shapeItems[shape_temp[j]].lineColor = tmp_clr;
                paintFigure( im, shapeItems[shape_temp[j]], xSc, ySc, false, false, false );
            }
            if ( inundationItems[i].imgFill.size() )
            {

                Point el1_temp = scaleRotate( (pnts)[shapeItems[shape_temp[0]].n1], xSc, ySc, true, true );
                
                double xMax = unscaleUnrotate( el1_temp, xSc, ySc, false, true ).x;
                double xMin = unscaleUnrotate( el1_temp, xSc, ySc, false, true ).x;
                double yMax = unscaleUnrotate( el1_temp, xSc, ySc, false, true ).y;
                double yMin = unscaleUnrotate( el1_temp, xSc, ySc, false, true ).y;
                
                double xMax_rot = scaleRotate( (pnts)[shapeItems[shape_temp[0]].n1], xSc, ySc, true, true ).x;
                double xMin_rot = scaleRotate( (pnts)[shapeItems[shape_temp[0]].n1], xSc, ySc, true, true ).x;
                double yMax_rot = scaleRotate( (pnts)[shapeItems[shape_temp[0]].n1], xSc, ySc, true, true ).y;
                double yMin_rot = scaleRotate( (pnts)[shapeItems[shape_temp[0]].n1], xSc, ySc, true, true ).y;

                double t, arc_a, arc_b, t_start, t_end, ang, arc_a_rot, arc_b_rot, ang_rot;
                Point pnt_, pnt_rot, pnt_temp;

                for ( int j=0; j<shape_temp.size(); j++ )
                {
                    ShapeItem item = shapeItems[shape_temp[j]] ;
                    if( item.type == 1 )
                    {
                        pnt_temp.x = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x;
                        pnt_temp.y = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y;

                        pnt_.x = unscaleUnrotate( pnt_temp, xSc, ySc, false, true ).x;
                        pnt_.y = unscaleUnrotate( pnt_temp, xSc, ySc, false, true ).y;
                        
                        if( pnt_.x < xMin ) xMin = pnt_.x;
                        if( pnt_.x > xMax ) xMax = pnt_.x;
                        if( pnt_.y < yMin ) yMin = pnt_.y;
                        if( pnt_.y > yMax ) yMax = pnt_.y;
                        
                        pnt_temp.x = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ).x;
                        pnt_temp.y = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ).y;

                        pnt_.x = unscaleUnrotate( pnt_temp, xSc, ySc, false, true ).x;
                        pnt_.y = unscaleUnrotate( pnt_temp, xSc, ySc, false, true ).y;
                        
                        if( pnt_.x < xMin ) xMin = pnt_.x;
                        if( pnt_.x > xMax ) xMax = pnt_.x;
                        if( pnt_.y < yMin ) yMin = pnt_.y;
                        if( pnt_.y > yMax ) yMax = pnt_.y;
                        
                        pnt_rot.x = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x;
                        pnt_rot.y = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y;

                        if( pnt_rot.x < xMin_rot ) xMin_rot = pnt_rot.x;
                        if( pnt_rot.x > xMax_rot ) xMax_rot = pnt_rot.x;
                        if( pnt_rot.y < yMin_rot ) yMin_rot = pnt_rot.y;
                        if( pnt_rot.y > yMax_rot ) yMax_rot = pnt_rot.y;
                        
                        pnt_rot.x = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ).x;
                        pnt_rot.y = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ).y;

                        if( pnt_rot.x < xMin_rot ) xMin_rot = pnt_rot.x;
                        if( pnt_rot.x > xMax_rot ) xMax_rot = pnt_rot.x;
                        if( pnt_rot.y < yMin_rot ) yMin_rot = pnt_rot.y;
                        if( pnt_rot.y > yMax_rot ) yMax_rot = pnt_rot.y;
                    }
                    if( item.type == 2 )
                    {
                        Point el_p1_rot = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true );
                        Point el_p2_rot = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true );
                        Point el_p3_rot = scaleRotate( (pnts)[item.n3], xSc, ySc, true, true );
                        Point el_p4_rot = scaleRotate( (pnts)[item.n4], xSc, ySc, true, true );
                        Point el_p5_rot = scaleRotate( (pnts)[item.n5], xSc, ySc, true, true );
                        
                        Point el_p1 = unscaleUnrotate( el_p1_rot, xSc, ySc, false, true );
                        Point el_p2 = unscaleUnrotate( el_p2_rot, xSc, ySc, false, true );
                        Point el_p3 = unscaleUnrotate( el_p3_rot, xSc, ySc, false, true );
                        Point el_p4 = unscaleUnrotate( el_p4_rot, xSc, ySc, false, true );
                        Point el_p5 = unscaleUnrotate( el_p5_rot, xSc, ySc, false, true );
                        Point el_p6 = item.ctrlPos4;
                        
                        if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        
                        if ( el_p5_rot.y <= el_p3_rot.y ) ang_rot = angle( el_p3_rot, el_p5_rot, el_p3_rot, Point( el_p3_rot.x+10, el_p3_rot.y ) );
                        else ang_rot = 360 - angle( el_p3_rot, el_p5_rot, el_p3_rot, Point( el_p3_rot.x+10, el_p3_rot.y ) );
                        
                        arc_a = length( el_p5, el_p3 );
                        arc_b = length( el_p3, el_p4 );
                        
                        arc_a_rot = length( el_p5_rot, el_p3_rot );
                        arc_b_rot = length( el_p3_rot, el_p4_rot );
                        t_start = item.ctrlPos4.x;
                        t_end = item.ctrlPos4.y;
                        t = t_start;
                        do
                        {
                            pnt_.x = el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x;
                            pnt_.y = el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y;
                            
                            pnt_rot.x = el_p3_rot.x + rotate( arc( t, arc_a_rot, arc_b_rot ), ang_rot ).x;
                            pnt_rot.y = el_p3_rot.y - rotate( arc( t, arc_a_rot, arc_b_rot ), ang_rot ).y;

                           if( pnt_.x < xMin ) xMin = pnt_.x;
                           if( pnt_.x > xMax ) xMax = pnt_.x;
                           if( pnt_.y < yMin ) yMin = pnt_.y;
                           if( pnt_.y > yMax ) yMax = pnt_.y;
                           
                           if( pnt_rot.x < xMin_rot ) xMin_rot = pnt_rot.x;
                           if( pnt_rot.x > xMax_rot ) xMax_rot = pnt_rot.x;
                           if( pnt_rot.y < yMin_rot ) yMin_rot = pnt_rot.y;
                           if( pnt_rot.y > yMax_rot ) yMax_rot = pnt_rot.y;
                           t += 0.00277777777778;
                        }
                        while ( t < t_end );
                    }
                    if( item.type == 3 )
                    {
                        t = 0;
                        do
                        {
                            Point el_p1_rot = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true );
                            Point el_p2_rot = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true );
                            Point el_p3_rot = scaleRotate( (pnts)[item.n3], xSc, ySc, true, true );
                            Point el_p4_rot = scaleRotate( (pnts)[item.n4], xSc, ySc, true, true );
                            
                            Point el_p1 = unscaleUnrotate( el_p1_rot, xSc, ySc, false, true );
                            Point el_p2 = unscaleUnrotate( el_p2_rot, xSc, ySc, false, true );
                            Point el_p3 = unscaleUnrotate( el_p3_rot, xSc, ySc, false, true );
                            Point el_p4 = unscaleUnrotate( el_p4_rot, xSc, ySc, false, true );
                        
                            pnt_.x = bezier( t, el_p1, el_p3, el_p4, el_p2 ).x;
                            pnt_.x = bezier( t, el_p1, el_p3, el_p4, el_p2 ).y;
                            
                            pnt_rot.x = bezier( t, el_p1_rot, el_p3_rot, el_p4_rot, el_p2_rot ).x;
                            pnt_rot.x = bezier( t, el_p1_rot, el_p3_rot, el_p4_rot, el_p2_rot ).y;
                            
                            if( pnt_.x < xMin ) xMin = pnt_.x;
                            if( pnt_.x > xMax ) xMax = pnt_.x;
                            if( pnt_.y < yMin ) yMin = pnt_.y;
                            if( pnt_.y > yMax ) yMax = pnt_.y;
                            
                            if( pnt_rot.x < xMin_rot ) xMin_rot = pnt_rot.x;
                            if( pnt_rot.x > xMax_rot ) xMax_rot = pnt_rot.x;
                            if( pnt_rot.y < yMin_rot ) yMin_rot = pnt_rot.y;
                            if( pnt_rot.y > yMax_rot ) yMax_rot = pnt_rot.y;
                            
                            t += 0.00277777777778;
                        }
                        while ( t < 1 );
                    }
                }
                XMLNode req("get");
                req.setAttr("path",id()+"/%2fwdg%2fres")->setAttr("id",imgDef);
                mod->cntrIfCmd(req,ses.user);
                string imgDef_temp = TSYS::strDecode(req.text(),TSYS::base64);
                gdImagePtr im_fill_in = gdImageCreateFromPngPtr(imgDef_temp.size(), (void*)imgDef_temp.data());
                gdImagePtr im_fill_out = gdImageCreate((int)TSYS::realRound( xMax - xMin ), (int)TSYS::realRound( yMax - yMin ) );
                double imXScale = ( xMax - xMin )/im_fill_in->sx;
                double imYScale = ( yMax - yMin )/im_fill_in->sy;
                int xIn, yIn;
                double alpha_pr;
                
                for (int yOut = 0; ( yOut < (int)TSYS::realRound( yMax - yMin )); yOut++)
                {
                    for (int xOut = 0; ( xOut < (int)TSYS::realRound( xMax - xMin )); xOut++)
                    {
                        yIn = (int)TSYS::realRound( yOut/imYScale, 2, true );
                        xIn = (int)TSYS::realRound( xOut/imXScale, 2, true );
                        if ( yIn < 1) yIn = 1;
                        if ( xIn < 1) xIn = 1;
                        if ( yIn >= im_fill_in->sy ) yIn = im_fill_in->sy - 1;
                        if ( xIn >= im_fill_in->sx ) xIn = im_fill_in->sx - 1;
                        int c = gdImageGetPixel( im_fill_in, xIn, yIn );
                        int alpha = gdImageAlpha( im_fill_in, c );
                        alpha_pr = (double)alpha / 127;
                        int fill_clr = gdImageColorResolveAlpha( im_fill_out, gdImageRed( im_fill_in, c), gdImageGreen( im_fill_in, c),
                                                                 gdImageBlue( im_fill_in, c), alpha );
                        gdImageSetPixel( im_fill_out, xOut, yOut, fill_clr );
                    }
                }
                double im_x, im_y, im_x_temp1, im_x_temp2;
                Point drw_pnt,drw_pnt1;
                int c_new, count_zah;
                im_y = yMin_rot;
                vector<Point> pointImage;
                bool flag_count;
                do
                {
                    im_x = xMin_rot;
                    flag_count = false;
                    count_zah = 0;
                    do
                    {
                       c_new = gdImageGetPixel( im, (int)TSYS::realRound( im_x, 2, true ), (int)TSYS::realRound( im_y, 2, true ) );
                       if( c_new == tmp_clr )
                        {
                            if( !flag_count )
                            {
                                im_x_temp1 = im_x;
                                flag_count = true;
                            }
                            else
                            {
                                if( (im_x - im_x_temp1) > 1)
                                {
                                    count_zah += 1;
                                    im_x_temp2 = im_x;
                                    flag_count = false;
                                    double i_x = im_x_temp1 + 1;
                                    do
                                    {
                                        pointImage.push_back( Point( i_x, im_y ) );
                                        i_x += 1;
                                    }
                                    while ( i_x < im_x_temp2 );
                                }
                                else im_x_temp1 = im_x;
                            }
                        }
                        im_x += 1;
                   }
                    while( im_x > xMin_rot && im_x <  xMax_rot + 1 );
                    im_y += 1;
                }
                while( im_y > yMin_rot && im_y < yMax_rot + 1 );

               for ( int i_x = 0; i_x < pointImage.size(); i_x++ )
               {
                   im_x = pointImage[i_x].x;
                   im_y = pointImage[i_x].y;

                   Point drw_pnt = unscaleUnrotate( Point( im_x, im_y ), xSc, ySc, false, true );
                   int rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin ),
                                              (int)TSYS::realRound( drw_pnt.y - yMin ) );
                   drw_pnt1.x = scaleRotate( drw_pnt, xSc, ySc, false, true ).x;
                   drw_pnt1.y = scaleRotate( drw_pnt, xSc, ySc, false, true ).y;
                   
                   double color_r = alpha_pr * ( gdImageRed( im_fill_out, rgb ) )  + (1 - alpha_pr) * ( (ui8)( inundationItems[i].P_color>>16 ) );
                   double color_g = alpha_pr * ( gdImageGreen( im_fill_out, rgb ) ) + (1 - alpha_pr) * ( (ui8)( inundationItems[i].P_color>>8 ) );
                   double color_b = alpha_pr * ( gdImageBlue( im_fill_out, rgb ) ) + (1 - alpha_pr) * ( ( ui8)inundationItems[i].P_color );
                   int color = gdImageColorResolve( im, (int)TSYS::realRound( color_r, 2, true ), (int)TSYS::realRound( color_g, 2,
                                                    true ), (int)TSYS::realRound( color_b, 2, true ) );
                   gdImageSetPixel( im, (int)TSYS::realRound( drw_pnt1.x ) , (int)TSYS::realRound( drw_pnt1.y ), color );
               }
            }
            else
            {
                // - Detecting the base point(the end or start point of the figure) for each fill -
                count_min_x = 0;
                count_min_y = 0;
                count_max_x = 0;
                count_max_y = 0;
                min_x = inundationItems[i].number_point[0];
                min_y = inundationItems[i].number_point[0];
                max_x = inundationItems[i].number_point[0];
                max_y = inundationItems[i].number_point[0];
                for( int j = 1; j < inundationItems[i].number_point.size(); j++ )
                {
                    if( (pnts)[inundationItems[i].number_point[j]].x < (pnts)[min_x].x )
                        min_x = inundationItems[i].number_point[j];
                    if( (pnts)[inundationItems[i].number_point[j]].y < (pnts)[min_y].y )
                        min_y = inundationItems[i].number_point[j];
                    if( (pnts)[inundationItems[i].number_point[j]].x > (pnts)[max_x].x )
                        max_x = inundationItems[i].number_point[j];
                    if( (pnts)[inundationItems[i].number_point[j]].y > (pnts)[max_y].y )
                        max_y = inundationItems[i].number_point[j];
                }
                for( int j = 0; j < inundationItems[i].number_point.size(); j++ )
                {
                    if( (pnts)[inundationItems[i].number_point[j]].x == (pnts)[min_x].x )
                        count_min_x++;
                    if( (pnts)[inundationItems[i].number_point[j]].x == (pnts)[max_x].x )
                        count_max_x++;
                    if( (pnts)[inundationItems[i].number_point[j]].y == (pnts)[min_y].y )
                        count_min_y++;
                    if( (pnts)[inundationItems[i].number_point[j]].y == (pnts)[max_y].y )
                        count_max_y++;
                }
                if( count_min_x < 3 ) num_pnt = min_x;
                else if( count_max_x < 3 ) num_pnt = max_x;
                else if( count_min_y < 3 ) num_pnt = min_y;
                else if( count_max_y < 3 ) num_pnt = max_y;
                //- Detecting two figures and their "free" points for computing the real "filling" point
                for( int j = 0; j < shapeItems.size(); j++ )
                {
                    if( shapeItems[j].n1 == num_pnt )
                        for( int k = 0; k < inundationItems[i].number_point.size(); k++ )
                            if( shapeItems[j].n2 == inundationItems[i].number_point[k] )
                            {
                                fig.push_back(j);
                                point_num.push_back( shapeItems[j].n2 );
                                break;
                            }
                    if( shapeItems[j].n2 == num_pnt )
                        for( int k = 0; k < inundationItems[i].number_point.size(); k++ )
                            if( shapeItems[j].n1 == inundationItems[i].number_point[k] )
                            {
                                fig.push_back(j);
                                point_num.push_back( shapeItems[j].n1 );
                                break;
                            }
                }
                //- Detecting the real "filling" point for all possible combinations of types of two connected figures -
                //-- Simple arc --
                if( point_num.size() == 1 && shapeItems[fig[0]].type == 2 )
                {
                    delta_point_center = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                    if( (int)(delta_point_center.x+0.5) < s_width && (int)(delta_point_center.y+0.5) < s_height )
                        paintFill( im, delta_point_center, inundationItems[i], tmp_clr );
                }
                if( point_num.size() > 1 &&
                    length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ) > 1 &&
                    length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) > 1 )
                {
                    //-- Line and line --
                    if( shapeItems[fig[0]].type == 1 && shapeItems[fig[1]].type == 1 )
                    {
                        Point P1, P2, P3, P4, P5, P6, P7, P8, dP1, dP2, num_pnt_new;
                        double a,b,a1,b1;
                        double scale;
                        scale = 0.0;
                        if( xSc<1 && xSc <= ySc ) scale = (1-xSc)/6;
                        else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/6;
                        if( (length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ) ) < 15 ||
                        (length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) ) < 15 )
                            delta = 0.5;
                        else
                            delta = 0.2 + scale;
                        double ang,ang1;
                        if( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y <= scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ).y )
                        ang = 360 - angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                        Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        else
                            ang = angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true , true), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                            Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        if( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y <= scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ).y )
                            ang1 = 360 - angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        else
                            ang1 = angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                          Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        //--- if there is any width(of figure itself or of its borders) ---
                        if( (shapeItems[fig[0]].width > 1 || shapeItems[fig[0]].border_width > 0) || (shapeItems[fig[1]].width > 1 || shapeItems[fig[1]].border_width > 0) )
                        {
                            if( shapeItems[fig[0]].width == 1 && shapeItems[fig[0]].border_width == 0 ) W1 = 0;
                            else W1 = (shapeItems[fig[0]].width)/2 + shapeItems[fig[0]].border_width;
                            if( shapeItems[fig[1]].width == 1 && shapeItems[fig[1]].border_width == 0 ) W2 = 0;
                            else W2 = (shapeItems[fig[1]].width)/2 + shapeItems[fig[1]].border_width;
                            //--- Line_1 ---
                            P1 = Point(scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,W1), ang ).x,
                                       scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,W1), ang ).y );
                            P2 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,-W1), ang ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,-W1), ang ).y );
                            P3 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), W1 ), ang ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), W1 ), ang ).y );
                            P4=Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), -W1 ), ang ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), -W1 ), ang ).y );
                            //--- Line_2 ---
                            P5 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,W2), ang1 ).y );
                            P6 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,-W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,-W2), ang1 ).y );
                            P7 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), W2 ), ang1 ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), W2 ), ang1 ).y );
                            P8 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), -W2 ), ang1 ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                                    rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), -W2 ), ang1 ).y );
                            P1 = Point( TSYS::realRound( P1.x, 2, true ), TSYS::realRound( P1.y, 2, true ) );
                            P2 = Point( TSYS::realRound( P2.x, 2, true ), TSYS::realRound( P2.y, 2, true ) );
                            P3 = Point( TSYS::realRound( P3.x, 2, true ), TSYS::realRound( P3.y, 2, true ) );
                            P4 = Point( TSYS::realRound( P4.x, 2, true ), TSYS::realRound( P4.y, 2, true ) );
                            P5 = Point( TSYS::realRound( P5.x, 2, true ), TSYS::realRound( P5.y, 2, true ) );
                            P6 = Point( TSYS::realRound( P6.x, 2, true ), TSYS::realRound( P6.y, 2, true ) );
                            P7 = Point( TSYS::realRound( P7.x, 2, true ), TSYS::realRound( P7.y, 2, true ) );
                            P8 = Point( TSYS::realRound( P8.x, 2, true ), TSYS::realRound( P8.y, 2, true ) );
                            bool flag_vert1 = true;
                            bool flag_vert2 = true;
                            if( lineIntersect( P1.x, P1.y, P3.x, P3.y,P5.x, P5.y, P7.x, P7.y ) )
                            {
                                if( P3.x != P1.x )
                                {
                                    b = (P3.y-P1.y) / (P3.x-P1.x);
                                    a = P1.y - b*P1.x;
                                    flag_vert1 = false;
                                }
                                if( P7.x != P5.x )
                                {
                                    b1 = (P7.y-P5.y) / (P7.x-P5.x);
                                    a1 = P5.y - b1*P5.x;
                                    flag_vert2 = false;
                                }
                                dP1 = P3;
                                dP2 = P7;
                            }
                            else if( lineIntersect( P1.x, P1.y, P3.x, P3.y,P6.x, P6.y, P8.x, P8.y ) )
                            {
                                if( P3.x != P1.x )
                                {
                                    b = (P3.y-P1.y) / (P3.x-P1.x);
                                    a = P1.y - b*P1.x;
                                    flag_vert1 = false;
                                }
                                if( P8.x != P6.x )
                                {
                                    b1 = (P8.y-P6.y) / (P8.x-P6.x);
                                    a1 = P6.y - b1*P6.x;
                                    flag_vert2 = false;
                                }
                                dP1 = P3;
                                dP2 = P8;
                            }
                            else if( lineIntersect( P2.x, P2.y, P4.x, P4.y,P5.x, P5.y, P7.x, P7.y ) )
                            {
                                if( P4.x != P2.x )
                                {
                                    b = (P4.y-P2.y) / (P4.x-P2.x);
                                    a = P2.y - b*P2.x;
                                    flag_vert1 = false;
                                }
                                if( P7.x != P5.x )
                                {
                                    b1 = (P7.y-P5.y) / (P7.x-P5.x);
                                    a1 = P5.y - b1*P5.x;
                                    flag_vert2 = false;
                                }
                                dP1 = P4;
                                dP2 = P7;
                            }
                            else if( lineIntersect( P2.x, P2.y, P4.x, P4.y,P6.x, P6.y, P8.x, P8.y ) )
                            {
                                if( P4.x != P2.x )
                                {
                                    b = (P4.y-P2.y) / (P4.x-P2.x);
                                    a = P2.y - b*P2.x;
                                    flag_vert1 = false;
                                }
                                if( P8.x != P6.x )
                                {
                                    b1 = (P8.y-P6.y) / (P8.x-P6.x);
                                    a1 = P6.y - b1*P6.x;
                                    flag_vert2 = false;
                                }
                                dP1 = P4;
                                dP2 = P8;
                            }
                            if ( !flag_vert2 &&  !flag_vert1 ) num_pnt_new = Point( (a-a1) / (b1-b), a + b*(a-a1) / (b1-b) );
                            else if ( flag_vert1 && !flag_vert2 ) num_pnt_new = Point( dP1.x, a1 + b1*dP1.x );
                            else if ( !flag_vert1 && flag_vert2 ) num_pnt_new = Point( dP2.x, a + b*dP2.x );
                        }
                        else
                        {
                            dP1 = scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true );
                            dP2 = scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true );
                            num_pnt_new = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                        }
                        if( (length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ) ) > 4 )
                        {
                            delta_point_1 = unrotate( dP1, ang, num_pnt_new.x, num_pnt_new.y );
                            delta_point_1.x = delta_point_1.x * (delta);
                            delta_point_1 = Point( num_pnt_new.x + rotate( delta_point_1, ang ).x,
                                    num_pnt_new.y - rotate( delta_point_1, ang ).y );
                        }
                        else delta_point_1 = dP1;
                        if( (length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) ) > 4 )
                        {
                            delta_point_2 = unrotate( dP2, ang1, num_pnt_new.x, num_pnt_new.y );
                            delta_point_2.x = delta_point_2.x * (delta);
                            delta_point_2 = Point( num_pnt_new.x + rotate( delta_point_2, ang1 ).x,
                                    num_pnt_new.y - rotate( delta_point_2, ang1 ).y );
                        }
                        else delta_point_2 = dP2;
                    }
                    //-- Arc and line --
                    else if( (shapeItems[fig[0]].type == 2 && shapeItems[fig[1]].type == 1) || (shapeItems[fig[1]].type == 2 && shapeItems[fig[0]].type == 1) )
                    {
                        if( shapeItems[fig[1]].type == 2 )
                        {
                            int tp = fig[1];
                            int tp1 = point_num[1];
                            fig[1] = fig[0];
                            fig[0] = tp;
                            point_num[1] = point_num[0];
                            point_num[0] = tp1;
                        }
                        Point new_pnt;
                        double delta_real;
                        double scale;
                        double ang, ang1;
                        double arc_a, arc_b, arc_a_small, arc_b_small, t_start, t_end,delta_t;
                        Point P1, P2, P3, P4;
                        scale = 0.0;
                        if( xSc < 1 && xSc <= ySc ) scale = (1-xSc)/10;
                        else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/10;
                        //-- Arc --
                        Point el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                        Point el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                        Point el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                        Point el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                        Point el_p5 = scaleRotate( (pnts)[shapeItems[fig[0]].n5], xSc, ySc, true, true );
                        if ( el_p5.y <= el_p3.y )
                            ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        //--- Line ---
                        if( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y <= scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ).y )
                            ang1 = 360 - angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                                Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        else
                            ang1 = angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                          Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        arc_a = length( el_p5, el_p3 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                        arc_b = length( el_p3, el_p4 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                        //--- if there is ane width(of figure itself or of its borders) ---
                        if( (shapeItems[fig[0]].width > 1 || shapeItems[fig[0]].border_width > 0 ) || ( shapeItems[fig[1]].width > 1 || shapeItems[fig[1]].border_width > 0 ) )
                        {
                            arc_a_small = arc_a - shapeItems[fig[0]].width - 2*shapeItems[fig[0]].border_width;
                            arc_b_small = arc_b - shapeItems[fig[0]].width - 2*shapeItems[fig[0]].border_width;
                            //--- Arc ---
                            if( shapeItems[fig[0]].width == 1 && shapeItems[fig[0]].border_width == 0 ) W1 = 0;
                            else W1 = (shapeItems[fig[0]].width)/2 + shapeItems[fig[0]].border_width;
                            //--- Line ---
                            if( shapeItems[fig[1]].width == 1 && shapeItems[fig[1]].border_width == 0 ) W2 = 0;
                            else W2 = (shapeItems[fig[1]].width)/2 + shapeItems[fig[1]].border_width;
                            //--- Line ---
                            P1 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,W2), ang1 ).y );
                            P2 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,-W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,-W2), ang1 ).y );
                            P3 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), W2 ), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true) ), W2 ), ang1 ).y );
                            P4 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), -W2 ), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), -W2 ), ang1 ).y );
                            P1 = Point( TSYS::realRound( P1.x, 2, true ), TSYS::realRound( P1.y, 2, true ) );
                            P2 = Point( TSYS::realRound( P2.x, 2, true ), TSYS::realRound( P2.y, 2, true ) );
                            P3 = Point( TSYS::realRound( P3.x, 2, true ), TSYS::realRound( P3.y, 2, true ) );
                            P4 = Point( TSYS::realRound( P4.x, 2, true ), TSYS::realRound( P4.y, 2, true ) );
                            //--- Line ---
                            bool flag_vert1 = true;
                            bool flag_vert2 = true;
                            if( P3.x != P1.x ) flag_vert1 = false;
                            if( P4.x != P2.x ) flag_vert2 = false;
                            t_start = shapeItems[fig[0]].ctrlPos4.x;
                            t_end = shapeItems[fig[0]].ctrlPos4.y;
                            double inc_delta;
                            if( num_pnt == shapeItems[fig[0]].n1 )
                            {
                                delta_t = t_start;
                                inc_delta = 0.001;
                            }
                            else
                            {
                                delta_t = t_end;
                                inc_delta = -0.001;
                            }
                            Point arc_pnt, arc_pnt_pred;
                            arc_pnt_pred = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                  (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                            do
                            {
                                delta_t += inc_delta;
                                arc_pnt = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                 (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                                if( lineIntersect( arc_pnt_pred.x, arc_pnt_pred.y, arc_pnt.x, arc_pnt.y,P1.x, P1.y, P3.x, P3.y ) )
                                {
                                    new_pnt = Point( arc_pnt.x, arc_pnt.y );
                                    break;
                                }
                                else if( lineIntersect( arc_pnt_pred.x, arc_pnt_pred.y, arc_pnt.x, arc_pnt.y,P2.x, P2.y, P4.x, P4.y ) )
                                {
                                    new_pnt = Point( arc_pnt.x, arc_pnt.y );
                                    break;
                                }
                                arc_pnt_pred = arc_pnt;
                            }
                            while ( ((delta_t<t_end) && (inc_delta>0)) || ((delta_t>t_start) && (inc_delta<0)) );
                            if( inc_delta > 0 ) delta = (t_end - delta_t)/2 + scale;
                            else delta =- ( (delta_t - t_start)/2 + scale );
                            delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t + delta, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                   (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t + delta, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                        }
                        else
                        {
                            t_start = shapeItems[fig[0]].ctrlPos4.x;
                            t_end = shapeItems[fig[0]].ctrlPos4.y;
                            if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/2 + scale;
                            else delta_real = t_end - (t_end-t_start)/2 + scale;
                            new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                            delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a, arc_b ), ang ).x, 2, true ),
                                                   (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a, arc_b ), ang ).y, 2, true ) );
                        }
                        delta_point_2 = unrotate( new_pnt, ang1, new_pnt.x, new_pnt.y );
                        delta_point_2.x = delta_point_2.x + length( new_pnt, scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) * (0.15 + scale);
                        delta_point_2 = Point( new_pnt.x + rotate( delta_point_2, ang1 ).x,
                                               new_pnt.y - rotate( delta_point_2, ang1 ).y );
                    }
                    //-- bezier curve and line --
                    else if( (shapeItems[fig[0]].type == 3 && shapeItems[fig[1]].type == 1) || (shapeItems[fig[1]].type == 3 && shapeItems[fig[0]].type == 1) )
                    {
                        if( shapeItems[fig[1]].type == 3 )
                        {
                            int tp = fig[1];
                            int tp1 = point_num[1];
                            fig[1] = fig[0];
                            fig[0] = tp;
                            point_num[1] = point_num[0];
                            point_num[0] = tp1;
                        }
                        Point new_pnt, new_pnt_1, new_pnt_2;
                        double delta_real, delta_t, delta_temp_1, delta_temp_2;
                        double scale;
                        double ang, ang1;
                        int num_bezier;
                        Point P1, P2, P3, P4, el_p1, el_p2, el_p3, el_p4;
                        scale = 0.0;
                        if( xSc < 1 && xSc <= ySc ) scale = (1-xSc)/10;
                        else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/10;
                        //--- bezier ---
                        el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                        el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                        el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                        el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                        if ( el_p1.y <= el_p2.y )
                            ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                        else ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                        //--- Line ---
                        if( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y <= scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ).y )
                            ang1 = 360 - angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                                Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        else
                            ang1 = angle( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ), scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ),
                                          Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x+10, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y ) );
                        //--- if there is ane width(of figure itself or of its borders) ---
                        if( (shapeItems[fig[0]].width > 1 || shapeItems[fig[0]].border_width > 0 ) || ( shapeItems[fig[1]].width > 1 || shapeItems[fig[1]].border_width > 0 ))
                        {
                            //--- bezier ---
                            if( shapeItems[fig[0]].width == 1 && shapeItems[fig[0]].border_width == 0 ) W1 = 0;
                            else W1 = (shapeItems[fig[0]].width)/2 + shapeItems[fig[0]].border_width;
                            //--- Line ---
                            if( shapeItems[fig[1]].width == 1 && shapeItems[fig[1]].border_width == 0 ) W2 = 0;
                            else W2 = (shapeItems[fig[1]].width)/2 + shapeItems[fig[1]].border_width;
                            //--- Line ---
                            P1 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,W2), ang1 ).y );
                            P2 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,-W2), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,-W2), ang1 ).y );
                            P3 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                        rotate( Point( length(scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), W2 ), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), W2 ), ang1 ).y );
                            P4 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ), -W2 ), ang1 ).x,
                                        scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                        rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true , true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true) ), -W2 ), ang1 ).y );
                            P1 = Point( TSYS::realRound( P1.x, 2, true ), TSYS::realRound( P1.y, 2, true ) );
                            P2 = Point( TSYS::realRound( P2.x, 2, true ), TSYS::realRound( P2.y, 2, true ) );
                            P3 = Point( TSYS::realRound( P3.x, 2, true ), TSYS::realRound( P3.y, 2, true ) );
                            P4 = Point( TSYS::realRound( P4.x, 2, true ), TSYS::realRound( P4.y, 2, true ) );
                            //--- Line ---
                            bool flag_vert1 = true;
                            bool flag_vert2 = true;
                            double inc_delta;
                            if( P3.x != P1.x ) flag_vert1 = false;
                            if( P4.x != P2.x ) flag_vert2 = false;
                            if( num_pnt == shapeItems[fig[0]].n1 )
                            {
                                delta_t = 0;
                                inc_delta = 0.001;
                            }
                            else
                            {
                                delta_t = 1;
                                inc_delta = -0.001;
                            }
                            Point bezier_pnt_1, bezier_pnt_2, bezier_pnt_pred_1, bezier_pnt_pred_2;
                            ShapeItem item = shapeItems[fig[0]];
                            el_p1 = unrotate( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            el_p2 = unrotate( scaleRotate( (pnts)[item.n3], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            el_p3 = unrotate( scaleRotate( (pnts)[item.n4], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            el_p4 = unrotate( scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                    
                            bezier_pnt_pred_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
                                                Point( el_p1.x, el_p1.y+W1 ),
                                                Point( el_p2.x, el_p2.y+W1 ),
                                                Point( el_p3.x, el_p3.y+W1 ),
                                                Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, 2, true ),
                                                       (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
                                                Point( el_p1.x, el_p1.y+W1 ),
                                                Point( el_p2.x, el_p2.y+W1 ),
                                                Point( el_p3.x, el_p3.y+W1 ),
                                                Point( el_p4.x, el_p4.y+W1) ), ang ).y, 2, true ) );
                    
                            bezier_pnt_pred_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
                                                Point( el_p1.x, el_p1.y-W1 ),
                                                Point( el_p2.x, el_p2.y-W1 ),
                                                Point( el_p3.x, el_p3.y-W1 ),
                                                Point( el_p4.x, el_p4.y-W1 ) ), ang).x, 2, true ),
                                                       (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
                                                Point( el_p1.x, el_p1.y-W1 ),
                                                Point( el_p2.x, el_p2.y-W1 ),
                                                Point( el_p3.x, el_p3.y-W1 ),
                                                Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, 2, true ) );
                            bool flag_brk_1, flag_brk_2;
                            flag_brk_1 = false;
                            flag_brk_2 = false;
                            do
                            {
                                delta_t += inc_delta;
                                bezier_pnt_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
                                               Point( el_p1.x, el_p1.y+W1 ),
                                               Point( el_p2.x, el_p2.y+W1 ),
                                               Point( el_p3.x, el_p3.y+W1 ),
                                               Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, 2, true ),
                                                      (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
                                               Point( el_p1.x, el_p1.y+W1 ),
                                               Point( el_p2.x, el_p2.y+W1 ),
                                               Point( el_p3.x, el_p3.y+W1 ),
                                               Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, 2, true ) );
                                if( W1 != 0 )
                                    bezier_pnt_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
                                                   Point( el_p1.x, el_p1.y-W1 ),
                                                   Point( el_p2.x, el_p2.y-W1 ),
                                                   Point( el_p3.x, el_p3.y-W1 ),
                                                   Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, 2, true ),
                                                          (int)TSYS::realRound( scaleRotate((pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
                                                   Point( el_p1.x, el_p1.y-W1 ),
                                                   Point( el_p2.x, el_p2.y-W1 ),
                                                   Point( el_p3.x, el_p3.y-W1 ),
                                                   Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, 2, true ) );
                                if( lineIntersect( bezier_pnt_pred_1.x, bezier_pnt_pred_1.y, bezier_pnt_1.x, bezier_pnt_1.y, P1.x, P1.y, P3.x, P3.y ) )
                                {
                                    new_pnt_1 = Point( bezier_pnt_1.x, bezier_pnt_1.y );
                                    delta_temp_1 = delta_t;
                                    flag_brk_1 = true;
                                }
                                else if( lineIntersect( bezier_pnt_pred_1.x,  bezier_pnt_pred_1.y, bezier_pnt_1.x, bezier_pnt_1.y, P2.x, P2.y, P4.x, P4.y ) )
                                {
                                    new_pnt_1 = Point( bezier_pnt_1.x, bezier_pnt_1.y );
                                    delta_temp_1 = delta_t;
                                    flag_brk_1 = true;
                                }
                                else if( lineIntersect( bezier_pnt_pred_2.x,  bezier_pnt_pred_2.y, bezier_pnt_2.x, bezier_pnt_2.y, P1.x, P1.y, P3.x, P3.y ) )
                                {
                                    new_pnt_2 = Point( bezier_pnt_2.x, bezier_pnt_2.y );
                                    delta_temp_2 = delta_t;
                                    flag_brk_2 = true;
                                }
                                else if( lineIntersect( bezier_pnt_pred_2.x,  bezier_pnt_pred_2.y, bezier_pnt_2.x, bezier_pnt_2.y, P2.x, P2.y, P4.x, P4.y ) )
                                {
                                    new_pnt_2 = Point( bezier_pnt_2.x, bezier_pnt_2.y );
                                    delta_temp_2 = delta_t;
                                    flag_brk_2 = true;
                                }
                                bezier_pnt_pred_1 = bezier_pnt_1;
                                bezier_pnt_pred_2 = bezier_pnt_2;
                                if( flag_brk_1 && flag_brk_2 )
                                    break;
                            }
                            while ( ((delta_t<1) && (inc_delta>0)) || ((delta_t>0) && (inc_delta<0)) );
                            if( !flag_brk_1 && !flag_brk_2 )
                            {
                                el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                                el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                                el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                                el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                                new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                                if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.25 + scale;
                                else delta_real = 0.75 - scale;
                                delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                                       (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ) );
                            }
                            else
                            {
                                if( length(new_pnt_1, (pnts)[point_num[1]]) < length(new_pnt_2, (pnts)[point_num[1]]) )
                                {
                                    new_pnt = new_pnt_1;
                                    delta_t = delta_temp_1;
                                    num_bezier = 1;
                                }
                                else
                                {
                                    new_pnt = new_pnt_2;
                                    delta_t = delta_temp_2;
                                    num_bezier = 2;
                                }
                                if( inc_delta > 0 ) delta = (1-delta_t)/3 + scale;
                                else delta = -((delta_t)/3 + scale);
                    
                                if( num_bezier == 2 )
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
                                                    Point( el_p1.x, el_p1.y-W1 ),
                                                    Point( el_p2.x, el_p2.y-W1 ),
                                                    Point( el_p3.x, el_p3.y-W1 ),
                                                    Point( el_p4.x, el_p4.y-W1 ) ), ang).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
                                                    Point( el_p1.x, el_p1.y-W1 ),
                                                    Point( el_p2.x, el_p2.y-W1 ),
                                                    Point( el_p3.x, el_p3.y-W1 ),
                                                    Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, 2, true ) );
                                if( num_bezier == 1 )
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
                                                    Point( el_p1.x, el_p1.y+W1 ),
                                                    Point( el_p2.x, el_p2.y+W1 ),
                                                    Point( el_p3.x, el_p3.y+W1 ),
                                                    Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
                                                    Point( el_p1.x, el_p1.y+W1 ),
                                                    Point( el_p2.x, el_p2.y+W1 ),
                                                    Point( el_p3.x, el_p3.y+W1 ),
                                                    Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, 2, true ) );
                            }
                        }
                        else
                        {
                            new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                            if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.2 + scale;
                            else delta_real = 0.8 - scale;
                            delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                                   (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ) );
                        }
                        delta_point_2 = unrotate( new_pnt, ang1, new_pnt.x, new_pnt.y );
                        delta_point_2.x = delta_point_2.x + length( new_pnt, scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) * (0.2 + scale);
                        delta_point_2 = Point( new_pnt.x + rotate( delta_point_2, ang1 ).x,
                                               new_pnt.y - rotate( delta_point_2, ang1 ).y );
                    }
                    //-- bezier cureve and Arc --
                    else if( (shapeItems[fig[0]].type == 3 && shapeItems[fig[1]].type == 2) || (shapeItems[fig[1]].type == 3 && shapeItems[fig[0]].type == 2 ) )
                    {
                        if( shapeItems[fig[1]].type == 2 )
                        {
                            int tp = fig[1];
                            int tp1 = point_num[1];
                            fig[1] = fig[0];
                            fig[0] = tp;
                            point_num[1] = point_num[0];
                            point_num[0] = tp1;
                        }
                        Point new_pnt, new_pnt_1, new_pnt_2;
                        double delta_real;
                        double scale;
                        double ang, ang1, delta_temp_1, delta_temp_2, delta_t;
                        double arc_a, arc_b, arc_a_small, arc_b_small, t_start, t_end, delta_t_arc, delta_t_bez;
                        int num_bezier;
                        Point P1, P2, P3, P4;
                        scale = 0.0;
                        if( xSc < 1 && xSc <= ySc ) scale = (1-xSc)/10;
                        else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/10;
                        //--- Arc ---
                        Point el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                        Point el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                        Point el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                        Point el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                        Point el_p5 = scaleRotate( (pnts)[shapeItems[fig[0]].n5], xSc, ySc, true, true );
                        if ( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                        arc_a = length( el_p5, el_p3 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                        arc_b = length( el_p3, el_p4 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                        //--- bezier ---
                        P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                        P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                        P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                        P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                        if ( P1.y <= P2.y ) ang1 = 360 - angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
                        else ang1 = angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
                        //-- if there is ane width(of figure itself or of its borders) --
                        if( (shapeItems[fig[0]].width > 1 || shapeItems[fig[0]].border_width > 0) || (shapeItems[fig[1]].width > 1 || shapeItems[fig[1]].border_width > 0) )
                        {
                            //--- Arc ---
                            if( shapeItems[fig[0]].width == 1 && shapeItems[fig[0]].border_width == 0 ) W1 = 0;
                            else W1 = (shapeItems[fig[0]].width)/2 + shapeItems[fig[0]].border_width;
                            //--- bezier ---
                            if( shapeItems[fig[1]].width == 1 && shapeItems[fig[1]].border_width == 0 ) W2 = 0;
                            else W2 = (shapeItems[fig[1]].width)/2 + shapeItems[fig[1]].border_width;
                    
                            arc_a_small = arc_a - shapeItems[fig[0]].width - 2*shapeItems[fig[0]].border_width;
                            arc_b_small = arc_b - shapeItems[fig[0]].width - 2*shapeItems[fig[0]].border_width;
                            //--- Arc ---
                            t_start = shapeItems[fig[0]].ctrlPos4.x;
                            t_end = shapeItems[fig[0]].ctrlPos4.y;
                            double inc_delta_arc;
                            double s = 0.825056176207;
                            double Len_arc = (4*(arc_a_small + arc_b_small) -
                                             (2*(4 - M_PI)* arc_a_small* arc_b_small)/
                                              pow( pow(arc_a_small,s)/2 + pow(arc_b_small,s)/2 ,(1/s)))*(t_end-t_start);
                            if( num_pnt == shapeItems[fig[0]].n1 )
                            {
                                delta_t_arc = t_start;
                                inc_delta_arc = 1/Len_arc;
                            }
                            else
                            {
                                delta_t_arc = t_end;
                                inc_delta_arc = -1/Len_arc;
                            }
                            Point arc_pnt, arc_pnt_pred;
                            arc_pnt_pred = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t_arc, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                  (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t_arc, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                            //--- bezier ---
                            double Len = length( (pnts)[shapeItems[fig[1]].n1], (pnts)[shapeItems[fig[1]].n3] ) +
                                         length( (pnts)[shapeItems[fig[1]].n3], (pnts)[shapeItems[fig[1]].n4] ) +
                                         length( (pnts)[shapeItems[fig[1]].n4], (pnts)[shapeItems[fig[1]].n2] );
                            double inc_delta_bez;
                            if( num_pnt == shapeItems[fig[1]].n1 )
                            {
                                delta_t_bez = 0;
                                inc_delta_bez = 1/Len;
                            }
                            else
                            {
                                delta_t_bez = 1;
                                inc_delta_bez = -1/Len;
                            }
                            Point bezier_pnt_1, bezier_pnt_2, bezier_pnt_pred_1, bezier_pnt_pred_2;
                            ShapeItem item = shapeItems[fig[1]];
                            P1 = unrotate( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            P2 = unrotate( scaleRotate( (pnts)[item.n3], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            P3 = unrotate( scaleRotate( (pnts)[item.n4], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            P4 = unrotate( scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                    
                            bezier_pnt_pred_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
                                                Point( P1.x, P1.y+W1 ),
                                                Point( P2.x, P2.y+W1 ),
                                                Point( P3.x, P3.y+W1 ),
                                                Point( P4.x, P4.y+W1 ) ), ang1 ).x, 2, true ),
                                                       (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
                                                Point( P1.x, P1.y+W1 ),
                                                Point( P2.x, P2.y+W1 ),
                                                Point( P3.x, P3.y+W1 ),
                                                Point( P4.x, P4.y+W1 ) ), ang1 ).y, 2, true ) );
                    
                            bezier_pnt_pred_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
                                                Point( P1.x, P1.y-W1 ),
                                                Point( P2.x, P2.y-W1 ),
                                                Point( P3.x, P3.y-W1 ),
                                                Point( P4.x, P4.y-W1 ) ), ang1 ).x, 2, true ),
                                                       (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
                                                Point( P1.x, P1.y-W1 ),
                                                Point( P2.x, P2.y-W1 ),
                                                Point( P3.x, P3.y-W1 ),
                                                Point( P4.x, P4.y-W1 ) ), ang1 ).y, 2, true ) );
                    
                            bool f_brk_1, f_brk_2;
                            f_brk_1 = false;
                            f_brk_2 = false;
                            do
                            {
                                bezier_pnt_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
                                               Point( P1.x, P1.y+W1 ),
                                               Point( P2.x, P2.y+W1 ),
                                               Point( P3.x, P3.y+W1 ),
                                               Point( P4.x, P4.y+W1 ) ), ang1 ).x, 2, true ),
                                                      (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
                                               Point( P1.x, P1.y+W1 ),
                                               Point( P2.x, P2.y+W1 ),
                                               Point( P3.x, P3.y+W1 ),
                                               Point( P4.x, P4.y+W1 ) ), ang1 ).y, 2, true ) );
                                if( W1 != 0 )
                                    bezier_pnt_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
                                                   Point( P1.x, P1.y-W1 ),
                                                   Point( P2.x, P2.y-W1 ),
                                                   Point( P3.x, P3.y-W1 ),
                                                   Point( P4.x, P4.y-W1 ) ), ang1 ).x, 2, true ),
                                                          (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
                                                   Point( P1.x, P1.y-W1 ),
                                                   Point( P2.x, P2.y-W1 ),
                                                   Point( P3.x, P3.y-W1 ),
                                                   Point( P4.x, P4.y-W1 ) ), ang1 ).y, 2, true ) );
                                delta_t_bez += inc_delta_bez;
                                double t_arc = delta_t_arc;
                                do
                                {
                                    arc_pnt = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( t_arc, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                     (int)TSYS::realRound( el_p3.y - rotate( arc( t_arc, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                                    if( ( ABS(arc_pnt.x - bezier_pnt_1.x) ) < 1 && ( ABS(arc_pnt.y - bezier_pnt_1.y) < 1 ) && !f_brk_1 )
                                    {
                                        new_pnt_1 = Point( arc_pnt.x, arc_pnt.y );
                                        f_brk_1 = true;
                                        delta_temp_1 = delta_t_bez;
                                    }
                                    if( ( ABS(arc_pnt.x - bezier_pnt_2.x) ) < 1 && ( ABS(arc_pnt.y - bezier_pnt_2.y) < 1 ) && !f_brk_2 )
                                    {
                                        new_pnt_2 = Point( arc_pnt.x, arc_pnt.y );
                                        f_brk_2 = true;
                                        delta_temp_2 = delta_t_bez;
                                    }
                                    t_arc += inc_delta_arc;
                                }
                                while( ((t_arc < t_end) && (inc_delta_arc > 0)) || ((t_arc > t_start) && (inc_delta_arc < 0)) );
                                if( f_brk_1 && f_brk_2 ) break;
                            }
                            while ( ((delta_t_bez < 1) && (inc_delta_bez > 0)) || ((delta_t_bez > 0) && (inc_delta_bez < 0)) );
                    
                            if( !f_brk_1 && !f_brk_2 )
                            {
                                //--- Arc ---
                                t_start = shapeItems[fig[0]].ctrlPos4.x;
                                t_end = shapeItems[fig[0]].ctrlPos4.y;
                                if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/4 + scale;
                                else delta_real = t_end - (t_end-t_start)/4 + scale;
                                new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                                delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                       (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                                //--- bezier ---
                                P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                                P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                                P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                                P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                                if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
                                else delta_real = 0.8 - scale;
                                delta_point_2 = Point( (int)TSYS::realRound (bezier( delta_real, P1, P3, P4, P2 ).x, 2, true ),
                                                       (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, 2, true ) );
                            }
                            else
                            {
                                if ( f_brk_1 && f_brk_2 )
                                    if( length( new_pnt_1, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ) ) >
                                        length( new_pnt_2, scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ) ) )
                                {
                                    new_pnt = new_pnt_1;
                                    delta_t = delta_temp_1;
                                    num_bezier = 1;
                                }
                                else
                                {
                                    new_pnt = new_pnt_2;
                                    delta_t = delta_temp_2;
                                    num_bezier = 2;
                                }
                                else
                                    if( f_brk_1 && !f_brk_2 )
                                {
                                    new_pnt = new_pnt_1;
                                    delta_t = delta_temp_1;
                                    num_bezier = 1;
                                }
                                else
                                    if( !f_brk_1 && f_brk_2 )
                                {
                                    new_pnt = new_pnt_2;
                                    delta_t = delta_temp_2;
                                    num_bezier = 2;
                                }
                                if( inc_delta_bez > 0 ) delta = (1-delta_t)/3 + scale;
                                else delta = -((delta_t)/3 + scale);
                    
                                if(num_bezier == 1)
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
                                                    Point( P1.x, P1.y+W1 ),
                                                    Point( P2.x, P2.y+W1 ),
                                                    Point( P3.x, P3.y+W1 ),
                                                    Point( P4.x, P4.y+W1 ) ), ang1 ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
                                                    Point( P1.x, P1.y+W1 ),
                                                    Point( P2.x, P2.y+W1 ),
                                                    Point( P3.x, P3.y+W1 ),
                                                    Point( P4.x, P4.y+W1 ) ), ang1 ).y, 2, true ) );
                                if( num_bezier == 2 )
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
                                                    Point( P1.x, P1.y-W1 ),
                                                    Point( P2.x, P2.y-W1 ),
                                                    Point( P3.x, P3.y-W1 ),
                                                    Point( P4.x, P4.y-W1 ) ), ang1 ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
                                                    Point( P1.x, P1.y-W1 ),
                                                    Point( P2.x, P2.y-W1 ),
                                                    Point( P3.x, P3.y-W1 ),
                                                    Point( P4.x, P4.y-W1 ) ), ang1 ).y, 2, true ) );
                                if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/4 + scale;
                                else delta_real = t_end - (t_end-t_start)/4 + scale;
                                delta_point_2 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).x, 2, true ),
                                                       (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).y, 2, true ) );
                            }
                        }
                        else
                        {
                            //--- Arc ---
                            t_start = shapeItems[fig[0]].ctrlPos4.x;
                            t_end = shapeItems[fig[0]].ctrlPos4.y;
                            if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/4 + scale;
                            else delta_real = t_end - (t_end-t_start)/4 + scale;
                            new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                            delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a, arc_b ), ang ).x, 2, true ),
                                                   (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a, arc_b ), ang ).y, 2, true ) );
                            //--- bezier ---
                            if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
                            else delta_real = 0.8 - scale;
                            delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, 2, true ),
                                                   (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, 2, true ) );
                        }
                    }
                    //- bezier curve and bezier curve -
                    else if( shapeItems[fig[0]].type == 3 && shapeItems[fig[1]].type == 3 )
                    {
                        Point new_pnt;
                        vector <Point> new_pnt_vect;
                        double delta_real, delta_t_1, delta_t_2, delta_t_bez_1,  delta_t_bez_2;
                        double delta_1, delta_2;
                        vector <double> delta_temp_1, delta_temp_2;
                        vector <int> num_bezier_1, num_bezier_2;
                        double scale;
                        double ang, ang1;
                        int num_bez_1, num_bez_2;
                        Point P1, P2, P3, P4, el_p1, el_p2, el_p3, el_p4;
                        scale = 0.0;
                        if( xSc < 1 && xSc <= ySc ) scale = (1 - xSc)/10;
                        else if( ySc<1 && ySc<=xSc ) scale = (1 - ySc)/10;
                        //--- bezier_1 ---
                        el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                        el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                        el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                        el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                        if ( el_p1.y <= el_p2.y )
                            ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                        else ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                        //--- bezier_2 ---
                        P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                        P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                        P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                        P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                        if ( P1.y <= P2.y )
                            ang1 = 360 - angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
                        else ang1 = angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
                        //-- if there is ane width(of figure itself or of its borders) --
                        if( ( shapeItems[fig[0]].width > 1 || shapeItems[fig[0]].border_width > 0 ) || ( shapeItems[fig[1]].width > 1 || shapeItems[fig[1]].border_width > 0 ) )
                        {
                            double inc_delta_bez_1, inc_delta_bez_2;
                            Point bezier_pnt_1_1, bezier_pnt_1_2, bezier_pnt_2_1, bezier_pnt_2_2;
                            //--- bezier_1 ---
                            if( shapeItems[fig[0]].width == 1 && shapeItems[fig[0]].border_width == 0 ) W1 = 0;
                            else W1 = (shapeItems[fig[0]].width)/2 + shapeItems[fig[0]].border_width;
                            //--- bezier_2 ---
                            if( shapeItems[fig[1]].width == 1 && shapeItems[fig[1]].border_width == 0 ) W2 = 0;
                            else W2 = (shapeItems[fig[1]].width)/2 + shapeItems[fig[1]].border_width;
                            double Len_1 = length( (pnts)[shapeItems[fig[0]].n1], (pnts)[shapeItems[fig[0]].n3] ) +
                                           length( (pnts)[shapeItems[fig[0]].n3], (pnts)[shapeItems[fig[0]].n4] ) +
                                           length( (pnts)[shapeItems[fig[0]].n4], (pnts)[shapeItems[fig[0]].n2] );
                            double Len_2 = length( (pnts)[shapeItems[fig[1]].n1], (pnts)[shapeItems[fig[1]].n3] ) +
                                           length( (pnts)[shapeItems[fig[1]].n3], (pnts)[shapeItems[fig[1]].n4] ) +
                                           length( (pnts)[shapeItems[fig[1]].n4], (pnts)[shapeItems[fig[1]].n2] );
                    
                            //--- bezier_1 ---
                            if( num_pnt == shapeItems[fig[0]].n1 )
                            {
                                delta_t_bez_1 = 0;
                                inc_delta_bez_1 = 1/Len_1;
                            }
                            else
                            {
                                delta_t_bez_1 = 1;
                                inc_delta_bez_1 = -1/Len_1;
                            }
                            ShapeItem item = shapeItems[fig[0]];
                            el_p1 = unrotate( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true).y );
                            el_p2 = unrotate( scaleRotate( (pnts)[item.n3], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            el_p3 = unrotate( scaleRotate( (pnts)[item.n4], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                            el_p4 = unrotate( scaleRotate( (pnts)[item.n2], xSc, ySc, true, true ), ang,
                                              scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y );
                    
                            //--- bezier_2 ---
                            if( num_pnt == shapeItems[fig[1]].n1 )
                            {
                                delta_t_bez_2 = 0;
                                inc_delta_bez_2 = 1/Len_2;
                            }
                            else
                            {
                                delta_t_bez_2 = 1;
                                inc_delta_bez_2 = -1/Len_2;
                            }
                            ShapeItem item_1 = shapeItems[fig[1]];
                            P1 = unrotate( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y );
                            P2 = unrotate( scaleRotate( (pnts)[item_1.n3], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y );
                            P3 = unrotate( scaleRotate( (pnts)[item_1.n4], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y );
                            P4 = unrotate( scaleRotate( (pnts)[item_1.n2], xSc, ySc, true, true ), ang1,
                                           scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x, scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y );
                    
                            bool f_brk_1, f_brk_2, f_brk_3, f_brk_4;
                            f_brk_1 = false;
                            f_brk_2 = false;
                            f_brk_3 = false;
                            f_brk_4 = false;
                            int red = gdImageColorAllocate( im, 0, 0, 255 );
                            do
                            {
                                bezier_pnt_1_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_1,
                                                 Point( el_p1.x, el_p1.y+W1 ),
                                                 Point( el_p2.x, el_p2.y+W1 ),
                                                 Point( el_p3.x, el_p3.y+W1 ),
                                                 Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, 2, true ),
                                                        (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_1,
                                                 Point( el_p1.x, el_p1.y+W1 ),
                                                 Point( el_p2.x, el_p2.y+W1 ),
                                                 Point( el_p3.x, el_p3.y+W1 ),
                                                 Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, 2, true ) );
                                if( W1 != 0 )
                                {
                                    bezier_pnt_1_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_1,
                                                     Point( el_p1.x, el_p1.y-W1 ),
                                                     Point( el_p2.x, el_p2.y-W1 ),
                                                     Point( el_p3.x, el_p3.y-W1 ),
                                                     Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, 2, true ),
                                                            (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_1,
                                                     Point( el_p1.x, el_p1.y-W1 ),
                                                     Point( el_p2.x, el_p2.y-W1 ),
                                                     Point( el_p3.x, el_p3.y-W1 ),
                                                     Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, 2, true ) );
                                }
                                delta_t_bez_1 += inc_delta_bez_1;
                                double delta_t_bez_2_do = delta_t_bez_2;
                                do
                                {
                                    bezier_pnt_2_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_2_do,
                                                     Point( P1.x, P1.y+W2 ),
                                                     Point( P2.x, P2.y+W2 ),
                                                     Point( P3.x, P3.y+W2 ),
                                                     Point( P4.x, P4.y+W2 ) ), ang1 ).x, 2, true ),
                                                            (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_2_do,
                                                     Point( P1.x, P1.y+W2 ),
                                                     Point( P2.x, P2.y+W2 ),
                                                     Point( P3.x, P3.y+W2 ),
                                                     Point( P4.x, P4.y+W2 ) ), ang1 ).y, 2, true ) );
                                    if( W2 != 0 )
                                    {
                                        bezier_pnt_2_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_2_do,
                                                         Point( P1.x, P1.y-W2 ),
                                                         Point( P2.x, P2.y-W2 ),
                                                         Point( P3.x, P3.y-W2 ),
                                                         Point( P4.x, P4.y-W2 ) ), ang1 ).x, 2, true ),
                                                                (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_2_do,
                                                         Point( P1.x, P1.y-W2 ),
                                                         Point( P2.x, P2.y-W2 ),
                                                         Point( P3.x, P3.y-W2 ),
                                                         Point( P4.x, P4.y-W2 ) ), ang1 ).y, 2, true ) );
                                    }
                                    if( (ABS( bezier_pnt_2_1.x - bezier_pnt_1_1.x)) < 1 && (ABS( bezier_pnt_2_1.y - bezier_pnt_1_1.y) < 1) && !f_brk_1 )
                                    {
                                        new_pnt_vect.push_back( Point( bezier_pnt_2_1.x, bezier_pnt_2_1.y ) );
                                        f_brk_1 = true;
                                        delta_temp_1.push_back(delta_t_bez_1);
                                        delta_temp_2.push_back(delta_t_bez_2_do);
                                        num_bezier_1.push_back(1);
                                        num_bezier_2.push_back(1);
                                    }
                                    if( (ABS( bezier_pnt_2_1.x - bezier_pnt_1_2.x)) < 1 && (ABS( bezier_pnt_2_1.y - bezier_pnt_1_2.y) < 1) && !f_brk_2 )
                                    {
                                        new_pnt_vect.push_back(Point(bezier_pnt_2_1.x, bezier_pnt_2_1.y));
                                        f_brk_2 = true;
                                        delta_temp_1.push_back(delta_t_bez_1);
                                        delta_temp_2.push_back(delta_t_bez_2_do);
                                        num_bezier_1.push_back(0);
                                        num_bezier_2.push_back(1);
                                    }
                                    if( (ABS( bezier_pnt_2_2.x - bezier_pnt_1_1.x)) < 1 && (ABS( bezier_pnt_2_2.y - bezier_pnt_1_1.y) < 1) && !f_brk_3 )
                                    {
                                        new_pnt_vect.push_back( Point( bezier_pnt_2_2.x, bezier_pnt_2_2.y ) );
                                        f_brk_3 = true;
                                        delta_temp_1.push_back(delta_t_bez_1);
                                        delta_temp_2.push_back(delta_t_bez_2_do);
                                        num_bezier_1.push_back(1);
                                        num_bezier_2.push_back(0);
                                    }
                                    if( (ABS( bezier_pnt_2_2.x - bezier_pnt_1_2.x)) < 1 && (ABS( bezier_pnt_2_2.y - bezier_pnt_1_2.y) < 1) && !f_brk_4 )
                                    {
                                        new_pnt_vect.push_back( Point( bezier_pnt_2_2.x, bezier_pnt_2_2.y ) );
                                        f_brk_4 = true;
                                        delta_temp_1.push_back(delta_t_bez_1);
                                        delta_temp_2.push_back(delta_t_bez_2_do);
                                        num_bezier_1.push_back(0);
                                        num_bezier_2.push_back(0);
                                    }
                                    delta_t_bez_2_do+=inc_delta_bez_2;
                                }
                                while( ((delta_t_bez_2_do < 1) && (inc_delta_bez_2 > 0)) || ((delta_t_bez_2_do > 0) && (inc_delta_bez_2 < 0)) );
                                if( f_brk_1 && f_brk_2 && f_brk_3 && f_brk_4 ) break;
                            }
                            while ( ((delta_t_bez_1 < 1) && (inc_delta_bez_1 > 0)) || ((delta_t_bez_1 > 0) && (inc_delta_bez_1 < 0)) );
                            if(!f_brk_1 && !f_brk_2 && !f_brk_3 && !f_brk_4)
                            {
                                el_p1 = scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true );
                                el_p2 = scaleRotate( (pnts)[shapeItems[fig[0]].n2], xSc, ySc, true, true );
                                el_p3 = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                                el_p4 = scaleRotate( (pnts)[shapeItems[fig[0]].n4], xSc, ySc, true, true );
                        
                                P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                                P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                                P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                                P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                        
                                new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                                if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.2 + scale;
                                else delta_real = 0.8 - scale;
                                delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                                       (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ) );
                                if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
                                else delta_real = 0.8 - scale;
                                delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, 2, true ),
                                                       (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, 2, true ) );
                            }
                            else
                            {
                                if( new_pnt_vect.size() )
                                {
                                    new_pnt = Point( new_pnt_vect[0].x, new_pnt_vect[0].y );
                                    double max_len = length( (pnts)[num_pnt], new_pnt );
                                    delta_t_1 = delta_temp_1[0];
                                    delta_t_2 = delta_temp_2[0];
                                    num_bez_1 = num_bezier_1[0];
                                    num_bez_2 = num_bezier_2[0];
                                    for( int i = 1; i < new_pnt_vect.size(); i++ )
                                        if( length( (pnts)[num_pnt], new_pnt_vect[i]) > max_len )
                                        {
                                            max_len = length( (pnts)[num_pnt], new_pnt_vect[i] );
                                            new_pnt = Point( new_pnt_vect[i].x, new_pnt_vect[i].y );
                                            delta_t_1 = delta_temp_1[i];
                                            delta_t_2 = delta_temp_2[i];
                                            num_bez_1 = num_bezier_1[i];
                                            num_bez_2 = num_bezier_2[i];
                                        }
                                }
                                if( inc_delta_bez_1 > 0 ) delta_1 = (1-delta_t_1)/3 + scale;
                                else delta_1 = -((delta_t_1)/3 + scale);
                                if( inc_delta_bez_2 > 0 ) delta_2 = (1-delta_t_2)/3 + scale;
                                else delta_2 = -((delta_t_2)/3 + scale);
                                if( num_bez_1 == 1 )
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_1 + delta_1,
                                                    Point( el_p1.x, el_p1.y+W1 ),
                                                    Point( el_p2.x, el_p2.y+W1 ),
                                                    Point( el_p3.x, el_p3.y+W1 ),
                                                    Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_1 + delta_1,
                                                    Point( el_p1.x, el_p1.y+W1 ),
                                                    Point( el_p2.x, el_p2.y+W1 ),
                                                    Point( el_p3.x, el_p3.y+W1 ),
                                                    Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, 2, true ) );
                                if( num_bez_1 == 0 )
                                    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_1 + delta_1,
                                                    Point( el_p1.x, el_p1.y-W1 ),
                                                    Point( el_p2.x, el_p2.y-W1 ),
                                                    Point( el_p3.x, el_p3.y-W1 ),
                                                    Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_1 + delta_1,
                                                    Point( el_p1.x, el_p1.y-W1 ),
                                                    Point( el_p2.x, el_p2.y-W1 ),
                                                    Point( el_p3.x, el_p3.y-W1 ),
                                                    Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, 2, true ) );
                    
                                if( num_bez_2 == 1 )
                                    delta_point_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_2 + delta_2,
                                                    Point( P1.x, P1.y+W2 ),
                                                    Point( P2.x, P2.y+W2 ),
                                                    Point( P3.x, P3.y+W2 ),
                                                    Point( P4.x, P4.y+W2 ) ), ang1 ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_2 + delta_2,
                                                    Point( P1.x, P1.y+W2 ),
                                                    Point( P2.x, P2.y+W2 ),
                                                    Point( P3.x, P3.y+W2 ),
                                                    Point( P4.x, P4.y+W2 ) ), ang1 ).y, 2, true ) );
                                if( num_bez_2 == 0 )
                                    delta_point_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_2 + delta_2,
                                                    Point( P1.x, P1.y-W2 ),
                                                    Point( P2.x, P2.y-W2 ),
                                                    Point( P3.x, P3.y-W2 ),
                                                    Point( P4.x, P4.y-W2 ) ), ang1 ).x, 2, true ),
                                                           (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_2 + delta_2,
                                                    Point( P1.x, P1.y-W2 ),
                                                    Point( P2.x, P2.y-W2 ),
                                                    Point( P3.x, P3.y-W2 ),
                                                    Point( P4.x, P4.y-W2 ) ), ang1 ).y, 2, true ) );
                            }
                        }
                        else
                        {
                            new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
                            if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.2 + scale;
                            else delta_real = 0.8 - scale;
                            delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, 2, true ),
                                                   (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, 2, true ) );
                            if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
                            else delta_real = 0.8 - scale;
                            delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, 2, true ),
                                                   (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, 2, true ) );
                        }
                    }
                    //-- Detecting the real "fill" point as the middle of the distance between two points on the figures --
                    delta_point_center.x = (delta_point_1.x+delta_point_2.x)/2;
                    delta_point_center.y = (delta_point_1.y+delta_point_2.y)/2;
                    //-- Calling fill procedure for each fill with the real "fill" point --
                    if( (int)(delta_point_center.x+0.5) < s_width && (int)(delta_point_center.y+0.5) < s_height )
                        paintFill( im, delta_point_center, inundationItems[i], tmp_clr );
                }
            }
        //- Changing the color to the real one for all figures used in each fill(inundation)
        for( int j = 0; j < shape_temp.size(); j++ )
        {
            shapeItems[shape_temp[j]].width = width_shape[j];
            shapeItems[shape_temp[j]].border_width = border_width_shape[j];
            shapeItems[shape_temp[j]].lineColor = line_color_shape[j];
            shapeItems[shape_temp[j]].borderColor = border_color_shape[j];
        }
        }
    }
    //- Painting all figures -
    //-- Repainting the borders of the line with the color of the line instead their borders' color (for "dashed", "dotted" figures) --
    for( int i = 0; i < shapeItems.size(); i++ )
        if( shapeItems[i].type == 2 && shapeItems[i].border_width > 0 && shapeItems[i].style != 0 && !shapeItems[i].flag_brd )
            paintFigure( im, shapeItems[i], xSc, ySc, true, false, true );
    for( int i = 0; i < shapeItems.size(); i++ )
        if( shapeItems[i].type != 2 && shapeItems[i].border_width > 0 && shapeItems[i].style != 0 && !shapeItems[i].flag_brd )
            paintFigure( im, shapeItems[i], xSc, ySc, true, false, true );
    int styleDotted[2];
    int clr;
    int for_length_1, for_length_2;
    for( int i = 0; i < shapeItems.size(); i++ )
        if( shapeItems[i].type == 2 )
            paintFigure( im, shapeItems[i], xSc, ySc, true, true, false );
    for( int i = 0; i < shapeItems.size(); i++ )
        if( shapeItems[i].type != 2 )
            paintFigure( im, shapeItems[i], xSc, ySc, true, true, false );
    //- Get image and transfer it -
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtr(im, &img_sz);
    ses.page.assign(img_ptr,img_sz);
    ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    gdFree(img_ptr);
}

void VCAElFigure::postReq( SSess &ses )
{
    ResAlloc res(mRes,true);

    map< string, string >::iterator prmEl = ses.prm.find("sub");
    if( prmEl != ses.prm.end() && prmEl->second == "point");
    {
	prmEl = ses.prm.find("x");
	int x_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : -1;
	prmEl = ses.prm.find("y");
	int y_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : -1;
	prmEl = ses.prm.find("key");
	string key = (prmEl!=ses.prm.end()) ? prmEl->second : "";
	if( x_coord < 0 || y_coord < 0 ) return;

	int color = gdImageGetPixel( im, x_coord, y_coord );
	string sev;
	for( int i=0; i < inundationItems.size(); i++ )
	    if( inundationItems[i].index_color == color )
		sev="ws_Fig"+TSYS::int2str(i);
	if( !sev.empty() )
	{
	    XMLNode req("set");
	    req.setAttr("path",ses.url+"/%2fserv%2fattr");
	    req.childAdd("el")->setAttr("id","event")->setText(sev+key);
	    req.childAdd("el")->setAttr("id","event")->setText("ws_FocusIn");
	    req.childAdd("el")->setAttr("id","focus")->setText("1");
	    mod->cntrIfCmd(req,ses.user);
	}
    }
}

void VCAElFigure::setAttrs( XMLNode &node, const string &user )
{
    ResAlloc res(mRes,true);
    XMLNode *req_el;
    Point StartMotionPos;
    Point EndMotionPos;
    Point CtrlMotionPos_1;
    Point CtrlMotionPos_2;
    Point CtrlMotionPos_3;
    Point CtrlMotionPos_4;
    double t_start, t_end, a, b, ang_t, ang;
    int MotionWidth;
    //pnts.clear();
    //shapeItems.clear();
    rel_list=false;
    for( int i_a = 0; i_a < node.childSize(); i_a++ )
    {
	req_el = node.childGet(i_a);
	int uiPrmPos = atoi(req_el->attr("pos").c_str());
        
	switch( uiPrmPos )
	{
	    case 6:	//active
                active = (bool)atoi(req_el->text().c_str());
        	break;
            case 9: 	//width
		width = atof(req_el->text().c_str());
		break;
            case 10:	//height
		height = atof(req_el->text().c_str());
		break;
    	    case 12:	//geomMargin
        	geomMargin = atoi(req_el->text().c_str());
		break;
            case 20:	//lineWdth
                lineWdth = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 21:	//lineClr
                lineClr =  mod->colorParse(req_el->text());
                if(lineClr==-1)  lineClr=0;
                rel_list = true;
                break;
            case 22:	//lineStyle
                lineStyle = req_el->text();
                rel_list = true;
                break;
            case 23:	//bordWdth
                bordWdth = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 24:	//bordClr
                bordClr = mod->colorParse(req_el->text());
                if(bordClr==-1)  bordClr=0;
                rel_list = true;
                break;
            case 25:	//fillClr
                fillClr = mod->colorParse(req_el->text());
                if(fillClr==-1)  fillClr=0;
                rel_list = true;
                break;
            case 26:
            {
                /* XMLNode req("get");
                req.setAttr("path",id()+"/%2fwdg%2fres")->setAttr("id",req.text());
                mod->cntrIfCmd(req,user);*/
                //imgDef = TSYS::strDecode(req.text(),TSYS::base64);
                imgDef = req_el->text();
                rel_list = true;
                break;
            }
            case 28:
            {
                orient = atof(req_el->text().c_str());
                rel_list = true;
                break;   
            }
            case 27:	//elLst
                elLst = req_el->text();
                rel_list = true;
                break;
            default:
                if( uiPrmPos >= 30 )
                {
                    int pnt  = (uiPrmPos-30)/6;
                    int patr = (uiPrmPos-30)%6;
                    double pval  = atof(req_el->text().c_str());
                    Point pnt_ = (pnts)[pnt];
                    if( patr == 0 ) pnt_.x=pval;
                    else pnt_.y=pval;
                    (pnts)[pnt] = pnt_;
                }
	}
    }
    if( rel_list)
    {
        for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); pi++ )
            if(pi->first <= -10 ) pnts.erase ( pi );
        string sel;
        int map_index = -10;
        int  p[5];
        int lnwidth;
        int bord_width;
        int color;
        int bord_color;
        string ln_st;
        int style;
        double t_start, t_end, a, b, ang_t, ang;
        int MotionWidth;
        Point ip[5];
        shapeItems.clear();
        inundationItems.clear();
        for( int off = 0; (sel=TSYS::strSepParse(elLst,0,'\n',&off)).size(); )
        {
            int el_off = 0;
            string el = TSYS::strSepParse(sel,0,':',&el_off);
            if( el == "line" )
            {
                bool fl_wdt = false;
                //-- Reading anf setting attributes for the current line --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }

                lnwidth = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !lnwidth ) lnwidth=lineWdth;
                color = mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( color==-1 ) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if(!bord_width) bord_width=bordWdth;
                bord_color= mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (bord_color==-1)  bord_color=bordClr; 
                ln_st = TSYS::strSepParse(sel,0,':',&el_off);
                if( !ln_st.size() ) ln_st = lineStyle;
                if( ln_st == "solid" ) style = 0;
                else if( ln_st == "dashed" ) style = 1;
                else if( ln_st == "dotted" ) style = 2;
                else style = 0;
                //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 2; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                if( ip[0].y<=ip[1].y )
                    ang=360-angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                else
                    ang=angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                bool flag_brd = false;
                if( lnwidth > 3 && bord_width == 0 )
                {
                    lnwidth -= 2;
                    bord_width = 1; 
                    bord_color = color;
                    flag_brd = true;
                }
                shapeItems.push_back( ShapeItem(p[0],p[1],-1,-1,-1,Point(0,0), ang,
                                      color,bord_color,lnwidth,bord_width,1,style,flag_brd));
            }
            if( el == "arc" )
            {
                bool fl_wdt = false;
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str());
                else 
                {
                    p[2] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str());
                else 
                {
                    p[3] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[4]  = atoi(el_s.c_str());
                else 
                {
                    p[4] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }

                lnwidth = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !lnwidth ) lnwidth=lineWdth;
                color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( color==-1 ) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=bordWdth;
                bord_color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (bord_color==-1)  bord_color=bordClr;
                ln_st = TSYS::strSepParse(sel,0,':',&el_off);
                if( !ln_st.size() ) ln_st = lineStyle;
                if( ln_st == "solid" ) style = 0;
                else if( ln_st == "dashed" ) style = 1;
                else if( ln_st == "dotted" ) style = 2;
                else style = 0;
                  //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                StartMotionPos=ip[0];
                EndMotionPos=ip[1];
                CtrlMotionPos_1=ip[2];
                CtrlMotionPos_2=ip[3];
                CtrlMotionPos_3=ip[4];
                MotionWidth=lnwidth;
                if (CtrlMotionPos_3.y<=CtrlMotionPos_1.y) 
                    ang=angle(CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y));
                else ang=360-angle(CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y));
                a=length(CtrlMotionPos_3, CtrlMotionPos_1);
                b=length(CtrlMotionPos_2, CtrlMotionPos_1);

                CtrlMotionPos_2=Point(CtrlMotionPos_1.x+rotate(arc(0.25,a,b),ang).x,
                                      CtrlMotionPos_1.y-rotate(arc(0.25,a,b),ang).y);
                StartMotionPos=unrotate(StartMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y);
                if (StartMotionPos.x>=a)
                {
                    StartMotionPos.y=(StartMotionPos.y/StartMotionPos.x)*a;
                    StartMotionPos.x=a;
                }
                if (StartMotionPos.x<-a)
                {
                    StartMotionPos.y=(StartMotionPos.y/StartMotionPos.x)*(-a);
                    StartMotionPos.x=-a;
                }
                if(StartMotionPos.y<=0)

                    t_start=acos(StartMotionPos.x/a)/(2*M_PI);
                else
                    t_start=1-acos(StartMotionPos.x/a)/(2*M_PI);
                EndMotionPos=unrotate(EndMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y);
                if (EndMotionPos.x<-a)
                {
                    EndMotionPos.y=(EndMotionPos.y/EndMotionPos.x)*(-a);
                    EndMotionPos.x=-a;
                }
                if (EndMotionPos.x>=a)
                {
                    EndMotionPos.y=(EndMotionPos.y/EndMotionPos.x)*a;
                    EndMotionPos.x=a;
                }
                if(EndMotionPos.y<=0)
                    t_end=acos(EndMotionPos.x/a)/(2*M_PI);
                else
                    t_end=1-acos(EndMotionPos.x/a)/(2*M_PI);
                if (t_start>t_end) t_end+=1;
                if ((t_end-1)>t_start) t_end-=1;
                if (t_start==t_end) t_end+=1;
                if (t_end>t_start && t_start>=1 && t_end>1)
                {
                    t_start-=1;
                    t_end-=1;
                }
                CtrlMotionPos_4=Point(t_start, t_end);
                bool flag_brd = false;
                if( lnwidth > 3 && bord_width == 0 )
                {
                    lnwidth -= 2;
                    bord_width = 1; 
                    bord_color = color;
                    flag_brd = true;
                }
                shapeItems.push_back(ShapeItem(p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,ang,color,bord_color,lnwidth,bord_width,2,style,flag_brd));
            }
            if( el == "bezier" )
            {
                bool fl_wdt = false;
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[0] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[0] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); pi++ )
                        if(pi->first <= -10 )
                            if( fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01 &&
                                fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01 )  
                            {
                                p[1] = pi->first;
                                fl = true;
                                break;
                            }
                    if( !fl )
                    {
                        p[1] = map_index;
                        (pnts)[map_index] = Point(x_s,y_s);
                        map_index -= 1;
                    }
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str());
                else 
                {
                    p[2] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str());
                else 
                {
                    p[3] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }

                lnwidth = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !lnwidth ) lnwidth=lineWdth;
                color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( color==-1) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=bordWdth;
                bord_color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (bord_color==-1)  bord_color=bordClr;
                ln_st = TSYS::strSepParse(sel,0,':',&el_off);
                if( !ln_st.size() ) ln_st = lineStyle;
                if( ln_st == "solid" ) style = 0;
                else if( ln_st == "dashed" ) style = 1;
                else if( ln_st == "dotted" ) style = 2;
                else style = 0;
                for( int i_p = 0; i_p < 4; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                if( ip[0].y<=ip[1].y ) ang=360-angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                else ang=angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                bool flag_brd = false;
                if( lnwidth > 3 && bord_width == 0 )
                {
                    lnwidth -= 2;
                    bord_width = 1; 
                    bord_color = color;
                    flag_brd = true;
                }
                shapeItems.push_back( ShapeItem(p[0], p[1], p[2], p[3],-1,Point(0,0),ang,color,bord_color,lnwidth,bord_width,3,style,flag_brd));
            }
            if( el == "fill" )
            {
                int zero_pnts = 0;
                string fl_color_1, fl_img;
                vector <int> fl_pnts;
                float x_s, y_s;
                int vl;
                while( true )
                {
                    string svl = TSYS::strSepParse(sel,0,':',&el_off);
                    if( sscanf(svl.c_str(), "(%f,%f)", &x_s, &y_s) != 2 ) vl = atoi(svl.c_str());
                    else 
                    {
                        bool fl = false;
                        //-- Detecting if there is a point with same coordinates in the map --
                        for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); pi++ )
                            if(pi->first <= -10 )
                            {
                                if( (fabs(TSYS::realRound(x_s,2) -  TSYS::realRound(pi->second.x,2)) < 0.01) &&
                                    (fabs(TSYS::realRound(y_s,2) -  TSYS::realRound(pi->second.y,2)) < 0.01) )  
                                    {
                                        vl = pi->first;
                                        fl = true;
                                        break;
                                    }
                            }
                        if( !fl )
                        {
                            vl = map_index;
                            (pnts)[map_index] = Point(x_s,y_s);
                            map_index -= 1;
                        }

                    }
                    if( vl ) fl_pnts.push_back(vl);
                    else if( zero_pnts == 0 ) { fl_color_1 = svl; zero_pnts++; }
                    else if( zero_pnts == 1 ) { fl_img= svl; zero_pnts++; }
                    else break;
                }
                int fl_color=mod->colorParse(fl_color_1);
                if( fl_color==-1) fl_color=fillClr;
                if ( fl_img.size() )
                    inundationItems.push_back(InundationItem(fl_pnts, fl_color, -1, fl_img));
                else
                    inundationItems.push_back(InundationItem(fl_pnts, fl_color, -1, imgDef));
            }
        }
    }
}



//*************************************************
//* VCADiagram                                    *
//*************************************************
VCADiagram::VCADiagram( const string &iid ) : VCAObj(iid), tTimeCurent(false), tTime(0), lstTrc(false)
{

}

void VCADiagram::getReq( SSess &ses )
{
    ResAlloc res(mRes,true);

    //-- Check for trend's data reload --
    bool rld = true;
    if( tTimeCurent )	tTime = (long long)time(NULL)*1000000;
    else if( trcPer && lstTrc < time(NULL) )
    { tTime += (time(NULL)-lstTrc)*1000000; lstTrc = time(NULL); }
    else rld = false;
    if( rld )
    {
	for( int i_p = 0; i_p < trnds.size(); i_p++ ) trnds[i_p].loadData(ses.user);
	//- Trace cursors value -
	if( active )
	{
	    long long tTimeGrnd = tTime - (long long)(tSize*1000000.);
	    if( curTime >= (tTime-2*(long long)trcPer*1000000) || curTime <= tTimeGrnd )
		setCursor(tTime,ses.user);
	}
    }

    int mrkHeight = 0;
    int clr_grid, clr_mrk;						//Colors

    //-- Get generic parameters --
    int parNum     = trnds.size();					//Parameter's number
    long long tSz  = (long long)(tSize*1000000.);			//Trends size (us)
    long long tEnd = tTime;						//Trends end point (us)
    tPict = tEnd;
    long long tBeg = tEnd - tSz;					//Trends begin point (us)
    if( !parNum || tSz <= 0 )
    {
	ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
	return;
    }

    //-- Get scale --
    map< string, string >::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    int imW = (int)TSYS::realRound((double)width*xSc,2,true);
    int imH = (int)TSYS::realRound((double)height*ySc,2,true);

    //- Prepare picture -
    gdImagePtr im = gdImageCreate(imW,imH);
    gdImageFilledRectangle(im,0,0,imW-1,imH-1,gdImageColorAllocateAlpha(im,0,0,0,127));

    //-- Make decoration and prepare trends area --
    tArX = 1, tArY = 1,						//Curves of trends area rect
    tArW = imW-2*(geomMargin+bordWidth+1),
    tArH = imH-2*(geomMargin+bordWidth+1);

    if( sclHor&0x3 || sclVer&0x3 )
    {
	//--- Set grid color ---
	clr_grid = gdImageColorAllocate(im,(ui8)(sclColor>>16),(ui8)(sclColor>>8),(ui8)sclColor);
	if( sclHor&0x2 || sclVer&0x2 )
	{
	    //--- Set markers font and color ---
	    clr_mrk = gdImageColorAllocate(im,(ui8)(sclMarkColor>>16),(ui8)(sclMarkColor>>8),(ui8)sclMarkColor);
	    //!!!! Want set font from "sclMarkFont"
	    mrkHeight = gdFontTiny->h;
	    if( sclHor&0x2 )
	    {
	        if( tArH < 100 ) sclHor &= ~(0x02);
	        else tArH -= 2*(mrkHeight+2);
	    }
	    if( sclVer&0x2 && tArW < 100 ) sclVer &= ~(0x02);
	}
    }
    //--- Calc horizontal scale ---
    long long aVend;							//Corrected for allow data the trend end point
    long long aVbeg;							//Corrected for allow data the trend begin point
    long long hDiv = 1, hDivBase = 1;					//Horisontal scale divisor

    int hmax_ln = tArW/((sclHor&0x2)?40:15);
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = tEnd - tBeg;
	if( hLen/86400000000ll >= 2 )    { hvLev = 5; hDivBase = hDiv = 86400000000ll; }	//Days
	else if( hLen/3600000000ll >= 2 ){ hvLev = 4; hDivBase = hDiv =  3600000000ll; }	//Hours
	else if( hLen/60000000 >= 2 )    { hvLev = 3; hDivBase = hDiv =    60000000; }		//Minutes
	else if( hLen/1000000 >= 2 )     { hvLev = 2; hDivBase = hDiv =     1000000; }		//Seconds
	else if( hLen/1000 >= 2 )        { hvLev = 1; hDivBase = hDiv =        1000; }		//Milliseconds
	while( hLen/hDiv > hmax_ln )     hDiv *= 10;
	while( hLen/hDiv < hmax_ln/2 )   hDiv /= 2;
	if( hLen/hDiv >= 5 && trcPer )
	{
	    tPict = hDiv*(tEnd/hDiv+1);
	    tBeg = tPict-hLen;
	}

	//--- Draw horisontal grid and markers ---
	if( sclHor&0x3 )
	{
	    time_t tm_t;
	    struct tm ttm, ttm1;
	    char lab_tm[50], lab_dt[50];
	    //---- Draw generic grid line ----
	    gdImageLine(im,tArX,tArY+tArH,tArX+tArW,tArY+tArH,clr_grid);
	    //---- Draw full trend's data and time to the trend end position ----
	    int begMarkBrd = -1;
	    int endMarkBrd = tArX+tArW;
	    if( sclHor&0x2 )
	    {
		tm_t = tPict/1000000;
		localtime_r(&tm_t,&ttm);
		snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
		if( ttm.tm_sec == 0 && tPict%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if( tPict%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
		else snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(tPict%1000000)/1e6);
		gdImageString(im,gdFontTiny,tArX+tArW-gdFontTiny->w*strlen(lab_dt),tArY+tArH+3+gdFontTiny->h,(unsigned char *)lab_dt,clr_mrk);
		gdImageString(im,gdFontTiny,tArX+tArW-gdFontTiny->w*strlen(lab_tm),tArY+tArH+3,(unsigned char *)lab_tm,clr_mrk);
		endMarkBrd = vmin(tArX+tArW-gdFontTiny->w*strlen(lab_dt),tArX+tArW-gdFontTiny->w*strlen(lab_tm));
	    }

	    //---- Draw grid and/or markers ----
	    bool first_m = true;
	    for( long long i_h = tBeg; true; )
	    {
		//---- Draw grid ----
		int h_pos = tArX+tArW*(i_h-tBeg)/(tPict-tBeg);
		if( sclHor&0x1 ) gdImageLine(im,h_pos,tArY,h_pos,tArY+tArH,clr_grid);
		else gdImageLine(im,h_pos,tArY+tArH-3,h_pos,tArY+tArH+3,clr_grid);
		//---- Draw markers ----
		if( sclHor&0x2 && !(i_h%hDiv) && i_h != tPict )
		{
		    tm_t = i_h/1000000;
		    localtime_r(&tm_t,&ttm);
		    int chLev = -1;
		    if( !first_m )
		    {
			if( ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year )  chLev = 5;
			else if( ttm.tm_mday > ttm1.tm_mday )  chLev = 4;
			else if( ttm.tm_hour > ttm1.tm_hour )  chLev = 3;
			else if( ttm.tm_min > ttm1.tm_min )    chLev = 2;
			else if( ttm.tm_sec > ttm1.tm_sec )    chLev = 1;
			else chLev = 0;
		    }
		    //Check for data present
		    lab_dt[0] = lab_tm[0] = 0;
		    if( hvLev == 5 || chLev >= 4 )					//Date
			(chLev>=5 || chLev==-1) ? snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) :
				     snprintf(lab_dt,sizeof(lab_dt),"%d",ttm.tm_mday);
		    if( (hvLev == 4 || hvLev == 3 || ttm.tm_min) && !ttm.tm_sec )	//Hours and minuts
			snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm.tm_hour,ttm.tm_min);
		    else if( (hvLev == 2 || ttm.tm_sec) && !(i_h%1000000) )		//Seconds
			(chLev>=2 || chLev==-1) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) :
				     snprintf(lab_tm,sizeof(lab_tm),"%ds",ttm.tm_sec);
		    else if( hvLev <= 1 || i_h%1000000 )				//Milliseconds
			(chLev>=2 || chLev==-1) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
			(chLev>=1) ? snprintf(lab_tm,sizeof(lab_tm),"%gs",(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				     snprintf(lab_tm,sizeof(lab_tm),"%gms",(double)(i_h%1000000)/1000.);
		    int wdth, tpos, endPosTm = 0, endPosDt = 0;
		    if( lab_tm[0] )
		    {
			wdth = gdFontTiny->w*strlen(lab_tm);
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    gdImageString(im,gdFontTiny,tpos,tArY+tArH+3,(unsigned char *)lab_tm,clr_mrk);
			    endPosTm = tpos+wdth;
			}
		    }
		    if( lab_dt[0] )
		    {
			wdth = gdFontTiny->w*strlen(lab_dt);
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    gdImageString(im,gdFontTiny,tpos,tArY+tArH+3+gdFontTiny->h,(unsigned char *)lab_dt,clr_mrk);
			    endPosDt = tpos+wdth;
			}
		    }
		    begMarkBrd = vmax(begMarkBrd,vmax(endPosTm,endPosDt));
		    memcpy((char*)&ttm1,(char*)&ttm,sizeof(tm));
		    first_m = false;
		}
		//---- Next ----
		if( i_h >= tPict )	break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tPict )	i_h = tPict;
	    }
	}
    }

    //--- Calc vertical scale ---
    //---- Check for scale mode ----
    double vsMax = 100, vsMin = 0;      //Trend's vertical scale border
    bool   vsPerc = true;               //Vertical scale percent mode
    if( parNum == 1 )
    {
	vsPerc = false;
	if( trnds[0].bordU() <= trnds[0].bordL() )
	{
	    //----- Check trend for valid data -----
	    aVbeg = vmax(tBeg,trnds[0].valBeg());
	    aVend = vmin(tEnd,trnds[0].valEnd());

	    if( aVbeg >= aVend )
	    {
		gdImageDestroy(im);
		ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
		return;
	    }
	    //----- Calc value borders -----
	    vsMax = -3e300, vsMin = 3e300;
	    bool end_vl = false;
	    int ipos = trnds[0].val(aVbeg);
	    if( ipos && trnds[0].val()[ipos].tm > aVbeg ) ipos--;
	    while( true )
	    {
		if( ipos >= trnds[0].val().size() || end_vl )	break;
		if( trnds[0].val()[ipos].tm >= aVend )	end_vl = true;
		if( trnds[0].val()[ipos].val != EVAL_REAL )
		{
		    vsMin  = vmin(vsMin,trnds[0].val()[ipos].val);
		    vsMax  = vmax(vsMax,trnds[0].val()[ipos].val);
		}
		ipos++;
	    }
	    if( vsMax == -3e300 )	{ vsMax = 1.0; vsMin = 0.0; }
	    else if( vsMax == vsMin )	{ vsMax += 1.0; vsMin -= 1.0; }
	    else if( (vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.001 )
	    {
		double wnt_dp = 0.001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
		vsMin -= wnt_dp/2;
		vsMax += wnt_dp/2;
	    }
	}
	else { vsMax = trnds[0].bordU(); vsMin = trnds[0].bordL(); }
    }

    float vmax_ln = tArH/20;
    if( vmax_ln >= 2 )
    {
	double vDiv = 1.;
	double v_len = vsMax - vsMin;
	while( v_len > vmax_ln )	{ vDiv*=10.; v_len/=10.; }
	while( v_len < vmax_ln/10 )	{ vDiv/=10.; v_len*=10.; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while(((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv/=2;

	//--- Draw vertical grid and markers ---
	if( sclVer&0x3 )
	{
	    char lab_vl[50];
	    gdImageLine(im,tArX,tArY,tArX,tArH,clr_grid);
	    for( double i_v = vsMin; i_v <= vsMax; i_v+=vDiv )
	    {
		//---- Draw grid ----
		int v_pos = tArY+tArH-(int)((double)tArH*(i_v-vsMin)/(vsMax-vsMin));
		if( sclVer&0x1 ) gdImageLine(im,tArX,v_pos,tArX+tArW,v_pos,clr_grid);
		else gdImageLine(im,tArX-3,v_pos,tArX+3,v_pos,clr_grid);
		//---- Draw markers ----
		if( sclVer&0x2 )
		    gdImageString(im,gdFontTiny,tArX+2,v_pos-((i_v==vsMax)?0:gdFontTiny->h),(unsigned char *)TSYS::real2str(i_v).c_str(),clr_mrk);
	    }
	}
    }

    //-- Draw trends --
    for( int i_t = 0; i_t < parNum; i_t++ )
    {
	//--- Set trend's pen ---
	int clr_t = gdImageColorAllocate(im,(ui8)(trnds[i_t].color()>>16),(ui8)(trnds[i_t].color()>>8),(ui8)trnds[i_t].color());

	//--- Prepare generic parameters ---
	aVbeg = vmax(tBeg,trnds[i_t].valBeg());
	aVend = vmin(tEnd,trnds[i_t].valEnd());
	if( aVbeg >= aVend ) continue;
	int aPosBeg = trnds[i_t].val(aVbeg);;
	if( aPosBeg && trnds[i_t].val()[aPosBeg].tm > aVbeg ) aPosBeg--;

	//--- Prepare border for percent trend ---
	float bordL = trnds[i_t].bordL();
	float bordU = trnds[i_t].bordU();
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    bool end_vl = false;
	    int ipos = aPosBeg;
	    while( true )
	    {
		if( ipos >= trnds[i_t].val().size() || end_vl )	break;
		if( trnds[i_t].val()[ipos].tm >= aVend )	end_vl = true;
		if( trnds[i_t].val()[ipos].val != EVAL_REAL )
		{
		    bordL = vmin(bordL,trnds[i_t].val()[ipos].val);
		    bordU = vmax(bordU,trnds[i_t].val()[ipos].val);
		}
		ipos++;
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL-= vMarg;
	    bordU+= vMarg;
	}

	//--- Draw trend ---
	bool	end_vl = false;
	double	curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int	curPos, averPos = 0, prevPos = 0;
	long long curTm, averTm = 0, averLstTm = 0;
	for( int a_pos = aPosBeg; true; a_pos++ )
	{
	    curTm = vmin(aVend,vmax(aVbeg,trnds[i_t].val()[a_pos].tm));
	    if( a_pos < trnds[i_t].val().size() && !end_vl )
	    {
		curVl = trnds[i_t].val()[a_pos].val;
		if( vsPerc && curVl != EVAL_REAL )
		{
		    curVl = 100.*(curVl-bordL)/(bordU-bordL);
		    curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
		}
		if( isnan(curVl) ) curVl = EVAL_REAL;
		curPos = tArX+tArW*(curTm-tBeg)/(tPict-tBeg);
	    }else curPos = 0;
	    if( trnds[i_t].val()[a_pos].tm >= aVend )	end_vl = true;
	    //Square Average
	    if( averPos == curPos )
	    {
		if( !(2*curTm-averTm-averLstTm) ) continue;
		if( averVl == EVAL_REAL )  averVl = curVl;
		else if( curVl != EVAL_REAL )
		    averVl = (averVl*(double)(curTm-averTm)+curVl*(double)(curTm-averLstTm))/
			     ((double)(2*curTm-averTm-averLstTm));
		averLstTm = curTm;
		continue;
	    }
	    //Write point and line
	    if( averVl != EVAL_REAL )
	    {
		int c_vpos = tArY+tArH-(int)((double)tArH*(averVl-vsMin)/(vsMax-vsMin));
		gdImageSetPixel(im,averPos,c_vpos,clr_t);
		if( prevVl != EVAL_REAL )
		{
		    int c_vpos_prv = tArY+tArH-(int)((double)tArH*(prevVl-vsMin)/(vsMax-vsMin));
		    gdImageLine(im,prevPos,c_vpos_prv,averPos,c_vpos,clr_t);
		}
	    }
	    prevVl  = averVl;
	    prevPos = averPos;
	    averVl  = curVl;
	    averPos = curPos;
	    averTm  = averLstTm = curTm;
	    if( !curPos ) break;
	}
    }

    //- Draw cursor -
    if( active && curTime && tBeg && tPict && (curTime >= tBeg || curTime <= tPict) )
    {
	//--- Set trend's pen ---
	int clr_cur = gdImageColorAllocate(im,(ui8)(curColor>>16),(ui8)(curColor>>8),(ui8)curColor);
	int curPos = tArX+tArW*(curTime-tBeg)/(tPict-tBeg);
	gdImageLine(im,curPos,tArY,curPos,tArY+tArH,clr_cur);
    }

    //- Get image and transfer it -
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtr(im, &img_sz);
    ses.page.assign(img_ptr,img_sz);
    ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    gdFree(img_ptr);
    gdImageDestroy(im);
}

void VCADiagram::postReq( SSess &ses )
{
    ResAlloc res(mRes,true);

    map< string, string >::iterator prmEl = ses.prm.find("sub");
    if( prmEl != ses.prm.end() && prmEl->second == "point" );
    {
	prmEl = ses.prm.find("x");
	int x_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : 0;
	prmEl = ses.prm.find("y");
	int y_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : 0;

	if( x_coord >= tArX && x_coord <= tArX+tArW )
	{
	    long long tTimeGrnd = tPict - (long long)(tSize*1000000.);
	    setCursor(  tTimeGrnd + (tPict-tTimeGrnd)*(x_coord-tArX)/tArW, ses.user );
	}
    }
}

void VCADiagram::setAttrs( XMLNode &node, const string &user )
{
    ResAlloc res(mRes,true);

    int  reld_tr_dt = 0;        //Reload trend's data ( 1-reload addons, 2-full reload)

    XMLNode *req_el;
    for( int i_a = 0; i_a < node.childSize(); i_a++ )
    {
	req_el = node.childGet(i_a);
	int uiPrmPos = atoi(req_el->attr("pos").c_str());
	switch( uiPrmPos )
	{
	    case 6:	//active
		active = (bool)atoi(req_el->text().c_str());
		break;
	    case 9: 	//width
		width = (int)(atof(req_el->text().c_str())+0.5);
		break;
	    case 10:	//height
		height = (int)(atof(req_el->text().c_str())+0.5);
		break;
	    case 12:	//geomMargin
		geomMargin = atoi(req_el->text().c_str());
		break;
	    case 22:	//bordWidth
		bordWidth = atoi(req_el->text().c_str());
		break;
	    case 24:	//trcPer
		trcPer = atoi(req_el->text().c_str());
		break;
	    case 26:	//tSek
		tTimeCurent = false;
		if( atoll(req_el->text().c_str()) == 0 )
		{
		    tTime = (long long)time(NULL)*1000000;
		    tTimeCurent = true;
		} else tTime = atoll(req_el->text().c_str())*1000000 + tTime%1000000;
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case 27:	//tUSek
		tTime = 1000000ll*(tTime/1000000)+atoll(req_el->text().c_str());
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case 28:	//tSize
		tSize = atof(req_el->text().c_str());
		reld_tr_dt = 1;
		break;
	    case 29:	//curSek
		curTime = atoll(req_el->text().c_str())*1000000 + curTime%1000000;	break;
	    case 30:	//curUSek
		curTime = 1000000ll*(curTime/1000000)+atoll(req_el->text().c_str());	break;
	    case 36:	//curColor
		curColor = mod->colorParse(req_el->text());				break;
	    case 31:	//sclColor
		sclColor = mod->colorParse(req_el->text());				break;
	    case 32:	//sclHor
		sclHor = atoi(req_el->text().c_str());					break;
	    case 33:	//sclVer
		sclVer = atoi(req_el->text().c_str());					break;
	    case 37:	//sclMarkColor
		sclMarkColor = mod->colorParse(req_el->text());				break;
	    case 38:	//sclMarkFont
		sclMarkFont = req_el->text();						break;
	    case 34:	//valArch
		valArch == req_el->text();
		reld_tr_dt = 2;
		break;
	    case 35:	//parNum
	    {
		int parNum = atoi(req_el->text().c_str());
		if( parNum == trnds.size() )	break;
		while( trnds.size() > parNum )	trnds.pop_back();
		while( parNum > trnds.size() )	trnds.push_back( TrendObj(this) );
		break;
	    }
	    default:
		//- Individual trend's attributes process -
		if( uiPrmPos >= 50 && uiPrmPos < 150 )
		{
		    int trndN = (uiPrmPos/10)-5;
		    if( trndN >= trnds.size() )	break;
		    switch( uiPrmPos%10 )
		    {
			case 0: trnds[trndN].setAddr(req_el->text());			break;	//addr
			case 1: trnds[trndN].setBordL(atof(req_el->text().c_str()));	break;	//bordL
			case 2: trnds[trndN].setBordU(atof(req_el->text().c_str()));	break;	//bordU
			case 3: trnds[trndN].setColor(mod->colorParse(req_el->text()));	break;	//color
			case 4: trnds[trndN].setCurVal(atof(req_el->text().c_str()));	break;	//value
		    }
		}
	}
    }

    if( reld_tr_dt )
	for( int i_p = 0; i_p < trnds.size(); i_p++ )
	    trnds[i_p].loadData(user,reld_tr_dt==2);
}

void VCADiagram::setCursor( long long itm, const string& user )
{
    long long tTimeGrnd = tTime - (long long)(tSize*1000000.);
    curTime = vmax(vmin(itm,tTime),tTimeGrnd);

    XMLNode req("set");
    req.setAttr("path",id()+"/%2fserv%2fattr");
    req.childAdd("el")->setAttr("id","curSek")->setText(TSYS::int2str(curTime/1000000));
    req.childAdd("el")->setAttr("id","curUSek")->setText(TSYS::int2str(curTime%1000000));

    //- Update trend's current values -
    for( int i_p = 0; i_p < trnds.size(); i_p++ )
    {
	int vpos = trnds[i_p].val(curTime);
	if( vpos >= trnds[i_p].val().size() ) continue;
	if( vpos && trnds[i_p].val()[vpos].tm > curTime ) vpos--;
	double val = trnds[i_p].val()[vpos].val;
	if( val != trnds[i_p].curVal() )
	    req.childAdd("el")->setAttr("id","prm"+TSYS::int2str(i_p)+"val")->setText(TSYS::real2str(val,6));
    }
    mod->cntrIfCmd(req,user);
}

//* Trend object's class                         *
//************************************************
VCADiagram::TrendObj::TrendObj( VCADiagram *iowner ) : 
    m_owner(iowner), m_bord_low(0), m_bord_up(0), m_curvl(EVAL_REAL), arh_beg(0), arh_end(0), arh_per(0),val_tp(0)
{
    loadData("root");
}

VCADiagram &VCADiagram::TrendObj::owner( )
{
    return *m_owner;
}

long long VCADiagram::TrendObj::valBeg()
{
    return vals.empty() ? 0 : vals[0].tm;
}

long long VCADiagram::TrendObj::valEnd()
{
    return vals.empty() ? 0 : vals[vals.size()-1].tm;
}

int VCADiagram::TrendObj::val( long long tm )
{
    int i_p = 0;
    for( int d_win = vals.size()/2; d_win > 10; d_win/=2 )
	if( tm < vals[i_p+d_win].tm )   i_p+=d_win;
	    for( int i_p = 0; i_p < vals.size(); i_p++ )
		if( vals[i_p].tm >= tm ) return i_p;
    return vals.size();
}

void VCADiagram::TrendObj::setAddr( const string &vl )
{
    if( vl == m_addr ) return;
    m_addr = vl;
    loadData( "root", true );
}

void VCADiagram::TrendObj::loadData( const string &user, bool full )
{
    long long tSize     = (long long)(owner().tSize*1000000.);
    long long tTime     = owner().tTime;
    long long tTimeGrnd = tTime - tSize;
    long long wantPer   = tSize/(int)(owner().width+0.5);
    string arch = owner().valArch;

    //- Clear trend for empty address and the full reload data -
    if( full || addr().empty() )
    {
	arh_per = arh_beg = arh_end = 0;
	val_tp = 0;
	vals.clear();
	if( addr().empty() )    return;
    }
    //- Get archive parameters -
    if( !arh_per || tTime > arh_end )
    {
	XMLNode req("info");
	req.setAttr("arch",arch)->setAttr("path",addr()+"/%2fserv%2fval");
	if( mod->cntrIfCmd(req,user,false) || atoi(req.attr("vtp").c_str()) == 5 )
	{ arh_per = arh_beg = arh_end = 0; return; }
	else
	{
	    val_tp  = atoi(req.attr("vtp").c_str());
	    arh_beg = atoll(req.attr("beg").c_str());
	    arh_end = atoll(req.attr("end").c_str());
	    arh_per = atoi(req.attr("per").c_str());
	}
    }

    //- One request check and prepare -
    int trcPer = owner().trcPer*1000000;
    if( owner().tTimeCurent && trcPer && (!arh_per || (arh_per >= trcPer && (tTime-valEnd())/trcPer < 2)) )
    {
	XMLNode req("get");
	req.setAttr("path",addr()+"/%2fserv%2fval")->
	    setAttr("tm",TSYS::ll2str(tTime))->
	    setAttr("tm_grnd","0");
	if( mod->cntrIfCmd(req,user,false) ) return;

	long long lst_tm = atoll(req.attr("tm").c_str());
	if( lst_tm > valEnd() )
	{
	    double curVal = atof(req.text().c_str());
	    if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	    if( valEnd() && (lst_tm-valEnd())/trcPer > 2 ) vals.push_back(SHg(lst_tm-trcPer,EVAL_REAL));
	    vals.push_back(SHg(lst_tm,curVal));
	    while( vals.size() > 2000 ) vals.pop_front();
	}
	return;
    }

    if( !arh_per )      return;
    //- Correct request to archive border -
    wantPer	= (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime	= vmin(tTime,arh_end);
    tTimeGrnd	= vmax(tTimeGrnd,arh_beg);
    //- Clear data at time error -
    if( tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer )
	vals.clear();
    if( tTime <= tTimeGrnd ) return;
    //- Check for request to present in buffer data -
    if( tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer )    return;
    //- Correct request to present data -
    if( valEnd() && tTime > valEnd() )          tTimeGrnd = valEnd()+1;
    else if( valBeg() && tTimeGrnd < valBeg() ) tTime = valBeg()-1;
    //- Get values data -
    long long bbeg, bend;
    int bper;
    int         curPos, prevPos;
    double      curVal, prevVal;
    string      svl;
    vector<SHg> buf;
    bool toEnd = (tTimeGrnd >= valEnd());
    int  endBlks = 0;
    XMLNode req("get");
    m1: req.clear()->
	    setAttr("arch",arch)->
	    setAttr("path",addr()+"/%2fserv%2fval")->
	    setAttr("tm",TSYS::ll2str(tTime))->
	    setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
	    setAttr("per",TSYS::ll2str(wantPer))->
	    setAttr("mode","1")->
	    setAttr("real_prec","6")->
	    setAttr("round_perc","0");//TSYS::real2str(100.0/(float)owner().height));
    if( mod->cntrIfCmd(req,user,false) )     return;
    //- Get data buffer parameters -
    bbeg = atoll(req.attr("tm_grnd").c_str());
    bend = atoll(req.attr("tm").c_str());
    bper = atoi(req.attr("per").c_str());

    prevPos = 0;
    prevVal = EVAL_REAL;
    buf.clear();
    for( int v_off = 0; (svl=TSYS::strSepParse(req.text(),0,'\n',&v_off)).size(); )
    {
	sscanf(svl.c_str(),"%d %lf",&curPos,&curVal);
	if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	for( ; prevPos < curPos-1; prevPos++ )  buf.push_back(SHg(bbeg+(prevPos+1)*bper,prevVal));
	buf.push_back(SHg(bbeg+curPos*bper,curVal));
	prevPos = curPos; prevVal = curVal;
    }
    for( ; prevPos < (bend-bbeg)/bper; prevPos++ ) buf.push_back(SHg(bbeg+(prevPos+1)*bper,prevVal));
    //- Append buffer to values deque -
    if( toEnd )
    {
	vals.insert(vals.end()-endBlks,buf.begin(),buf.end());
	while( vals.size() > 2000 )     vals.pop_front();
	endBlks+=buf.size();
    }
    else
    {
	vals.insert(vals.begin(),buf.begin(),buf.end());
	while( vals.size() > 2000 )     vals.pop_back();
    }
    //- Check for archive jump -
    if( arch.empty() && (bbeg-tTimeGrnd)/bper ) { tTime = bbeg-bper; goto m1; }
}

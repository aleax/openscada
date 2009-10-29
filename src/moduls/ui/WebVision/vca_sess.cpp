
//OpenSCADA system module UI.WebVision file: vca_sess.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko (rom_as@fromru.com)       *
 *                           by Lysenko Maxim (mlisenko@oscada.org.ua)     *
 *                           by Yashina Kseniya (ksu@oscada.org.ua)        *
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

#include <stdint.h>

#include <string.h>

#include <tsys.h>
#include <tmess.h>

#include "web_vision.h"
#include "vca_sess.h"

extern char *WebVisionVCA_js;

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
    //> Access time to session is updating
    lst_ses_req = time(NULL);

    map<string,string>::iterator prmEl = ses.prm.find("com");
    string first_lev = TSYS::pathLev(ses.url,1);
    string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";
    if( wp_com.empty() )
    {
	string prjNm;
	//>> Get project's name
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fobj%2fst%2fprj");
	if( !mod->cntrIfCmd(req,ses.user) )
	{
	    req.setAttr("path","/prj_"+req.text()+"/%2fobj%2fcfg%2fname");
	    if( !mod->cntrIfCmd(req,ses.user) )	prjNm = req.text();
	}

	ses.page = mod->pgHead("",prjNm)+"<SCRIPT>\n"+mod->trMessReplace(WebVisionVCA_js)+"\n</SCRIPT>\n"+mod->pgTail();
	ses.page = mod->httpHead("200 OK",ses.page.size())+ses.page;

	//>> Cache clear
	ResAlloc res(nodeRes(),true);
	mCacheRes.clear();
    }
    //> Session/projects icon
    else if( wp_com == "ico" )
    {
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fico");
	mod->cntrIfCmd(req,ses.user);
	ses.page = TSYS::strDecode(req.text(),TSYS::base64);
	ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    }
    //> Get open pages list
    else if( wp_com == "pgOpenList" && first_lev.empty() )
    {
	prmEl = ses.prm.find("tm");
	XMLNode req("openlist");
	req.setAttr("path",ses.url+"/%2fserv%2fpg")->
	    setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
    }
    //> Attribute get
    else if( wp_com == "attr" )
    {
	prmEl = ses.prm.find("attr");
	string attr = (prmEl!=ses.prm.end()) ? prmEl->second : "";

	XMLNode req("get"); req.setAttr("path",ses.url+"/%2fattr%2f"+attr);
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
    }
    //> Widget's (page) full attributes branch request
    else if( wp_com == "attrsBr" )
    {
	prmEl = ses.prm.find("tm");
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fserv%2fattrBr")->setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	mod->cntrIfCmd(req,ses.user);

	//>> Backend objects' attributes set
	vector<int> pos;	int cpos = 0;
	vector<string> addr;	string caddr = ses.url;
	XMLNode *cn = &req;
	while(true)
	{
	    if( cpos < cn->childSize() )
	    {
		if( cn->childGet(cpos)->name() == "w" )
		{
		    cn = cn->childGet(cpos);
		    pos.push_back(cpos+1);
		    cpos = 0;
		    addr.push_back(caddr);
		    caddr = caddr+"/wdg_"+cn->attr("id");
		}
		else cpos++;
		continue;
	    }
	    if( objPresent(caddr) )	objAt(caddr).at().setAttrs(*cn,ses.user);
	    if( !cn->parent() )	break;
	    cn = cn->parent();
	    cpos = pos.back();	pos.pop_back();
	    caddr = addr.back(); addr.pop_back();
	}

	//>> Send request to browser
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml; charset=UTF-8")+ses.page;
    }
    //> Resources request (images and other files)
    else if( wp_com == "res" )
    {
	prmEl = ses.prm.find("val");
	if( prmEl != ses.prm.end() )
	{
	    string mime;
	    ses.page = resGet(prmEl->second,ses.url,ses.user,&mime);
	    ses.page = mod->httpHead("200 OK",ses.page.size(),mime)+ses.page;
	} else ses.page = mod->httpHead("404 Not Found");
    }
    //> Request to primitive object. Used for data caching
    else if( wp_com == "obj" )
    {
	if( !objPresent(ses.url) )
	{
	    //>> Request to widget type
	    bool new_obj = false;
	    XMLNode req("get");
	    req.setAttr("path",ses.url+"/%2fwdg%2fcfg%2froot");
	    mod->cntrIfCmd(req,ses.user);
	    if( req.text() == "ElFigure" )	{ objAdd( new VCAElFigure(ses.url) ); new_obj = true; }
	    else if( req.text() == "Diagram" )	{ objAdd( new VCADiagram(ses.url) ); new_obj = true; }
	    if( new_obj )
	    {
		//>> Request new object's attributes
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
    //> Commands process
    map<string,string>::iterator cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //> Attributes set
    if( wp_com == "attrs" )
    {
	XMLNode req("set");
	req.load(ses.content);
	req.setAttr("path",ses.url+"/%2fserv%2fattr");
	mod->cntrIfCmd(req,ses.user);
    }
    //> Open page command
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

string VCASess::resGet( const string &res, const string &path, const string &user, string *mime )
{
    if( res.empty() )	return "";

    string ret = cacheResGet(res,mime);
    if( ret.empty() )
    {
	XMLNode req("get");
	req.setAttr("path",path+"/%2fwdg%2fres")->setAttr("id",res);
	mod->cntrIfCmd(req,user);
	ret = TSYS::strDecode(req.text(),TSYS::base64);
	if( !ret.empty() )
	{
	    if( mime ) *mime = req.attr("mime");
	    cacheResSet(res,ret,req.attr("mime"));
	}
    }

    return ret;
}

string VCASess::cacheResGet( const string &res, string *mime )
{
    ResAlloc resAlc(nodeRes(),false);
    map<string,CacheEl>::iterator ires = mCacheRes.find(res);
    if( ires == mCacheRes.end() ) return "";
    ires->second.tm = time(NULL);
    if( mime ) *mime = ires->second.mime;
    return ires->second.val;
}

void VCASess::cacheResSet( const string &res, const string &val, const string &mime )
{
    if( val.size() > 1024*1024 ) return;
    ResAlloc resAlc(nodeRes(),true);
    mCacheRes[res] = CacheEl(time(NULL),val,mime);
    if( mCacheRes.size() > 100 )
    {
	map<string,CacheEl>::iterator ilast = mCacheRes.begin();
	for( map<string,CacheEl>::iterator ires = mCacheRes.begin(); ires != mCacheRes.end(); ++ires )
	    if( ires->second.tm < ilast->second.tm )	ilast = ires;
	mCacheRes.erase(ilast);
    }
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

    if( r3 != 0 && r4 != 0 && SAME_SIGNS( r3, r4 ))
        return false;

    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;

    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;

    if( r1 != 0 && r2 != 0 && SAME_SIGNS( r1, r2 ) )
        return false;

    return true;
}


//- rotation of the point -
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
        if( !flag_scale ) center = Point( TSYS::realRound( width*xScale/2 ), TSYS::realRound( height*yScale/2 ) );
        else center = Point( TSYS::realRound( width/2 ), TSYS::realRound( height/2 ) );
        rpnt.x = rpnt.x - center.x;
        rpnt.y = rpnt.y - center.y;
        rpnt = rotate( rpnt, orient);
        rpnt.x = rpnt.x + center.x;
        rpnt.y = rpnt.y + center.y;
    }
    if( flag_scale ) rpnt = Point( rpnt.x*xScale, rpnt.y*yScale );

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
        if( !flag_scale ) center = Point( TSYS::realRound( width*xScale/2 ), TSYS::realRound( height*yScale/ 2 ) );
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
void VCAElFigure::paintFill( gdImagePtr im, Point pnt, InundationItem &in_item )
{
    int fill_clr = gdImageColorResolveAlpha( im, (uint8_t)(in_item.P_color>>16), (uint8_t)(in_item.P_color>>8), (uint8_t)in_item.P_color, 127 - (uint8_t)(in_item.P_color>>24) ); 
    in_item.index_color = fill_clr;
    gdImageFill( im, (int) TSYS::realRound(pnt.x), 
                     (int) TSYS::realRound(pnt.y),  fill_clr );
}

//- Detecting if any point of the figure is out of the borders of the image -
bool VCAElFigure::isPaintable( ShapeItem item, double xScale, double yScale )
{
    double s_width = width*xScale;
    double s_height = height*yScale;
    switch(item.type)
    {
        case 1:
            if(  (scaleRotate((pnts)[item.n1],xScale,yScale,true, true).x >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x <= (s_width)) &&
                 (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y <= (s_height)) &&
                 (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x <= (s_width)) &&
                 (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y <= (s_height))
              )
                return true;
            else return false;
        case 2:
            if(
                (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n5],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n5],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n5],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n5],xScale,yScale,true,true).y <= (s_height))
              )
                return true;
            else return false;
        case 3:
            if(
                (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n1],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n2],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n3],xScale,yScale,true,true).y <= (s_height)) &&
                (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x >= 0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).x <= (s_width)) &&
                (scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y >= 0 && scaleRotate((pnts)[item.n4],xScale,yScale,true,true).y <= (s_height))
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
            if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
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
            int kol = (int)TSYS::realRound( wdt/len, 2, true) ;
            int kol_1 = (int)TSYS::realRound( wdt_1/len, 2, true );
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
            while( true );
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
            while( true );
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
            if( (int)(x_center + 0.5) < s_width && (int)(y_center + 0.5) < s_height )
                gdImageFillToBorder( im, (int)(x_center + 0.5), (int)(y_center + 0.5), clr_el, clr_el_line );
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
            while( t < t_end );
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
            while( t < t_end );
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
            if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
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
            while( t < t_end );
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
            while( t < t_end );
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
            if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
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
            while( t < t_end );
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
                else  calc_1++;
                t += delta;
            }
            while( t < 1 );

        }
            break;
    }
}

//- Painting existing figures -
void VCAElFigure::paintFigure( gdImagePtr im, ShapeItem item, double xScale, double yScale, bool flag_allocate, bool flag_style )
{
    int clr_el, clr_el_line;
    double t;
    double arc_a, arc_b, t_start, t_end,arc_a_small,arc_b_small, ang;
    double el_width, el_border_width;
    //-- Arc --
    if( item.type == 2 )
        if( !isPaintable( item, xScale, yScale ) ) mess_debug(nodePath().c_str(),_("At least one of the points of the 'arc' is out of the drawing area. The 'arc' is not drawn."));
        else
            if( item.border_width == 0 )//--- Drawing the arc with borders' width == 0 ---
            {
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                Point el_p5 = scaleRotate( (pnts)[item.n5], xScale, yScale, true, true );
                Point el_p6 = item.ctrlPos4;
                if( el_p5.y <= el_p3.y ) 
                    ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                arc_a = length( el_p5, el_p3 );
                arc_b = length( el_p3, el_p4 );
                t_start = item.ctrlPos4.x;
                t_end = item.ctrlPos4.y;
                if( flag_allocate )
                    clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
                else clr_el = item.lineColor;
                if( item.style != 0 && flag_style )//---- Drawing the dashed or dotted arc with borders' width == 0 ----
                {
                    gdImageAlphaBlending(im,0);
                    dashDot( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6, clr_el, item.width, 2, item.style );
                    gdImageAlphaBlending(im,1);
                }
                else//---- Drawing the solid arc with borders' width == 0 ----
                {
                    gdImageSetThickness( im, item.width );
                    gdImageAlphaBlending(im,0);
                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el );
                        t += 0.00277777777778;
                    }
                    while( t < t_end );
                    gdImageAlphaBlending(im,1);
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
                if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                clr_el_line = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
                if( flag_allocate )
                    clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.borderColor>>16), (uint8_t)(item.borderColor>>8), (uint8_t)item.borderColor, 127 - (uint8_t)(item.borderColor>>24) );
                else
                {
                    clr_el = item.borderColor;
                    if( item.flag_brd ) clr_el_line = clr_el;
                }
                //---- Drawing the dashed or dotted arc with borders' width < 4 and flag_brd ----
                if( item.border_width < 4 && item.style != 0 && item.flag_brd && flag_style )
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
                    gdImageAlphaBlending(im,0);
                    dashDotFigureBorders( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6,  clr_el, clr_el_line, el_width, el_border_width, 2, wdt, wdt_1, xScale, yScale  );
                    gdImageAlphaBlending(im,1);
                }
                //---- Drawing the dashed or dotted arc with borders' width < 4 and !flag_brd ----
                if( item.border_width < 4 && item.style != 0 && !item.flag_brd && flag_style )
                {
                    Point el_pb1, el_pb2, el_pb3, el_pb4, el_pb5;
                    t_start = item.ctrlPos4.x;
                    t_end = item.ctrlPos4.y;

                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "arc" -----
                    arc_a = length( el_p5, el_p3 )+el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 )+el_width/2 + el_border_width/2;
                    gdImageSetThickness(im,1);
                    gdImageAlphaBlending(im,0);
                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while( t < t_end );

                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    arc_a = length( el_p5, el_p3 )-el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 )-el_width/2 - el_border_width/2;

                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while( t < t_end );

                    el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),
                                     (int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb3.x, 2, true ),
                                     (int)TSYS::realRound( el_pb3.y, 2, true ), clr_el_line );

                    gdImageLine( im, (int)TSYS::realRound( el_pb2.x, 2, true ),
                                     (int)TSYS::realRound( el_pb2.y, 2, true ),
                                     (int)TSYS::realRound( el_pb4.x, 2, true ),
                                     (int)TSYS::realRound( el_pb4.y, 2, true ), clr_el_line );


                    arc_a = length( el_p5, el_p3 );
                    arc_b = length( el_p3, el_p4 );
 
                    Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, 2, true ),
                                            TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, 2, true ) );
                    gdImageFillToBorder( im, (int)TSYS::realRound( p_center.x ), (int)TSYS::realRound( p_center.y ), clr_el_line, clr_el_line );

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2,0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x, 
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

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
                    dashDot( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, item.ctrlPos4, clr_el, item.border_width, 2, item.style );

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
                    dashDot( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, item.ctrlPos4, clr_el, item.border_width, 2, item.style );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);

                }
                //---- Drawing the solid arc with borders' width <4 ----
                if( item.border_width < 4 && (item.style == 0 || !flag_style) )
                {
                    gdImageAlphaBlending(im,0);
                    paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4, el_p5, el_p6, clr_el, clr_el_line, el_width, el_border_width, 2, xScale, yScale  );
                    gdImageAlphaBlending(im,1);
                }
                //---- Drawing the dashed or dotted border of the arc with borders' width >= 4 ----
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
                    Point el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6;

                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "arc" -----
                    arc_a = length( el_p5, el_p3 )+el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 )+el_width/2 + el_border_width/2;
                    gdImageSetThickness(im,1);
                    gdImageAlphaBlending(im,0);
                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while ( t < t_end );

                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    arc_a = length( el_p5, el_p3 )-el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 )-el_width/2 - el_border_width/2;

                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while ( t < t_end );

                    el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),
                                     (int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb3.x, 2, true ),
                                     (int)TSYS::realRound( el_pb3.y, 2, true ), clr_el_line );
                    gdImageLine( im, (int)TSYS::realRound( el_pb2.x, 2, true ),
                                     (int)TSYS::realRound( el_pb2.y, 2, true ),
                                     (int)TSYS::realRound( el_pb4.x, 2, true ),
                                     (int)TSYS::realRound( el_pb4.y, 2, true ), clr_el_line );

                    arc_a = length( el_p5, el_p3 );
                    arc_b = length( el_p3, el_p4 );
 
                    Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, 2, true ),
                                            TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, 2, true ) );
                    gdImageFillToBorder( im, (int)TSYS::realRound( p_center.x ), (int)TSYS::realRound( p_center.y ), clr_el_line, clr_el_line );

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2,0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                    //----- Drawing the first line connecting two arcs -----
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    p1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    p2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    //----- Drawing the second line connecting two arcs -----
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    p1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    p2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    //----- Drawing the big arc -----
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
                    dashDotFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6,  clr_el, clr_el, el_border_width-2, 1, 2, wdt, wdt_1, xScale, yScale  );
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
                    dashDotFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6,  clr_el, clr_el, el_border_width-2, 1, 2, wdt, wdt_1, xScale, yScale  );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);
                }
                //---- Drawing the solid arc with borders' width >=4 ----
                if( item.border_width >= 4 && ( item.style == 0 || !flag_style ) )
                {
                    Point el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6;
                    t_start = el_p6.x;
                    t_end = el_p6.y;

                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "arc" -----
                    arc_a = length( el_p5, el_p3 )+el_width/2 + el_border_width/2;
                    arc_b = length( el_p3, el_p4 )+el_width/2 + el_border_width/2;
                    gdImageSetThickness(im,1);
                    gdImageAlphaBlending(im,0);
                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while( t < t_end );

                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    arc_a = length( el_p5, el_p3 )-el_width/2 - el_border_width/2;
                    arc_b = length( el_p3, el_p4 )-el_width/2 - el_border_width/2;

                    t = t_start;
                    do
                    {
                        gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, 2, true ),
                                         (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, 2, true ),
                                         (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, 2, true ), clr_el_line );
                        t += 0.00277777777778;
                    }
                    while( t < t_end );

                    el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),
                                     (int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb3.x, 2, true ),
                                     (int)TSYS::realRound( el_pb3.y, 2, true ), clr_el_line );
                    gdImageLine( im, (int)TSYS::realRound( el_pb2.x, 2, true ),
                                     (int)TSYS::realRound( el_pb2.y, 2, true ),
                                     (int)TSYS::realRound( el_pb4.x, 2, true ),
                                     (int)TSYS::realRound( el_pb4.y, 2, true ), clr_el_line );


                    arc_a = length( el_p5, el_p3 );
                    arc_b = length( el_p3, el_p4 );

                    Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, 2, true ),
                                            TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, 2, true ) );
                    gdImageFillToBorder( im, (int)TSYS::realRound( p_center.x ), (int)TSYS::realRound( p_center.y ), clr_el_line, clr_el_line );

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2,0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                    t_start = el_p6.x;
                    t_end = el_p6.y; 
                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
                                          el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 ) + el_width/2 + el_border_width - 2;
                    arc_b = length( el_p3, el_p4 ) + el_width/2 + el_border_width - 2;
                    el_pb1 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    arc_a = length( el_p5, el_p3 ) - el_width/2 - el_border_width + 2;
                    arc_b = length( el_p3, el_p4 ) - el_width/2 - el_border_width + 2;
                    el_pb2 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
                                    el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale  );

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
                    paintFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6, clr_el, clr_el, el_border_width-2, 1, 2, xScale, yScale  );

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
                    paintFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, el_pb5, el_pb6, clr_el, clr_el, el_border_width-2, 1, 2, xScale, yScale  );

                    arc_a = length( el_p5, el_p3 );
                    arc_b = length( el_p3, el_p4 );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);
                }
                //---- Recalculating the points of the arc to make them really belonging to the arc ----
                el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
                el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
                el_p5 = scaleRotate( (pnts)[item.n5], xScale, yScale, true, true );
                el_p6 = item.ctrlPos4;
                if( el_p5.y <= el_p3.y ) 
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
    if( item.type == 3)
        if( !isPaintable( item, xScale, yScale ) ) mess_debug(nodePath().c_str(),_("At least one of the points of the 'bezier curve' is out of the drawing area. The 'bezier curve' is not drawn."));
        else
        if( item.border_width == 0 )//--- Drawing the bezier curve with borders' width == 0 ---
        {
            if( flag_allocate )
                clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
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
                gdImageAlphaBlending(im,0);
                dashDot( im, el_p1, el_p2, el_p3, el_p4, Point(0,0), Point(0,0), clr_el, item.width, 3, item.style );
                gdImageAlphaBlending(im,1);
            }
            else//---- Drawing the solid bezier curve with borders' width == 0 ----
            {
                gdImageAlphaBlending(im,0);
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
                while( t < t_end );
                gdImageAlphaBlending(im,1);
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
            clr_el_line = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
            if( flag_allocate ) clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.borderColor>>16), (uint8_t)(item.borderColor>>8), (uint8_t)item.borderColor, 127 - (uint8_t)(item.borderColor>>24) );
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
                gdImageAlphaBlending(im, 0);
                dashDotFigureBorders( im, el_p1, el_p2, el_p3, el_p4, Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, wdt, wdt_1, xScale, yScale  );
                gdImageAlphaBlending(im, 1);
            }
            if( item.border_width < 4 && item.style != 0 && !item.flag_brd && flag_style )//---- Drawing the dashed or dotted bezier curve with borders' width < 4 and without flag_brd ----
            {
                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                Point el_pb1, el_pb2, el_pb3, el_pb4;
                //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "bezier curve" -----
                gdImageAlphaBlending(im, 0);
                gdImageSetThickness( im, 1 );
                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );

                double delta = bezierDeltaT( el_pb1, el_pb3, el_pb4, el_pb2 );

                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound( bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                gdImageAlphaBlending(im, 1);
                Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, 2, true ),
                                        (int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, 2, true ) );
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el_line, clr_el_line );
                //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                gdImageAlphaBlending(im2,0);
                gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                dashDot( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, item.border_width, 3, item.style );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                dashDot( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, item.border_width, 3, item.style );

                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).x, 
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                dashDot( im, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im, 1);
                gdImageAlphaBlending(im2,1);
                gdImageSaveAlpha(im2, 1);
                gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                if( im2 ) gdImageDestroy(im2);

            }
            //----- Drawing the solid bezier curve with borders' width < 4 -----
            if( item.border_width < 4 && ( item.style == 0 || !flag_style ) )
            {
                gdImageAlphaBlending(im,0);
                paintFigureBorders( im, el_p1, el_p2, el_p3, el_p4, Point(0.0,1.0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 3, xScale, yScale  );
                gdImageAlphaBlending(im,1);
            }
            //----- Drawing the dashed or dotted bezier curve with borders' width >= 4 -----
            if( item.border_width >=4 && item.style != 0 && flag_style )
            {
                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                Point el_pb1, el_pb2, el_pb3, el_pb4;
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
                //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "bezier curve" -----
                gdImageAlphaBlending(im, 0);
                gdImageSetThickness( im, 1 );
                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );

                double delta = bezierDeltaT( el_pb1, el_pb3, el_pb4, el_pb2 );

                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound( bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                gdImageAlphaBlending(im, 1);
                Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, 2, true ),
                                        (int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, 2, true ) );
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el_line, clr_el_line );
                //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                gdImageAlphaBlending(im2,0);
                gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                dashDotFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, wdt, wdt_1, xScale, yScale  );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                dashDotFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, wdt, wdt_1, xScale, yScale  );

                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).x, 
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).y );
                dashDotFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );

                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                dashDotFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );

                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im, 1);
                gdImageAlphaBlending(im2,1);
                gdImageSaveAlpha(im2, 1);
                gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                if( im2 ) gdImageDestroy(im2);

            }
            if( item.border_width >=4 && ( item.style == 0 || !flag_style ) )//----- Drawing the solid bezier curve with borders' width >= 4 -----
            {
                Point el_pb1, el_pb2, el_pb3, el_pb4;

                Point p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
                Point p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
                Point p3 = unrotate( el_p3, el_ang,el_p1.x, el_p1.y );
                Point p4 = unrotate( el_p4, el_ang, el_p1.x, el_p1.y );
                //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "bezier curve" -----
                gdImageAlphaBlending(im, 0);
                gdImageSetThickness( im, 1 );
                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );

                double delta = bezierDeltaT( el_pb1, el_pb3, el_pb4, el_pb2 );

                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound( bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                t = 0;
                do
                {
                    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, 2, true ),
                                     (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, 2, true ), clr_el_line );
                    t += delta;
                }
                while( t < 1 );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                 (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                gdImageAlphaBlending(im, 1);
                Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, 2, true ),
                                        (int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, 2, true ) );
                gdImageFillToBorder( im, (int)( p_center.x + 0.5 ), (int)( p_center.y + 0.5 ), clr_el_line, clr_el_line );
                //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                gdImageAlphaBlending(im2,0);
                gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(el_width/2+el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(el_width/2+el_border_width/2) ), el_ang ).y );
                paintFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0.0,1.0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point(p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point(p2.x, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb3 = Point( el_p1.x + rotate( Point(p3.x, p3.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p3.x, p3.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                el_pb4 = Point( el_p1.x + rotate( Point(p4.x, p4.y+(-el_width/2-el_border_width/2)), el_ang ).x,
                                el_p1.y - rotate( Point( p4.x, p4.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
                paintFigureBorders( im2, el_pb1, el_pb2, el_pb3, el_pb4, Point(0.0,1.0), Point(0,0), clr_el, clr_el, el_border_width-2, 1, 3, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).x, 
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y+el_width/2+el_border_width-1 ), el_ang ).y );
                el_pb2 = Point( el_p1.x + rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).x,
                                el_p1.y - rotate( Point( p1.x-el_border_width/2, p1.y - (el_width/2+el_border_width-1 ) ), el_ang ).y );
                paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale );

                el_pb1 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y+el_width/2+el_border_width-1 ), el_ang ).y );;
                el_pb2 = Point( el_p1.x + rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).x,
                                el_p1.y - rotate( Point( p2.x+el_border_width/2, p2.y-(el_width/2+el_border_width-1) ), el_ang ).y );
                paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width-2, 0.5, 1, xScale, yScale );

                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im, 1);
                gdImageAlphaBlending(im2,1);
                gdImageSaveAlpha(im2, 1);
                gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                if( im2 ) gdImageDestroy(im2);
            }
        }
    //-- Line --
    if( item.type == 1 )
        if( !isPaintable( item, xScale, yScale ) ) mess_debug(nodePath().c_str(),_("At least one of the points of the 'line' is out of the drawing area. The 'line' is not drawn."));
        else
            if( item.border_width == 0 )//--- Drawing the line with borders' width == 0 ---
            {
                if( flag_allocate )
                    clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
                else clr_el =item.lineColor;
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
                                     (int)TSYS::realRound( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y, 2, true ),  clr_el );
                }
            }
            else//--- Drawing the line with borders' width > 0 ---
            {
                clr_el_line = gdImageColorResolveAlpha( im, (uint8_t)(item.lineColor>>16), (uint8_t)(item.lineColor>>8), (uint8_t)item.lineColor, 127 - (uint8_t)(item.lineColor>>24) );
                if( flag_allocate )
                    clr_el = gdImageColorResolveAlpha( im, (uint8_t)(item.borderColor>>16), (uint8_t)(item.borderColor>>8), (uint8_t)item.borderColor, 127 - (uint8_t)(item.borderColor>>24) );
                else 
                {
                    clr_el = item.borderColor;
                    if( item.flag_brd ) clr_el_line = clr_el;
                }
                Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                double el_border_width = (double)item.border_width/2;
                double el_width = item.width;
                //---- Drawing the dashed or dotted line with borders' width == 1(for lines with width > 3) ----
                if( item.border_width < 4 && item.style != 0 && item.flag_brd && flag_style )
                {
                    double wdt; 
                    if( item.style == 1 ) wdt = 4*(item.width+2)-1;
                    else if( item.style == 2 ) wdt = item.width+1;
                    gdImageAlphaBlending(im, 0);
                    dashDotFigureBorders( im, el_p1, el_p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, wdt, 0.0, xScale, yScale  );
                    gdImageAlphaBlending(im, 1);
                }
                //---- Drawing the dashed or dotted borders of the line (for borders with width < 4) ----
                if( item.border_width < 4 && item.style != 0 && !item.flag_brd  && flag_style)
                {
                    double el_ang;
                    Point el_pb1,el_pb2;
                    Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
                    Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    double el_width = item.width;
                    double el_border_width = item.border_width;

                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "line" -----
                    el_border_width = el_border_width/2;
                    gdImageSetThickness( im, 1 );
                    gdImageAlphaBlending(im, 0); 
                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );
                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );
                    gdImageAlphaBlending(im, 1); 

                    double x_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x)/2;
                    double y_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y)/2;
                    gdImageFillToBorder( im, (int)(x_center+0.5), (int)(y_center+0.5), clr_el_line, clr_el_line);

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2, 0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                    el_pb1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );

                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );
                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );                                              
                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );                    
                    dashDot( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, item.border_width, 1, item.style );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);

                }
                //----- Drawing the solid line with borders' width == 1(for lines with width > 3) -----
                if(  item.border_width < 4  && ( item.style == 0 || !flag_style ) )
                {
                    gdImageAlphaBlending(im, 0);
                    paintFigureBorders( im, el_p1, el_p2, Point(0,0), Point(0,0),  Point(0,0), Point(0,0), clr_el, clr_el_line, el_width, el_border_width, 1, xScale, yScale  );
                    gdImageAlphaBlending(im, 1);
                }
                //---- Drawing the dashed or dotted line with the borders' width >= 4 ----
                if( item.border_width >= 4 && item.style != 0 && flag_style )
                {
                    double el_ang;
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    double ang;
                    Point p1, p2, un_p1, un_p2,el_pb1,el_pb2;
                    int kol; 
                    el_width = el_width - 1;
                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "line" -----
                    gdImageAlphaBlending(im, 0);
                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageSetThickness( im, 1 );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    gdImageAlphaBlending(im, 1);
                    double x_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x)/2;
                    double y_center=(scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y)/2;
                    gdImageFillToBorder( im, (int)(x_center+0.5), (int)(y_center+0.5), clr_el_line, clr_el_line);
                    el_width = el_width + 1;

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2, 0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                    p1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );
                    double wdt;
                    if( item.style == 1 ) wdt = 4*item.border_width-1;
                    else if( item.style == 2 ) wdt = item.border_width-1;
                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );

                    p1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );
                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );

                    p1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    p2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );

                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );
                    p1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    p2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );
                    dashDotFigureBorders( im2, p1, p2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, 2*el_border_width-2, 0.5, 1, wdt, 0.0, xScale, yScale  );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);

                }
                //----- Drawing the solid borders of the line -----
                if( item.border_width >= 4 && ( item.style == 0 || !flag_style ) )
                {
                    double el_ang;
                    if( el_p1.y <= el_p2.y )
                        el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    else
                        el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                    Point el_pb1, el_pb2;
                    //----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "line" -----
                    gdImageAlphaBlending(im, 0);
                    gdImageSetThickness( im, 1 );
                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );

                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
                    gdImageLine( im, (int)TSYS::realRound( el_pb1.x, 2, true ),(int)TSYS::realRound( el_pb1.y, 2, true ),
                                     (int)TSYS::realRound( el_pb2.x, 2, true ),(int)TSYS::realRound( el_pb2.y, 2, true ),clr_el_line );
                    gdImageAlphaBlending(im, 1);
                    double x_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x)/2;
                    double y_center = (scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y + scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y)/2;
                    gdImageFillToBorder( im, (int)(x_center+0.5), (int)(y_center+0.5), clr_el_line, clr_el_line);\

                    //----- Drawing the lines with their real width on the other image and merging it with the previous one -----
                    gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                    gdImageAlphaBlending(im2, 0);
                    gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );

                    el_pb1 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, - (el_width/2+el_border_width)), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + 2*el_border_width-1, -(el_width/2+el_border_width) ), el_ang ).y );
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -el_width/2 ), el_ang ).y );;
                    el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2 ), el_ang ).y );
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );

                    el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, ( el_width/2+el_border_width) ), el_ang ).x,
                                    el_p1.y - rotate( Point( length( el_p2, el_p1 ) + 2*el_border_width-1, (el_width/2+el_border_width) ), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width) ), el_ang).x,
                                    el_p1.y - rotate( Point( -2*el_border_width+1, (el_width/2+el_border_width)), el_ang ).y );                                              
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );

                    el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2 ), el_ang).x,
                                    el_p1.y - rotate( Point( -el_border_width, el_width/2), el_ang ).y );
                    el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2 ), el_ang ).x,
                                    el_p1.y - rotate( Point( -el_border_width, - el_width/2 ), el_ang ).y );
                    paintFigureBorders( im2, el_pb1, el_pb2, Point(0,0), Point(0,0), Point(0,0), Point(0,0), clr_el, clr_el, el_border_width*2-2, 0.5, 1, xScale, yScale );

                    gdImageAlphaBlending(im,1);
                    gdImageSaveAlpha(im, 1);
                    gdImageAlphaBlending(im2,1);
                    gdImageSaveAlpha(im2, 1);
                    gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                    if( im2 ) gdImageDestroy(im2);
                }
            }
}

int VCAElFigure::drawElF( SSess &ses, double xSc, double ySc, Point clickPnt )
{
    vector<int> shape_temp;
    vector<int> shape_temp_all;
    vector<int> line_color_shape;
    vector<int> border_color_shape;
    vector<int> width_shape;
    vector<int> border_width_shape;

    double scale;
    double el_width;
    double border_width;
    bool flag_min;
    bool flag_push_back;
    scaleHeight = (int)TSYS::realRound(height*ySc, 2, true);
    scaleWidth = (int)TSYS::realRound(width*xSc, 2, true);
    if( xSc < ySc ) scale = xSc;
    else scale = ySc;
    if( scale != 1.0 )
    {
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
    }
    for(int i = 0; i < inundationItems.size(); i++ )
    {
        //- Detecting which figures correspond the points of each fill -
        bool flag_fill = true;
        bool flag_fill_alpha = false;
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
        gdImagePtr im1 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
        gdImageAlphaBlending(im1, 0);
        gdImageFilledRectangle( im1, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im1,0,0,0,127) );
        gdImageAlphaBlending(im1, 1);

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
                    if( shape_temp.size() == 2 ) break;
                }
        }
        else
        {
            for( int j = 0; j < inundationItems[i].number_point.size()-1; j++ )
                for( int k = 0; k < shapeItems.size(); k++ )
                    if( ((inundationItems[i].number_point[j] == shapeItems[k].n1) && (inundationItems[i].number_point[j+1] == shapeItems[k].n2)) ||
                        ((inundationItems[i].number_point[j] == shapeItems[k].n2) && (inundationItems[i].number_point[j+1] == shapeItems[k].n1)) )
                    {
                        flag_push_back = true;
                        for( int p=0; p < shape_temp.size(); p++ )
                            if( (shapeItems[shape_temp[p]].n1 == shapeItems[k].n1 && shapeItems[shape_temp[p]].n2 == shapeItems[k].n2) ||
                                 (shapeItems[shape_temp[p]].n1 == shapeItems[k].n2 && shapeItems[shape_temp[p]].n2 == shapeItems[k].n1) )
                            {
                                flag_push_back = false;
                                if( (shapeItems[shape_temp[p]].type==2 && shapeItems[k].type==1) && (shape_temp[p]!=k) )
                                {
                                    shape_temp[p]=k;
                                    width_shape[p] = shapeItems[k].width;
                                    border_width_shape[p] = shapeItems[k].border_width;
                                    line_color_shape[p] = shapeItems[k].lineColor;
                                    if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                    else border_color_shape[p] = shapeItems[k].lineColor ;
                                }
                                if( (shapeItems[shape_temp[p]].type==3 && shapeItems[k].type==1) && (shape_temp[p]!=k) )
                                {
                                    shape_temp[p]=k;
                                    width_shape[p] = shapeItems[k].width;
                                    border_width_shape[p] = shapeItems[k].border_width;
                                    line_color_shape[p] = shapeItems[k].lineColor;
                                    if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                    else border_color_shape[p] = shapeItems[k].lineColor ;

                                }
                                if( (shapeItems[shape_temp[p]].type==2 && shapeItems[k].type==3) && (shape_temp[p]!=k) )
                                {
                                    shape_temp[p]=k;
                                    width_shape[p] = shapeItems[k].width;
                                    border_width_shape[p] = shapeItems[k].border_width;
                                    line_color_shape[p] = shapeItems[k].lineColor;
                                    if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                    else border_color_shape[p] = shapeItems[k].lineColor ;

                                }
                            }
                        if( flag_push_back )// inundation_fig_num.push_back(j);
                        {
                            shape_temp.push_back(k);
                            width_shape.push_back( shapeItems[k].width );
                            border_width_shape.push_back( shapeItems[k].border_width );
                            line_color_shape.push_back( shapeItems[k].lineColor );
                            if( shapeItems[k].border_width != 0 ) border_color_shape.push_back( shapeItems[k].borderColor );
                            else border_color_shape.push_back( shapeItems[k].lineColor );
                        }
                    }
            for( int k = 0; k < shapeItems.size(); k++ )
                if( ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n1) && (inundationItems[i].number_point[0] == shapeItems[k].n2)) ||
                    ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n2) && (inundationItems[i].number_point[0] == shapeItems[k].n1)) )
                {
                    flag_push_back = true;
                    for( int p=0; p < shape_temp.size(); p++ )
                        if( (shapeItems[shape_temp[p]].n1 == shapeItems[k].n1 && shapeItems[shape_temp[p]].n2 == shapeItems[k].n2) ||
                             (shapeItems[shape_temp[p]].n1 == shapeItems[k].n2 && shapeItems[shape_temp[p]].n2 == shapeItems[k].n1) )
                        {
                            flag_push_back = false;
                            if( (shapeItems[shape_temp[p]].type==2 && shapeItems[k].type==1) && (shape_temp[p]!=k) )
                            {
                                shape_temp[p]=k;
                                width_shape[p] = shapeItems[k].width;
                                border_width_shape[p] = shapeItems[k].border_width;
                                line_color_shape[p] = shapeItems[k].lineColor;
                                if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                else border_color_shape[p] = shapeItems[k].lineColor ;
                            }
                            if( (shapeItems[shape_temp[p]].type==3 && shapeItems[k].type==1) && (shape_temp[p]!=k) )
                            {
                                shape_temp[p]=k;
                                width_shape[p] = shapeItems[k].width;
                                border_width_shape[p] = shapeItems[k].border_width;
                                line_color_shape[p] = shapeItems[k].lineColor;
                                if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                else border_color_shape[p] = shapeItems[k].lineColor ;

                            }
                            if( (shapeItems[shape_temp[p]].type==2 && shapeItems[k].type==3) && (shape_temp[p]!=k) )
                            {
                                shape_temp[p]=k;
                                width_shape[p] = shapeItems[k].width;
                                border_width_shape[p] = shapeItems[k].border_width;
                                line_color_shape[p] = shapeItems[k].lineColor;
                                if( shapeItems[k].border_width != 0 ) border_color_shape[p] = shapeItems[k].borderColor;
                                else border_color_shape[p] = shapeItems[k].lineColor ;

                            }
                        }
                    if( flag_push_back )
                    {
                        shape_temp.push_back(k);
                        width_shape.push_back( shapeItems[k].width );
                        border_width_shape.push_back( shapeItems[k].border_width );
                        line_color_shape.push_back( shapeItems[k].lineColor );
                        if( shapeItems[k].border_width != 0 ) border_color_shape.push_back( shapeItems[k].borderColor );
                        else border_color_shape.push_back( shapeItems[k].lineColor );
                    }
                }
        }
        //- Changing the color of the figure for the same for all figures from which the each fill is consist and painting them -
        for(int j = 0; j < shape_temp.size(); j++ )
            if( !isPaintable( shapeItems[shape_temp[j]], xSc, ySc ) )
            {
                flag_fill = false;
                break;
            }
        if( !flag_fill ) mess_debug(nodePath().c_str(),_("At least one of the elementary figures from each the 'fill' consists of is out of drawing area. The 'fill' is not drawn."));
        else
        {
            int tmp_clr;
            tmp_clr = gdImageColorResolveAlpha( im1, (uint8_t)(inundationItems[i].P_color>>16), (uint8_t)(inundationItems[i].P_color>>8), 
                                                     (uint8_t)inundationItems[i].P_color, 127 - (uint8_t)(inundationItems[i].P_color>>24) );

            for( int j = 0; j < shape_temp.size(); j++ )
                if( shapeItems[shape_temp[j]].type == 2 )
                {
                    shapeItems[shape_temp[j]].width = 1;
                    shapeItems[shape_temp[j]].border_width = 0;
                    shapeItems[shape_temp[j]].lineColor = tmp_clr;
                    paintFigure( im1, shapeItems[shape_temp[j]], xSc, ySc, false, false );
                }
            for( int j = 0; j < shape_temp.size(); j++ )
                if( shapeItems[shape_temp[j]].type != 2 )
                {
                    shapeItems[shape_temp[j]].width = 1;
                    shapeItems[shape_temp[j]].border_width = 0;
                    shapeItems[shape_temp[j]].lineColor = tmp_clr;
                    paintFigure( im1, shapeItems[shape_temp[j]], xSc, ySc, false, false );
                }
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
                for( int j = 0; j < shape_temp.size(); j++ )
                {
                    if( shapeItems[shape_temp[j]].n1 == num_pnt )
                        for( int k = 0; k < inundationItems[i].number_point.size(); k++ )
                            if( shapeItems[shape_temp[j]].n2 == inundationItems[i].number_point[k] )
                            {
                                fig.push_back(shape_temp[j]);
                                point_num.push_back( shapeItems[shape_temp[j]].n2 );
                                break;
                            }
                    if( shapeItems[shape_temp[j]].n2 == num_pnt )
                        for( int k = 0; k < inundationItems[i].number_point.size(); k++ )
                            if( shapeItems[shape_temp[j]].n1 == inundationItems[i].number_point[k] )
                            {
                                fig.push_back(shape_temp[j]);
                                point_num.push_back( shapeItems[shape_temp[j]].n1 );
                                break;
                            }
                }
                //- Detecting the real "filling" point for all possible combinations of types of two connected figures -
                if( (point_num.size() > 1 &&
                    length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ) > 1 &&
                    length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[1]], xSc, ySc, true, true ) ) > 1) ||
                    ((point_num.size() == 2 && fig.size() == 2 && shapeItems[fig[0]].type == 2 && shapeItems[fig[1]].type == 2) ||
                        (point_num.size() == 1 && fig.size() == 1 && shapeItems[fig[0]].type == 2 && 
                        fabs((pnts)[shapeItems[fig[0]].n1].x - (pnts)[shapeItems[fig[0]].n2].x) < 0.01 &&
                        fabs((pnts)[shapeItems[fig[0]].n1].y - (pnts)[shapeItems[fig[0]].n2].y) < 0.01)) )
                {
                    //-- Simple arc --
                    if( (point_num.size() == 2 && fig.size() == 2 && shapeItems[fig[0]].type == 2 && shapeItems[fig[1]].type == 2) ||
                        (point_num.size() == 1 && fig.size() == 1 && shapeItems[fig[0]].type == 2 && 
                            fabs((pnts)[shapeItems[fig[0]].n1].x - (pnts)[shapeItems[fig[0]].n2].x) < 0.01 &&
                            fabs((pnts)[shapeItems[fig[0]].n1].y - (pnts)[shapeItems[fig[0]].n2].y) < 0.01) )
                        delta_point_center = scaleRotate( (pnts)[shapeItems[fig[0]].n3], xSc, ySc, true, true );
                    else
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
                            else delta = 0.2 + scale;
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
                                P1 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,W1), ang ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,W1), ang ).y );
                                P2 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x + rotate( Point(0,-W1), ang ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y - rotate( Point(0,-W1), ang ).y );
                                P3 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
                                            rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), W1 ), ang ).x,
                                            scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).y -
                                            rotate( Point( length( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ), scaleRotate( (pnts)[point_num[0]], xSc, ySc, true, true ) ), W1 ), ang ).y );
                                P4 = Point( scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true ).x +
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
                                if( !flag_vert2 &&  !flag_vert1 ) num_pnt_new = Point( (a-a1) / (b1-b), a + b*(a-a1) / (b1-b) );
                                else if( flag_vert1 && !flag_vert2 ) num_pnt_new = Point( dP1.x, a1 + b1*dP1.x );
                                else if( !flag_vert1 && flag_vert2 ) num_pnt_new = Point( dP2.x, a + b*dP2.x );
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
                            if( el_p5.y <= el_p3.y )
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
                            //--- if there is any width(of figure itself or of its borders) ---
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
                            delta_point_2 = Point( new_pnt.x + rotate( delta_point_2, ang1 ).x, new_pnt.y - rotate( delta_point_2, ang1 ).y );
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
                            if( el_p1.y <= el_p2.y )
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
                                    if( flag_brk_1 && flag_brk_2 ) break;
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
                            delta_point_2 = Point( new_pnt.x + rotate( delta_point_2, ang1 ).x, new_pnt.y - rotate( delta_point_2, ang1 ).y );
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
                            if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                            else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                            arc_a = length( el_p5, el_p3 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                            arc_b = length( el_p3, el_p4 ) + shapeItems[fig[0]].width/2 + shapeItems[fig[0]].border_width;
                            //--- bezier ---
                            P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                            P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                            P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                            P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                            if( P1.y <= P2.y ) ang1 = 360 - angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
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
                                    if( f_brk_1 && f_brk_2 )
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
                                    else if( f_brk_1 && !f_brk_2 )
                                    {
                                        new_pnt = new_pnt_1;
                                        delta_t = delta_temp_1;
                                        num_bezier = 1;
                                    }
                                    else if( !f_brk_1 && f_brk_2 )
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
                            if( el_p1.y <= el_p2.y ) ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                            else ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
                            //--- bezier_2 ---
                            P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
                            P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
                            P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
                            P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
                            if( P1.y <= P2.y ) ang1 = 360 - angle( P1, P2, P1, Point( P1.x+10, P1.y ) );
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
                    }
                    //-- Calling fill procedure for each fill with the real "fill" point --
                    if( (int)(delta_point_center.x+0.5) < scaleWidth && (int)(delta_point_center.y+0.5) < scaleHeight )
                    {
                        if( inundationItems[i].imgFill.size() )
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

                            for( int j=0; j<shape_temp.size(); j++ )
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

                                    if( el_p5.y <= el_p3.y ) ang = angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );
                                    else ang = 360 - angle( el_p3, el_p5, el_p3, Point( el_p3.x+10, el_p3.y ) );

                                    if( el_p5_rot.y <= el_p3_rot.y ) ang_rot = angle( el_p3_rot, el_p5_rot, el_p3_rot, Point( el_p3_rot.x+10, el_p3_rot.y ) );
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
                                    Point el_p1_rot = scaleRotate( (pnts)[item.n1], xSc, ySc, true, true );
                                    Point el_p2_rot = scaleRotate( (pnts)[item.n2], xSc, ySc, true, true );
                                    Point el_p3_rot = scaleRotate( (pnts)[item.n3], xSc, ySc, true, true );
                                    Point el_p4_rot = scaleRotate( (pnts)[item.n4], xSc, ySc, true, true );

                                    Point el_p1 = unscaleUnrotate( el_p1_rot, xSc, ySc, false, true );
                                    Point el_p2 = unscaleUnrotate( el_p2_rot, xSc, ySc, false, true );
                                    Point el_p3 = unscaleUnrotate( el_p3_rot, xSc, ySc, false, true );
                                    Point el_p4 = unscaleUnrotate( el_p4_rot, xSc, ySc, false, true );

                                    do
                                    {
                                        pnt_.x = bezier( t, el_p1, el_p3, el_p4, el_p2 ).x;
                                        pnt_.y = bezier( t, el_p1, el_p3, el_p4, el_p2 ).y;

                                        pnt_rot.x = bezier( t, el_p1_rot, el_p3_rot, el_p4_rot, el_p2_rot ).x;
                                        pnt_rot.y = bezier( t, el_p1_rot, el_p3_rot, el_p4_rot, el_p2_rot ).y;

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
                            paintFill( im1, delta_point_center, inundationItems[i] );
                            if( clickPnt.x > -1 && clickPnt.y > -1 )
                            {
                                if( gdImageGetPixel( im1, (int)TSYS::realRound(clickPnt.x,2,true), (int)TSYS::realRound(clickPnt.y,2,true) )
                                    != gdImageColorResolveAlpha(im1,0,0,0,127) )
                                {
                                    if( im1 ) gdImageDestroy(im1);
                                    return i;
                                }
                            }
                            else
                            {
                                xMin = (int)TSYS::realRound( xMin, 2, true );
                                yMin = (int)TSYS::realRound( yMin, 2, true );
                                xMax = (int)TSYS::realRound( xMax, 2, true );
                                yMax = (int)TSYS::realRound( yMax, 2, true );

                                string imgDef_temp = owner().resGet(inundationItems[i].imgFill,id(),ses.user);
                                gdImagePtr im_fill_in = gdImageCreateFromPngPtr(imgDef_temp.size(), (void*)imgDef_temp.data());
                                gdImagePtr im_fill_out = gdImageCreateTrueColor((int)TSYS::realRound( xMax - xMin ) + 1, (int)TSYS::realRound( yMax - yMin ) + 1 );
                                gdImageAlphaBlending(im_fill_out, 0);
                                gdImageAlphaBlending(im_fill_in, 0);
                                int alpha;
                                double alpha_pr;
                                gdImageCopyResampled(im_fill_out, im_fill_in, 0, 0, 0, 0, im_fill_out->sx, im_fill_out->sy, im_fill_in->sx, im_fill_in->sy);
                                int im_x, im_y, im_x_temp1, im_x_temp2;
                                Point drw_pnt,drw_pnt1;
                                int c_new;
                                xMin_rot = (int)TSYS::realRound( xMin_rot, 2, true );
                                yMin_rot = (int)TSYS::realRound( yMin_rot, 2, true );
                                xMax_rot = (int)TSYS::realRound( xMax_rot, 2, true );
                                yMax_rot = (int)TSYS::realRound( yMax_rot, 2, true );

                                double alpha_col = (double)(uint8_t)(inundationItems[i].P_color>>24)/127;
                                double color_r, color_g, color_b;
                                int rgb;
                                gdImageAlphaBlending(im1,0);

                                im_y = (int)yMin_rot;
                                do
                                {
                                    im_x = (int)xMin_rot;
                                    do
                                    {
                                        if(  gdImageGetPixel( im1, im_x, im_y ) == tmp_clr )
                                        {
                                            Point drw_pnt = unscaleUnrotate( Point( im_x, im_y ), xSc, ySc, false, true );
                                            rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin, 2, true ),
                                                                                (int)TSYS::realRound( drw_pnt.y - yMin, 2, true ) );
                                            if( ((int)TSYS::realRound( drw_pnt.x - xMin, 2, true ) == (int)TSYS::realRound( xMax - xMin ) + 1) && rgb == 0 )
                                                rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin - 1, 2, true ),
                                                                                    (int)TSYS::realRound( drw_pnt.y - yMin, 2, true ) );
                                            else if( ((int)TSYS::realRound( drw_pnt.y - yMin, 2, true ) == (int)TSYS::realRound( yMax - yMin ) + 1) && rgb == 0 )
                                                rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin, 2, true ),
                                                                                    (int)TSYS::realRound( drw_pnt.y - yMin - 1, 2, true ) );

                                            alpha = gdImageAlpha( im_fill_out, rgb );
                                            alpha_pr = 1 - (double)alpha / 127;
                                            drw_pnt1.x = scaleRotate( drw_pnt, xSc, ySc, false, true ).x;
                                            drw_pnt1.y = scaleRotate( drw_pnt, xSc, ySc, false, true ).y;

                                            if( fabs(alpha_pr - 0) < 0.001 ) alpha_pr = 1;
                                            color_r = alpha_pr*((rgb>>16)&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)( inundationItems[i].P_color>>16 ) );
                                            color_g = alpha_pr*((rgb>>8)&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)( inundationItems[i].P_color>>8 ) );
                                            color_b = alpha_pr*(rgb&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)inundationItems[i].P_color );
                                            /*int color = gdImageColorResolve( im1, (int)TSYS::realRound( color_r, 2, true ),
                                                                                (int)TSYS::realRound( color_g, 2, true ),
                                                                                (int)TSYS::realRound( color_b, 2, true ) );*/
                                            int color = gdImageColorResolveAlpha( im1, (int)TSYS::realRound( color_r, 2, true ),
                                                                                       (int)TSYS::realRound( color_g, 2, true ),
                                                                                       (int)TSYS::realRound( color_b, 2, true ),
                                                                                       127 - (uint8_t)(inundationItems[i].P_color>>24) );
                                            gdImageSetPixel( im1, (int)TSYS::realRound( drw_pnt1.x, 2, true ) , (int)TSYS::realRound( drw_pnt1.y, 2, true ), color );
                                        }
                                        im_x += 1;
                                    }
                                    while( im_x <= xMax_rot );
                                    im_y += 1;
                                }
                                while( im_y <= yMax_rot );
                                if( im_fill_out ) gdImageDestroy(im_fill_out);
                                if( im_fill_in ) gdImageDestroy(im_fill_in);
                                gdImageAlphaBlending(im1,1);
                            }
                        }
                        else
                        {
                            paintFill( im1, delta_point_center, inundationItems[i] );
                            if( clickPnt.x > -1 && clickPnt.y > -1 )
                            {
                                if( gdImageGetPixel( im1, (int)TSYS::realRound(clickPnt.x,2,true), (int)TSYS::realRound(clickPnt.y,2,true) ) != gdImageColorResolveAlpha(im1,0,0,0,127) )
                                {
                                    if( im1 ) gdImageDestroy(im1);
                                    return i;
                                }
                            }

                        }
                    }
                }
            if( (int)TSYS::realRound( clickPnt.x, 2, true ) == -1 && (int)TSYS::realRound( clickPnt.y, 2, true ) == -1 )
            {
                for( int j = 0; j < shape_temp.size(); j++ )
                    shape_temp_all.push_back(shape_temp[j]);
                //- Changing the color to the real one for all figures used in each fill(inundation)
                for( int j = 0; j < shape_temp.size(); j++ )
                {
                    shapeItems[shape_temp[j]].width = width_shape[j];
                    shapeItems[shape_temp[j]].border_width = border_width_shape[j];
                    shapeItems[shape_temp[j]].lineColor = line_color_shape[j];
                    shapeItems[shape_temp[j]].borderColor = border_color_shape[j];
                }
                //- Painting all figures -
                std::sort(shape_temp.begin(), shape_temp.end());
                for( int j = 0; j < shape_temp.size(); j++ )
                    if( shapeItems[shape_temp[j]].type == 2 )
                    {
                        gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                        gdImageAlphaBlending(im2, 0);
                        gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
                        gdImageAlphaBlending(im2, 1);
                        paintFigure( im2, shapeItems[shape_temp[j]], xSc, ySc, true, true );
                        gdImageAlphaBlending(im1,1);
                        gdImageSaveAlpha(im1, 1);
                        gdImageAlphaBlending(im2,1);
                        gdImageSaveAlpha(im2, 1);
                        gdImageCopy(im1, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                        if( im2 ) gdImageDestroy(im2);
                    }
                for( int j = 0; j < shape_temp.size(); j++ )
                    if( shapeItems[shape_temp[j]].type != 2 )
                    {
                        gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                        gdImageAlphaBlending(im2, 0);
                        gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
                        gdImageAlphaBlending(im2, 1);
                        paintFigure( im2, shapeItems[shape_temp[j]], xSc, ySc, true, true );
                        gdImageAlphaBlending(im1,1);
                        gdImageSaveAlpha(im1,1);
                        gdImageAlphaBlending(im2,1);
                        gdImageSaveAlpha(im2, 1);
                        gdImageCopy(im1, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                        if( im2 ) gdImageDestroy(im2);
                    }
                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im,1);
                gdImageAlphaBlending(im1,1);
                gdImageSaveAlpha(im1, 1);
                gdImageCopy(im, im1, 0, 0, 0, 0, scaleWidth, scaleHeight);
            }
        }
        if( im1 ) gdImageDestroy(im1);
    }
    if( (int)TSYS::realRound( clickPnt.x, 2, true ) == -1 && (int)TSYS::realRound( clickPnt.y, 2, true ) == -1 )
    {
        for( int j = 0; j < shapeItems.size(); j++ )
        {
            bool fl_paint = false;
            for( int i =0; i < shape_temp_all.size(); i++ )
                if( j == shape_temp_all[i] )
                {
                    fl_paint = true;
                    break;
                }
            if( !fl_paint && shapeItems[j].type == 2 )
            {
                gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                gdImageAlphaBlending(im2, 0);
                gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
                gdImageAlphaBlending(im2, 1);
                paintFigure( im2, shapeItems[j], xSc, ySc, true, true );
                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im, 1);
                gdImageAlphaBlending(im2,1);
                gdImageSaveAlpha(im2, 1);
                gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                if( im2 ) gdImageDestroy(im2);
            }
        }
        for( int j = 0; j < shapeItems.size(); j++ )
        {
            bool fl_paint = false;
            for( int i =0; i < shape_temp_all.size(); i++ )
                if( j == shape_temp_all[i] )
                {
                    fl_paint = true;
                    break;
                }
            if( !fl_paint && shapeItems[j].type != 2 )
            {
                gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
                gdImageAlphaBlending(im2, 0);
                gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
                gdImageAlphaBlending(im2, 1);
                paintFigure( im2, shapeItems[j], xSc, ySc, true, true );
                gdImageAlphaBlending(im,1);
                gdImageSaveAlpha(im, 1);
                gdImageAlphaBlending(im2,1);
                gdImageSaveAlpha(im2, 1);
                gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
                if( im2 ) gdImageDestroy(im2);
            }
        }
    }
    return -1;
}

void VCAElFigure::getReq( SSess &ses )
{
    ResAlloc res(mRes,true);
    //- Prepare picture -
    map< string, string >::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    scaleHeight = (int)TSYS::realRound(height*ySc, 2, true);
    scaleWidth = (int)TSYS::realRound(width*xSc, 2, true);
    if( im ) gdImageDestroy(im);
    im = gdImageCreateTrueColor( scaleWidth, scaleHeight );
    if( !im ) ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    else
    {
        gdImageAlphaBlending(im, 0);
        gdImageFilledRectangle( im, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im,0,0,0,127) );
        gdImageAlphaBlending(im, 1);
        drawElF( ses, xSc, ySc, Point(-1,-1) );
        //- Get image and transfer it -
        int img_sz;
        char *img_ptr = (char *)gdImagePngPtr(im, &img_sz);
        ses.page.assign(img_ptr,img_sz);
        ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
        gdFree(img_ptr);
    }
}

void VCAElFigure::postReq( SSess &ses )
{
    ResAlloc res(mRes,true);

    map< string, string >::iterator prmEl = ses.prm.find("sub");
    if( prmEl != ses.prm.end() && prmEl->second == "point");
    {
        prmEl = ses.prm.find("xSc");
        double xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
        prmEl = ses.prm.find("ySc");
        double ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
	prmEl = ses.prm.find("x");
	int x_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : -1;
	prmEl = ses.prm.find("y");
	int y_coord = (prmEl!=ses.prm.end()) ? atoi(prmEl->second.c_str()) : -1;
	prmEl = ses.prm.find("key");
	string key = (prmEl!=ses.prm.end()) ? prmEl->second : "";
	if( x_coord < 0 || y_coord < 0 ) return;

        int clickFillNum = drawElF( ses, xSc, ySc, Point(x_coord,y_coord) );
        if( clickFillNum != -1 )
	{
	    XMLNode req("set");
	    req.setAttr("path",ses.url+"/%2fserv%2fattr");
            req.childAdd("el")->setAttr("id","event")->setText("ws_Fig"+clickFillNum+key);
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
    rel_list = false;
    for( int i_a = 0; i_a < node.childSize(); i_a++ )
    {
	req_el = node.childGet(i_a);
	if( req_el->name() != "el" )	continue;
	int uiPrmPos = atoi(req_el->attr("p").c_str());

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
                lineWdth = (int)TSYS::realRound(atof(req_el->text().c_str()));
                rel_list = true;
                break;
            case 21:	//lineClr
                lineClr =  mod->colorParse(req_el->text());
                if(lineClr == -1) lineClr = (127<<24)+(0<<16)+(0<<8)+0;
                if(lineClr == 0) lineClr = (0<<24)+(250<<16)+(0<<8)+0;
                rel_list = true;
                break;
            case 22:	//lineStyle
                lineStyle = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 23:	//bordWdth
                bordWdth = (int)TSYS::realRound(atof(req_el->text().c_str()));
                rel_list = true;
                break;
            case 24:	//bordClr
                bordClr = mod->colorParse(req_el->text());
                if(bordClr == -1) bordClr = (127<<24)+(0<<16)+(0<<8)+0;
                if(bordClr == 0) bordClr = (0<<24)+(250<<16)+(0<<8)+0;
                rel_list = true;
                break;
            case 25:	//fillClr
                fillClr = mod->colorParse(req_el->text());
                if(fillClr == -1)fillClr = (127<<24)+(0<<16)+(0<<8)+0;
                if(fillClr == 0) fillClr = (0<<24)+(250<<16)+(0<<8)+0;
                rel_list = true;
                break;
            case 26:
            {
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
                    Point pnt_ = (pnts)[pnt];
                    switch( patr )
                    {
                        case 0 : 
                            pnt_.x = atof(req_el->text().c_str());
                            (pnts)[pnt] = pnt_;
                            rel_list = true;
                            break;
                        case 1 :
                            pnt_.y = atof(req_el->text().c_str());
                            (pnts)[pnt] = pnt_;
                            rel_list = true;
                            break;
                        case 2 : 
                            (widths)[pnt] = (int)TSYS::realRound(atof(req_el->text().c_str()));
                            rel_list = true;
                            break;
                        case 3 : 
                            (colors)[pnt] = mod->colorParse(req_el->text());
                            if((colors)[pnt] == -1) (colors)[pnt] = (127<<24)+(0<<16)+(0<<8)+0;
                            if((colors)[pnt] == 0) (colors)[pnt] = (0<<24)+(250<<16)+(0<<8)+0;
                            rel_list = true;
                            break;
                        case 4 :
                            (images)[pnt] = req_el->text();
                            rel_list = true;
                            break;
                        case 5:
                            (styles)[pnt] = atoi(req_el->text().c_str());
                            rel_list = true;
                            break;
                    }

                }
	}
    }
    if( rel_list)
    {
        for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); )
            if(pi->first <= -10 ) pnts.erase ( pi++ );
            else ++pi;
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); ++pi )
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); ++pi )
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

                int w;
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) lnwidth  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) lnwidth = w;
                else lnwidth = lineWdth;
                
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) color = (0<<24)+(250<<16)+(0<<8)+0;
                    else color = mod->colorParse(el_s);
                }
                else color = lineClr;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) bord_width  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) bord_width = w;
                else bord_width = bordWdth;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) bord_color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) bord_color = (0<<24)+(250<<16)+(0<<8)+0;
                    else bord_color = mod->colorParse(el_s);
                }
                else bord_color = bordClr;
                
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "s%d", &w) == 1 ) style  = (styles)[w];
                else if( el_s.size() && (atoi(el_s.c_str()) == 0 || atoi(el_s.c_str()) == 1 || atoi(el_s.c_str()) == 2) )
                    style = atoi(el_s.c_str());
                else style = lineStyle;

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
                                      color,bord_color,lnwidth,bord_width,1,style,flag_brd) );
            }
            if( el == "arc" )
            {
                bool fl_wdt = false;
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str() );
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); ++pi )
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::iterator pi = pnts.begin(); pi != pnts.end(); ++pi )
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str() );
                else 
                {
                    p[2] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str() );
                else 
                {
                    p[3] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[4]  = atoi(el_s.c_str() );
                else 
                {
                    p[4] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }

                int w;
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) lnwidth  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) lnwidth = w;
                else lnwidth = lineWdth;
                
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) color = (0<<24)+(250<<16)+(0<<8)+0;
                    else color = mod->colorParse(el_s);
                }
                else color = lineClr;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) bord_width  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) bord_width = w;
                else bord_width = bordWdth;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) bord_color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) bord_color = (0<<24)+(250<<16)+(0<<8)+0;
                    else bord_color = mod->colorParse(el_s);
                }
                else bord_color = bordClr;
                
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "s%d", &w) == 1 ) style  = (styles)[w];
                else if( el_s.size() && (atoi(el_s.c_str()) == 0 || atoi(el_s.c_str()) == 1 || atoi(el_s.c_str()) == 2) )
                    style = atoi(el_s.c_str());
                else style = lineStyle;

                  //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                StartMotionPos=ip[0];
                EndMotionPos=ip[1];
                CtrlMotionPos_1=ip[2];
                CtrlMotionPos_2=ip[3];
                CtrlMotionPos_3=ip[4];
                MotionWidth=lnwidth;
                if(CtrlMotionPos_3.y<=CtrlMotionPos_1.y) 
                    ang = angle( CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y) );
                else ang = 360 - angle( CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y) );
                a = length(CtrlMotionPos_3, CtrlMotionPos_1);
                b = length(CtrlMotionPos_2, CtrlMotionPos_1);

                CtrlMotionPos_2 = Point( CtrlMotionPos_1.x+rotate(arc(0.25,a,b),ang).x,
                                         CtrlMotionPos_1.y-rotate(arc(0.25,a,b),ang).y );
                StartMotionPos = unrotate( StartMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y );
                if( StartMotionPos.x >= a )
                {
                    StartMotionPos.y = (StartMotionPos.y/StartMotionPos.x)*a;
                    StartMotionPos.x = a;
                }
                if( StartMotionPos.x < -a )
                {
                    StartMotionPos.y = (StartMotionPos.y/StartMotionPos.x)*(-a);
                    StartMotionPos.x = -a;
                }
                if( StartMotionPos.y <= 0 ) t_start = acos(StartMotionPos.x/a)/(2*M_PI);
                else t_start = 1 - acos(StartMotionPos.x/a)/(2*M_PI);
                EndMotionPos = unrotate(EndMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y);
                if( EndMotionPos.x < -a )
                {
                    EndMotionPos.y = (EndMotionPos.y/EndMotionPos.x)*(-a);
                    EndMotionPos.x = -a;
                }
                if( EndMotionPos.x >= a )
                {
                    EndMotionPos.y = (EndMotionPos.y/EndMotionPos.x)*a;
                    EndMotionPos.x = a;
                }
                if( EndMotionPos.y <= 0 ) t_end = acos(EndMotionPos.x/a)/(2*M_PI);
                else t_end = 1-acos(EndMotionPos.x/a)/(2*M_PI);
                if( t_start > t_end ) t_end += 1;
                if( (t_end-1) > t_start ) t_end -= 1;
                //if( t_start == t_end ) t_end += 1;
                if( fabs(t_start - t_end) < 0.0027777777777 ) t_end+=1;
                if( t_end > t_start && t_start >= 1 && t_end > 1 )
                {
                    t_start -= 1;
                    t_end -= 1;
                }
                CtrlMotionPos_4 = Point( t_start, t_end );
                bool flag_brd = false;
                if( lnwidth > 3 && bord_width == 0 )
                {
                    lnwidth -= 2;
                    bord_width = 1; 
                    bord_color = color;
                    flag_brd = true;
                }
                shapeItems.push_back( ShapeItem(p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,ang,color,bord_color,lnwidth,bord_width,2,style,flag_brd) );
            }
            if( el == "bezier" )
            {
                bool fl_wdt = false;
                //-- Reading anf setting attributes for the current arc --
                float x_s, y_s;
                string el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[0]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); ++pi )
                        if( pi->first <= -10 )
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[1]  = atoi(el_s.c_str());
                else 
                {
                    bool fl = false;
                    //-- Detecting if there is a point with same coordinates in the map --
                    for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); ++pi )
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
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[2]  = atoi(el_s.c_str());
                else 
                {
                    p[2] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) p[3]  = atoi(el_s.c_str());
                else 
                {
                    p[3] = map_index;
                    (pnts)[map_index] = Point(x_s,y_s);
                    map_index -= 1;
                }


                int w;
                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) lnwidth  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) lnwidth = w;
                else lnwidth = lineWdth;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) color = (0<<24)+(250<<16)+(0<<8)+0;
                    else color = mod->colorParse(el_s);
                }
                else color = lineClr;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "w%d", &w) == 1 ) bord_width  = (widths)[w];
                else if( sscanf(el_s.c_str(), "%d", &w) == 1 ) bord_width = w;
                else bord_width = bordWdth;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "c%d", &w) == 1 ) bord_color  = (colors)[w];
                else if( mod->colorParse(el_s) != -1 )
                {
                    if(mod->colorParse(el_s) == 0) bord_color = (0<<24)+(250<<16)+(0<<8)+0;
                    else bord_color = mod->colorParse(el_s);
                }
                else bord_color = bordClr;

                el_s = TSYS::strSepParse(sel,0,':',&el_off);
                if( sscanf(el_s.c_str(), "s%d", &w) == 1 ) style  = (styles)[w];
                else if( el_s.size() && (atoi(el_s.c_str()) == 0 || atoi(el_s.c_str()) == 1 || atoi(el_s.c_str()) == 2) )
                    style = atoi(el_s.c_str());
                else style = lineStyle;

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
                shapeItems.push_back( ShapeItem(p[0], p[1], p[2], p[3],-1,Point(0,0),ang,color,bord_color,lnwidth,bord_width,3,style,flag_brd) );
            }
            if( el == "fill" )
            {
                int zero_pnts = 0;
                string fl_color_1, fl_img, img;
                vector <int> fl_pnts;
                float x_s, y_s;
                int vl, wn, fl_color;
                while( true )
                {
                    string svl = TSYS::strSepParse(sel,0,':',&el_off);
                    if( sscanf(svl.c_str(), "(%f|%f)", &x_s, &y_s) != 2 ) vl = atoi(svl.c_str());
                    else 
                    {
                        bool fl = false;
                        //-- Detecting if there is a point with same coordinates in the map --
                        for( PntMap::reverse_iterator pi = pnts.rbegin(); pi != pnts.rend(); ++pi )
                            if( pi->first <= -10 )
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
                if( sscanf(fl_color_1.c_str(), "c%d", &wn) == 1 ) fl_color  = (colors)[wn];
                else if( mod->colorParse(fl_color_1) != -1 ) fl_color = mod->colorParse(fl_color_1);
                else fl_color = fillClr;

                if( sscanf(fl_img.c_str(), "i%d", &wn) == 1 ) img  = (images)[wn];
                else if( fl_img.size() ) img = fl_img;
                else img = imgDef;

		string imgDef_temp = owner().resGet(img,id(),user);
                if( imgDef_temp == "" ) img = "";
                inundationItems.push_back( InundationItem(fl_pnts, fl_color, -1, img) );
            }
        }
        for( WidthMap::iterator pi = widths.begin(); pi != widths.end(); )
        {
            bool unDel = false;
            for( int i=0; i < shapeItems.size(); i++ )
                if( pi->first > 0 && ( pi->second == shapeItems[i].width || pi->second == shapeItems[i].border_width ) )
                {
                    unDel = true;
                    break;
                }
            if( pi->first > 0 && unDel == false ) (widths).erase ( pi++ );
            else ++pi;
        }
        for( ColorMap::iterator pi = colors.begin(); pi != colors.end(); )
        {
            bool unDel = false;
            for( int i=0; i < shapeItems.size(); i++ )
                if( pi->first > 0 && ( pi->second == shapeItems[i].lineColor || pi->second == shapeItems[i].borderColor ) )
                {
                    unDel = true;
                    break;
                }
            if( !unDel )
                for( int i=0; i < inundationItems.size(); i++  )
                    if( pi->first > 0 && ( pi->second == inundationItems[i].P_color ) )
                    {
                        unDel = true;
                        break;
                    }
            if( pi->first > 0 && unDel == false ) (colors).erase ( pi++ );
            else ++pi;
        }
        for( ImageMap::iterator pi = images.begin(); pi != images.end(); )
        {
            bool unDel = false;
            for( int i=0; i < inundationItems.size(); i++ )
                if( pi->first > 0 && ( pi->second == inundationItems[i].imgFill ) )
                {
                    unDel = true;
                    break;
                }
            if( pi->first > 0 && unDel == false ) (images).erase ( pi++ );
            else ++pi;
        }
        for( StyleMap::iterator pi = styles.begin(); pi != styles.end(); )
        {
            bool unDel = false;
            for( int i=0; i < shapeItems.size(); i++ )
                if( pi->first > 0 && ( pi->second == shapeItems[i].style ) )
                {
                    unDel = true;
                    break;
                }
            if( pi->first > 0 && unDel == false ) (styles).erase ( pi++ );
            else ++pi;
        }

    }
}

//*************************************************
//* VCADiagram                                    *
//*************************************************
VCADiagram::VCADiagram( const string &iid ) : 
    VCAObj(iid), tTimeCurent(false), tTime(0), lstTrc(false), type(0), sclVerScl(100), sclVerSclOff(0)
{

}

void VCADiagram::getReq( SSess &ses )
{
    switch( type )
    {
	case 0:	makeTrendsPicture(ses);		break;
	case 1:	makeSpectrumPicture(ses);	break;
    }
}

void VCADiagram::makeImgPng( SSess &ses, gdImagePtr im )
{
    gdImageSaveAlpha(im,1);
    int img_sz;
    char *img_ptr = (char*)gdImagePngPtr( im, &img_sz );
    ses.page.assign( img_ptr, img_sz );
    ses.page = mod->httpHead( "200 OK", ses.page.size(), "image/png" ) + ses.page;
    gdFree(img_ptr);
    gdImageDestroy(im);
}

void VCADiagram::makeTrendsPicture( SSess &ses )
{
    ResAlloc res(mRes,true);

    //> Check for trend's data reload
    bool rld = true;
    if( tTimeCurent )	tTime = (long long)time(NULL)*1000000;
    else if( trcPer && lstTrc < time(NULL) )
    { tTime += (time(NULL)-lstTrc)*1000000; lstTrc = time(NULL); }
    else rld = false;
    if( rld )
    {
	for( int i_p = 0; i_p < trnds.size(); i_p++ ) trnds[i_p].loadData(ses.user);
	//> Trace cursors value
	if( active )
	{
	    long long tTimeGrnd = tTime - (long long)(1e6*tSize);
	    if( curTime >= (tTime-2*(long long)trcPer*1000000) || curTime <= tTimeGrnd )
		setCursor(tTime,ses.user);
	}
    }

    int mrkFontSize = 0;
    int mrkHeight = 0;
    int clr_grid, clr_mrk;						//Colors

    //> Get generic parameters
    long long tSz  = (long long)(1e6*tSize);				//Trends size (us)
    long long tEnd = tTime;						//Trends end point (us)
    tPict = tEnd;
    long long tBeg = tEnd - tSz;					//Trends begin point (us)

    //> Get scale
    map<string,string>::iterator prmEl = ses.prm.find("xSc");
    float xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    prmEl = ses.prm.find("ySc");
    float ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    int imW = (int)TSYS::realRound((float)width*xSc,2,true);
    int imH = (int)TSYS::realRound((float)height*ySc,2,true);

    //> Prepare picture
    gdImagePtr im = gdImageCreateTrueColor(imW,imH);
    gdImageAlphaBlending(im,0);
    gdImageFilledRectangle(im,0,0,imW-1,imH-1,gdImageColorResolveAlpha(im,0,0,0,127));
    int brect[8];

    if( !trnds.size() || tSz <= 0 )	{ makeImgPng(ses,im); return; }

    //> Make decoration and prepare trends area
    tArX = 1, tArY = 1,						//Curves of trends area rect
    tArW = imW-2*(geomMargin+bordWidth+1),
    tArH = imH-2*(geomMargin+bordWidth+1);

    if( sclHor&0x3 || sclVer&0x3 )
    {
	//>> Set grid color
	clr_grid = gdImageColorResolveAlpha(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor,127-(uint8_t)(sclColor>>24));
	//gdImageColorAllocate(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor);
	if( sclHor&0x2 || sclVer&0x2 )
	{
	    //>> Set markers font and color
	    mrkFontSize = (int)((float)sclMarkFontSize*vmin(xSc,ySc));
	    clr_mrk = gdImageColorResolveAlpha(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor,127-(uint8_t)(sclMarkColor>>24));
	    //gdImageColorAllocate(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor);
	    char *rez = gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.,0,0,"000000");
	    if( rez ) mess_err(nodePath().c_str(),_("gdImageStringFT for font '%s' error: %s\n"),sclMarkFont.c_str(),rez);
	    else mrkHeight = brect[3]-brect[7];
	    if( sclHor&0x2 )
	    {
	        if( tArH < (int)(100.0*vmin(xSc,ySc)) ) sclHor &= ~(0x02);
	        else tArH -= 2*(mrkHeight+2);
	    }
	    if( sclVer&0x2 && tArW < (int)(100.0*vmin(xSc,ySc)) ) sclVer &= ~(0x02);
	}
    }
    //> Calc horizontal scale
    long long hDiv = 1;					//Horisontal scale divisor
    int hmax_ln = tArW / (int)((sclHor&0x2 && mrkHeight)?(brect[2]-brect[6]):15.0*vmin(xSc,ySc));
    if( hmax_ln >= 2 )
    {
	int hvLev = 0;
	long long hLen = tEnd - tBeg;
	if( hLen/86400000000ll >= 2 )    { hvLev = 5; hDiv = 86400000000ll; }	//Days
	else if( hLen/3600000000ll >= 2 ){ hvLev = 4; hDiv =  3600000000ll; }	//Hours
	else if( hLen/60000000 >= 2 )    { hvLev = 3; hDiv =    60000000; }	//Minutes
	else if( hLen/1000000 >= 2 )     { hvLev = 2; hDiv =     1000000; }	//Seconds
	else if( hLen/1000 >= 2 )        { hvLev = 1; hDiv =        1000; }	//Milliseconds
	while( hLen/hDiv > hmax_ln )     hDiv *= 10;
	while( hLen/hDiv < hmax_ln/2 )   hDiv /= 2;
	if( hLen/hDiv >= 5 && trcPer )
	{
	    tPict = hDiv*(tEnd/hDiv+1);
	    tBeg = tPict-hLen;
	}

	//>> Draw horisontal grid and markers
	if( sclHor&0x3 )
	{
	    time_t tm_t;
	    struct tm ttm, ttm1;
	    string lab_tm, lab_dt;
	    //>>> Draw generic grid line
	    gdImageLine(im,tArX,tArY+tArH,tArX+tArW,tArY+tArH,clr_grid);
	    //>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -1;
	    int endMarkBrd = tArX+tArW;
	    if( sclHor&0x2 && mrkHeight )
	    {
		tm_t = tPict/1000000;
		localtime_r(&tm_t,&ttm);
		lab_dt = TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
		if( ttm.tm_sec == 0 && tPict%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if( tPict%1000000 == 0 ) lab_tm = TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
		else lab_tm = TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(tPict%1000000)/1e6);
		gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_dt.c_str());
		int markBrd = tArX+tArW-(brect[2]-brect[6]);
		endMarkBrd = markBrd;
		gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,tArY+tArH+3+2*(brect[3]-brect[7]),(char*)lab_dt.c_str());
		gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_tm.c_str());
		markBrd = tArX+tArW-(brect[2]-brect[6]);
		endMarkBrd = vmin(endMarkBrd,markBrd);
		gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,tArY+tArH+3+(brect[3]-brect[7]),(char*)lab_tm.c_str());
	    }

	    //>>> Draw grid and/or markers
	    bool first_m = true;
	    for( long long i_h = tBeg; true; )
	    {
		//>>>> Draw grid
		int h_pos = tArX+tArW*(i_h-tBeg)/(tPict-tBeg);
		if( sclHor&0x1 ) gdImageLine(im,h_pos,tArY,h_pos,tArY+tArH,clr_grid);
		else gdImageLine(im,h_pos,tArY+tArH-3,h_pos,tArY+tArH+3,clr_grid);
		//>>>> Draw markers
		if( sclHor&0x2 && mrkHeight && !(i_h%hDiv) && i_h != tPict )
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
		    lab_dt = lab_tm = "";
		    //Date
		    if( hvLev == 5 || chLev >= 4 )
			lab_dt = (chLev>=5 || chLev==-1) ? TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900) : TSYS::strMess("%d",ttm.tm_mday);
		    //Hours and minuts
		    if( (hvLev == 4 || hvLev == 3 || ttm.tm_hour || ttm.tm_min) && !ttm.tm_sec ) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		    //Seconds
		    else if( (hvLev == 2 || ttm.tm_sec) && !(i_h%1000000) )
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) : TSYS::strMess(_("%ds"),ttm.tm_sec);
		    //Milliseconds
		    else if( hvLev <= 1 || i_h%1000000 )
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				 (chLev>=1) ? TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
					      TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.);
		    int wdth, tpos, endPosTm = 0, endPosDt = 0;
		    if( lab_tm.size() )
		    {
			gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_tm.c_str());
			wdth = brect[2]-brect[6];
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tpos,tArY+tArH+3+(brect[3]-brect[7]),(char*)lab_tm.c_str());
			    endPosTm = tpos+wdth;
			}
		    }
		    if( lab_dt.size() )
		    {
			gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_dt.c_str());
			wdth = brect[2]-brect[6];
			tpos = vmax(h_pos-wdth/2,0);
			if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			{
			    gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tpos,tArY+tArH+3+2*(brect[3]-brect[7]),(char*)lab_dt.c_str());
			    endPosDt = tpos+wdth;
			}
		    }
		    begMarkBrd = vmax(begMarkBrd,vmax(endPosTm,endPosDt));
		    memcpy((char*)&ttm1,(char*)&ttm,sizeof(tm));
		    first_m = false;
		}
		//>>>> Next
		if( i_h >= tPict )	break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tPict )	i_h = tPict;
	    }
	}
    }

    int prmRealSz = -1;
    //>> Calc real parameters size
    for( int i_p = 0; i_p < trnds.size(); i_p++ )
	if( trnds[i_p].val().size() && !((trnds[i_p].color()>>31)&0x01) )
	{
	    if( prmRealSz == -1 ) prmRealSz = i_p;
	    else if( prmRealSz >= 0 ) prmRealSz = -2;
	    else prmRealSz -= 1;
	}

    //> Calc vertical scale
    long long aVend;					//Corrected for allow data the trend end point
    long long aVbeg;					//Corrected for allow data the trend begin point
    double vsMax = 100, vsMin = 0;      //Trend's vertical scale border
    bool   vsPerc = true;               //Vertical scale percent mode
    if( prmRealSz >= 0 )
    {
	vsPerc = false;
	if( trnds[prmRealSz].bordU() <= trnds[prmRealSz].bordL() && trnds[prmRealSz].valTp() != 0 )
	{
	    //>> Check trend for valid data
	    aVbeg = vmax(tBeg,trnds[prmRealSz].valBeg());
	    aVend = vmin(tEnd,trnds[prmRealSz].valEnd());

	    if( aVbeg >= aVend )	{ makeImgPng(ses,im); return; }
	    //>> Calc value borders
	    vsMax = -3e300, vsMin = 3e300;
	    bool end_vl = false;
	    int ipos = trnds[prmRealSz].val(aVbeg);
	    if( ipos && trnds[prmRealSz].val()[ipos].tm > aVbeg ) ipos--;
	    while( true )
	    {
		if( ipos >= trnds[prmRealSz].val().size() || end_vl )	break;
		if( trnds[prmRealSz].val()[ipos].tm >= aVend )	end_vl = true;
		if( trnds[prmRealSz].val()[ipos].val != EVAL_REAL )
		{
		    vsMin  = vmin(vsMin,trnds[prmRealSz].val()[ipos].val);
		    vsMax  = vmax(vsMax,trnds[prmRealSz].val()[ipos].val);
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
	else if( trnds[prmRealSz].bordU() <= trnds[prmRealSz].bordL() && trnds[prmRealSz].valTp() == 0 ) { vsMax = 1.5; vsMin = -0.5; }
	else { vsMax = trnds[prmRealSz].bordU(); vsMin = trnds[prmRealSz].bordL(); }
    }

    //>> Vertical scale and offset apply
    float vsDif = vsMax - vsMin;
    vsMax += sclVerSclOff*vsDif/100; vsMin += sclVerSclOff*vsDif/100;
    vsMax += (sclVerScl*vsDif/100-vsDif)/2; vsMin -= (sclVerScl*vsDif/100-vsDif)/2;

    float vmax_ln = tArH / ( (sclVer&0x2 && mrkHeight)?(2*mrkHeight):(int)(15.0*vmin(xSc,ySc)) );
    if( vmax_ln >= 2 )
    {
	double vDiv = 1;
	double v_len = vsMax - vsMin;
	while( v_len > vmax_ln )	{ vDiv *= 10; v_len /= 10; }
	while( v_len < vmax_ln/10 )	{ vDiv /= 10; v_len *= 10; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while( ((vsMax-vsMin)/vDiv) < vmax_ln/2 ) vDiv/=2;

	//>> Draw vertical grid and markers
	if( sclVer&0x3 )
	{
	    gdImageLine(im,tArX,tArY,tArX,tArH,clr_grid);
	    for( double i_v = vsMin; (vsMax-i_v)/vDiv > -0.1; i_v+=vDiv )
	    {
		//>>> Draw grid
		int v_pos = tArY+tArH-(int)((double)tArH*(i_v-vsMin)/(vsMax-vsMin));
		if( sclVer&0x1 ) gdImageLine(im,tArX,v_pos,tArX+tArW,v_pos,clr_grid);
		else gdImageLine(im,tArX-3,v_pos,tArX+3,v_pos,clr_grid);
		//>>> Draw markers
		if( sclVer&0x2 && mrkHeight )
		{
		    bool isMax = (fabs((vsMax-i_v)/vDiv) < 0.1);
		    gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tArX+2,v_pos+(isMax?mrkHeight:0),
			(char*)(TSYS::strMess("%0.4g",i_v)+((vsPerc&&isMax)?" %":"")).c_str());
		}
	    }
	}
    }

    //> Draw trends
    for( int i_t = 0; i_t < trnds.size(); i_t++ )
    {
	//>> Set trend's pen
	int clr_t = gdImageColorResolveAlpha(im,(uint8_t)(trnds[i_t].color()>>16),(uint8_t)(trnds[i_t].color()>>8),(uint8_t)trnds[i_t].color(),127-(uint8_t)(trnds[i_t].color()>>24));
	//gdImageColorAllocate(im,(uint8_t)(trnds[i_t].color()>>16),(uint8_t)(trnds[i_t].color()>>8),(uint8_t)trnds[i_t].color());

	//>> Prepare generic parameters
	aVbeg = vmax(tBeg,trnds[i_t].valBeg());
	aVend = vmin(tEnd,trnds[i_t].valEnd());
	if( aVbeg >= aVend || (trnds[i_t].color()>>31)&0x01 ) continue;
	int aPosBeg = trnds[i_t].val(aVbeg);;
	if( aPosBeg && trnds[i_t].val()[aPosBeg].tm > aVbeg ) aPosBeg--;

	//>> Prepare border for percent trend
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
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>> Draw trend
	bool	end_vl = false;
	double	curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int	curPos, averPos = 0, prevPos = 0, c_vpos, z_vpos;
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
		if( trnds[i_t].valTp() == 0 )
		    z_vpos = tArY+tArH-(int)((double)tArH*( (vsPerc ? (100.*(0-bordL)/(bordU-bordL)) : 0) - vsMin )/(vsMax-vsMin));
		int c_vpos = tArY+tArH-(int)((double)tArH*vmax(0,vmin(1,(averVl-vsMin)/(vsMax-vsMin))));
		if( trnds[i_t].valTp() != 0 ) gdImageSetPixel(im,averPos,c_vpos,clr_t);
		else gdImageLine(im,averPos,z_vpos,averPos,c_vpos,clr_t);
		if( prevVl != EVAL_REAL )
		{
		    int c_vpos_prv = tArY+tArH-(int)((double)tArH*vmax(0,vmin(1,(prevVl-vsMin)/(vsMax-vsMin))));
		    if( trnds[i_t].valTp() != 0 ) gdImageLine(im,prevPos,c_vpos_prv,averPos,c_vpos,clr_t);
		    else
			for( int sps = prevPos+1; sps <= averPos; sps++ )
			    gdImageLine(im,sps,z_vpos,sps,c_vpos,clr_t);
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

    //> Draw cursor
    if( active && curTime && tBeg && tPict && (curTime >= tBeg || curTime <= tPict) )
    {
	//>> Set trend's pen
	int clr_cur = gdImageColorResolveAlpha(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor,127-(uint8_t)(curColor>>24));
	//gdImageColorAllocate(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor);
	int curPos = tArX+tArW*(curTime-tBeg)/(tPict-tBeg);
	gdImageLine(im,curPos,tArY,curPos,tArY+tArH,clr_cur);
    }

    //> Get image and transfer it
    makeImgPng(ses,im);
}

void VCADiagram::makeSpectrumPicture( SSess &ses )
{
    ResAlloc res(mRes,true);

    //> Check for trend's data reload
    bool rld = true;
    if( tTimeCurent )	tTime = (long long)time(NULL)*1000000;
    else if( trcPer && lstTrc < time(NULL) )
    { tTime += (time(NULL)-lstTrc)*1000000; lstTrc = time(NULL); }
    else rld = false;
    if( rld ) for( int i_p = 0; i_p < trnds.size(); i_p++ ) trnds[i_p].loadData(ses.user);

    int mrkFontSize = 0;
    int mrkHeight = 0;
    int clr_grid, clr_mrk;						//Colors

    //> Get generic parameters
    long long tSz  = (long long)(1e6*tSize);				//Time size (us)
    long long tEnd = tTime;						//Time end point (us)
    long long tBeg = tEnd - tSz;					//Time begin point (us)

    //> Get scale
    map<string,string>::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? atof(prmEl->second.c_str()) : 1.0;
    int imW = (int)TSYS::realRound((double)width*xSc,2,true);
    int imH = (int)TSYS::realRound((double)height*ySc,2,true);

    //> Prepare picture
    gdImagePtr im = gdImageCreateTrueColor(imW,imH);
    gdImageAlphaBlending(im,0);
    gdImageFilledRectangle(im,0,0,imW-1,imH-1,gdImageColorResolveAlpha(im,0,0,0,127));
    int brect[8];

    if( !trnds.size() || tSz <= 0 )	{ makeImgPng(ses,im); return; }

    //> Make decoration and prepare trends area
    tArX = 1, tArY = 1,						//Curves of trends area rect
    tArW = imW-2*(geomMargin+bordWidth+1),
    tArH = imH-2*(geomMargin+bordWidth+1);

    //> Process scale
    if( sclHor&0x3 || sclVer&0x3 )
    {
	//>> Set grid color
	clr_grid = gdImageColorResolveAlpha(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor,127-(uint8_t)(sclColor>>24));
	//gdImageColorAllocate(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor);
	if( (sclHor&0x2 || sclVer&0x2) && mrkHeight )
	{
	    //>> Set markers font and color
	    mrkFontSize = (int)((double)sclMarkFontSize*vmin(xSc,ySc));
	    clr_mrk = gdImageColorResolveAlpha(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor,127-(uint8_t)(sclMarkColor>>24));
	    //gdImageColorAllocate(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor);
	    char *rez = gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.,0,0,"000000");
	    if( rez ) mess_err(nodePath().c_str(),_("gdImageStringFT for font '%s' error: %s\n"),sclMarkFont.c_str(),rez);
	    else mrkHeight = brect[3]-brect[7];
	    if( sclHor&0x2 )
	    {
		if( tArH < (int)(100.0*vmin(xSc,ySc)) ) sclHor &= ~(0x02);
		else tArH -= mrkHeight+4;
	    }
	    if( sclVer&0x2 && tArW < (int)(100.0*vmin(xSc,ySc)) ) sclVer &= ~(0x02);
	}
    }

    //> Calc horizontal scale
    int fftN = (int)(width+0.5);
    fftBeg = 1e6/(double)tSz;			//Minimum frequency or maximum period time (s)
    fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
    double hDiv = 1;				//Horisontal scale divisor
    int hmax_ln = tArW / (int)((sclHor&0x2 && mrkHeight)?(brect[2]-brect[6]):15.0*vmin(xSc,ySc));
    if( hmax_ln >= 2 )
    {
	double hLen = fftEnd-fftBeg;
	while( hLen/hDiv > hmax_ln )	hDiv *= 10;
	while( hLen/hDiv < hmax_ln/10)	hDiv /= 10;
	fftBeg = floor(10*fftBeg/hDiv)*hDiv/10;
	fftEnd = ceil(10*fftEnd/hDiv)*hDiv/10;
	while(((fftEnd-fftBeg)/hDiv) < hmax_ln/2) hDiv/=2;

	//>> Draw horisontal grid and markers
	if( sclHor&0x3 )
	{
	    string labH;
	    double labDiv = 1;
	    if( fftEnd>1000 ) labDiv = 1000;
	    //>>> Draw generic grid line
	    gdImageLine(im,tArX,tArY+tArH,tArX+tArW,tArY+tArH,clr_grid);
	    //>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -1;
	    int endMarkBrd = tArX+tArW;
	    if( sclHor&0x2 && mrkHeight )
	    {
		labH = TSYS::strMess("%0.4g",fftEnd/labDiv)+((labDiv==1000)?_("kHz"):_("Hz"));
		gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)labH.c_str());
		int markBrd = tArX+tArW-(brect[2]-brect[6]);
		endMarkBrd = vmin(endMarkBrd,markBrd);
		gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,tArY+tArH+3+(brect[3]-brect[7]),(char*)labH.c_str());
	    }
	    //>>> Draw grid and/or markers
	    for( double i_h = fftBeg; (fftEnd-i_h)/hDiv > -0.1; i_h+=hDiv )
	    {
		//>>>> Draw grid
		int h_pos = tArX+(int)((double)tArW*(i_h-fftBeg)/(fftEnd-fftBeg));
		if( sclHor&0x1 ) gdImageLine(im,h_pos,tArY,h_pos,tArY+tArH,clr_grid);
		else gdImageLine(im,h_pos,tArY+tArH-3,h_pos,tArY+tArH+3,clr_grid);

		if( sclHor&0x2 && mrkHeight )
		{
		    labH = TSYS::strMess("%0.4g",i_h/labDiv);
		    gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)labH.c_str());
		    int wdth = brect[2]-brect[6];
		    int tpos = vmax(h_pos-wdth/2,0);
		    if( (tpos+wdth) < endMarkBrd && tpos > begMarkBrd )
			gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tpos,tArY+tArH+3+(brect[3]-brect[7]),(char*)labH.c_str());
		    begMarkBrd = vmax(begMarkBrd,tpos+wdth);
		}
	    }
	}
    }

    int prmRealSz = -1;
    //>> Calc real parameters size
    for( int i_p = 0; i_p < trnds.size(); i_p++ )
	if( trnds[i_p].fftN && !((trnds[i_p].color()>>31)&0x01) )
	{
	    if( prmRealSz == -1 ) prmRealSz = i_p;
	    else if( prmRealSz >= 0 ) prmRealSz = -2;
	    else prmRealSz -= 1;
	}

    //>> Calc vertical scale
    double vsMax = 100, vsMin = 0, curVl;	//Trend's vertical scale border
    bool   vsPerc = true;			//Vertical scale percent mode
    if( prmRealSz >= 0 )
    {
	if( !trnds[prmRealSz].fftN ) { makeImgPng(ses,im); return; }

	vsPerc = false;
	if( trnds[prmRealSz].bordU() > trnds[prmRealSz].bordL() )
	{ vsMax = trnds[prmRealSz].bordU(); vsMin = trnds[prmRealSz].bordL(); }
	else
	{
	    //>>> Calc value borders
	    vsMax = -3e300, vsMin = 3e300;
	    double vlOff = trnds[prmRealSz].fftOut[0][0]/trnds[prmRealSz].fftN;
	    for( int i_v = 1; i_v < (trnds[prmRealSz].fftN/2+1); i_v++ )
	    {
		curVl = vlOff+pow(pow(trnds[prmRealSz].fftOut[i_v][0],2)+pow(trnds[prmRealSz].fftOut[i_v][1],2),0.5)/(trnds[prmRealSz].fftN/2+1);
		vsMin = vmin(vsMin,curVl);
		vsMax = vmax(vsMax,curVl);
	    }
	    if( vsMax == vsMin )	{ vsMax += 1.0; vsMin -= 1.0; }
	    else if( (vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.001 )
	    {
		double wnt_dp = 0.001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
		vsMin -= wnt_dp/2;
		vsMax += wnt_dp/2;
	    }
	}
    }

    //>> Vertical scale and offset apply
    float vsDif = vsMax - vsMin;
    vsMax += sclVerSclOff*vsDif/100; vsMin += sclVerSclOff*vsDif/100;
    vsMax += (sclVerScl*vsDif/100-vsDif)/2; vsMin -= (sclVerScl*vsDif/100-vsDif)/2;

    double vmax_ln = tArH / ( (sclVer&0x2 && mrkHeight)?(2*mrkHeight):(int)(15.0*vmin(xSc,ySc)) );
    if( vmax_ln >= 2 )
    {
	double vDiv = 1.;
	double v_len = vsMax - vsMin;
	while( v_len > vmax_ln )	{ vDiv *= 10; v_len /= 10; }
	while( v_len < vmax_ln/10 )	{ vDiv /= 10; v_len *= 10; }
	vsMin = floor(vsMin/vDiv)*vDiv;
	vsMax = ceil(vsMax/vDiv)*vDiv;
	while( ((vsMax-vsMin)/vDiv) < vmax_ln/2 ) vDiv/=2;

	//>>> Draw vertical grid and markers
	if( sclVer&0x3 )
	{
	    gdImageLine(im,tArX,tArY,tArX,tArH,clr_grid);
	    for( double i_v = vsMin; (vsMax-i_v)/vDiv > -0.1; i_v += vDiv )
	    {
		int v_pos = tArY+tArH-(int)((double)tArH*(i_v-vsMin)/(vsMax-vsMin));
		if( sclVer&0x1 ) gdImageLine(im,tArX,v_pos,tArX+tArW,v_pos,clr_grid);
		else gdImageLine(im,tArX-3,v_pos,tArX+3,v_pos,clr_grid);
		if( sclVer&0x2 && mrkHeight )
		{
		    bool isMax = (fabs((vsMax-i_v)/vDiv) < 0.1);
		    gdImageStringFT(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,tArX+2,v_pos+(isMax?mrkHeight:0),
			(char*)(TSYS::strMess("%0.4g",i_v)+((vsPerc&&isMax)?" %":"")).c_str());
		}
	    }
	}
    }

    //>> Draw trends trnds[i_t];
    for( int i_t = 0; i_t < trnds.size(); i_t++ )
    {
	if( !trnds[i_t].fftN || (trnds[i_t].color()>>31)&0x01 ) continue;

	int clr_t = gdImageColorResolveAlpha(im,(uint8_t)(trnds[i_t].color()>>16),(uint8_t)(trnds[i_t].color()>>8),(uint8_t)trnds[i_t].color(),127-(uint8_t)(trnds[i_t].color()>>24));
	//gdImageColorAllocate(im,(uint8_t)(trnds[i_t].color()>>16),(uint8_t)(trnds[i_t].color()>>8),(uint8_t)trnds[i_t].color());
	double vlOff = trnds[i_t].fftOut[0][0]/trnds[i_t].fftN;
	double fftDt = (1e6/(double)tSz)*(double)width/trnds[i_t].fftN;

	//>>> Prepare border for percent trend
	double bordL = trnds[i_t].bordL();
	double bordU = trnds[i_t].bordU();
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    for( int i_v = 1; i_v < (trnds[i_t].fftN/2+1); i_v++ )
	    {
		curVl = vlOff+pow(pow(trnds[i_t].fftOut[i_v][0],2)+pow(trnds[i_t].fftOut[i_v][1],2),0.5)/(trnds[i_t].fftN/2+1);
		bordL = vmin(bordL,curVl);
		bordU = vmax(bordU,curVl);
	    }
	    double vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>>> Draw trend
	double prevVl = EVAL_REAL;
	int curPos = 0, prevPos = 0;
	for( int i_v = 1; i_v < (trnds[i_t].fftN/2+1); i_v++ )
	{
	    curVl = vlOff+pow(pow(trnds[i_t].fftOut[i_v][0],2)+pow(trnds[i_t].fftOut[i_v][1],2),0.5)/(trnds[i_t].fftN/2+1);
	    if( vsPerc )
	    {
		curVl = 100.*(curVl-bordL)/(bordU-bordL);
		curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
	    }
	    curPos = tArX+(int)((double)tArW*(fftDt*i_v-fftBeg)/(fftEnd-fftBeg));

	    int c_vpos = tArY+tArH-(int)((double)tArH*vmax(0,vmin(1,(curVl-vsMin)/(vsMax-vsMin))));
	    if( prevVl == EVAL_REAL ) gdImageSetPixel(im,curPos,c_vpos,clr_t);
	    else
	    {
		int c_vpos_prv = tArY+tArH-(int)((double)tArH*vmax(0,vmin(1,(prevVl-vsMin)/(vsMax-vsMin))));
		gdImageLine(im,prevPos,c_vpos_prv,curPos,c_vpos,clr_t);
	    }
	    prevPos = curPos;
	    prevVl = curVl;
	}

	//>>> Update value on cursor
	if( active )
	{
	    double curFrq = vmax(vmin(1e6/(double)curTime,fftEnd),fftBeg);
	    curPos = (int)(curFrq/fftDt);
	    if( curPos >= 1 && curPos < (trnds[i_t].fftN/2+1) )
	    {
		double val = trnds[i_t].fftOut[0][0]/trnds[i_t].fftN + pow(pow(trnds[i_t].fftOut[curPos][0],2)+pow(trnds[i_t].fftOut[curPos][1],2),0.5)/(trnds[i_t].fftN/2+1);
		XMLNode req("set");
		req.setAttr("path",id()+"/%2fserv%2fattr")->
		    childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",i_t))->setText(TSYS::real2str(val,6));
		mod->cntrIfCmd(req,ses.user);
	    }
	}
    }

    //> Draw cursor
    if( active && tSz )
    {
	float curFrq = vmax(vmin(1e6/(float)curTime,fftEnd),fftBeg);
	int curPos = tArX+(int)(tArW*(curFrq-fftBeg)/(fftEnd-fftBeg));
	int clr_cur = gdImageColorResolveAlpha(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor,127-(uint8_t)(curColor>>24));
	//gdImageColorAllocate(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor);
	gdImageLine(im,curPos,tArY,curPos,tArY+tArH,clr_cur);
    }

    //> Get image and transfer it
    makeImgPng(ses,im);
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
	    if( type == 0 )
	    {
		long long tTimeGrnd = tPict - (long long)(1e6*tSize);
		setCursor( tTimeGrnd + (tPict-tTimeGrnd)*(x_coord-tArX)/tArW, ses.user );
	    }
	    else if( type == 1 )
		setCursor( (long long)(1e6/(fftBeg+(fftEnd-fftBeg)*(x_coord-tArX)/tArW)), ses.user );
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
	if( req_el->name() != "el" )	continue;
	int uiPrmPos = atoi(req_el->attr("p").c_str());
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
	    case 25:	//trcPer
		trcPer = atoi(req_el->text().c_str());
		break;
	    case 26:	//type
		type = atoi(req_el->text().c_str());
		reld_tr_dt = 2;
		break;
	    case 27:	//tSek
		tTimeCurent = false;
		if( atoll(req_el->text().c_str()) == 0 )
		{
		    tTime = (long long)time(NULL)*1000000;
		    tTimeCurent = true;
		} else tTime = atoll(req_el->text().c_str())*1000000 + tTime%1000000;
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case 28:	//tUSek
		tTime = 1000000ll*(tTime/1000000)+atoll(req_el->text().c_str());
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case 29:	//tSize
		tSize = atof(req_el->text().c_str());
		reld_tr_dt = 2;
		break;
	    case 30:	//curSek
		curTime = atoll(req_el->text().c_str())*1000000 + curTime%1000000;	break;
	    case 31:	//curUSek
		curTime = 1000000ll*(curTime/1000000)+atoll(req_el->text().c_str());	break;
	    case 32:	//curColor
		curColor = mod->colorParse(req_el->text());				break;
	    case 33:	//sclColor
		sclColor = mod->colorParse(req_el->text());				break;
	    case 34:	//sclHor
		sclHor = atoi(req_el->text().c_str());					break;
	    case 35:	//sclVer
		sclVer = atoi(req_el->text().c_str());					break;
	    case 36:	//sclMarkColor
		sclMarkColor = mod->colorParse(req_el->text());				break;
	    case 37:	//sclMarkFont
	    {
		char family[101]; strcpy(family,"Arial");
		int bold = 0, italic = 0;
		sclMarkFontSize = 10;
		sscanf(req_el->text().c_str(),"%100s %d %d %d",family,&sclMarkFontSize,&bold,&italic);
		sclMarkFont = family;
		for( int p = 0; p < sclMarkFont.size(); p++ ) if( sclMarkFont[p] == '_' ) sclMarkFont[p] = ' ';
		if( bold ) sclMarkFont += ":bold";
		if( italic ) sclMarkFont += ":italic";
		//> Font size correct
		int brect[8];
		gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),sclMarkFontSize,0.,0,0,"Test");
		if( (brect[3]-brect[7]) > sclMarkFontSize )
		    sclMarkFontSize = (int)((float)sclMarkFontSize*((float)sclMarkFontSize/(float)(brect[3]-brect[7])));
		break;
	    }
	    case 38:	//valArch
		valArch = req_el->text();
		reld_tr_dt = 2;
		break;
	    case 39:	//parNum
	    {
		int parNum = atoi(req_el->text().c_str());
		if( parNum == trnds.size() )	break;
		while( trnds.size() > parNum )	trnds.pop_back();
		while( parNum > trnds.size() )	trnds.push_back( TrendObj(this) );
		break;
	    }
	    case 40:        //sclVerScl
		if( sclVerScl == atof(req_el->text().c_str()) )		break;
		sclVerScl = atof(req_el->text().c_str());
		break;
	    case 41:        //sclVerSclOff
		if( sclVerSclOff == atof(req_el->text().c_str()) )	break;
		sclVerSclOff = atof(req_el->text().c_str());
		break;
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
    if( type == 0 )
    {
	long long tTimeGrnd = tTime - (long long)(1e6*tSize);
	curTime = vmax(vmin(itm,tTime),tTimeGrnd);

	XMLNode req("set");
	req.setAttr("path",id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(TSYS::int2str(curTime/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(TSYS::int2str(curTime%1000000));

	//> Update trend's current values
	for( int i_p = 0; i_p < trnds.size(); i_p++ )
	{
	    int vpos = trnds[i_p].val(curTime);
	    if( !trnds[i_p].val().size() || (!tTimeCurent && vpos >= trnds[i_p].val().size()) ) continue;
	    vpos = vmax(0,vmin(trnds[i_p].val().size()-1,vpos));
	    if( vpos && trnds[i_p].val()[vpos].tm > curTime ) vpos--;
	    double val = trnds[i_p].val()[vpos].val;
	    if( val != trnds[i_p].curVal() )
		req.childAdd("el")->setAttr("id","prm"+TSYS::int2str(i_p)+"val")->setText(TSYS::real2str(val,6));
	}
	mod->cntrIfCmd(req,user);
    }
    else if( type == 1 )
    {
	float curFrq = vmax(vmin(1e6/(float)itm,fftEnd),fftBeg);

	XMLNode req("set");
	req.setAttr("path",id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(TSYS::int2str(((long long)(1e6/curFrq))/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(TSYS::int2str(((long long)(1e6/curFrq))%1000000));

	//> Update trend's current values
	for( int i_p = 0; i_p < trnds.size(); i_p++ )
	{
	    if( !trnds[i_p].fftN ) continue;
	    float fftDt = (1/tSize)*(float)width/trnds[i_p].fftN;
	    int vpos = (int)(curFrq/fftDt);
	    double val = EVAL_REAL;
	    if( vpos >= 1 && vpos < (trnds[i_p].fftN/2+1) )
		val = trnds[i_p].fftOut[0][0]/trnds[i_p].fftN +
		    pow(pow(trnds[i_p].fftOut[vpos][0],2)+pow(trnds[i_p].fftOut[vpos][1],2),0.5)/(trnds[i_p].fftN/2+1);
	    req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",i_p))->setText(TSYS::real2str(val,6));
	}
	mod->cntrIfCmd(req,user);
    }
}

//* Trend object's class                         *
//************************************************
VCADiagram::TrendObj::TrendObj( VCADiagram *iowner ) :
    m_owner(iowner), m_bord_low(0), m_bord_up(0), m_curvl(EVAL_REAL), arh_beg(0), arh_end(0), arh_per(0), val_tp(0), fftOut(NULL), fftN(0)
{
    loadData("root");
}

VCADiagram::TrendObj::~TrendObj( )
{
    if( fftOut ) { delete fftOut; fftN = 0; }
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
    switch( owner().type )
    {
	case 0:	loadTrendsData(user,full);	break;
	case 1:	loadSpectrumData(user,full);	break;
    }
}

void VCADiagram::TrendObj::loadTrendsData( const string &user, bool full )
{
    long long tSize     = (long long)(1e6*owner().tSize);
    long long tTime     = owner().tTime;
    long long tTimeGrnd = tTime - tSize;
    long long wantPer   = tSize/(int)(owner().width+0.5);
    string arch = owner().valArch;

    //> Clear trend for empty address and the full reload data
    if( full || addr().empty() )
    {
	arh_per = arh_beg = arh_end = 0;
	val_tp = 0;
	vals.clear();
	if( addr().empty() )    return;
    }
    //> Get archive parameters
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

    //> One request check and prepare
    int trcPer = owner().trcPer*1000000;
    if( owner().tTimeCurent && trcPer && owner().valArch.empty() && (!arh_per || (arh_per >= trcPer && (tTime-valEnd())/vmax(wantPer,trcPer) < 2)) )
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
	    if( valEnd() && (lst_tm-valEnd())/vmax(wantPer,trcPer) > 2 ) vals.push_back(SHg(lst_tm-trcPer,EVAL_REAL));
	    else if( (lst_tm-valEnd()) >= wantPer ) vals.push_back(SHg(lst_tm,curVal));
	    else if( vals[vals.size()-1].val == EVAL_REAL ) vals[vals.size()-1].val = curVal;
	    else if( curVal != EVAL_REAL )
	    {
		int s_k = lst_tm-wantPer*(lst_tm/wantPer), n_k = trcPer;
		vals[vals.size()-1].val = (vals[vals.size()-1].val*s_k+curVal*n_k)/(s_k+n_k);
	    }
	    while( vals.size() > 2000 ) vals.pop_front();
	}
	return;
    }

    if( !arh_per )      return;
    //> Correct request to archive border
    wantPer	= (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime	= vmin(tTime,arh_end);
    tTimeGrnd	= vmax(tTimeGrnd,arh_beg);
    //> Clear data at time error
    if( tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer )
	vals.clear();
    if( tTime <= tTimeGrnd ) return;
    //> Check for request to present in buffer data
    if( tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer )    return;
    //> Correct request to present data
    if( valEnd() && tTime > valEnd() )          tTimeGrnd = valEnd()+1;
    else if( valBeg() && tTimeGrnd < valBeg() ) tTime = valBeg()-1;
    //> Get values data
    long long bbeg, bend, bper;
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
    //> Get data buffer parameters
    bbeg = atoll(req.attr("tm_grnd").c_str());
    bend = atoll(req.attr("tm").c_str());
    bper = atoll(req.attr("per").c_str());

    if( !bbeg || !bend || req.text().empty() ) return;

    prevPos = 0;
    prevVal = EVAL_REAL;
    buf.clear();
    for( int v_off = 0; true; )
    {
	svl = TSYS::strSepParse(req.text(),0,'\n',&v_off);
	if( svl.size() )
	{
	    sscanf(svl.c_str(),"%d %lf",&curPos,&curVal);
	    if( (val_tp == 0 && curVal == EVAL_BOOL) || (val_tp == 1 && curVal == EVAL_INT) ) curVal = EVAL_REAL;
	}
	else curPos = ((bend-bbeg)/bper)+1;
	for( ; prevPos < curPos; prevPos++ ) buf.push_back(SHg(bbeg+prevPos*bper,prevVal));
	prevVal = curVal;
	if( prevPos > (bend-bbeg)/bper ) break;
    }

    //> Append buffer to values deque
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
    //> Check for archive jump
    if( arch.empty() && (bbeg-tTimeGrnd)/bper ) { tTime = bbeg-bper; goto m1; }
}

void VCADiagram::TrendObj::loadSpectrumData( const string &user, bool full )
{
    loadTrendsData(user,full);

    if( !valBeg( ) || !valEnd( ) ) return;

    if( fftOut ) { delete fftOut; fftN = 0; }

    long long tSize	= (long long)(1e6*owner().tSize);
    long long tTime	= owner().tTime;
    long long tTimeGrnd	= tTime - tSize;
    long long workPer	= tSize/(int)(owner().width+0.5);

    tTimeGrnd = vmax(tTimeGrnd,valBeg());
    tTime = vmin(tTime,valEnd());

    fftN = (tTime-tTimeGrnd)/workPer;
    double fftIn[fftN];
    fftOut = (fftw_complex*)malloc(sizeof(fftw_complex)*(fftN/2+1));

    int fftFirstPos = -1, fftLstPos = -1;
    for( int a_pos = val(tTimeGrnd); a_pos < val().size() && val()[a_pos].tm <= tTime; a_pos++ )
    {
	int fftPos = (val()[a_pos].tm-tTimeGrnd)/workPer;
	if( fftPos >= fftN ) break;
	if( val()[a_pos].val == EVAL_REAL ) continue;
	if( fftFirstPos < 0 ) fftFirstPos = fftPos;

	if( fftLstPos == fftPos ) fftIn[fftPos-fftFirstPos] = (fftIn[fftPos-fftFirstPos]+val()[a_pos].val)/2;
	else fftIn[fftPos-fftFirstPos] = val()[a_pos].val;

	for( ; fftLstPos >= 0 && (fftLstPos+1) < fftPos; fftLstPos++ )
	    fftIn[fftLstPos-fftFirstPos+1] = fftIn[fftLstPos-fftFirstPos];
	fftLstPos = fftPos;
    }

    fftN = fftLstPos-fftFirstPos;
    if( fftN < 20 ) { delete fftOut; fftOut = NULL; fftN = 0; return; }

    fftw_plan p = fftw_plan_dft_r2c_1d( fftN, fftIn, fftOut, FFTW_ESTIMATE );
    fftw_execute(p);
    fftw_destroy_plan(p);
}

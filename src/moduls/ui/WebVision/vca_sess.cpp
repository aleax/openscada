
//OpenSCADA system module UI.WebVision file: vca_sess.cpp
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.	           *
 ***************************************************************************/

#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>

#include <tsys.h>
#include <tmess.h>

#include "../VCAEngine/types.h"

#include "web_vision.h"
#include "vca_sess.h"

extern char *WebVisionVCA_js;

using namespace WebVision;
using namespace VCA;

//*************************************************
//* VCASess					  *
//*************************************************
VCASess::VCASess( const string &iid, bool isCreate ) : mId(iid), mIsCreate(isCreate)
{
    lst_ses_req	= time(NULL);
    id_objs	= grpAdd("obj_");
}

void VCASess::postDisable( int flag )
{
    TCntrNode::postDisable(flag);

    //> Disconnect/delete session
    if(mIsCreate)
    {
	XMLNode req("disconnect");
	req.setAttr("path","/%2fserv%2fsess")->setAttr("sess",id());
	mod->cntrIfCmd(req,"root");
    }
}

void VCASess::getReq( SSess &ses )
{
    string oAddr;
    //> Access time to session is updating
    lst_ses_req = time(NULL);

    //int64_t curTm = TSYS::curTime();

    map<string,string>::iterator prmEl = ses.prm.find("com"), prmEl1;
    string first_lev = TSYS::pathLev(ses.url,1);
    string wp_com = (prmEl!=ses.prm.end()) ? prmEl->second : "";
    if(wp_com.empty())
    {
	string prjNm, extJS;
	//>> Get project's name
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fobj%2fst%2fprj");
	if(!mod->cntrIfCmd(req,ses.user))
	{
	    req.setAttr("path","/prj_"+req.text()+"/%2fobj%2fcfg%2fname");
	    if(!mod->cntrIfCmd(req,ses.user))	prjNm = req.text();
	}

	//>> External JS file loading try
	int hd = open("WebVisionVCA.js", O_RDONLY);
	if(hd >= 0)
	{
	    char buf[STR_BUF_LEN];
	    for(int len = 0; (len=read(hd,buf,sizeof(buf))) > 0; ) extJS.append(buf, len);
	    close(hd);
	}
	ses.page = mod->pgHead("",prjNm)+"<SCRIPT>\n"+mod->trMessReplace(extJS.size()?extJS.c_str():WebVisionVCA_js)+"\n</SCRIPT>\n"+mod->pgTail();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/html")+ses.page;

	//>> Cache clear
	ResAlloc res(nodeRes(),true);
	mCacheRes.clear();
    }
    //> Session/projects icon
    else if(wp_com == "ico")
    {
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fico");
	mod->cntrIfCmd(req,ses.user);
	ses.page = TSYS::strDecode(req.text(),TSYS::base64);
	ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    }
    //> Get open pages list
    else if(wp_com == "pgOpenList" && first_lev.empty())
    {
	prmEl = ses.prm.find("tm");

	XMLNode req("CntrReqs");
	req.setAttr("path",ses.url);
	req.childAdd("openlist")->setAttr("path","/%2fserv%2fpg")->setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	req.childAdd("get")->setAttr("path","/%2fobj%2fcfg%2fper");
	mod->cntrIfCmd(req,ses.user);
	req.childGet(0)->setAttr("per",req.childGet(1)->text());

	ses.page = req.childGet(0)->save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
    }
    //> Attribute get
    else if(wp_com == "attr")
    {
	prmEl = ses.prm.find("attr");
	string attr = (prmEl!=ses.prm.end()) ? prmEl->second : "";

	XMLNode req("get"); req.setAttr("path",ses.url+"/%2fattr%2f"+attr);
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
    }
    //> Widget's (page) full attributes branch request
    else if(wp_com == "attrsBr")
    {
	prmEl = ses.prm.find("tm");
	prmEl1 = ses.prm.find("FullTree");
	XMLNode req("get");
	req.setAttr("path",ses.url+"/%2fserv%2fattrBr")->setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0")->
	    setAttr("FullTree",(prmEl1!=ses.prm.end())?prmEl1->second:"0");
	mod->cntrIfCmd(req, ses.user);

	//>> Backend objects' attributes set
	vector<int> pos;	unsigned cpos = 0;
	vector<string> addr;	string caddr = ses.url;
	XMLNode *cn = &req;
	while(true)
	{
	    if(cpos < cn->childSize())
	    {
		if(cn->childGet(cpos)->name() == "w")
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
	    //> Check for objects represent some widgets type creation if attribute "root" present, typical for init requests
	    XMLNode *rootId = cn->getElementBy("id", "root");
	    oAddr = TSYS::path2sepstr(caddr);
	    if(rootId) objCheck(rootId->text(), oAddr);
	    if(objPresent(oAddr)) objAt(oAddr).at().setAttrs(*cn, ses.user);
	    if(!cn->parent())	break;
	    cn = cn->parent();
	    cpos = pos.back();	pos.pop_back();
	    caddr = addr.back(); addr.pop_back();
	}

	//>> Send request to browser
	ses.page = req.save();
	ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml","","UTF-8")+ses.page;
    }
    //> Resources request (images and other files)
    else if(wp_com == "res")
    {
	prmEl = ses.prm.find("val");
	if( prmEl != ses.prm.end() )
	{
	    string mime;
	    ses.page = resGet(prmEl->second,ses.url,ses.user,&mime);
	    mod->imgConvert(ses);
	    ses.page = mod->httpHead("200 OK",ses.page.size(),mime)+ses.page;
	} else ses.page = mod->httpHead("404 Not Found");
    }
    //> Request to primitive object. Used for data caching
    else if(wp_com == "obj" && objPresent(oAddr=TSYS::path2sepstr(ses.url))) objAt(oAddr).at().getReq(ses);
    else
    {
	mess_warning(nodePath().c_str(),_("Unknown command: %s."),wp_com.c_str());
	ses.page = mod->pgHead()+"<center>Call page/widget '"+ses.url+"' command: '"+wp_com+"'</center>\n<br/>"+mod->pgTail();
	ses.page = mod->httpHead("200 OK",ses.page.size())+ses.page;
    }

    //if(1e-3*(TSYS::curTime()-curTm) > 20)
    //	printf("TEST 00: %gms: '%s': '%s'\n",1e-3*(TSYS::curTime()-curTm),wp_com.c_str(),ses.url.c_str());
}

void VCASess::postReq( SSess &ses )
{
    string oAddr;
    //> Commands process
    map<string,string>::iterator cntEl = ses.prm.find("com");
    string wp_com = (cntEl!=ses.prm.end()) ? cntEl->second : "";
    //> Attributes set
    if(wp_com == "attrs")
    {
	XMLNode req("set");
	req.load(ses.content);
	req.setAttr("path",ses.url+"/%2fserv%2fattr");
	mod->cntrIfCmd(req,ses.user);
    }
    //> Open page command
    else if(wp_com == "pgClose" || wp_com == "pgOpen")
    {
	XMLNode req((wp_com=="pgOpen")?"open":"close");
	req.setAttr("path","/"+TSYS::pathLev(ses.url,0)+"/%2fserv%2fpg")->setAttr("pg",ses.url);
	mod->cntrIfCmd(req,ses.user);
    }
    else if(wp_com == "obj" && objPresent(oAddr=TSYS::path2sepstr(ses.url))) objAt(oAddr).at().postReq(ses);
    ses.page = mod->httpHead("200 OK",ses.page.size(),"text/html")+ses.page;
}

void VCASess::objCheck( const string &rootId, const string &wPath )
{
    if(objPresent(wPath)) return;
    if(rootId == "ElFigure")		objAdd(new VCAElFigure(wPath));
    else if(rootId == "Text")		objAdd(new VCAText(wPath));
    else if(rootId == "Diagram")	objAdd(new VCADiagram(wPath));
    else if(rootId == "Document")	objAdd(new VCADocument(wPath));
}

void VCASess::objAdd( VCAObj *obj )
{
    if(!obj) return;
    if(objPresent(obj->nodeName())) delete obj;
    else chldAdd(id_objs, obj);
}

string VCASess::resGet( const string &res, const string &path, const string &user, string *mime )
{
    if(res.empty()) return "";

    string ret = cacheResGet(res, mime);
    if(ret.empty())
    {
	XMLNode req("get");
	req.setAttr("path",path+"/%2fwdg%2fres")->setAttr("id",res);
	mod->cntrIfCmd(req,user);
	ret = TSYS::strDecode(req.text(),TSYS::base64);
	if(!ret.empty())
	{
	    if(mime) *mime = req.attr("mime");
	    cacheResSet(res, ret, req.attr("mime"));
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
//* VCAObj					  *
//*************************************************
VCAObj::VCAObj( const string &iid ) : mId(iid)
{

}

VCASess &VCAObj::owner( )
{
    return *(VCASess*)nodePrev( );
}


//*************************************************
//* ElFigure					  *
//*************************************************
VCAElFigure::VCAElFigure( const string &iid ) : VCAObj(iid), im(NULL)
{

}

VCAElFigure::~VCAElFigure( )
{
    if(im) { gdImageDestroy(im); im = NULL; }
}

#define SAME_SIGNS(a, b) ((a) * (b) >= 0)

// Line intersection algorithm, copied from Graphics Gems II
bool VCAElFigure::lineIntersect( double x1, double y1, double x2, double y2,
				 double x3, double y3, double x4, double y4 )
{
    double a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    double r1, r2, r3, r4;	 /* 'Sign' values */

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
    Point add((geomX+0.5)-floor(geomX+0.5)-0.5,(geomY+0.5)-floor(geomY+0.5)-0.5);

    return Point(rpnt.x+add.x, rpnt.y+add.y);
}

//- Unscaling and unrotating the point -
Point VCAElFigure::unscaleUnrotate( const Point point, double xScale, double yScale, bool flag_scale, bool flag_rotate )
{
    Point add((geomX+0.5)-floor(geomX+0.5)-0.5,(geomY+0.5)-floor(geomY+0.5)-0.5);
    Point rpnt = Point( point.x - add.x, point.y - add.y );
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
/*bool VCAElFigure::isPaintable( ShapeItem item, double xScale, double yScale )
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
	    else return true;
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
	    else return true;
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
	    else return true;
    }
    return false;
}*/

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
	    int kol = (int)TSYS::realRound( wdt/len, POS_PREC_DIG, true) ;
	    int kol_1 = (int)TSYS::realRound( wdt_1/len, POS_PREC_DIG, true );
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
	    int kol = (int)TSYS::realRound( wdt/len, POS_PREC_DIG, true);
	    int kol_1 = (int)TSYS::realRound( wdt_1/len, POS_PREC_DIG, true);
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

	    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, - (el_width/2+el_border_width)), el_ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
	    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, ( el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
	    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, (el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, (el_width/2+el_border_width)), el_ang ).y, POS_PREC_DIG, true ), clr_el );
	    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, (el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.x + rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y - rotate( Point( -el_border_width, -(el_width/2+el_border_width) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
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
	    p_center = Point( (int)TSYS::realRound( el_p1.x + rotate( bezier( t_start + (t_end - t_start)/2, p1, p2, p3, p4 ), el_ang ).x, POS_PREC_DIG, true ),
			      (int)TSYS::realRound( el_p1.y  - rotate( bezier( t_start + (t_end - t_start)/2, p1, p2, p3, p4 ), el_ang ).y, POS_PREC_DIG, true ) );
	    t = t_start;
	    do
	    {
		gdImageLine( im, (int)TSYS::realRound(el_p1.x + rotate( bezier( t,
				 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound(el_p1.y - rotate( bezier( t,
				 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p1.x + rotate( bezier( t+delta,
				 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p1.y - rotate( bezier( t+delta,
				 Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
				 Point( p3.x,p3.y+(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
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
				 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p1.y - rotate( bezier( t,
				 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p1.x + rotate( bezier( t+delta,
				 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound(el_p1.y - rotate( bezier( t+delta,
				 Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
				 Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
				 Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
				 Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
		t += delta;
	    }
	    while( t < t_end );
	    pb1 = Point(
		    (int)TSYS::realRound( el_p1.x + rotate( bezier( t,
		    Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
		    Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
		    Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
		    Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
		    (int)TSYS::realRound(el_p1.y - rotate( bezier( t,
		    Point( p1.x, p1.y-(el_width/2+el_border_width/2) ),
		    Point( p2.x, p2.y-(el_width/2+el_border_width/2) ),
		    Point( p3.x, p3.y-(el_width/2+el_border_width/2) ),
		    Point( p4.x, p4.y-(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true )
		  );
	    pb2 = Point(
		    (int)TSYS::realRound( el_p1.x + rotate( bezier( t,
		    Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
		    Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
		    Point( p3.x, p3.y+(el_width/2+el_border_width/2) ),
		    Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).x, POS_PREC_DIG, true ),
		    (int)TSYS::realRound( el_p1.y - rotate( bezier( t,
		    Point( p1.x, p1.y+(el_width/2+el_border_width/2) ),
		    Point( p2.x, p2.y+(el_width/2+el_border_width/2) ),
		    Point( p3.x,p3.y+(el_width/2+el_border_width/2) ),
		    Point( p4.x, p4.y+(el_width/2+el_border_width/2) ) ), el_ang ).y, POS_PREC_DIG, true )
		  );
	    gdImageLine( im, (int)pb1.x, (int)pb1.y, (int)pb2.x, (int)pb2.y, clr_el );
	    if( t_start > 0.0 || t_end < 1.0 )
		p1 = Point( bezier( t_start, p1, p2, p3, p4 ).x,
			    bezier( t_start, p1, p2, p3, p4 ).y );
	    gdImageLine( im, (int)TSYS::realRound(el_p1.x +
			     rotate( Point( p1.x, p1.y+el_width/2+el_border_width/2 ), el_ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y -
			     rotate( Point( p1.x, p1.y+el_width/2+el_border_width/2 ), el_ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.x +
			     rotate( Point( p1.x, p1.y - (el_width/2+el_border_width/2) ), el_ang).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p1.y -
			     rotate( Point( p1.x, p1.y - (el_width/2+el_border_width/2) ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
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
		gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el );
		t += 0.00277777777778;
	    }
	    while( t < t_end );
	    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t_end, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.y - rotate( arc( t_end, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.x + rotate( arc( t_end, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.y - rotate( arc( t_end, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ), clr_el );
	    t = t_start;
	    do
	    {
		gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
				 (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ),clr_el );
		t += 0.00277777777778;
	    }
	    while( t < t_end );
	    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t_start, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.y - rotate( arc( t_start, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.x + rotate( arc( t_start, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
			     (int)TSYS::realRound( el_p3.y - rotate( arc( t_start, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ), clr_el );
	    p_center = Point( TSYS::realRound( el_p3.x +rotate( arc( (t_end + t_start)/2, arc_a - el_width/2-1, arc_b - el_width/2-1 ), ang ).x, POS_PREC_DIG, true ),
			      TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a - el_width/2-1, arc_b - el_width/2-1 ), ang ).y, POS_PREC_DIG, true ) );
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
	    double el_ang;
	    if( el_p1.y <= el_p2.y ) el_ang = 360 - angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
	    else el_ang = angle( el_p1, el_p2, el_p1, Point( el_p1.x+10, el_p1.y ) );
	    un_p1 = unrotate( el_p1, el_ang, el_p1.x, el_p1.y );
	    un_p2 = unrotate( el_p2, el_ang, el_p1.x, el_p1.y );
	    gdImageSetThickness( im, (int)el_width );
	    double wdt = 0, wdt_1 = 0;
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
		    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( un_p1, el_ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.y - rotate( un_p1, el_ang ).y, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.x + rotate( Point( un_p1.x + wdt, un_p1.y ), el_ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.y - rotate( Point ( un_p1.x + wdt, un_p1.y ), el_ang ).y, POS_PREC_DIG, true ), clr_el );
		    un_p1.x += wdt + wdt_1;
		}
		else
		{
		    gdImageLine( im, (int)TSYS::realRound( el_p1.x + rotate( un_p1, el_ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.y - rotate( un_p1, el_ang ).y, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.x + rotate( un_p2, el_ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p1.y - rotate( un_p2, el_ang ).y, POS_PREC_DIG, true ), clr_el );
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
	    double wdt = 0, wdt_1 = 0;
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
	    int kol = (int)TSYS::realRound( wdt/len, POS_PREC_DIG, true);
	    int kol_1 = (int)TSYS::realRound( wdt_1/len, POS_PREC_DIG, true);
	    if( kol_1 < 1 ) kol_1 = 1;
	    t = t_start;
	    int calc=0;
	    int calc_1=0;
	    do
	    {
		if( calc_1 == kol_1 ) { calc = 0; calc_1=0; }
		if( calc < kol )
		{
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el );
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
	    double wdt = 0, wdt_1 = 0, t;
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
	    int kol = (int)TSYS::realRound( wdt/len, POS_PREC_DIG, true);
	    int kol_1 = (int)TSYS::realRound( wdt_1/len, POS_PREC_DIG, true);
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
		    gdImageLine( im, (int)TSYS::realRound( bezier( t, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( bezier( t, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( bezier( t+delta, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
				     (int)TSYS::realRound( bezier( t+delta, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ), clr_el );
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
    double arc_a, arc_b, t_start, t_end, ang;
    double el_width, el_border_width;
    //-- Arc --
    if( item.type == 2 )
    {
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el );
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
		double wdt = 0, wdt_1 = 0;
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
		    t += 0.00277777777778;
		}
		while( t < t_end );

		el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
		el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.y, POS_PREC_DIG, true ), clr_el_line );

		gdImageLine( im, (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.y, POS_PREC_DIG, true ), clr_el_line );


		arc_a = length( el_p5, el_p3 );
		arc_b = length( el_p3, el_p4 );

		Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }

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
		Point p1, p2, un_p1, un_p2;
		double wdt = 0, wdt_1 = 0;
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
		    t += 0.00277777777778;
		}
		while ( t < t_end );

		el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
		el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.y, POS_PREC_DIG, true ), clr_el_line );
		gdImageLine( im, (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.y, POS_PREC_DIG, true ), clr_el_line );

		arc_a = length( el_p5, el_p3 );
		arc_b = length( el_p3, el_p4 );

		Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound( el_p3.x + rotate( arc( t, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.x + rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p3.y - rotate( arc( t+0.00277777777778, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ), clr_el_line );
		    t += 0.00277777777778;
		}
		while( t < t_end );

		el_pb3 = Point( el_p3.x + rotate(  arc( t_start, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_start, arc_a, arc_b ), ang  ).y );
		el_pb4 = Point( el_p3.x + rotate(  arc( t_end, arc_a, arc_b ), ang  ).x,
				el_p3.y - rotate(  arc( t_end, arc_a, arc_b ), ang  ).y );

		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb3.y, POS_PREC_DIG, true ), clr_el_line );
		gdImageLine( im, (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb4.y, POS_PREC_DIG, true ), clr_el_line );


		arc_a = length( el_p5, el_p3 );
		arc_b = length( el_p3, el_p4 );

		Point p_center = Point( TSYS::realRound( el_p3.x + rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
					TSYS::realRound( el_p3.y - rotate( arc( (t_end + t_start)/2, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }
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
    }
    //-- bezier curve --
    if( item.type == 3)
    {
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
		    scaleRotate((pnts)[item.n2],xScale,yScale,true, true)).x, POS_PREC_DIG, true ),
		    (int)TSYS::realRound(bezier(t,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).y, POS_PREC_DIG, true ),
		    (int)TSYS::realRound(bezier(t+delta,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).x, POS_PREC_DIG, true ),
		    (int)TSYS::realRound(bezier(t+delta,scaleRotate((pnts)[item.n1],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n3],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n4],xScale,yScale,true, true ),
		    scaleRotate((pnts)[item.n2],xScale,yScale,true, true )).y, POS_PREC_DIG, true ),clr_el );
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
	    double el_ang;
	    Point un_p1, un_p2;
	    Point el_p1 = scaleRotate( (pnts)[item.n1], xScale, yScale, true, true );
	    Point el_p2 = scaleRotate( (pnts)[item.n2], xScale, yScale, true, true );
	    Point el_p3 = scaleRotate( (pnts)[item.n3], xScale, yScale, true, true );
	    Point el_p4 = scaleRotate( (pnts)[item.n4], xScale, yScale, true, true );
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
		double wdt = 0, wdt_1 = 0;
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
		    gdImageLine( im, (int)TSYS::realRound( bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
		    t += delta;
		}
		while( t < 1 );

		el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		gdImageAlphaBlending(im, 1);
		Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }

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
		double wdt = 0, wdt_1 = 0;
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
		    gdImageLine( im, (int)TSYS::realRound( bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
		    t += delta;
		}
		while( t < 1 );

		el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		gdImageAlphaBlending(im, 1);
		Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }

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
		    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
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
		    gdImageLine( im, (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound(bezier(t+delta,el_pb1, el_pb3, el_pb4, el_pb2).y, POS_PREC_DIG, true ), clr_el_line );
		    t += delta;
		}
		while( t < 1 );

		el_pb1 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(el_width/2+el_border_width/2)), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p1.x - el_border_width/2, p1.y+(-el_width/2-el_border_width/2)), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(el_width/2+el_border_width/2) ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2)), el_ang ).x,
				el_p1.y - rotate( Point(p2.x + el_border_width/2, p2.y+(-el_width/2-el_border_width/2) ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				(int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		gdImageAlphaBlending(im, 1);
		Point p_center = Point( (int)TSYS::realRound(el_p1.x + rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).x, POS_PREC_DIG, true ),
					(int)TSYS::realRound( el_p1.y - rotate( bezier( 0.5, p1, p3, p4, p2 ), el_ang ).y, POS_PREC_DIG, true ) );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }
	    }
	}
    }
    //-- Line --
    if( item.type == 1 )
    {
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
		gdImageLine( im, (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xScale, yScale, true, true ).y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).x, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( scaleRotate( (pnts)[item.n2], xScale, yScale, true, true ).y, POS_PREC_DIG, true ),  clr_el );
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
		double wdt = 0;
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
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );
		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }

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
		Point p1, p2, un_p1, un_p2,el_pb1,el_pb2;
		el_width = el_width - 1;
		//----- Drawing the lines with width = 1 instead their real width and filling the path with the color of the "line" -----
		gdImageAlphaBlending(im, 0);
		el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width ), el_ang ).y );
		gdImageSetThickness( im, 1 );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

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
		double wdt = 0;
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }

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
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, el_width/2+el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( length( el_p2, el_p1 ) + el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );

		el_pb1 = Point( el_p1.x + rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, -el_width/2-el_border_width  ), el_ang ).y );
		el_pb2 = Point( el_p1.x + rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).x,
				el_p1.y - rotate( Point( -el_border_width, el_width/2+el_border_width ), el_ang ).y );
		gdImageLine( im, (int)TSYS::realRound( el_pb1.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb1.y, POS_PREC_DIG, true ),
				    (int)TSYS::realRound( el_pb2.x, POS_PREC_DIG, true ),(int)TSYS::realRound( el_pb2.y, POS_PREC_DIG, true ),clr_el_line );
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
		if(im2) { gdImageDestroy(im2); im2 = NULL; }
	    }
	}
    }
}

int VCAElFigure::drawElF( SSess &ses, double xSc, double ySc, Point clickPnt )
{
    vector<unsigned> shape_temp;
    vector<unsigned> shape_temp_all;
    vector<int> line_color_shape;
    vector<int> border_color_shape;
    vector<int> width_shape;
    vector<int> border_width_shape;

    double scale;
    double el_width;
    double border_width;
    bool flag_min;
    bool flag_push_back;
    scaleHeight = (int)TSYS::realRound(height*ySc, POS_PREC_DIG, true);
    scaleWidth = (int)TSYS::realRound(width*xSc, POS_PREC_DIG, true);
    if( xSc < ySc ) scale = xSc;
    else scale = ySc;
    if( scale != 1.0 )
    {
	for(unsigned i = 0; i < shapeItems.size(); i++)
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
    for(unsigned i = 0; i < inundationItems.size(); i++)
    {
	//- Detecting which figures correspond the points of each fill -
	int num_pnt = 0;
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
	    for(unsigned k = 0; k < shapeItems.size(); k++)
		if( ((inundationItems[i].number_point[1] == shapeItems[k].n1) && (inundationItems[i].number_point[0] == shapeItems[k].n2)) ||
		    ((inundationItems[i].number_point[1] == shapeItems[k].n2) && (inundationItems[i].number_point[0] == shapeItems[k].n1)))
		{
		    shape_temp.push_back(k);
		    width_shape.push_back(shapeItems[k].width);
		    border_width_shape.push_back(shapeItems[k].border_width);
		    line_color_shape.push_back(shapeItems[k].lineColor);
		    if(shapeItems[k].border_width != 0) border_color_shape.push_back(shapeItems[k].borderColor);
		    else border_color_shape.push_back(shapeItems[k].lineColor);
		    if(shape_temp.size() == 2) break;
		}
	}
	else
	{
	    for(unsigned j = 0; j < inundationItems[i].number_point.size()-1; j++)
		for(unsigned k = 0; k < shapeItems.size(); k++)
		    if( ((inundationItems[i].number_point[j] == shapeItems[k].n1) && (inundationItems[i].number_point[j+1] == shapeItems[k].n2)) ||
			((inundationItems[i].number_point[j] == shapeItems[k].n2) && (inundationItems[i].number_point[j+1] == shapeItems[k].n1)) )
		    {
			flag_push_back = true;
			for(unsigned p = 0; p < shape_temp.size(); p++)
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
	    for(unsigned k = 0; k < shapeItems.size(); k++)
		if( ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n1) && (inundationItems[i].number_point[0] == shapeItems[k].n2)) ||
		    ((inundationItems[i].number_point[inundationItems[i].number_point.size()-1] == shapeItems[k].n2) && (inundationItems[i].number_point[0] == shapeItems[k].n1)) )
		{
		    flag_push_back = true;
		    for(unsigned p = 0; p < shape_temp.size(); p++)
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
	//-- Sorting the figures in each fill(inundation) and drawing the figures, which are to be under the each fill --
	vector<unsigned> number_shape;
	number_shape = shape_temp;
	std::sort(number_shape.begin(), number_shape.end());
	//>>> Making the array of the figures to be drawn before the each fill
	vector<unsigned> draw_before;
	bool fl_numb;
	for(unsigned k=0; k < shapeItems.size(); k++)
	{
	    fl_numb = false;
	    for(unsigned j = 0; j < number_shape.size(); j++)
		if( k >= number_shape[j] ){ fl_numb = true; break; }
	    if( !fl_numb ){ draw_before.push_back( k ); }
	    else continue;
	}
	//>>>Drawing the figures and push_bask them into the array of the already drawn figures
	bool flag_dr;
	for(unsigned k = 0; k < draw_before.size(); k++)
	{
	    flag_dr = true;
	    for(unsigned j = 0; j < shape_temp_all.size(); j++)
		if( draw_before[k] == shape_temp_all[j] )
	    {
		flag_dr = false;
		break;
	    }
	    if( flag_dr )//-- If the figure is out of this array, then draw it(it is the figures which are lower than the current fill ) --
	    {
		shape_temp_all.push_back( draw_before[k] );
		gdImagePtr im2 = gdImageCreateTrueColor( scaleWidth, scaleHeight );
		gdImageAlphaBlending(im2, 0);
		gdImageFilledRectangle( im2, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im2,0,0,0,127) );
		gdImageAlphaBlending(im2, 1);
		paintFigure( im2, shapeItems[draw_before[k]], xSc, ySc, true, true );
		gdImageAlphaBlending(im,1);
		gdImageSaveAlpha(im, 1);
		gdImageAlphaBlending(im2,1);
		gdImageSaveAlpha(im2, 1);
		gdImageCopy(im, im2, 0, 0, 0, 0, scaleWidth, scaleHeight);
		if(im2) { gdImageDestroy(im2); im2 = NULL; }
	    }
	}
	int tmp_clr;
	tmp_clr = gdImageColorResolveAlpha( im1, (uint8_t)(inundationItems[i].P_color>>16), (uint8_t)(inundationItems[i].P_color>>8),
						    (uint8_t)inundationItems[i].P_color, 127 - (uint8_t)(inundationItems[i].P_color>>24) );

	for(unsigned j = 0; j < shape_temp.size(); j++)
	    if( shapeItems[shape_temp[j]].type == 2 )
	    {
		shapeItems[shape_temp[j]].width = 1;
		shapeItems[shape_temp[j]].border_width = 0;
		shapeItems[shape_temp[j]].lineColor = tmp_clr;
		paintFigure( im1, shapeItems[shape_temp[j]], xSc, ySc, false, false );
	    }
	for(unsigned j = 0; j < shape_temp.size(); j++)
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
	    for(unsigned j = 1; j < inundationItems[i].number_point.size(); j++)
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
	    for(unsigned j = 0; j < inundationItems[i].number_point.size(); j++)
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
	    for(unsigned j = 0; j < shape_temp.size(); j++)
	    {
		if( shapeItems[shape_temp[j]].n1 == num_pnt )
		    for(unsigned k = 0; k < inundationItems[i].number_point.size(); k++)
			if( shapeItems[shape_temp[j]].n2 == inundationItems[i].number_point[k] )
			{
			    fig.push_back(shape_temp[j]);
			    point_num.push_back( shapeItems[shape_temp[j]].n2 );
			    break;
			}
		if( shapeItems[shape_temp[j]].n2 == num_pnt )
		    for(unsigned k = 0; k < inundationItems[i].number_point.size(); k++)
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
			double a = 0, b = 0, a1 = 0, b1 = 0;
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
			    P1 = Point( TSYS::realRound( P1.x, POS_PREC_DIG, true ), TSYS::realRound( P1.y, POS_PREC_DIG, true ) );
			    P2 = Point( TSYS::realRound( P2.x, POS_PREC_DIG, true ), TSYS::realRound( P2.y, POS_PREC_DIG, true ) );
			    P3 = Point( TSYS::realRound( P3.x, POS_PREC_DIG, true ), TSYS::realRound( P3.y, POS_PREC_DIG, true ) );
			    P4 = Point( TSYS::realRound( P4.x, POS_PREC_DIG, true ), TSYS::realRound( P4.y, POS_PREC_DIG, true ) );
			    P5 = Point( TSYS::realRound( P5.x, POS_PREC_DIG, true ), TSYS::realRound( P5.y, POS_PREC_DIG, true ) );
			    P6 = Point( TSYS::realRound( P6.x, POS_PREC_DIG, true ), TSYS::realRound( P6.y, POS_PREC_DIG, true ) );
			    P7 = Point( TSYS::realRound( P7.x, POS_PREC_DIG, true ), TSYS::realRound( P7.y, POS_PREC_DIG, true ) );
			    P8 = Point( TSYS::realRound( P8.x, POS_PREC_DIG, true ), TSYS::realRound( P8.y, POS_PREC_DIG, true ) );
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
			double arc_a, arc_b, arc_a_small, arc_b_small, t_start, t_end, delta_t = 0;
			Point P1, P2, P3, P4;
			scale = 0.0;
			if( xSc < 1 && xSc <= ySc ) scale = (1-xSc)/10;
			else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/10;
			//-- Arc --
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
			    P1 = Point( TSYS::realRound( P1.x, POS_PREC_DIG, true ), TSYS::realRound( P1.y, POS_PREC_DIG, true ) );
			    P2 = Point( TSYS::realRound( P2.x, POS_PREC_DIG, true ), TSYS::realRound( P2.y, POS_PREC_DIG, true ) );
			    P3 = Point( TSYS::realRound( P3.x, POS_PREC_DIG, true ), TSYS::realRound( P3.y, POS_PREC_DIG, true ) );
			    P4 = Point( TSYS::realRound( P4.x, POS_PREC_DIG, true ), TSYS::realRound( P4.y, POS_PREC_DIG, true ) );
			    //--- Line ---
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
			    arc_pnt_pred = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
			    do
			    {
				delta_t += inc_delta;
				arc_pnt = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
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
			    delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t + delta, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t + delta, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
			}
			else
			{
			    t_start = shapeItems[fig[0]].ctrlPos4.x;
			    t_end = shapeItems[fig[0]].ctrlPos4.y;
			    if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/2 + scale;
			    else delta_real = t_end - (t_end-t_start)/2 + scale;
			    new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
			    delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ) );
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
			double delta_real, delta_t = 0, delta_temp_1 = 0, delta_temp_2 = 0;
			double scale;
			double ang, ang1;
			int num_bezier = 0;
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
			    P1 = Point( TSYS::realRound( P1.x, POS_PREC_DIG, true ), TSYS::realRound( P1.y, POS_PREC_DIG, true ) );
			    P2 = Point( TSYS::realRound( P2.x, POS_PREC_DIG, true ), TSYS::realRound( P2.y, POS_PREC_DIG, true ) );
			    P3 = Point( TSYS::realRound( P3.x, POS_PREC_DIG, true ), TSYS::realRound( P3.y, POS_PREC_DIG, true ) );
			    P4 = Point( TSYS::realRound( P4.x, POS_PREC_DIG, true ), TSYS::realRound( P4.y, POS_PREC_DIG, true ) );
			    //--- Line ---
			    double inc_delta;
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
						Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y+W1 ),
						Point( el_p2.x, el_p2.y+W1 ),
						Point( el_p3.x, el_p3.y+W1 ),
						Point( el_p4.x, el_p4.y+W1) ), ang ).y, POS_PREC_DIG, true ) );

			    bezier_pnt_pred_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y-W1 ),
						Point( el_p2.x, el_p2.y-W1 ),
						Point( el_p3.x, el_p3.y-W1 ),
						Point( el_p4.x, el_p4.y-W1 ) ), ang).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y-W1 ),
						Point( el_p2.x, el_p2.y-W1 ),
						Point( el_p3.x, el_p3.y-W1 ),
						Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, POS_PREC_DIG, true ) );
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
						Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y+W1 ),
						Point( el_p2.x, el_p2.y+W1 ),
						Point( el_p3.x, el_p3.y+W1 ),
						Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, POS_PREC_DIG, true ) );
				if( W1 != 0 )
				    bezier_pnt_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y-W1 ),
						Point( el_p2.x, el_p2.y-W1 ),
						Point( el_p3.x, el_p3.y-W1 ),
						Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate((pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t,
						Point( el_p1.x, el_p1.y-W1 ),
						Point( el_p2.x, el_p2.y-W1 ),
						Point( el_p3.x, el_p3.y-W1 ),
						Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, POS_PREC_DIG, true ) );
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
				delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ) );
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
						    Point( el_p4.x, el_p4.y-W1 ) ), ang).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
						    Point( el_p1.x, el_p1.y-W1 ),
						    Point( el_p2.x, el_p2.y-W1 ),
						    Point( el_p3.x, el_p3.y-W1 ),
						    Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, POS_PREC_DIG, true ) );
				if( num_bezier == 1 )
				    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[0]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
						    Point( el_p1.x, el_p1.y+W1 ),
						    Point( el_p2.x, el_p2.y+W1 ),
						    Point( el_p3.x, el_p3.y+W1 ),
						    Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
						    Point( el_p1.x, el_p1.y+W1 ),
						    Point( el_p2.x, el_p2.y+W1 ),
						    Point( el_p3.x, el_p3.y+W1 ),
						    Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, POS_PREC_DIG, true ) );
			    }
			}
			else
			{
			    new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
			    if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.2 + scale;
			    else delta_real = 0.8 - scale;
			    delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ) );
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
			double ang, ang1, delta_temp_1 = 0, delta_temp_2 = 0, delta_t = 0;
			double arc_a, arc_b, arc_a_small, arc_b_small, t_start, t_end, delta_t_arc, delta_t_bez;
			int num_bezier = 0;
			Point P1, P2, P3, P4;
			scale = 0.0;
			if( xSc < 1 && xSc <= ySc ) scale = (1-xSc)/10;
			else if( ySc < 1 && ySc <= xSc ) scale = (1-ySc)/10;
			//--- Arc ---
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
			    arc_pnt_pred = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_t_arc, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_t_arc, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
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
						Point( P4.x, P4.y+W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
						Point( P1.x, P1.y+W1 ),
						Point( P2.x, P2.y+W1 ),
						Point( P3.x, P3.y+W1 ),
						Point( P4.x, P4.y+W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );

			    bezier_pnt_pred_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
						Point( P1.x, P1.y-W1 ),
						Point( P2.x, P2.y-W1 ),
						Point( P3.x, P3.y-W1 ),
						Point( P4.x, P4.y-W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
						Point( P1.x, P1.y-W1 ),
						Point( P2.x, P2.y-W1 ),
						Point( P3.x, P3.y-W1 ),
						Point( P4.x, P4.y-W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );

			    bool f_brk_1, f_brk_2;
			    f_brk_1 = false;
			    f_brk_2 = false;
			    do
			    {
				bezier_pnt_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
					    Point( P1.x, P1.y+W1 ),
					    Point( P2.x, P2.y+W1 ),
					    Point( P3.x, P3.y+W1 ),
					    Point( P4.x, P4.y+W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
					    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
					    Point( P1.x, P1.y+W1 ),
					    Point( P2.x, P2.y+W1 ),
					    Point( P3.x, P3.y+W1 ),
					    Point( P4.x, P4.y+W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				if( W1 != 0 )
				    bezier_pnt_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez,
						Point( P1.x, P1.y-W1 ),
						Point( P2.x, P2.y-W1 ),
						Point( P3.x, P3.y-W1 ),
						Point( P4.x, P4.y-W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez,
						Point( P1.x, P1.y-W1 ),
						Point( P2.x, P2.y-W1 ),
						Point( P3.x, P3.y-W1 ),
						Point( P4.x, P4.y-W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				delta_t_bez += inc_delta_bez;
				double t_arc = delta_t_arc;
				do
				{
				    arc_pnt = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( t_arc, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( el_p3.y - rotate( arc( t_arc, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
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
				delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
				//--- bezier ---
				P1 = scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true );
				P2 = scaleRotate( (pnts)[shapeItems[fig[1]].n2], xSc, ySc, true, true );
				P3 = scaleRotate( (pnts)[shapeItems[fig[1]].n3], xSc, ySc, true, true );
				P4 = scaleRotate( (pnts)[shapeItems[fig[1]].n4], xSc, ySc, true, true );
				if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
				else delta_real = 0.8 - scale;
				delta_point_2 = Point( (int)TSYS::realRound (bezier( delta_real, P1, P3, P4, P2 ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, POS_PREC_DIG, true ) );
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
						    Point( P4.x, P4.y+W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
						    Point( P1.x, P1.y+W1 ),
						    Point( P2.x, P2.y+W1 ),
						    Point( P3.x, P3.y+W1 ),
						    Point( P4.x, P4.y+W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				if( num_bezier == 2 )
				    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[shapeItems[fig[1]].n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t + delta,
						    Point( P1.x, P1.y-W1 ),
						    Point( P2.x, P2.y-W1 ),
						    Point( P3.x, P3.y-W1 ),
						    Point( P4.x, P4.y-W1 ) ), ang1 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t + delta,
						    Point( P1.x, P1.y-W1 ),
						    Point( P2.x, P2.y-W1 ),
						    Point( P3.x, P3.y-W1 ),
						    Point( P4.x, P4.y-W1 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = t_start + (t_end-t_start)/4 + scale;
				else delta_real = t_end - (t_end-t_start)/4 + scale;
				delta_point_2 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a_small, arc_b_small ), ang ).y, POS_PREC_DIG, true ) );
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
			    delta_point_1 = Point( (int)TSYS::realRound( el_p3.x + rotate( arc( delta_real, arc_a, arc_b ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( el_p3.y - rotate( arc( delta_real, arc_a, arc_b ), ang ).y, POS_PREC_DIG, true ) );
			    //--- bezier ---
			    if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
			    else delta_real = 0.8 - scale;
			    delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, POS_PREC_DIG, true ) );
			}
		    }
		    //- bezier curve and bezier curve -
		    else if( shapeItems[fig[0]].type == 3 && shapeItems[fig[1]].type == 3 )
		    {
			Point new_pnt;
			vector <Point> new_pnt_vect;
			double delta_real, delta_t_1 = 0, delta_t_2 = 0, delta_t_bez_1,  delta_t_bez_2;
			double delta_1, delta_2;
			vector <double> delta_temp_1, delta_temp_2;
			vector <int> num_bezier_1, num_bezier_2;
			double scale;
			double ang, ang1;
			int num_bez_1 = 0, num_bez_2 = 0;
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
						Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, POS_PREC_DIG, true ),
						(int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_1,
						Point( el_p1.x, el_p1.y+W1 ),
						Point( el_p2.x, el_p2.y+W1 ),
						Point( el_p3.x, el_p3.y+W1 ),
						Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, POS_PREC_DIG, true ) );
				if( W1 != 0 )
				{
				    bezier_pnt_1_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_1,
						    Point( el_p1.x, el_p1.y-W1 ),
						    Point( el_p2.x, el_p2.y-W1 ),
						    Point( el_p3.x, el_p3.y-W1 ),
						    Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_1,
						    Point( el_p1.x, el_p1.y-W1 ),
						    Point( el_p2.x, el_p2.y-W1 ),
						    Point( el_p3.x, el_p3.y-W1 ),
						    Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, POS_PREC_DIG, true ) );
				}
				delta_t_bez_1 += inc_delta_bez_1;
				double delta_t_bez_2_do = delta_t_bez_2;
				do
				{
				    bezier_pnt_2_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_2_do,
						    Point( P1.x, P1.y+W2 ),
						    Point( P2.x, P2.y+W2 ),
						    Point( P3.x, P3.y+W2 ),
						    Point( P4.x, P4.y+W2 ) ), ang1 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_2_do,
						    Point( P1.x, P1.y+W2 ),
						    Point( P2.x, P2.y+W2 ),
						    Point( P3.x, P3.y+W2 ),
						    Point( P4.x, P4.y+W2 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				    if( W2 != 0 )
				    {
					bezier_pnt_2_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_bez_2_do,
							Point( P1.x, P1.y-W2 ),
							Point( P2.x, P2.y-W2 ),
							Point( P3.x, P3.y-W2 ),
							Point( P4.x, P4.y-W2 ) ), ang1 ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_bez_2_do,
							Point( P1.x, P1.y-W2 ),
							Point( P2.x, P2.y-W2 ),
							Point( P3.x, P3.y-W2 ),
							Point( P4.x, P4.y-W2 ) ), ang1 ).y, POS_PREC_DIG, true ) );
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
				delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ) );
				if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
				else delta_real = 0.8 - scale;
				delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, POS_PREC_DIG, true ),
							(int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, POS_PREC_DIG, true ) );
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
				    for(unsigned i = 1; i < new_pnt_vect.size(); i++)
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
						    Point( el_p4.x, el_p4.y+W1 ) ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_1 + delta_1,
						    Point( el_p1.x, el_p1.y+W1 ),
						    Point( el_p2.x, el_p2.y+W1 ),
						    Point( el_p3.x, el_p3.y+W1 ),
						    Point( el_p4.x, el_p4.y+W1 ) ), ang ).y, POS_PREC_DIG, true ) );
				if( num_bez_1 == 0 )
				    delta_point_1 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_1 + delta_1,
						    Point( el_p1.x, el_p1.y-W1 ),
						    Point( el_p2.x, el_p2.y-W1 ),
						    Point( el_p3.x, el_p3.y-W1 ),
						    Point( el_p4.x, el_p4.y-W1 ) ), ang ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_1 + delta_1,
						    Point( el_p1.x, el_p1.y-W1 ),
						    Point( el_p2.x, el_p2.y-W1 ),
						    Point( el_p3.x, el_p3.y-W1 ),
						    Point( el_p4.x, el_p4.y-W1 ) ), ang ).y, POS_PREC_DIG, true ) );

				if( num_bez_2 == 1 )
				    delta_point_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_2 + delta_2,
						    Point( P1.x, P1.y+W2 ),
						    Point( P2.x, P2.y+W2 ),
						    Point( P3.x, P3.y+W2 ),
						    Point( P4.x, P4.y+W2 ) ), ang1 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_2 + delta_2,
						    Point( P1.x, P1.y+W2 ),
						    Point( P2.x, P2.y+W2 ),
						    Point( P3.x, P3.y+W2 ),
						    Point( P4.x, P4.y+W2 ) ), ang1 ).y, POS_PREC_DIG, true ) );
				if( num_bez_2 == 0 )
				    delta_point_2 = Point( (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).x + rotate( bezier( delta_t_2 + delta_2,
						    Point( P1.x, P1.y-W2 ),
						    Point( P2.x, P2.y-W2 ),
						    Point( P3.x, P3.y-W2 ),
						    Point( P4.x, P4.y-W2 ) ), ang1 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( scaleRotate( (pnts)[item_1.n1], xSc, ySc, true, true ).y - rotate( bezier( delta_t_2 + delta_2,
						    Point( P1.x, P1.y-W2 ),
						    Point( P2.x, P2.y-W2 ),
						    Point( P3.x, P3.y-W2 ),
						    Point( P4.x, P4.y-W2 ) ), ang1 ).y, POS_PREC_DIG, true ) );
			    }
			}
			else
			{
			    new_pnt = scaleRotate( (pnts)[num_pnt], xSc, ySc, true, true );
			    if( num_pnt == shapeItems[fig[0]].n1 ) delta_real = 0.2 + scale;
			    else delta_real = 0.8 - scale;
			    delta_point_1 = Point( (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( bezier( delta_real, el_p1, el_p3, el_p4, el_p2 ).y, POS_PREC_DIG, true ) );
			    if( num_pnt == shapeItems[fig[1]].n1 ) delta_real = 0.2 + scale;
			    else delta_real = 0.8 - scale;
			    delta_point_2 = Point( (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).x, POS_PREC_DIG, true ),
						    (int)TSYS::realRound( bezier( delta_real, P1, P3, P4, P2 ).y, POS_PREC_DIG, true ) );
			}
		    }
		    //-- Detecting the real "fill" point as the middle of the distance between two points on the figures --
		    delta_point_center.x = (delta_point_1.x+delta_point_2.x)/2;
		    delta_point_center.y = (delta_point_1.y+delta_point_2.y)/2;
		}
		//-- Calling fill procedure for each fill with the real "fill" point --
		if( (((int) TSYS::realRound(delta_point_center.x)) > 0) && (((int) TSYS::realRound(delta_point_center.x)) <= scaleWidth) &&
		    (((int) TSYS::realRound(delta_point_center.y)) > 0) && (((int) TSYS::realRound(delta_point_center.y)) <= scaleHeight) )
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

			for(unsigned j = 0; j < shape_temp.size(); j++)
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
				Point el_p3_rot = scaleRotate( (pnts)[item.n3], xSc, ySc, true, true );
				Point el_p4_rot = scaleRotate( (pnts)[item.n4], xSc, ySc, true, true );
				Point el_p5_rot = scaleRotate( (pnts)[item.n5], xSc, ySc, true, true );

				Point el_p3 = unscaleUnrotate( el_p3_rot, xSc, ySc, false, true );
				Point el_p4 = unscaleUnrotate( el_p4_rot, xSc, ySc, false, true );
				Point el_p5 = unscaleUnrotate( el_p5_rot, xSc, ySc, false, true );

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
			    if( gdImageGetPixel( im1, (int)TSYS::realRound(clickPnt.x,POS_PREC_DIG,true), (int)TSYS::realRound(clickPnt.y,POS_PREC_DIG,true) )
				!= gdImageColorResolveAlpha(im1,0,0,0,127) )
			    {
				if(im1) gdImageDestroy(im1);
				return i;
			    }
			}
			else
			{
			    xMin = (int)TSYS::realRound( xMin, POS_PREC_DIG, true );
			    yMin = (int)TSYS::realRound( yMin, POS_PREC_DIG, true );
			    xMax = (int)TSYS::realRound( xMax, POS_PREC_DIG, true );
			    yMax = (int)TSYS::realRound( yMax, POS_PREC_DIG, true );

			    gdImagePtr im_fill_in = NULL;
			    string imgDef_temp = owner().resGet(inundationItems[i].imgFill,path(),ses.user);
			    if( !(im_fill_in = gdImageCreateFromPngPtr(imgDef_temp.size(), (void*)imgDef_temp.data())) &&
				!(im_fill_in = gdImageCreateFromGifPtr(imgDef_temp.size(), (void*)imgDef_temp.data())) &&
				!(im_fill_in = gdImageCreateFromJpegPtr(imgDef_temp.size(), (void*)imgDef_temp.data())) )
				mess_debug(nodePath().c_str(),_("Fill image type is not supported."));
			    gdImagePtr im_fill_out = gdImageCreateTrueColor((int)TSYS::realRound( xMax - xMin ) + 1, (int)TSYS::realRound( yMax - yMin ) + 1 );
			    gdImageAlphaBlending(im_fill_out, 0);
			    int alpha;
			    double alpha_pr, alpha_rez;
			    if( im_fill_in )
			    {
				gdImageAlphaBlending(im_fill_in, 0);
				gdImageCopyResampled(im_fill_out, im_fill_in, 0, 0, 0, 0, im_fill_out->sx, im_fill_out->sy, im_fill_in->sx, im_fill_in->sy);
			    }
			    int im_x, im_y;
			    Point drw_pnt,drw_pnt1;
			    xMin_rot = (int)TSYS::realRound( xMin_rot, POS_PREC_DIG, true );
			    yMin_rot = (int)TSYS::realRound( yMin_rot, POS_PREC_DIG, true );
			    xMax_rot = (int)TSYS::realRound( xMax_rot, POS_PREC_DIG, true );
			    yMax_rot = (int)TSYS::realRound( yMax_rot, POS_PREC_DIG, true );

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
					rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin, POS_PREC_DIG, true ),
									    (int)TSYS::realRound( drw_pnt.y - yMin, POS_PREC_DIG, true ) );
					if( ((int)TSYS::realRound( drw_pnt.x - xMin, POS_PREC_DIG, true ) == (int)TSYS::realRound( xMax - xMin ) + 1) && rgb == 0 )
					    rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin - 1, POS_PREC_DIG, true ),
										(int)TSYS::realRound( drw_pnt.y - yMin, POS_PREC_DIG, true ) );
					else if( ((int)TSYS::realRound( drw_pnt.y - yMin, POS_PREC_DIG, true ) == (int)TSYS::realRound( yMax - yMin ) + 1) && rgb == 0 )
					    rgb = gdImageGetPixel( im_fill_out, (int)TSYS::realRound( drw_pnt.x - xMin, POS_PREC_DIG, true ),
										(int)TSYS::realRound( drw_pnt.y - yMin - 1, POS_PREC_DIG, true ) );

					alpha = gdImageAlpha( im_fill_out, rgb );
					alpha_pr = 1 - (double)alpha / 127;
					drw_pnt1.x = scaleRotate( drw_pnt, xSc, ySc, false, true ).x;
					drw_pnt1.y = scaleRotate( drw_pnt, xSc, ySc, false, true ).y;

					//if( fabs(alpha_pr - 0) < 0.001 ) alpha_pr = 1;
					color_r = alpha_pr*((rgb>>16)&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)( inundationItems[i].P_color>>16 ) );
					color_g = alpha_pr*((rgb>>8)&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)( inundationItems[i].P_color>>8 ) );
					color_b = alpha_pr*(rgb&0xff) + (1-alpha_pr)*alpha_col*( (uint8_t)inundationItems[i].P_color );
					alpha_rez = (1 - alpha_col) * (1 - alpha_pr);
					/*int color = gdImageColorResolve( im1, (int)TSYS::realRound( color_r, POS_PREC_DIG, true ),
									    (int)TSYS::realRound( color_g, POS_PREC_DIG, true ),
									    (int)TSYS::realRound( color_b, POS_PREC_DIG, true ) );*/
					int color = gdImageColorResolveAlpha( im1, (int)TSYS::realRound( color_r, POS_PREC_DIG, true ),
										   (int)TSYS::realRound( color_g, POS_PREC_DIG, true ),
										   (int)TSYS::realRound( color_b, POS_PREC_DIG, true ),
										   127 - (int)TSYS::realRound( 127*(1 - alpha_rez), POS_PREC_DIG, true ) );
					gdImageSetPixel( im1, (int)TSYS::realRound( drw_pnt1.x, POS_PREC_DIG, true ) , (int)TSYS::realRound( drw_pnt1.y, POS_PREC_DIG, true ), color );
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
			    if( gdImageGetPixel(im1, (int)TSYS::realRound(clickPnt.x,POS_PREC_DIG,true),
							(int)TSYS::realRound(clickPnt.y,POS_PREC_DIG,true) ) != gdImageColorResolveAlpha(im1,0,0,0,127) )
			    {
				if( im1 ) gdImageDestroy(im1);
				return i;
			    }
			}

		    }
		}
		else mess_debug(nodePath().c_str(),_("At least one of the elementary figures from each the 'fill' consists of is out of drawing area."));
	    }
	if( (int)TSYS::realRound( clickPnt.x, POS_PREC_DIG, true ) == -1 && (int)TSYS::realRound( clickPnt.y, POS_PREC_DIG, true ) == -1 )
	{
	    for(unsigned j = 0; j < shape_temp.size(); j++)
		shape_temp_all.push_back(shape_temp[j]);
	    //- Changing the color to the real one for all figures used in each fill(inundation)
	    for(unsigned j = 0; j < shape_temp.size(); j++)
	    {
		shapeItems[shape_temp[j]].width = width_shape[j];
		shapeItems[shape_temp[j]].border_width = border_width_shape[j];
		shapeItems[shape_temp[j]].lineColor = line_color_shape[j];
		shapeItems[shape_temp[j]].borderColor = border_color_shape[j];
	    }
	    //- Painting all figures -
	    std::sort(shape_temp.begin(), shape_temp.end());
	    for(unsigned j = 0; j < shape_temp.size(); j++)
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
	if( im1 ) gdImageDestroy(im1);
    }
    if( (int)TSYS::realRound( clickPnt.x, POS_PREC_DIG, true ) == -1 && (int)TSYS::realRound( clickPnt.y, POS_PREC_DIG, true ) == -1 )
    {
	for(unsigned j = 0; j < shapeItems.size(); j++)
	{
	    bool fl_paint = false;
	    for(unsigned i = 0; i < shape_temp_all.size(); i++)
		if( j == shape_temp_all[i] )
		{
		    fl_paint = true;
		    break;
		}
	    if( !fl_paint )
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
    ResAlloc res(mRes, true);
    //> Prepare picture
    map<string, string>::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    prmEl = ses.prm.find("geomX");
    geomX = (prmEl!=ses.prm.end()) ? s2r(prmEl->second) : 0;
    prmEl = ses.prm.find("geomY");
    geomY = (prmEl!=ses.prm.end()) ? s2r(prmEl->second) : 0;

    scaleHeight = (int)TSYS::realRound(height*ySc, POS_PREC_DIG, true);
    scaleWidth = (int)TSYS::realRound(width*xSc, POS_PREC_DIG, true);
    if(im) { gdImageDestroy(im); im = NULL; }
    im = gdImageCreateTrueColor(scaleWidth, scaleHeight);
    if(!im) ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    else
    {
	//> Draw image
	gdImageAlphaBlending(im, 0);
	gdImageFilledRectangle(im, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im,0,0,0,127));
	gdImageAlphaBlending(im, 1);
	drawElF(ses, xSc, ySc, Point(-1,-1));

	//> Get image and transfer it
	int img_sz;
	char *img_ptr = (char *)gdImagePngPtrEx(im, &img_sz, mod->PNGCompLev());
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
	double xSc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
	prmEl = ses.prm.find("ySc");
	double ySc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
	prmEl = ses.prm.find("x");
	int x_coord = (prmEl!=ses.prm.end()) ? s2i(prmEl->second) : -1;
	prmEl = ses.prm.find("y");
	int y_coord = (prmEl!=ses.prm.end()) ? s2i(prmEl->second) : -1;
	prmEl = ses.prm.find("key");
	string key = (prmEl!=ses.prm.end()) ? prmEl->second : "";
	if( x_coord < 0 || y_coord < 0 ) return;

	int clickFillNum = drawElF( ses, xSc, ySc, Point(x_coord,y_coord) );
	if( clickFillNum != -1 )
	{
	    XMLNode req("set");
	    req.setAttr("path",ses.url+"/%2fserv%2fattr");
	    req.childAdd("el")->setAttr("id","event")->setText("ws_Fig"+key);
	    req.childAdd("el")->setAttr("id","event")->setText("ws_Fig"+i2s(clickFillNum)+key);
	    req.childAdd("el")->setAttr("id","event")->setText("ws_FocusIn");
	    req.childAdd("el")->setAttr("id","focus")->setText("1");
	    mod->cntrIfCmd(req,ses.user);
	}
    }
}

void VCAElFigure::setAttrs( XMLNode &node, const string &user )
{
    ResAlloc res(mRes, true);
    XMLNode *req_el;
    Point StartMotionPos, EndMotionPos, CtrlMotionPos_1, CtrlMotionPos_2, CtrlMotionPos_3, CtrlMotionPos_4;
    rel_list = false;
    for(unsigned i_a = 0; i_a < node.childSize(); i_a++)
    {
	req_el = node.childGet(i_a);
	if(req_el->name() != "el") continue;

	int uiPrmPos = s2i(req_el->attr("p"));
	switch(uiPrmPos)
	{
	    case A_ACTIVE: active = (bool)s2i(req_el->text());	break;
	    case A_GEOM_W: width = s2r(req_el->text());		break;
	    case A_GEOM_H: height = s2r(req_el->text());	break;
	    case A_GEOM_MARGIN: geomMargin = s2i(req_el->text()); break;
	    case A_GEOM_X_SC: rel_list = true;	break;
	    case A_GEOM_Y_SC: rel_list = true;	break;
	    case A_ElFigLineW: lineWdth = (int)TSYS::realRound(s2r(req_el->text())); rel_list = true;	break;
	    case A_ElFigLineClr:
		lineClr = mod->colorParse(req_el->text());
		if(lineClr == -1) lineClr = (127<<24)+(0<<16)+(0<<8)+0;
		if(lineClr == 0) lineClr = (0<<24)+(250<<16)+(0<<8)+0;
		rel_list = true;
		break;
	    case A_ElFigLineStl: lineStyle = s2i(req_el->text()); rel_list = true;	break;
	    case A_ElFigBordW: bordWdth = (int)TSYS::realRound(s2r(req_el->text())); rel_list = true;	break;
	    case A_ElFigBordClr:
		bordClr = mod->colorParse(req_el->text());
		if(bordClr == -1) bordClr = 0x7F000000;
		if(bordClr == 0)  bordClr = 0x00FA0000;
		rel_list = true;
		break;
	    case A_ElFigFillClr:
		fillClr = mod->colorParse(req_el->text());
		if(fillClr == -1) fillClr = 0x7F000000;
		if(fillClr == 0)  fillClr = 0x00FA0000;
		rel_list = true;
		break;
	    case A_ElFigFillImg: imgDef = req_el->text(); rel_list = true;	break;
	    case A_ElFigOrient: orient = s2r(req_el->text()); rel_list = true;	break;
	    case A_ElFigElLst: elLst = req_el->text(); rel_list = true;		break;
	    default:
		if(uiPrmPos >= A_ElFigIts)
		{
		    int pnt  = (uiPrmPos-A_ElFigIts)/A_ElFigItsSz;
		    int patr = (uiPrmPos-A_ElFigIts)%A_ElFigItsSz;
		    Point pnt_ = pnts[pnt];
		    switch(patr)
		    {
			case A_ElFigItPntX: pnt_.x = s2r(req_el->text()); pnts[pnt] = pnt_; rel_list = true;	break;
			case A_ElFigItPntY: pnt_.y = s2r(req_el->text()); pnts[pnt] = pnt_; rel_list = true;	break;
			case A_ElFigItW: widths[pnt] = (int)TSYS::realRound(s2r(req_el->text())); rel_list = true;	break;
			case A_ElFigItClr:
			    colors[pnt] = mod->colorParse(req_el->text());
			    if(colors[pnt] == -1) colors[pnt] = 0x7F000000;
			    if(colors[pnt] == 0)  colors[pnt] = 0x00FA0000;
			    rel_list = true;
			    break;
			case A_ElFigItImg: images[pnt] = req_el->text(); rel_list = true;		break;
			case A_ElFigItStl: styles[pnt] = s2i(req_el->text()); rel_list = true;	break;
		    }
		}
	}
    }
    if(rel_list)
    {
	for(PntMap::iterator pi = pnts.begin(); pi != pnts.end(); )
	    if(pi->first <= ShapeItem::StatIts) pnts.erase(pi++); else ++pi;
	string sel, el_s;
	vector<int> p;
	int map_index = ShapeItem::StatIts, lnwidth, bord_width, color, bord_color;
	string ln_st;
	int style;
	double t_start, t_end, a, b, ang;
	Point ip[5];
	shapeItems.clear();
	inundationItems.clear();
	for(int off = 0, el_off = 0; (sel=TSYS::strLine(elLst,0,&off)).size(); el_off = 0)
	{
	    string el = TSYS::strSepParse(sel, 0, ':', &el_off);
	    ShapeItem::Type elTp;
	    int nPnts = 0, servPnts;
	    if(el == "line")            { elTp = ShapeItem::Line; nPnts = 2; servPnts = 2; }
	    else if(el == "arc")        { elTp = ShapeItem::Arc; nPnts = 5; servPnts = 2; }
	    else if(el == "bezier")     { elTp = ShapeItem::Bezier; nPnts = 4; servPnts = 2; }
	    else if(el == "fill")       { elTp = ShapeItem::Fill; nPnts = -1; servPnts = -1; }
	    else continue;

	    //>> Reading anf setting attributes for the current line
	    //>>> Points
	    float x_s, y_s;
	    int w_s;
	    bool pnts_ok = true;
	    p.clear();
	    for(int i_p = 0, off_last = 0; pnts_ok && (nPnts < 0 || i_p < nPnts); i_p++)
	    {
		el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		if(sscanf(el_s.c_str(),"(%f|%f)",&x_s,&y_s) == 2)
		{
		    bool fl = false;
		    //>>> Detecting if there is a point with same coordinates in the map
		    for(PntMap::reverse_iterator pi = pnts.rbegin(); (servPnts < 0 || i_p < servPnts) && !fl && pi != pnts.rend(); ++pi)
			if(pi->first <= ShapeItem::StatIts && fabs(TSYS::realRound(x_s,POS_PREC_DIG) - TSYS::realRound(pi->second.x,POS_PREC_DIG)) < 0.01 &&
			    fabs(TSYS::realRound(y_s,POS_PREC_DIG) - TSYS::realRound(pi->second.y,POS_PREC_DIG)) < 0.01)
			{ p.push_back(pi->first); fl = true; }
		    if(!fl) { p.push_back(map_index--); pnts[p[i_p]] = Point(x_s, y_s); }
		}
		else if(sscanf(el_s.c_str(),"%d",&w_s) == 1) p.push_back(w_s);
		else { pnts_ok = false; el_off = off_last; }
                off_last = el_off;
            }
            if(!pnts_ok && nPnts > 0) continue;
	    //>>> Other properties
            switch(elTp)
            {
                case ShapeItem::Line: case ShapeItem::Arc: case ShapeItem::Bezier:
            	{
            	    //>>> Line width
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"w%d",&w_s) == 1) lnwidth  = widths[w_s];
		    else if(sscanf(el_s.c_str(), "%d", &w_s) == 1 ) lnwidth = w_s;
		    else lnwidth = lineWdth;

		    //>>> Line color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) color = colors[w_s];
		    else
		    {
			color = mod->colorParse(el_s);
			if(color == -1) color = lineClr;
			else if(color == 0) color = 0x00FA0000;
		    }

		    //>>> Border width
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"w%d",&w_s) == 1) bord_width = widths[w_s];
		    else if(sscanf(el_s.c_str(),"%d",&w_s) == 1) bord_width = w_s;
		    else bord_width = bordWdth;

		    //>>> Border color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) bord_color = colors[w_s];
		    else
		    {
			bord_color = mod->colorParse(el_s);
			if(bord_color == -1) bord_color = bordClr;
			else if(bord_color == 0) bord_color = 0x00FA0000;
		    }

		    //>>> Line style
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"s%d",&w_s) == 1) style = styles[w_s];
		    else if(sscanf(el_s.c_str(),"%d",&w_s) == 1 && (w_s == EF_SOLID || w_s == EF_DASH || w_s == EF_DOT)) style = w_s;
		    else style = lineStyle;

		    //>> Reading coordinates for the points of the line
		    for(unsigned i_p = 0; i_p < p.size(); i_p++) ip[i_p] = pnts[p[i_p]];

		    if(elTp == ShapeItem::Arc)
                    {
			StartMotionPos = ip[0];
			EndMotionPos = ip[1];
			CtrlMotionPos_1 = ip[2];
			CtrlMotionPos_2 = ip[3];
			CtrlMotionPos_3 = ip[4];
			ang = angle(CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y));
			if(CtrlMotionPos_3.y > CtrlMotionPos_1.y) ang = 360 - ang;
			a = length(CtrlMotionPos_3, CtrlMotionPos_1);
			b = length(CtrlMotionPos_2, CtrlMotionPos_1);

			CtrlMotionPos_2 = Point(CtrlMotionPos_1.x+rotate(arc(0.25,a,b),ang).x, CtrlMotionPos_1.y-rotate(arc(0.25,a,b),ang).y);
			StartMotionPos = unrotate(StartMotionPos, ang, CtrlMotionPos_1.x, CtrlMotionPos_1.y);
			if(StartMotionPos.x >= a)
			{
			    StartMotionPos.y = (StartMotionPos.y/StartMotionPos.x)*a;
			    StartMotionPos.x = a;
			}
			if(StartMotionPos.x < -a)
			{
			    StartMotionPos.y = (StartMotionPos.y/StartMotionPos.x)*(-a);
			    StartMotionPos.x = -a;
			}
			t_start = acos(StartMotionPos.x/a)/(2*M_PI);
			if(StartMotionPos.y > 0) t_start = 1 - t_start;
			EndMotionPos = unrotate(EndMotionPos, ang, CtrlMotionPos_1.x, CtrlMotionPos_1.y);
			if(EndMotionPos.x < -a)
			{
			    EndMotionPos.y = (EndMotionPos.y/EndMotionPos.x)*(-a);
			    EndMotionPos.x = -a;
			}
			if(EndMotionPos.x >= a)
			{
			    EndMotionPos.y = (EndMotionPos.y/EndMotionPos.x)*a;
			    EndMotionPos.x = a;
			}
			t_end = acos(EndMotionPos.x/a)/(2*M_PI);
			if(EndMotionPos.y > 0) t_end = 1 - t_end;
			if(t_start > t_end) t_end += 1;
			if((t_end-1) > t_start) t_end -= 1;
			//if( t_start == t_end ) t_end += 1;
			if(fabs(t_start-t_end) < 0.0027777777777) t_end += 1;
			if(t_end > t_start && t_start >= 1 && t_end > 1) { t_start -= 1; t_end -= 1; }
			CtrlMotionPos_4 = Point(t_start, t_end);
                    }
		    else
		    {
			ang = angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
			if(ip[0].y <= ip[1].y) ang = 360-ang;
		    }

		    bool flag_brd = false;
		    if(lnwidth > 3 && bord_width == 0)
		    {
			lnwidth -= 2;
			bord_width = 1;
			bord_color = color;
			flag_brd = true;
		    }

		    p.resize(5);
		    shapeItems.push_back(ShapeItem(p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,ang,color,bord_color,lnwidth,bord_width,elTp,style,flag_brd));
		    break;
		}
		case ShapeItem::Fill:
		{
		    int fl_color;
		    string img;
		    //>>> Fill color
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"c%d",&w_s) == 1) fl_color = colors[w_s];
		    else if((fl_color=mod->colorParse(el_s)) == -1) fl_color = fillClr;

		    //>>> Fill image
		    el_s = TSYS::strSepParse(sel, 0, ':', &el_off);
		    if(sscanf(el_s.c_str(),"i%d",&w_s) == 1) img = images[w_s];
		    else if(!(img=TSYS::strDecode(el_s)).size()) img = imgDef;
		    if(owner().resGet(img,path(),user) == "") img = "";

		    inundationItems.push_back(InundationItem(p,fl_color,-1,img));
		    break;
		}
	    }
	}
	for(WidthMap::iterator pi = widths.begin(); pi != widths.end(); )
	{
	    bool unDel = false;
	    for(unsigned i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->second == shapeItems[i].width || pi->second == shapeItems[i].border_width);
	    if(pi->first > 0 && !unDel) widths.erase(pi++); else ++pi;
	}
	for(ColorMap::iterator pi = colors.begin(); pi != colors.end(); )
	{
	    bool unDel = false;
	    for(unsigned i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->second == shapeItems[i].lineColor || pi->second == shapeItems[i].borderColor);
	    for(unsigned i = 0; !unDel && i < inundationItems.size(); i++)
		unDel = pi->first > 0 && pi->second == inundationItems[i].P_color;
	    if(pi->first > 0 && !unDel) colors.erase(pi++); else ++pi;
	}
	for(ImageMap::iterator pi = images.begin(); pi != images.end(); )
	{
	    bool unDel = false;
	    for(unsigned i = 0; !unDel && i < inundationItems.size(); i++)
		unDel = pi->first > 0 && (pi->second == inundationItems[i].imgFill);
	    if(pi->first > 0 && !unDel) images.erase(pi++); else ++pi;
	}
	for(StyleMap::iterator pi = styles.begin(); pi != styles.end(); )
	{
	    bool unDel = false;
	    for(unsigned i = 0; !unDel && i < shapeItems.size(); i++)
		unDel = pi->first > 0 && (pi->second == shapeItems[i].style);
	    if(pi->first > 0 && !unDel) styles.erase(pi++); else ++pi;
	}
    }
}

//*************************************************
//* Text				      *
//*************************************************
VCAText::VCAText( const string &iid ) : VCAObj(iid), im(NULL)
{

}

VCAText::~VCAText( )
{
    if(im) { gdImageDestroy(im); im = NULL; }
}

Point VCAText::rot( const Point pnt, double alpha, const Point center )
{
    return Point(center.x + ((pnt.x - center.x)*cos((alpha*M_PI)/180) - (pnt.y - center.y)*sin((alpha*M_PI)/180)),
		 center.y + ((pnt.x - center.x)*sin((alpha*M_PI)/180) + (pnt.y - center.y)*cos((alpha*M_PI)/180)));
}
vector<int> VCAText::textRotate( double ang, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4 )
{
    vector<int> wh;
    wh.push_back(0); wh.push_back(0);
    Point center = Point((x2-x4)/2, (y2-y4)/2);
    Point p1_rot = rot(Point(x1,y1), ang, center); Point p2_rot = rot(Point(x2,y2), ang, center);
    Point p3_rot = rot(Point(x3,y3), ang, center); Point p4_rot = rot(Point(x4,y4), ang, center);
    if(ang > 0 && ang < 90)
    {
	if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) < (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p4_rot.y - p1_rot.y)/(p4_rot.x - p1_rot.x),
		    k2Rot = (p4_rot.y - p3_rot.y)/(p4_rot.x - p3_rot.x);
	    Point p1Rez = Point(x1, p1_rot.y);
	    Point p3Rez = Point(x3, p3_rot.y);
	    double B1 = p1Rez.y - k1Rot*p1Rez.x;
	    double B2 = p3Rez.y - k2Rot*p3Rez.x;
	    Point p4Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot)+B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p4Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p4Rez,p3Rez), POS_PREC_DIG, true);
	}
	else if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) > (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p2_rot.y - p1_rot.y)/(p2_rot.x - p1_rot.x),
		    k2Rot = (p4_rot.y - p1_rot.y)/(p4_rot.x - p1_rot.x);
	    Point p4Rez = Point(p4_rot.x, y4);
	    Point p2Rez = Point(p2_rot.x, y2);
	    double B1 = p2Rez.y - k1Rot*p2Rez.x;
	    double B2 = p4Rez.y - k2Rot*p4Rez.x;
	    Point p1Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p4Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p2Rez), POS_PREC_DIG, true);
	}
	else
	{
	    int ln = (int)TSYS::realRound(VCAElFigure::length(Point(x1,p1_rot.y),Point(p2_rot.x,y2)), POS_PREC_DIG, true);
	    wh[0] = wh[1] = ln;
	}
    }
    else if(ang > 90 && ang < 180)
    {
	if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) < (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p4_rot.y - p1_rot.y)/(p4_rot.x - p1_rot.x),
		    k2Rot = (p2_rot.y - p1_rot.y)/(p2_rot.x - p1_rot.x);
	    Point p2Rez = Point(x4, p2_rot.y);
	    Point p4Rez = Point(x1, p4_rot.y);
	    double B1 = p4Rez.y - k1Rot*p4Rez.x;
	    double B2 = p2Rez.y - k2Rot*p2Rez.x;
	    Point p1Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot)+B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p4Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p2Rez), POS_PREC_DIG, true);
	}
	else if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) > (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p2_rot.y - p1_rot.y)/(p2_rot.x - p1_rot.x),
		    k2Rot = (p3_rot.y - p2_rot.y)/(p3_rot.x - p2_rot.x);
	    Point p1Rez = Point(p1_rot.x, y2);
	    Point p3Rez = Point(p3_rot.x, y1);
	    double B1 = p1Rez.y - k1Rot*p1Rez.x;
	    double B2 = p3Rez.y - k2Rot*p3Rez.x;
	    Point p2Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p2Rez,p3Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p1Rez, p2Rez), POS_PREC_DIG, true);
	}
	else
	{
	    int ln = (int)TSYS::realRound(VCAElFigure::length(Point(p1_rot.x,y2),Point(x3,p2_rot.y)), POS_PREC_DIG, true);
	    wh[0] = wh[1] = ln;
	}

    }
    else if(ang > 180 && ang < 270)
    {
	if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) < (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p2_rot.y - p1_rot.y)/(p2_rot.x - p1_rot.x),
		    k2Rot = (p3_rot.y - p2_rot.y)/(p3_rot.x - p2_rot.x);
	    Point p1Rez = Point(x4, p1_rot.y);
	    Point p3Rez = Point(x2, p3_rot.y);
	    double B1 = p1Rez.y - k1Rot*p1Rez.x;
	    double B2 = p3Rez.y - k2Rot*p3Rez.x;
	    Point p2Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p2Rez,p3Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p2Rez), POS_PREC_DIG, true);
	}
	else if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) > (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p3_rot.y - p2_rot.y)/(p3_rot.x - p2_rot.x),
		    k2Rot = (p4_rot.y - p3_rot.y)/(p4_rot.x - p3_rot.x);
	    Point p4Rez = Point(p4_rot.x, y2);
	    Point p2Rez = Point(p2_rot.x, y1);
	    double B1 = p2Rez.y - k1Rot*p2Rez.x;
	    double B2 = p4Rez.y - k2Rot*p4Rez.x;
	    Point p3Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p2Rez,p3Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p3Rez,p4Rez), POS_PREC_DIG, true);
	}
	else
	{
	    int ln = (int)TSYS::realRound(VCAElFigure::length(Point(x3,p1_rot.y),Point(p2_rot.x,y4)), POS_PREC_DIG, true);
	    wh[0] = wh[1] = ln;
	}
    }
    else if(ang > 270 && ang < 360)
    {
	if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) < (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p4_rot.y - p3_rot.y)/(p4_rot.x - p3_rot.x),
		    k2Rot = (p3_rot.y - p2_rot.y)/(p3_rot.x - p2_rot.x);
	    Point p4Rez = Point(x4, p4_rot.y);
	    Point p2Rez = Point(x2, p2_rot.y);
	    double B1 = p4Rez.y - k1Rot*p4Rez.x;
	    double B2 = p2Rez.y - k2Rot*p2Rez.x;
	    Point p3Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p2Rez,p3Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p3Rez,p4Rez), POS_PREC_DIG, true);
	}
	else if((int)TSYS::realRound(VCAElFigure::ABS(x1-x3),POS_PREC_DIG,true) > (int)TSYS::realRound(VCAElFigure::ABS(y2-y1),POS_PREC_DIG,true))
	{
	    double  k1Rot = (p2_rot.y - p1_rot.y)/(p2_rot.x - p1_rot.x),
		    k2Rot = (p3_rot.y - p2_rot.y)/(p3_rot.x - p2_rot.x);
	    Point p1Rez = Point(p1_rot.x, y1);
	    Point p3Rez = Point(p3_rot.x, y3);
	    double B1 = p1Rez.y - k1Rot*p1Rez.x;
	    double B2 = p3Rez.y - k2Rot*p3Rez.x;
	    Point p2Rez = Point((B2-B1)/(k1Rot-k2Rot), k1Rot*(B2-B1)/(k1Rot-k2Rot) + B1);
	    wh[0] = (int)TSYS::realRound(VCAElFigure::length(p2Rez,p3Rez), POS_PREC_DIG, true);
	    wh[1] = (int)TSYS::realRound(VCAElFigure::length(p1Rez,p2Rez), POS_PREC_DIG, true);
	}
	else
	{
	    int ln = (int)TSYS::realRound(VCAElFigure::length(Point(p1_rot.x,y1),Point(x1,p2_rot.y)), POS_PREC_DIG, true);
	    wh[0] = wh[1] = ln;
	}
    }
    return wh;
}

void VCAText::getReq( SSess &ses )
{
    ResAlloc res(mRes,false);

    //Prepare picture
    map< string, string >::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1;
    scaleHeight = (int)TSYS::realRound(height*ySc, POS_PREC_DIG, true);
    scaleWidth = (int)TSYS::realRound(width*xSc, POS_PREC_DIG, true);
    int txtFontSize = 0;
    txtFontSize = (int)((float)textFontSize*vmin(xSc, ySc));
    if(im) gdImageDestroy(im);
    im = gdImageCreateTrueColor(scaleWidth, scaleHeight);
    if(!im) ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    else
    {
	gdImageAlphaBlending(im, 0);
	gdImageFilledRectangle(im, 0, 0, scaleWidth-1, scaleHeight-1, gdImageColorResolveAlpha(im,0,0,0,127));
	gdImageAlphaBlending(im, 1);

	int brect[8];
	gdFTStringExtra strex;
	strex.flags =  gdFTEX_RESOLUTION;
	strex.vdpi = 72;
	strex.hdpi = 72;

	int rotateWidth, rotateHeight, lnSpace = (int)txtFontSize/3;
	if(VCAElFigure::ABS(orient-90) < 0.01 || VCAElFigure::ABS(orient-270) < 0.01)
	{
	    rotateWidth = scaleHeight;
	    rotateHeight = scaleWidth;
	}
	else if(VCAElFigure::ABS(orient-180) < 0.01 || VCAElFigure::ABS(orient-360) < 0.01)
	{
	    rotateWidth = scaleWidth;
	    rotateHeight = scaleHeight;
	}
	else
	{
	    vector<int> wh = textRotate(orient, scaleWidth, 0, scaleWidth, scaleHeight, 0, scaleHeight, 0, 0);
	    rotateWidth = wh[0];
	    rotateHeight = wh[1];
	}

	//Replacing the "\t" in the source string with the " "
	size_t fnd = text.find("\t");
	if(fnd != string::npos)
	    do
	    {
		text.replace(fnd, 1, " ");
		fnd = text.find("\t");
	    }
	    while(fnd != string::npos);

	//Formation of the string's vector from the source string using the "\n" separator
	string wrap_text = text, wrap_end;
	vector<string> str_wrap;
	vector<int> hgt_wrap;
	fnd = wrap_text.find("\n");
	vector<string> wrp_txt;
	if(fnd != string::npos)
	{
	    bool flg = false;
	    do
	    {
		wrp_txt.push_back(wrap_text.substr(0,fnd));
		wrap_text = wrap_text.substr(fnd+1);
		fnd = wrap_text.find("\n");
		if( fnd == string::npos ){ wrp_txt.push_back(wrap_text); flg = true; }
	    }
	    while(flg == false);
	}
	else wrp_txt.push_back(wrap_text);

	//Word wrap algorithm
	if(wordWrap)
	{
	    // Parsing each string in the formed vector and makin wordWrap in it
	    for(unsigned f = 0; f < wrp_txt.size(); f++)
	    {
		int brect_wrap[8];
		int wrapWidth;
		gdImageStringFTEx(NULL, &brect_wrap[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0, (char*)(wrp_txt[f].c_str()), &strex);
		wrapWidth = brect_wrap[2] - brect_wrap[6];
		//  Check if the width of the string is more than the width of the image
		if(wrapWidth > rotateWidth)
		{
		    wrap_text = wrp_txt[f];
		    string wrap_temp;
		    bool flWrapEnd = false;
		    do
		    {
			int brect_wr[8];
			size_t found, fnd;
			found = wrap_text.find_first_of(" ");
			if(found != string::npos)
			{
			    string wrap_before = wrap_text.substr(0, found+1);
			    wrap_text = wrap_text.substr(found+1);
			    //Connecting the words, divided with the " " till their sum length <= rotateWidth
			    gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0, (char*)(wrap_temp.c_str()), &strex);
			    int wdtTmp = brect_wr[2] - brect_wr[6];
			    gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0, (char*)(wrap_before.c_str()), &strex);
			    if(((brect_wr[2]-brect_wr[6]) + wdtTmp) <= rotateWidth) wrap_temp.append(wrap_before);
			    //Check if the was no any append to the wrap_temp and the size of the wrap_before > rotateWidth
			    else if(wrap_temp.size() == 0)
			    {
				//Erase the " " at the end of the string
				fnd = wrap_before.rfind(" ");
				if(fnd == (wrap_before.size()-1)) wrap_before.erase(fnd);

				str_wrap.push_back(wrap_before);
				hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
			    }
			    else
			    {
				wrap_text.insert(0, wrap_before);
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0, (char*)(wrap_temp.c_str()), &strex);
				//Erase the " " at the end of the string
				fnd = wrap_temp.rfind(" ");
				if(fnd == (wrap_temp.size()-1)) wrap_temp.erase(fnd);

				str_wrap.push_back(wrap_temp);
				hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
				wrap_temp.clear();
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)(wrap_text.c_str()), &strex);
				if((brect_wr[2]-brect_wr[6]) <= rotateWidth)
				{
				    fnd = wrap_text.rfind(" ");
				    if(fnd == (wrap_text.size()-1)) wrap_text.erase(fnd);

				    str_wrap.push_back(wrap_text);
				    hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
				    flWrapEnd = true;
				}
			    }
			}
			else//If there is no " " in the string or in the rest of the string
			{
			    bool app = false;
			    if(wrap_temp.size())
			    {
				//Check if the rest of the string without " " is small anough to append it the wrap_temp and push_back to the array
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)wrap_temp.c_str(), &strex);
				int wdtTmp = brect_wr[2]-brect_wr[6];
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)wrap_text.c_str(), &strex);
				if((brect_wr[2]-brect_wr[6]) + wdtTmp <= rotateWidth) { wrap_temp.append(wrap_text); app = true; }
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)wrap_temp.c_str(), &strex);
				//Erase the " " at the end of the string
				if(!app)
				{
				    fnd = wrap_temp.rfind(" ");
				    if(fnd == (wrap_temp.size()-1)) wrap_temp.erase(fnd);
				}

				str_wrap.push_back(wrap_temp);
				hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
			    }
			    if(!app)
			    {
				gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)wrap_text.c_str(), &strex);
				str_wrap.push_back(wrap_text);
				hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
			    }
			    flWrapEnd = true;
			}
		    }
		    while(!flWrapEnd);
		}
		else
		{
		    string res;
		    int brect_wr[8];
		    //Check if the string is empty and if it is so, change it with the "text" for the normal string height
		    res = (wrp_txt[f] != "") ? wrp_txt[f] : "text";
		    gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)res.c_str(), &strex);
		    str_wrap.push_back(wrp_txt[f]);
		    hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);
		}
	    }
	}
	else
	    for(unsigned f = 0; f < wrp_txt.size(); f++)
	    {
		string res;
		int brect_wr[8];
		//Check if the string is empty and if it is so, change it with the "text" for the normal string height
		res = (wrp_txt[f] != "") ? wrp_txt[f] : "text";
		gdImageStringFTEx(NULL, &brect_wr[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0,(char*)res.c_str(), &strex);
		str_wrap.push_back(wrp_txt[f]);
		hgt_wrap.push_back(brect_wr[3]-brect_wr[7]);

	    }
	//Calculating the summary height of the all strings in the array plus the linespacing interval between them
	int wrapHgt = 0;
	for(unsigned f = 0; f < str_wrap.size(); f++) { wrapHgt += hgt_wrap[f]; }
	wrapHgt += str_wrap.size()*lnSpace;

	gdImagePtr im_txt = gdImageCreateTrueColor(rotateWidth, rotateHeight);
	//Calculating the offset from the top left corner of the 'rotate' image(with the rotateWidth and rotateHeight)
	int offsetY = 0, offsetX = 0;
	if(alignVer == 1)	offsetY = 0;
	else if(alignVer == 2)	offsetY = wrapHgt - (rotateHeight-2);
	else if(alignVer == 3)	offsetY = (wrapHgt - (rotateHeight-2))/2;
	gdImageAlphaBlending(im_txt, 0);
	gdImageFilledRectangle(im_txt, 0, 0, rotateWidth-1, rotateHeight-1, gdImageColorResolveAlpha(im_txt,0,0,0,127));
	gdImageAlphaBlending(im_txt, 1);
	int clr_txt = gdImageColorResolveAlpha(im_txt, (uint8_t)(textColor>>16), (uint8_t)(textColor>>8), (uint8_t)textColor, 127-(uint8_t)(textColor>>24));

	//Drawing the all strings from the array
	int y_new = 0;
	for(unsigned k = 0; k < str_wrap.size(); k++)
	{
	    gdImageStringFTEx(NULL, &brect[0], 0, (char*)textFont.c_str(), txtFontSize, 0, 0, 0, (char*)str_wrap[k].c_str(), &strex);
	    if(alignHor == 1)	   offsetX = 1;
	    else if(alignHor == 2) offsetX = rotateWidth - (brect[4]-brect[0]);
	    else if(alignHor == 3) offsetX = (rotateWidth - (brect[4]-brect[0]))/2;
	    else if(alignHor == 4) offsetX = 0;
	    int realY = hgt_wrap[k] - offsetY + y_new;
	    char *rez = gdImageStringFTEx(im_txt, &brect[0], clr_txt, (char*)textFont.c_str(), txtFontSize, 0, offsetX, realY, (char*)str_wrap[k].c_str(), &strex);
	    if(rez) mess_err(nodePath().c_str(),_("gdImageStringFTex for font '%s' error: %s."),textFont.c_str(),rez);
	    else
	    {
		int wdt = bold ? (int)TSYS::realRound(txtFontSize/6,POS_PREC_DIG,true) : (int)TSYS::realRound(txtFontSize/12,POS_PREC_DIG,true);
		gdImageSetThickness(im_txt, wdt);
		if(underline && !str_wrap[k].empty())
		    gdImageLine(im_txt, offsetX, realY+(int)lnSpace/2, offsetX+(brect[4]-brect[0]), realY+(int)lnSpace/2, clr_txt);
		if(strikeout && !str_wrap[k].empty())
		    gdImageLine(im_txt, offsetX, realY+(int)lnSpace/2-(brect[3]-brect[7])/2, offsetX+(brect[4]-brect[0]), realY+(int)lnSpace/2-(brect[3]-brect[7])/2, clr_txt);
	    }
	    y_new += hgt_wrap[k] + lnSpace;
	}
	gdImageSaveAlpha(im_txt, 1);
	gdImageCopyRotated(im, im_txt, scaleWidth/2, scaleHeight/2, 0, 0, rotateWidth, rotateHeight, (int)(360-orient));
	gdImageDestroy(im_txt);
	gdImageSaveAlpha(im, 1);
	//Get image and transfer it
	int img_sz;
	char *img_ptr = (char *)gdImagePngPtrEx(im, &img_sz, mod->PNGCompLev());
	ses.page.assign(img_ptr, img_sz);
	ses.page = mod->httpHead("200 OK", ses.page.size(), "image/png") + ses.page;
	gdFree(img_ptr);
    }
}

void VCAText::setAttrs( XMLNode &node, const string &user )
{
    ResAlloc res(mRes, true);
    XMLNode *req_el;
    bool reform = false;
    for(unsigned i_a = 0; i_a < node.childSize(); i_a++)
    {
	req_el = node.childGet(i_a);
	if(req_el->name() != "el") continue;
	unsigned uiPrmPos = s2i(req_el->attr("p"));
	switch(uiPrmPos)
	{
	    case A_GEOM_W: width = s2r(req_el->text());	break;
	    case A_GEOM_H: height = s2r(req_el->text());break;
	    case A_TextFont:
	    {
		char family[101]; strcpy(family,"Arial");
		int bld = 0, italic = 0, undLine = 0, strOut = 0;
		textFontSize = 10;
		sscanf(req_el->text().c_str(),"%100s %d %d %d %d %d",family,&textFontSize,&bld,&italic,&undLine,&strOut);
		textFont = family;
		for(unsigned p = 0; p < textFont.size(); p++) if( textFont[p] == '_' ) textFont[p] = ' ';
		if(bld)	{ textFont += ":bold"; bold = true; }
		else	bold = false;
		if(italic)	textFont += ":italic";
		if(undLine)	underline = true;
		else		underline = false;
		if(strOut)	strikeout = true;
		else		strikeout = false;
		break;
	    }
	    case A_TextColor: textColor =  mod->colorParse(req_el->text());	break;
	    case A_TextOrient:
		orient = s2r(req_el->text());
		if(orient < 0) orient = 360 + orient;
		break;
	    case A_TextWordWrap: wordWrap = s2i(req_el->text());	break;
	    case A_TextAlignment:
	    {
		int txtAlign = s2i(req_el->text());
		switch(txtAlign&0x3)
		{
		    case 0: alignHor = 1; break;
		    case 1: alignHor = 2; break;
		    case 2: alignHor = 3; break;
		    case 3: alignHor = 4; break;
		}
		switch(txtAlign>>2)
		{
		    case 0: alignVer = 1; break;
		    case 1: alignVer = 2; break;
		    case 2: alignVer = 3; break;
		}
		break;
	    }
	    case A_TextText:
	    {
		string newText = Mess->codeConvOut("UTF-8", req_el->text());
		if(text_tmpl == newText)	break;
		text_tmpl = newText;
		reform = true;
		break;
	    }
	    case A_TextNumbArg:
	    {
		unsigned numbArg = s2i(req_el->text());
		while(args.size() < numbArg)	args.push_back(ArgObj());
		while(args.size() > numbArg)	args.pop_back();
		reform = true;
		break;
	    }
	    default:
		//Individual arguments process
		if(uiPrmPos >= A_TextArs)
		{
		    unsigned argN = (uiPrmPos-A_TextArs)/A_TextArsSz;
		    if(argN >= args.size())	break;
		    if((uiPrmPos%A_TextArsSz) == A_TextArsVal)		args[argN].setVal(req_el->text());
		    else if((uiPrmPos%A_TextArsSz) == A_TextArsTp)	args[argN].setType(s2i(req_el->text()));
		    else if((uiPrmPos%A_TextArsSz) == A_TextArsCfg)	args[argN].setCfg(req_el->text().c_str());
		    reform = true;
		}
	}

    }
    if(reform)
    {
	string txt = text_tmpl.c_str();
	string argVal;
	//Placing the arguments to the text
	for(unsigned i_a = 0; i_a < args.size(); i_a++)
	{
	    switch(args[i_a].type())
	    {
		case FT_INT: case FT_STR: argVal = args[i_a].val();	break;
		case FT_REAL:
		{
		    string atp = TSYS::strSepParse(args[i_a].cfg(),1,';');
		    argVal = r2s(s2r(args[i_a].val()),
			    s2i(TSYS::strSepParse(args[i_a].cfg(),2,';')), (atp.size()?atp[0]:'f'));
		    break;
		}
	    }
	    int argSize = s2i(TSYS::strSepParse(args[i_a].cfg(),0,';'));
	    argSize = vmax(-1000,vmin(1000,argSize));
	    string argPad = "";
	    for(int j = argVal.length(); j < VCAElFigure::ABS(argSize); j++) argPad += ' ';
	    if(argSize > 0) argVal = argPad+argVal; else argVal += argPad;
	    string rep = "%"+i2s(i_a+1);
	    size_t fnd = txt.find(rep);
	    if(fnd != string::npos)
		do
		{
		    txt.replace(fnd, rep.length(), argVal);
		    fnd = txt.find(rep);
		}
		while(fnd != string::npos);
	}
	if(txt != text.c_str()) text = txt;
    }
}

//*************************************************
//* VCADiagram					  *
//*************************************************
VCADiagram::VCADiagram( const string &iid ) : VCAObj(iid), type(0),
    tTimeCurent(false), holdCur(false), tTime(0), sclHorPer(0), tSize(1), sclVerScl(100), sclVerSclOff(0), lstTrc(false)
{

}

void VCADiagram::getReq( SSess &ses )
{
    switch(type)
    {
	case FD_TRND: makeTrendsPicture(ses);		break;
	case FD_SPECTR: makeSpectrumPicture(ses);	break;
    }
}

void VCADiagram::makeImgPng( SSess &ses, gdImagePtr im )
{
    gdImageSaveAlpha(im, 1);
    int img_sz;
    char *img_ptr = (char*)gdImagePngPtrEx(im, &img_sz, mod->PNGCompLev());
    ses.page.assign(img_ptr, img_sz);
    ses.page = mod->httpHead("200 OK", ses.page.size(), "image/png") + ses.page;
    gdFree(img_ptr);
    gdImageDestroy(im);
}

void VCADiagram::makeTrendsPicture( SSess &ses )
{
    ResAlloc res(mRes, true);

    int64_t dbTm = 0;
    if(mess_lev() == TMess::Debug) dbTm = TSYS::curTime();

    //> Check for trend's data reload
    bool rld = true;
    if(tTimeCurent)	tTime = (int64_t)time(NULL)*1000000;
    else if(trcPer && lstTrc < time(NULL)) { tTime += (time(NULL)-lstTrc)*1000000; lstTrc = time(NULL); }
    else rld = false;
    if(rld)
    {
	for(unsigned i_p = 0; i_p < trnds.size(); i_p++) trnds[i_p].loadData(ses.user);
	//> Trace cursors value
	if(active)
	{
	    int64_t tTimeGrnd = tTime - (int64_t)(1e6*tSize);
	    if(holdCur || curTime >= (tTime-2*(int64_t)trcPer*1000000) || curTime <= tTimeGrnd)
		setCursor(tTime, ses.user);
	}
    }

    int mrkFontSize = 0;
    int mrkHeight = 0, mrkWidth = 0;
    int clr_grid = 0, clr_mrk = 0;					//Colors

    //> Get generic parameters
    int64_t tSz  = (int64_t)(1e6*tSize);				//Trends size (us)
    int64_t tEnd = tTime;						//Trends end point (us)
    tPict = tEnd;
    int64_t tBeg = tEnd - tSz;					//Trends begin point (us)

    //> Get scale
    map<string,string>::iterator prmEl = ses.prm.find("xSc");
    float xSc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    prmEl = ses.prm.find("ySc");
    float ySc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    int imW = (int)TSYS::realRound((float)width*xSc,POS_PREC_DIG,true);
    int imH = (int)TSYS::realRound((float)height*ySc,POS_PREC_DIG,true);

    //> Prepare picture
    gdImagePtr im = gdImageCreateTrueColor(imW,imH);
    if(!im) { ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page; return; }
    gdImageAlphaBlending(im, 0);
    gdImageFilledRectangle(im, 0, 0, imW-1, imH-1, gdImageColorResolveAlpha(im,0,0,0,127));
    gdImageAlphaBlending(im, 1);
    int brect[8];

    if(!trnds.size() || tSz <= 0)	{ makeImgPng(ses,im); return; }

    //> Make decoration and prepare trends area
    tArX = 1, tArY = 1,						//Curves of trends area rect
    tArW = imW - 2*(geomMargin+bordWidth+1),
    tArH = imH - 2*(geomMargin+bordWidth+1);

    //> Setting the resolution for the text's font
    gdFTStringExtra strex;
    strex.flags =  gdFTEX_RESOLUTION;
    strex.vdpi = 72;
    strex.hdpi = 72;

    if(sclHor&FD_GRD_MARKS || sclVer&FD_GRD_MARKS)
    {
	gdImageSetThickness(im, vmax(1,(int)TSYS::realRound(vmin(xSc,ySc))));
	//>> Set grid color
	clr_grid = TWEB::colorResolve(im, sclColor);
	//gdImageColorAllocate(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor);
	if(sclHor&FD_MARKS || sclVer&FD_MARKS)
	{
	    //>> Set markers font and color
	    mrkFontSize = (float)sclMarkFontSize * vmin(xSc,ySc);
	    clr_mrk = TWEB::colorResolve(im, sclMarkColor);
	    //gdImageColorAllocate(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor);
	    char *rez = gdImageStringFTEx(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.,0,0,(char*)"000000", &strex);
	    if(rez) mess_err(nodePath().c_str(),_("gdImageStringFTEx for font '%s' error: %s."),sclMarkFont.c_str(),rez);
	    else { mrkHeight = brect[3]-brect[7]; mrkWidth = brect[2]-brect[6]; }
	    if(sclHor & FD_MARKS)
	    {
		if(tArH < (int)(100*vmin(xSc,ySc))) sclHor &= ~(FD_MARKS);
		else tArH -= 2*(mrkHeight+2);
	    }
	    if(sclVer&FD_MARKS && tArW < (int)(100*vmin(xSc,ySc)) ) sclVer &= ~(FD_MARKS);
	}
    }

    //> Calc vertical scale
    int64_t aVend;					//Corrected for allow data the trend end point
    int64_t aVbeg;					//Corrected for allow data the trend begin point
    bool    vsPerc = true;				//Vertical scale percent mode
    bool    isLog = sclVer&FD_LOG;			//Logarithmic scale
    bool    isScale = (fabs(sclVerSclOff) > 1 || fabs(sclVerScl-100) > 1);
    double  curVl, vsMax = -3e300, vsMin = 3e300;	//Trend's vertical scale border

    //>> Get main scale for non individual parameters
    int prmInGrp = 0, prmGrpLast = -1;
    for(unsigned i_p = 0, mainPerc = false; i_p < trnds.size(); i_p++)
    {
	TrendObj &cP = trnds[i_p];
	if(!cP.val().size() || ((cP.color()>>31)&0x01))	continue;

	cP.adjU = -3e300, cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL() && cP.valTp() != 0)
	{
	    //>> Check trend for valid data
	    aVbeg = vmax(tBeg, cP.valBeg());
	    aVend = vmin(tEnd, cP.valEnd());

	    if(aVbeg >= aVend)	{ makeImgPng(ses,im); return; }
	    //>> Calc value borders
	    bool end_vl = false;
	    int ipos = cP.val(aVbeg);
	    if(ipos && cP.val()[ipos].tm > aVbeg) ipos--;
	    while(true)
	    {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend)	end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL)
		{
		    curVl = cP.val()[ipos].val;
		    cP.adjL = vmin(cP.adjL, curVl); cP.adjU = vmax(cP.adjU, curVl);
		}
		ipos++;
	    }
	    if(cP.adjU == -3e300)		{ cP.adjU = 1.0; cP.adjL = 0.0; }
	    else if((cP.adjU-cP.adjL) < 1e-30 && fabs(cP.adjU) < 1e-30) { cP.adjU += 0.5; cP.adjL -= 0.5; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001)
	    {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	else if(cP.bordU() <= cP.bordL() && cP.valTp() == 0)	{ cP.adjU = 1.5; cP.adjL = -0.5; }
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclVer|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS)      continue;

	//>>> Check for value border allow
	if(!mainPerc && (vsMin > vsMax || vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = vmin(vsMin,cP.adjL); vsMax = vmax(vsMax,cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = i_p;
    }

    //>> Check for individual parameters and for possibility to merge it to group or create new for no group
    int prmIndiv = 0;
    int prmIndivSc = -1;
    vector<int> prmsInd;
    for(unsigned i_p = 0; i_p < trnds.size(); i_p++)
    {
	TrendObj &cP = trnds[i_p];
	cP.isIndiv = false;
	if(!cP.val().size() || ((cP.color()>>31)&0x01) || !(cP.wScale&FD_GRD_MARKS)) continue;
	//>> Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (vsMin < vsMax && vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2)) &&
	    (cP.mScale&FD_LOG) == (sclVer&FD_LOG))
	{
	    vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = i_p;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = i_p;
	else prmsInd.push_back(i_p);
	if(cP.mScale&FD_LOG)
	{
	    cP.adjU = log10(vmax(1e-100,cP.adjU)); cP.adjL = log10(vmax(1e-100,cP.adjL));
	    if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.0001)
	    {
		double wnt_dp = 0.0001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	if(isScale)     //Vertical scale and offset apply
	{
	    float vsDif = cP.adjU - cP.adjL;
	    cP.adjU += sclVerSclOff*vsDif/100;		cP.adjL += sclVerSclOff*vsDif/100;
	    cP.adjU += (sclVerScl*vsDif/100-vsDif)/2;	cP.adjL -= (sclVerScl*vsDif/100-vsDif)/2;
	}
    }
    if(prmInGrp) prmsInd.push_back(-1);
    if(prmIndivSc >= 0) prmsInd.push_back(prmIndivSc);

    //>> Final main scale adapting
    if(vsMin > vsMax) { vsPerc = true; vsMax = 100; vsMax = 100, vsMin = isLog ? pow(10,vmin(0,2-(tArH/150))) : 0; }
    else vsPerc = false;
    if(isLog)
    {
	vsMax = log10(vmax(1e-100,vsMax)); vsMin = log10(vmax(1e-100,vsMin));
	if((vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.0001)
	{
	    double wnt_dp = 0.0001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
	    vsMin -= wnt_dp/2; vsMax += wnt_dp/2;
	}
    }
    if(isScale)	//Vertical scale and offset apply
    {
	float vsDif = vsMax - vsMin;
	vsMax += sclVerSclOff*vsDif/100;	vsMin += sclVerSclOff*vsDif/100;
	vsMax += (sclVerScl*vsDif/100-vsDif)/2;	vsMin -= (sclVerScl*vsDif/100-vsDif)/2;
    }

    //> Draw main and individual vertical scales
    float vmax_ln = tArH / ((sclVer&FD_MARKS && mrkHeight)?(2*mrkHeight):(int)(15*vmin(xSc,ySc)));
    for(unsigned i_p = 0; vmax_ln >= 2 && i_p < prmsInd.size(); i_p++)       //prmsInd[i]=-1 - for main scale
    {
	bool	isLogT, vsPercT;
	char	sclVerT;
	int     clrGridT = TWEB::colorResolve(im, sclColor);
	double	vsMinT, vsMaxT;
	double	vDiv = 1;
	if(prmsInd[i_p] < 0)    //Main scale process
	{
	    //>> Draw environment
	    vsPercT = vsPerc;
	    isLogT = isLog;
	    sclVerT = sclVer;
	    clrGridT = TWEB::colorResolve(im, sclColor);
	    clr_mrk = TWEB::colorResolve(im, sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0)        //Set color for single parameter in main group
		clrGridT = clr_mrk = TWEB::colorResolve(im, trnds[prmGrpLast].color());
	    //>> Rounding
	    double v_len = vsMax - vsMin;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv; }
	    while(!isLogT && ((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = vsMin; vsMaxT = vsMax;
	}
	else    //Individual scale process
        {
	    TrendObj &cP = trnds[prmsInd[i_p]];
	    //>> Draw environment
	    vsPercT = false;
	    isLogT = cP.mScale&FD_LOG;
	    sclVerT = cP.wScale;
	    clrGridT = clr_mrk = TWEB::colorResolve(im, cP.color());
	    //>> Rounding
	    double v_len = cP.adjU - cP.adjL;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv; }
	    while(!isLogT && ((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(i_p < (prmsInd.size()-1))    sclVerT &= ~(FD_GRD);  //Hide grid for no last scale

	//>> Draw vertical grid and markers
	int markWdth = 0;
	if(sclVerT&FD_GRD_MARKS)
	{
	    string labVal;
	    gdImageLine(im, tArX-1, tArY, tArX-1, tArH, clrGridT);
	    for(double i_v = ceil(vsMinT/vDiv)*vDiv; (vsMaxT-i_v)/vDiv > -0.1; i_v += vDiv)
	    {
		//>>> Draw grid
		int v_pos = tArY + tArH - (int)((double)tArH*(i_v-vsMinT)/(vsMaxT-vsMinT));
		if(sclVerT&FD_GRD) gdImageLine(im, tArX, v_pos, tArX+tArW, v_pos, clr_grid);
		else gdImageLine(im, tArX-3, v_pos, tArX+3, v_pos, clrGridT);
		//>>> Draw markers
		if(sclVerT&FD_MARKS && mrkHeight)
		{
		    bool isPerc = vsPercT && ((vsMaxT-i_v-vDiv)/vDiv <= -0.1);
		    bool isMax = (v_pos-1-mrkHeight) < tArY;
		    labVal = TSYS::strMess("%0.5g",(isLogT?pow(10,i_v):i_v)) + (isPerc?" %":"");
		    gdImageStringFTEx(im, &brect[0], clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0,
			tArX+2, v_pos-1+(isMax?mrkHeight:0), (char*)labVal.c_str(), &strex);
		    markWdth = vmax(markWdth, brect[2]-brect[6]);
		}
	    }
	}
	if(i_p < (prmsInd.size()-1)) { tArX += markWdth?(markWdth+5):0; tArW -= markWdth?(markWdth+5):0; }
    }
    clr_mrk = TWEB::colorResolve(im, sclMarkColor);

    //> Calc horizontal scale
    int64_t hDiv = 1;					//Horisontal scale divisor
    int hmax_ln = tArW / (int)((sclHor&FD_MARKS && mrkWidth)?mrkWidth:15.0*vmin(xSc,ySc));
    if(hmax_ln >= 2)
    {
	int hvLev = 0;
	int64_t hLen = tEnd - tBeg;
	if(hLen/86400000000ll >= 2)	{ hvLev = 5; hDiv = 86400000000ll; }	//Days
	else if(hLen/3600000000ll >= 2)	{ hvLev = 4; hDiv =  3600000000ll; }	//Hours
	else if(hLen/60000000 >= 2)	{ hvLev = 3; hDiv =    60000000; }	//Minutes
	else if(hLen/1000000 >= 2)	{ hvLev = 2; hDiv =     1000000; }	//Seconds
	else if(hLen/1000 >= 2)		{ hvLev = 1; hDiv =	1000; }		//Milliseconds
	while(hLen/hDiv > hmax_ln)     hDiv *= 10;
	while(hLen/hDiv < hmax_ln/2)   hDiv /= 2;
	if(hLen/hDiv >= 5 && trcPer)
	{
	    tPict = hDiv*(tEnd/hDiv+1);
	    tBeg = tPict-hLen;
	}

	if(sclHorPer > 0 && (hLen/sclHorPer) > 2 && (tArW/(hLen/sclHorPer)) > 15) hDiv = sclHorPer;

	//>> Draw horisontal grid and markers
	if(sclHor&FD_GRD_MARKS)
	{
	    time_t tm_t = 0;
	    struct tm ttm, ttm1 = ttm;
	    string lab_tm, lab_dt;

	    localtime_r(&tm_t, &ttm);
	    int64_t UTChourDt = (int64_t)ttm.tm_hour*3600000000ll;

	    //>>> Draw generic grid line
	    gdImageLine(im, tArX, tArY+tArH, tArX+tArW, tArY+tArH, clr_grid);
	    //>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -5;
	    int endMarkBrd = tArX+tArW;
	    if(sclHor&FD_MARKS && mrkHeight)
	    {
		tm_t = tPict/1000000;
		localtime_r(&tm_t,&ttm);
		lab_dt = TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
		if(ttm.tm_sec == 0 && tPict%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if(tPict%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
		else lab_tm = TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(tPict%1000000)/1e6);
		gdImageStringFTEx(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_dt.c_str(), &strex);
		int markBrd = tArX+tArW-(brect[2]-brect[6]);
		endMarkBrd = markBrd;
		gdImageStringFTEx(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,tArY+tArH+3+2*(brect[3]-brect[7]),(char*)lab_dt.c_str(), &strex);
		gdImageStringFTEx(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,0,0,(char*)lab_tm.c_str(), &strex);
		markBrd = tArX+tArW-(brect[2]-brect[6]);
		endMarkBrd = vmin(endMarkBrd,markBrd);
		gdImageStringFTEx(im,NULL,clr_mrk,(char*)sclMarkFont.c_str(),mrkFontSize,0.0,markBrd,tArY+tArH+3+(brect[3]-brect[7]),(char*)lab_tm.c_str(), &strex);
	    }

	    //>>> Draw grid and/or markers
	    bool first_m = true;
	    for(int64_t i_h = tBeg; true; )
	    {
		//>>>> Draw grid
		int h_pos = tArX + tArW*(i_h-tBeg)/(tPict-tBeg);
		if(sclHor&FD_GRD) gdImageLine(im, h_pos, tArY, h_pos, tArY+tArH, clr_grid);
		else gdImageLine(im, h_pos, tArY+tArH-3, h_pos, tArY+tArH+3, clr_grid);
		//>>>> Draw markers
		if(sclHor&FD_MARKS && mrkHeight && !((i_h+UTChourDt)%hDiv) && i_h != tPict)
		{
		    tm_t = i_h/1000000;
		    localtime_r(&tm_t, &ttm);
		    int chLev = -1;
		    if(!first_m)
		    {
			if(ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year) chLev = 5;
			else if(ttm.tm_mday > ttm1.tm_mday) chLev = 4;
			else if(ttm.tm_hour > ttm1.tm_hour) chLev = 3;
			else if(ttm.tm_min > ttm1.tm_min)   chLev = 2;
			else if(ttm.tm_sec > ttm1.tm_sec)   chLev = 1;
			else chLev = 0;
		    }
		    //Check for data present
		    lab_dt = lab_tm = "";
		    //Date
		    if(hvLev == 5 || chLev >= 4)
			lab_dt = TSYS::strMess(((chLev>=5 || chLev==-1)?"%d-%02d-%d":"%d"), ttm.tm_mday, ttm.tm_mon+1, ttm.tm_year+1900);
		    //Hours and minuts
		    if((hvLev == 4 || hvLev == 3 || ttm.tm_hour || ttm.tm_min) && !ttm.tm_sec)
			lab_tm = TSYS::strMess("%d:%02d", ttm.tm_hour, ttm.tm_min);
		    //Seconds
		    else if((hvLev == 2 || ttm.tm_sec) && !(i_h%1000000))
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) :
							   Mess->codeConvOut("UTF-8",TSYS::strMess(_("%ds"),ttm.tm_sec));
		    //Milliseconds
		    else if(hvLev <= 1 || i_h%1000000)
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				 (chLev>=1) ? Mess->codeConvOut("UTF-8",TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6)) :
					      Mess->codeConvOut("UTF-8",TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.));
		    int wdth, tpos, endPosTm = 0, endPosDt = 0;
		    if(lab_tm.size())
		    {
			gdImageStringFTEx(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_tm.c_str(), &strex);
			wdth = brect[2]-brect[6];
			tpos = vmax(h_pos-wdth/2, 0);
			if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3))
			{
			    gdImageStringFTEx(im, NULL, clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0, tpos, tArY+tArH+3+(brect[3]-brect[7]), (char*)lab_tm.c_str(), &strex);
			    endPosTm = tpos+wdth;
			}
		    }
		    if(lab_dt.size())
		    {
			gdImageStringFTEx(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)lab_dt.c_str(), &strex);
			wdth = brect[2]-brect[6];
			tpos = vmax(h_pos-wdth/2, 0);
			if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3))
			{
			    gdImageStringFTEx(im, NULL, clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0, tpos, tArY+tArH+3+2*(brect[3]-brect[7]), (char*)lab_dt.c_str(), &strex);
			    endPosDt = tpos+wdth;
			}
		    }
		    begMarkBrd = vmax(begMarkBrd, vmax(endPosTm,endPosDt));
		    memcpy((char*)&ttm1, (char*)&ttm, sizeof(tm));
		    first_m = false;
		}
		//>>>> Next
		if(i_h >= tPict) break;
		i_h = ((i_h+UTChourDt)/hDiv)*hDiv + hDiv - UTChourDt;
		if(i_h > tPict)	i_h = tPict;
	    }
	}
    }

    //> Draw trends
    for(unsigned i_t = 0; i_t < trnds.size(); i_t++)
    {
	TrendObj &cP = trnds[i_t];

	//>> Set trend's pen
	int lnWdth = vmax(1,vmin(10,(int)TSYS::realRound(cP.width()*vmin(xSc,ySc))));
	gdImageSetThickness(im, lnWdth);
	int clr_t = TWEB::colorResolve(im, cP.color());

	//>> Prepare generic parameters
	aVbeg = vmax(tBeg, cP.valBeg());
	aVend = vmin(tEnd, cP.valEnd());
	if(aVbeg >= aVend || (cP.color()>>31)&0x01) continue;
	int aPosBeg = cP.val(aVbeg);;
	if(aPosBeg && cP.val()[aPosBeg].tm > aVbeg) aPosBeg--;
	bool vsPercT = cP.isIndiv ? false : vsPerc;
	bool isLogT = cP.isIndiv ? (cP.wScale&FD_LOG) : isLog;
	double vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	double vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	//>> Prepare border for percent trend
	float bordL = cP.bordL();
	float bordU = cP.bordU();
	if(vsPercT && bordL >= bordU)
	{
	    bordU = -3e300, bordL = 3e300;
	    bool end_vl = false;
	    int ipos = aPosBeg;
	    while(true)
	    {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend)	end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL)
		{
		    bordL = vmin(bordL, cP.val()[ipos].val);
		    bordU = vmax(bordU, cP.val()[ipos].val);
		}
		ipos++;
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>> Draw trend
	bool	end_vl = false;
	double	curVl = EVAL_REAL, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int	curPos, averPos = 0, prevPos = 0, z_vpos = 0;
	int64_t	curTm = 0, averTm = 0, averLstTm = 0;
	for(int a_pos = aPosBeg; true; a_pos++)
	{
	    if(a_pos < (int)cP.val().size() && !end_vl)
	    {
		curTm = vmin(aVend, vmax(aVbeg,cP.val()[a_pos].tm));
		curVl = cP.val()[a_pos].val;
		if(vsPercT && curVl != EVAL_REAL) curVl = 100*(curVl-bordL)/(bordU-bordL);
		if(isnan(curVl)) curVl = EVAL_REAL;
		curPos = tArX + tArW*(curTm-tBeg)/(tPict-tBeg);
	    }else curPos = 0;
	    if(!curPos || cP.val()[a_pos].tm >= aVend)	end_vl = true;
	    //Square Average
	    if(averPos == curPos)
	    {
		if(!(2*curTm-averTm-averLstTm)) continue;
		if(averVl == EVAL_REAL)  averVl = curVl;
		else if(curVl != EVAL_REAL)
		    averVl = (averVl*(double)(curTm-averTm)+curVl*(double)(curTm-averLstTm))/((double)(2*curTm-averTm-averLstTm));
		averLstTm = curTm;
		continue;
	    }
	    //Write point and line
	    if(averVl != EVAL_REAL)
	    {
		if(cP.valTp() == 0)
		    z_vpos = tArY + tArH - (int)((double)tArH*vmax(0,vmin(1,((vsPercT?(100*(0-bordL)/(bordU-bordL)):0)-vsMinT)/(vsMaxT-vsMinT))));
		int c_vpos = tArY + tArH - (int)((double)tArH*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,averVl)):averVl)-vsMinT)/(vsMaxT-vsMinT))));
		if(prevVl == EVAL_REAL)
		{
		    if(cP.valTp() != 0) gdImageSetPixel(im, averPos, c_vpos, clr_t);
		    else gdImageLine(im, averPos, z_vpos, averPos, vmin(z_vpos-lnWdth,c_vpos), clr_t);
		}
		else
		{
		    int c_vpos_prv = tArY + tArH - (int)((double)tArH*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,prevVl)):prevVl)-vsMinT)/(vsMaxT-vsMinT))));
		    if(cP.valTp() != 0) gdImageLine(im, prevPos, c_vpos_prv, averPos, c_vpos, clr_t);
		    else for(int sps = prevPos+1; sps <= averPos; sps++)
			gdImageLine(im, sps, z_vpos, sps, vmin(z_vpos-lnWdth,c_vpos), clr_t);
		}
	    }
	    prevVl  = averVl;
	    prevPos = averPos;
	    averVl  = curVl;
	    averPos = curPos;
	    averTm  = averLstTm = curTm;
	    if(!curPos) break;
	}
    }

    //> Draw cursor
    if(active && curTime && tBeg && tPict && (curTime >= tBeg || curTime <= tPict))
    {
	//>> Set trend's pen
	gdImageSetThickness(im, 1);
	int clr_cur = TWEB::colorResolve(im, curColor);
	//gdImageColorAllocate(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor);
	int curPos = tArX + tArW*(curTime-tBeg)/(tPict-tBeg);
	gdImageLine(im, curPos, tArY, curPos, tArY+tArH, clr_cur);
    }

    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("Trend creation time %gms"), 1e-3*(TSYS::curTime()-dbTm));

    //> Get image and transfer it
    makeImgPng(ses, im);

    if(mess_lev() == TMess::Debug) mess_debug(nodePath().c_str(), _("+ PNG-image(%d) creation time %gms"), ses.page.size(), 1e-3*(TSYS::curTime()-dbTm));
}

void VCADiagram::makeSpectrumPicture( SSess &ses )
{
    ResAlloc res(mRes, true);

    //> Check for trend's data reload
    bool rld = true;
    if(tTimeCurent)	tTime = (int64_t)time(NULL)*1000000;
    else if(trcPer && lstTrc < time(NULL)) { tTime += (time(NULL)-lstTrc)*1000000; lstTrc = time(NULL); }
    else rld = false;
    if(rld) for(unsigned i_p = 0; i_p < trnds.size(); i_p++) trnds[i_p].loadData(ses.user);

    int mrkFontSize = 0;
    int mrkHeight = 0, mrkWidth = 0;
    int clr_grid = 0, clr_mrk = 0;					//Colors

    //> Get generic parameters
    int64_t tSz  = (int64_t)(1e6*tSize);				//Time size (us)

    //> Get scale
    map<string,string>::iterator prmEl = ses.prm.find("xSc");
    double xSc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    prmEl = ses.prm.find("ySc");
    double ySc = (prmEl!=ses.prm.end()) ? vmin(100,vmax(0.1,s2r(prmEl->second))) : 1.0;
    int imW = (int)TSYS::realRound((double)width*xSc,POS_PREC_DIG,true);
    int imH = (int)TSYS::realRound((double)height*ySc,POS_PREC_DIG,true);

    //> Prepare picture
    gdImagePtr im = gdImageCreateTrueColor(imW, imH);
    if(!im) { ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page; return; }
    gdImageAlphaBlending(im,0);
    gdImageFilledRectangle(im, 0, 0, imW-1, imH-1, gdImageColorResolveAlpha(im,0,0,0,127));
    gdImageAlphaBlending(im,1);
    int brect[8];

    if(!trnds.size() || tSz <= 0)	{ makeImgPng(ses,im); return; }

    //> Make decoration and prepare trends area
    tArX = 1, tArY = 1,						//Curves of trends area rect
    tArW = imW-2*(geomMargin+bordWidth+1),
    tArH = imH-2*(geomMargin+bordWidth+1);

    //> Setting the resolution for the text's font
    gdFTStringExtra strex;
    strex.flags =  gdFTEX_RESOLUTION;
    strex.vdpi = 72;
    strex.hdpi = 72;

    //> Process scale
    if(sclHor&FD_GRD_MARKS || sclVer&FD_GRD_MARKS)
    {
	gdImageSetThickness(im, vmax(1,TSYS::realRound(vmin(xSc,ySc))));
	//>> Set grid color
	clr_grid = TWEB::colorResolve(im, sclColor);
	//gdImageColorAllocate(im,(uint8_t)(sclColor>>16),(uint8_t)(sclColor>>8),(uint8_t)sclColor);
	if(sclHor&FD_MARKS || sclVer&FD_MARKS)
	{
	    //>> Set markers font and color
	    mrkFontSize = (double)sclMarkFontSize*vmin(xSc, ySc);
	    clr_mrk = TWEB::colorResolve(im, sclMarkColor);
	    //gdImageColorAllocate(im,(uint8_t)(sclMarkColor>>16),(uint8_t)(sclMarkColor>>8),(uint8_t)sclMarkColor);
	    char *rez = gdImageStringFTEx(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)"000000", &strex);
	    if(rez) mess_err(nodePath().c_str(),_("gdImageStringFTEx for font '%s' error: %s."),sclMarkFont.c_str(),rez);
	    else { mrkHeight = brect[3]-brect[7]; mrkWidth = brect[2]-brect[6]; }
	    if(sclHor&FD_MARKS)
	    {
		if(tArH < (int)(100*vmin(xSc,ySc))) sclHor &= ~(FD_MARKS);
		else tArH -= mrkHeight+4;
	    }
	    if(sclVer&FD_MARKS && tArW < (int)(100*vmin(xSc,ySc))) sclVer &= ~(FD_MARKS);
	}
    }

#if HAVE_FFTW3_H
    //>> Calc vertical scale for main and individual
    double curVl, vsMax = -3e300, vsMin = 3e300;//Trend's vertical scale border
    bool   vsPerc = true;			//Vertical scale percent mode
    bool   isScale = (fabs(sclVerSclOff) > 1 || fabs(sclVerScl-100) > 1);

    //>> Get main scale for non individual parameters
    int prmInGrp = 0, prmGrpLast = -1;
    for(unsigned i_p = 0, mainPerc = false; i_p < trnds.size(); i_p++)
    {
	TrendObj &cP = trnds[i_p];
	if(!cP.fftN || ((cP.color()>>31)&0x01))	continue;

	cP.adjU = -3e300; cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL())
	{
	    //>>> Calc value borders
	    double vlOff = cP.fftOut[0][0]/cP.fftN;
	    for(int i_v = 1; i_v < (cP.fftN/2+1); i_v++)
	    {
		curVl = vlOff + pow(pow(cP.fftOut[i_v][0],2)+pow(cP.fftOut[i_v][1],2),0.5)/(cP.fftN/2+1);
		cP.adjL = vmin(cP.adjL, curVl);
		cP.adjU = vmax(cP.adjU, curVl);
	    }
	    if(cP.adjU == cP.adjL)	{ cP.adjU += 1.0; cP.adjL -= 1.0; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001)
	    {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2;
		cP.adjU += wnt_dp/2;
	    }
	}
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclVer|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS) continue;

	//>>> Check for value border allow
	if(!mainPerc && (vsMin > vsMax || vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = i_p;
    }

    //>> Check for individual parameters and for possibility to merge it to group or create new for no group
    int prmIndiv = 0;
    int prmIndivSc = -1;
    vector<int> prmsInd;
    for(unsigned i_p = 0; i_p < trnds.size(); i_p++)
    {
	TrendObj &cP = trnds[i_p];
	cP.isIndiv = false;
	if(!cP.fftN || ((cP.color()>>31)&0x01) || !(cP.wScale&FD_GRD_MARKS)) continue;
	//>> Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (vsMin < vsMax && vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2)))
	{
	    vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = i_p;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = i_p;
	else prmsInd.push_back(i_p);
	if(isScale)     //Vertical scale and offset apply
	{
	    float vsDif = cP.adjU - cP.adjL;
	    cP.adjU += sclVerSclOff*vsDif/100;		cP.adjL += sclVerSclOff*vsDif/100;
	    cP.adjU += (sclVerScl*vsDif/100-vsDif)/2;	cP.adjL -= (sclVerScl*vsDif/100-vsDif)/2;
	}
    }
    if(prmInGrp) prmsInd.push_back(-1);
    if(prmIndivSc >= 0) prmsInd.push_back(prmIndivSc);

    //>> Final main scale adapting
    if(vsMin > vsMax)	{ vsPerc = true; vsMax = 100; vsMin = 0; }
    else vsPerc = false;
    if(isScale)	//Vertical scale and offset apply
    {
	float vsDif = vsMax - vsMin;
	vsMax += sclVerSclOff*vsDif/100;	vsMin += sclVerSclOff*vsDif/100;
	vsMax += (sclVerScl*vsDif/100-vsDif)/2;	vsMin -= (sclVerScl*vsDif/100-vsDif)/2;
    }

    //> Draw main and individual vertical scales
    double vmax_ln = tArH / ((sclVer&FD_MARKS && mrkHeight)?(2*mrkHeight):(int)(15*vmin(xSc,ySc)));
    for(unsigned i_p = 0; vmax_ln >= 2 && i_p < prmsInd.size(); i_p++)       //prmsInd[i]=-1 - for main scale
    {
	bool	vsPercT;
	char	sclVerT;
	int	clrGridT = TWEB::colorResolve(im, sclColor);
	double	vsMinT, vsMaxT;
	double	vDiv = 1;
	if(prmsInd[i_p] < 0)    //Main scale process
	{
	    //>> Draw environment
	    vsPercT = vsPerc;
	    sclVerT = sclVer;
	    clrGridT = TWEB::colorResolve(im, sclColor);
	    clr_mrk = TWEB::colorResolve(im, sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0)        //Set color for single parameter in main group
		clrGridT = clr_mrk = TWEB::colorResolve(im, trnds[prmGrpLast].color());
	    //>> Rounding
	    double v_len = vsMax - vsMin;
	    while(v_len > vmax_ln)	{ vDiv *= 10; v_len /= 10; }
	    while(v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale)	{ vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv; }
	    while(((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = vsMin; vsMaxT = vsMax;
	}
	else    //Individual scale process
	{
	    TrendObj &cP = trnds[prmsInd[i_p]];
	    //>> Draw environment
	    vsPercT = false;
	    sclVerT = cP.wScale;
	    clrGridT = clr_mrk = TWEB::colorResolve(im, cP.color());
	    //>> Rounding
	    double v_len = cP.adjU - cP.adjL;
	    while(v_len > vmax_ln)	{ vDiv *= 10; v_len /= 10; }
	    while(v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale)	{ cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv; }
	    while(((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(i_p < (prmsInd.size()-1)) sclVerT &= ~(FD_GRD);  //Hide grid for no last scale

	//>>> Draw vertical grid and markers
	int markWdth = 0;
	if(sclVerT&FD_GRD_MARKS)
	{
	    string labVal;
	    gdImageLine(im, tArX-1, tArY, tArX-1, tArH, clrGridT);
	    for(double i_v = ceil(vsMinT/vDiv)*vDiv; (vsMaxT-i_v)/vDiv > -0.1; i_v += vDiv)
	    {
		int v_pos = tArY + tArH - (int)((double)tArH*(i_v-vsMinT)/(vsMaxT-vsMinT));
		if(sclVerT&FD_GRD) gdImageLine(im, tArX, v_pos, tArX+tArW, v_pos, clr_grid);
		else gdImageLine(im, tArX-3, v_pos, tArX+3, v_pos, clrGridT);
		if(sclVerT&FD_MARKS && mrkHeight)
		{
		    bool isPerc = vsPercT && ((vsMaxT-i_v-vDiv)/vDiv <= -0.1);
		    bool isMax = (v_pos-1-mrkHeight) < tArY;
		    labVal = TSYS::strMess("%0.5g",i_v)+(isPerc?" %":"");
		    gdImageStringFTEx(im, &brect[0], clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0,
			tArX+2, v_pos-1+(isMax?mrkHeight:0), (char*)labVal.c_str(), &strex);	//!!!! Check for correct work combining mode
		    markWdth = vmax(markWdth, brect[2]-brect[6]);
		}
	    }
	}
	if(i_p < (prmsInd.size()-1)) { tArX += markWdth?(markWdth+5):0; tArW -= markWdth?(markWdth+5):0; }
    }
    clr_mrk = TWEB::colorResolve(im, sclMarkColor);
#endif

    //> Calc horizontal scale
    int fftN = (int)(width+0.5);
    fftBeg = 1e6/(double)tSz;			//Minimum frequency or maximum period time (s)
    fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
    double hDiv = 1;				//Horisontal scale divisor
    int hmax_ln = tArW / (int)((sclHor&FD_MARKS && mrkWidth)?mrkWidth:(15*vmin(xSc,ySc)));
    if(hmax_ln >= 2)
    {
	double hLen = fftEnd-fftBeg;
	while(hLen/hDiv > hmax_ln)	hDiv *= 10;
	while(hLen/hDiv < hmax_ln/10)	hDiv /= 10;
	fftBeg = floor(10*fftBeg/hDiv)*hDiv/10;
	fftEnd = ceil(10*fftEnd/hDiv)*hDiv/10;
	while(((fftEnd-fftBeg)/hDiv) < hmax_ln/2) hDiv /= 2;

	//>> Draw horisontal grid and markers
	if(sclHor&FD_GRD_MARKS)
	{
	    string labH;
	    double labDiv = 1;
	    if(fftEnd > 1000) labDiv = 1000;
	    //>>> Draw generic grid line
	    gdImageLine(im, tArX, tArY+tArH, tArX+tArW, tArY+tArH, clr_grid);
	    //>>> Draw full trend's data and time to the trend end position
	    int begMarkBrd = -5;
	    int endMarkBrd = tArX + tArW;
	    if(sclHor&FD_MARKS && mrkHeight)
	    {
		labH = TSYS::strMess("%0.5g",fftEnd/labDiv) + Mess->codeConvOut("UTF-8",(labDiv==1000)?_("kHz"):_("Hz"));
		gdImageStringFTEx(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)labH.c_str(), &strex);
		int markBrd = tArX + tArW - (brect[2]-brect[6]);
		endMarkBrd = vmin(endMarkBrd, markBrd);
		gdImageStringFTEx(im, NULL, clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0, markBrd, tArY+tArH+3+(brect[3]-brect[7]), (char*)labH.c_str(), &strex);
	    }
	    //>>> Draw grid and/or markers
	    for(double i_h = fftBeg; (fftEnd-i_h)/hDiv > -0.1; i_h += hDiv)
	    {
		//>>>> Draw grid
		int h_pos = tArX + (int)((double)tArW*(i_h-fftBeg)/(fftEnd-fftBeg));
		if(sclHor&FD_GRD) gdImageLine(im, h_pos, tArY, h_pos, tArY+tArH, clr_grid);
		else gdImageLine(im, h_pos, tArY+tArH-3, h_pos, tArY+tArH+3, clr_grid);

		if(sclHor&FD_MARKS && mrkHeight)
		{
		    labH = TSYS::strMess("%0.5g", i_h/labDiv);
		    gdImageStringFTEx(NULL, &brect[0], 0, (char*)sclMarkFont.c_str(), mrkFontSize, 0, 0, 0, (char*)labH.c_str(), &strex);
		    int wdth = brect[2]-brect[6];
		    int tpos = vmax(h_pos-wdth/2,0);
		    if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3))
			gdImageStringFTEx(im, NULL, clr_mrk, (char*)sclMarkFont.c_str(), mrkFontSize, 0, tpos, tArY+tArH+3+(brect[3]-brect[7]), (char*)labH.c_str(), &strex);
		    begMarkBrd = vmax(begMarkBrd, tpos+wdth);
		}
	    }
	}
    }

#if HAVE_FFTW3_H
    //>> Draw trends trnds[i_t];
    for(unsigned i_t = 0; i_t < trnds.size(); i_t++)
    {
	TrendObj &cP = trnds[i_t];
	if(!cP.fftN || (cP.color()>>31)&0x01) continue;

	//>> Set trend's pen
	gdImageSetThickness(im,vmax(1,vmin(10,(int)TSYS::realRound(cP.width()*vmin(xSc,ySc)))));
	int clr_t = TWEB::colorResolve(im, cP.color());
	//gdImageColorAllocate(im,(uint8_t)(cP.color()>>16),(uint8_t)(cP.color()>>8),(uint8_t)cP.color());
	double vlOff = cP.fftOut[0][0]/cP.fftN;
	double fftDt = (1e6/(double)tSz)*(double)width/cP.fftN;

	bool vsPercT = cP.isIndiv ? false : vsPerc;
	double vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	double vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	//>>> Prepare border for percent trend
	double bordL = cP.bordL();
	double bordU = cP.bordU();
	if(vsPercT && bordL >= bordU)
	{
	    bordU = -3e300, bordL = 3e300;
	    for(int i_v = 1; i_v < (cP.fftN/2+1); i_v++)
	    {
		curVl = vlOff + pow(pow(cP.fftOut[i_v][0],2)+pow(cP.fftOut[i_v][1],2),0.5)/(cP.fftN/2+1);
		bordL = vmin(bordL, curVl);
		bordU = vmax(bordU, curVl);
	    }
	    double vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//>>> Draw trend
	double prevVl = EVAL_REAL;
	int curPos = 0, prevPos = 0;
	for(int i_v = 1; i_v < (cP.fftN/2+1); i_v++)
	{
	    curVl = vlOff + pow(pow(cP.fftOut[i_v][0],2)+pow(cP.fftOut[i_v][1],2),0.5)/(cP.fftN/2+1);
	    if(vsPercT) curVl = 100*(curVl-bordL)/(bordU-bordL);
	    curPos = tArX + (int)((double)tArW*(fftDt*i_v-fftBeg)/(fftEnd-fftBeg));

	    int c_vpos = tArY + tArH - (int)((double)tArH*vmax(0,vmin(1,(curVl-vsMinT)/(vsMaxT-vsMinT))));
	    if(prevVl == EVAL_REAL) gdImageSetPixel(im, curPos, c_vpos, clr_t);
	    else
	    {
		int c_vpos_prv = tArY + tArH - (int)((double)tArH*vmax(0,vmin(1,(prevVl-vsMinT)/(vsMaxT-vsMinT))));
		gdImageLine(im, prevPos, c_vpos_prv, curPos, c_vpos, clr_t);
	    }
	    prevPos = curPos;
	    prevVl = curVl;
	}

	//>>> Update value on cursor
	if(active)
	{
	    double curFrq = vmax(vmin(1e6/(double)curTime,fftEnd),fftBeg);
	    curPos = (int)(curFrq/fftDt);
	    if(curPos >= 1 && curPos < (cP.fftN/2+1))
	    {
		double val = cP.fftOut[0][0]/cP.fftN + pow(pow(cP.fftOut[curPos][0],2)+pow(cP.fftOut[curPos][1],2),0.5)/(cP.fftN/2+1);
		XMLNode req("set");
		req.setAttr("path",path()+"/%2fserv%2fattr")->
		    childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",i_t))->setText(r2s(val,6));
		mod->cntrIfCmd(req,ses.user);
	    }
	}
    }
#endif

    //> Draw cursor
    if(active && tSz)
    {
	float curFrq = vmax(vmin(1e6/(float)curTime,fftEnd), fftBeg);
	int curPos = tArX + (int)(tArW*(curFrq-fftBeg)/(fftEnd-fftBeg));
	int clr_cur = TWEB::colorResolve(im, curColor);
	//gdImageColorAllocate(im,(uint8_t)(curColor>>16),(uint8_t)(curColor>>8),(uint8_t)curColor);
	gdImageLine(im, curPos, tArY, curPos, tArY+tArH, clr_cur);
    }

    //> Get image and transfer it
    makeImgPng(ses, im);
}

void VCADiagram::postReq( SSess &ses )
{
    ResAlloc res(mRes, true);

    map<string, string>::iterator prmEl = ses.prm.find("sub");
    if(prmEl != ses.prm.end() && prmEl->second == "point");
    {
	prmEl = ses.prm.find("x");
	int x_coord = (prmEl!=ses.prm.end()) ? s2i(prmEl->second) : 0;
	if(x_coord >= tArX && x_coord <= (tArX+tArW))
	    switch(type)
	    {
		case FD_TRND:
		{
		    int64_t tTimeGrnd = tPict - (int64_t)(1e6*tSize);
		    setCursor(tTimeGrnd + (tPict-tTimeGrnd)*(x_coord-tArX)/tArW, ses.user);
		    break;
		}
		case FD_SPECTR:
		    setCursor((int64_t)(1e6/(fftBeg+(fftEnd-fftBeg)*(x_coord-tArX)/tArW)), ses.user);
		    break;
	    }
    }
}

void VCADiagram::setAttrs( XMLNode &node, const string &user )
{
    ResAlloc res(mRes, true);

    int  reld_tr_dt = 0;	//Reload trend's data (1-reload addons, 2-full reload)

    XMLNode *req_el;
    for(unsigned i_a = 0; i_a < node.childSize(); i_a++)
    {
	req_el = node.childGet(i_a);
	if(req_el->name() != "el") continue;
	int uiPrmPos = s2i(req_el->attr("p"));
	switch(uiPrmPos)
	{
	    case A_ACTIVE: active = (bool)s2i(req_el->text());		break;
	    case A_GEOM_W: width = (int)(s2r(req_el->text())+0.5);	break;
	    case A_GEOM_H: height = (int)(s2r(req_el->text())+0.5);	break;
	    case A_GEOM_MARGIN: geomMargin = s2i(req_el->text());	break;
	    case A_BordWidth: bordWidth = s2i(req_el->text());		break;
	    case A_DiagramTrcPer: trcPer = s2i(req_el->text());	break;
	    case A_DiagramType: type = s2i(req_el->text()); reld_tr_dt = 2;	break;
	    case A_DiagramTSek:
		tTimeCurent = false;
		if(s2ll(req_el->text()) == 0)
		{
		    tTime = (int64_t)time(NULL)*1000000;
		    tTimeCurent = true;
		} else tTime = s2ll(req_el->text())*1000000 + tTime%1000000;
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case A_DiagramTUSek:
		tTime = 1000000ll*(tTime/1000000)+s2ll(req_el->text());
		lstTrc = time(NULL);
		reld_tr_dt = 1;
		break;
	    case A_DiagramTSize: tSize = vmax(1e-3,s2r(req_el->text())); reld_tr_dt = 2;	break;
	    case A_DiagramCurSek:
		if((curTime/1000000) == s2i(req_el->text())) break;
		curTime = s2ll(req_el->text())*1000000 + curTime%1000000;
		holdCur = (curTime>=tTime);
		setCursor(curTime, user);
		break;
	    case A_DiagramCurUSek:
		if((curTime%1000000) == s2i(req_el->text())) break;
		curTime = 1000000ll*(curTime/1000000)+s2ll(req_el->text());
		holdCur = (curTime>=tTime);
		setCursor(curTime, user);
		break;
	    case A_DiagramCurColor: curColor = mod->colorParse(req_el->text());		break;
	    case A_DiagramSclColor: sclColor = mod->colorParse(req_el->text());		break;
	    case A_DiagramSclHor: sclHor = s2i(req_el->text());		break;
	    case A_DiagramSclVer: sclVer = s2i(req_el->text());		break;
	    case A_DiagramSclMarkColor: sclMarkColor = mod->colorParse(req_el->text());	break;
	    case A_DiagramSclMarkFont:
	    {
		char family[101]; strcpy(family,"Arial");
		int bold = 0, italic = 0;
		sclMarkFontSize = 10;
		sscanf(req_el->text().c_str(),"%100s %d %d %d",family,&sclMarkFontSize,&bold,&italic);
		sclMarkFont = family;
		for(unsigned p = 0; p < sclMarkFont.size(); p++) if(sclMarkFont[p] == '_') sclMarkFont[p] = ' ';
		if(bold) sclMarkFont += ":bold";
		if(italic) sclMarkFont += ":italic";
		//> Font size correct
		/*int brect[8];
		gdImageStringFT(NULL,&brect[0],0,(char*)sclMarkFont.c_str(),sclMarkFontSize,0.,0,0,"Test");
		if( (brect[3]-brect[7]) > sclMarkFontSize )
		    sclMarkFontSize = (int)((float)sclMarkFontSize*((float)sclMarkFontSize/(float)(brect[3]-brect[7])));*/
		break;
	    }
	    case A_DiagramValArch: valArch = req_el->text(); reld_tr_dt = 2;		break;
	    case A_DiagramParNum:
	    {
		unsigned parNum = s2i(req_el->text());
		if(parNum == trnds.size())	break;
		while(trnds.size() > parNum)	trnds.pop_back();
		while(parNum > trnds.size())	trnds.push_back(TrendObj(this));
		break;
	    }
	    case A_DiagramSclVerScl: sclVerScl = s2r(req_el->text());		break;
	    case A_DiagramSclVerSclOff: sclVerSclOff = s2r(req_el->text());	break;
	    case A_DiagramSclHorPer: sclHorPer = vmax(0,s2r(req_el->text()))*1e6;	break;
	    default:
		//> Individual trend's attributes process
		if(uiPrmPos >= A_DiagramTrs)
		{
		    unsigned trndN = (uiPrmPos-A_DiagramTrs)/A_DiagramTrsSz;
		    if(trndN >= trnds.size())	break;
		    switch(uiPrmPos%A_DiagramTrsSz)
		    {
			case A_DiagramTrAddr: trnds[trndN].setAddr(req_el->text());		break;
			case A_DiagramTrBordL: trnds[trndN].setBordL(s2r(req_el->text()));	break;
			case A_DiagramTrBordU: trnds[trndN].setBordU(s2r(req_el->text()));	break;
			case A_DiagramTrClr: trnds[trndN].setColor(mod->colorParse(req_el->text()));	break;
			case A_DiagramTrVal:
			    trnds[trndN].setCurVal((req_el->text()==EVAL_STR) ? EVAL_REAL : s2r(req_el->text()));
			    break;
			case A_DiagramTrScl: trnds[trndN].setScale(s2i(req_el->text()));	break;
			case A_DiagramTrWdth: trnds[trndN].setWidth(s2i(req_el->text()));	break;
		    }
		}
	}
    }

    if(reld_tr_dt)
    {
	XMLNode req("set");
	req.setAttr("path", path()+"/%2fserv%2fattr");
	for(unsigned i_p = 0; i_p < trnds.size(); i_p++)
	{
	    trnds[i_p].loadData(user, reld_tr_dt==2);
	    if(trnds[i_p].arh_beg && trnds[i_p].arh_end)
		req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dprop",i_p))->
		    setText(TSYS::strMess("%.15g:%.15g:%.15g",
			(double)trnds[i_p].arh_beg*1e-6,(double)trnds[i_p].arh_end*1e-6,(double)trnds[i_p].arh_per*1e-6));
	}
	if(req.childSize()) mod->cntrIfCmd(req,user);
    }
}

void VCADiagram::setCursor( int64_t itm, const string& user )
{
    if(type == FD_TRND)
    {
	int64_t tTimeGrnd = tTime - (int64_t)(1e6*tSize);
	curTime = vmax(vmin(itm,tTime),tTimeGrnd);

	holdCur = (curTime==tTime);

	XMLNode req("set");
	req.setAttr("path", path()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(i2s(curTime/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(i2s(curTime%1000000));

	//> Update trend's current values
	for(unsigned i_p = 0; i_p < trnds.size(); i_p++)
	{
	    int vpos = trnds[i_p].val(curTime);
	    double val = EVAL_REAL;
	    if( !(!trnds[i_p].val().size() || curTime < trnds[i_p].valBeg() || (!tTimeCurent && vpos >= (int)trnds[i_p].val().size())) )
	    {
		vpos = vmax(0,vmin((int)trnds[i_p].val().size()-1,vpos));
		if( vpos && trnds[i_p].val()[vpos].tm > curTime ) vpos--;
		val = trnds[i_p].val()[vpos].val;
	    }
	    if( val != trnds[i_p].curVal() )
		req.childAdd("el")->setAttr("id","prm"+i2s(i_p)+"val")->setText(r2s(val,6));
	}
	mod->cntrIfCmd(req,user);
    }
    else if(type == FD_SPECTR)
    {
	float curFrq = vmax(vmin(1e6/(float)itm,fftEnd),fftBeg);

	XMLNode req("set");
	req.setAttr("path", path()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(i2s(((int64_t)(1e6/curFrq))/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(i2s(((int64_t)(1e6/curFrq))%1000000));

#if HAVE_FFTW3_H
	//> Update trend's current values
	for(unsigned i_p = 0; i_p < trnds.size(); i_p++)
	{
	    if( !trnds[i_p].fftN ) continue;
	    float fftDt = (1/tSize)*(float)width/trnds[i_p].fftN;
	    int vpos = (int)(curFrq/fftDt);
	    double val = EVAL_REAL;
	    if( vpos >= 1 && vpos < (trnds[i_p].fftN/2+1) )
		val = trnds[i_p].fftOut[0][0]/trnds[i_p].fftN +
		    pow(pow(trnds[i_p].fftOut[vpos][0],2)+pow(trnds[i_p].fftOut[vpos][1],2),0.5)/(trnds[i_p].fftN/2+1);
	    req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",i_p))->setText(r2s(val,6));
	}
#endif
	mod->cntrIfCmd(req,user);
    }
}

//* Trend object's class			 *
//************************************************
VCADiagram::TrendObj::TrendObj( VCADiagram *iowner ) :
#if HAVE_FFTW3_H
    fftN(0), fftOut(NULL),
#endif
    mBordLow(0), mBordUp(0), mCurvl(EVAL_REAL), mWidth(1),
    arh_per(0), arh_beg(0), arh_end(0), val_tp(0), m_owner(iowner)
{
    loadData("root");
}

VCADiagram::TrendObj::~TrendObj( )
{
#if HAVE_FFTW3_H
    if(fftOut) { delete fftOut; fftN = 0; }
#endif
}

VCADiagram &VCADiagram::TrendObj::owner( )	{ return *m_owner; }

int64_t VCADiagram::TrendObj::valBeg( )		{ return vals.empty() ? 0 : vals[0].tm; }

int64_t VCADiagram::TrendObj::valEnd( )		{ return vals.empty() ? 0 : vals[vals.size()-1].tm; }

int VCADiagram::TrendObj::val( int64_t tm )
{
    unsigned i_p = 0;
    for(unsigned d_win = vals.size()/2; d_win > 10; d_win /= 2)
	if(tm > vals[i_p+d_win].tm) i_p += d_win;
    for( ; i_p < vals.size(); i_p++)
	if(vals[i_p].tm >= tm) return i_p;
    return vals.size();
}

void VCADiagram::TrendObj::setAddr( const string &vl )
{
    if(vl == mAddr) return;
    mAddr = vl;
    loadData("root", true);
}

void VCADiagram::TrendObj::loadData( const string &user, bool full )
{
    switch(owner().type)
    {
	case FD_TRND: loadTrendsData(user,full);	break;
	case FD_SPECTR:	loadSpectrumData(user,full);	break;
    }
}

void VCADiagram::TrendObj::loadTrendsData( const string &user, bool full )
{
    int64_t tSize	= (int64_t)(1e6*owner().tSize);
    int64_t tTime	= owner().tTime;
    int64_t tTimeGrnd	= tTime - tSize;
    int64_t wantPer	= tSize/(int)(owner().width+0.5);
    unsigned bufLim	= 2*owner().width;
    string arch = owner().valArch;
    XMLNode req("get");

    //> Clear trend for empty address and the full reload data
    if(full || addr().empty())
    {
	arh_per = arh_beg = arh_end = 0;
	val_tp = TFld::Boolean;
	vals.clear();
	if(addr().empty()) return;
    }

    bool isDataDir = (addr().compare(0,5,"data:") == 0 || addr().compare(0,5,"line:") == 0);

    if(!isDataDir)      // From archive by address
    {
	//> Get archive parameters
	if(!arh_per || tTime > arh_end)
	{
	    XMLNode req("info");
	    req.setAttr("arch",arch)->setAttr("path",addr()+"/%2fserv%2fval");
	    if(mod->cntrIfCmd(req,user,false) || (val_tp=s2i(req.attr("vtp"))) == TFld::String || val_tp == TFld::Object)
	    { arh_per = arh_beg = arh_end = 0; return; }
	    else
	    {
		val_tp  = s2i(req.attr("vtp"));
		arh_beg = s2ll(req.attr("beg"));
		arh_end = s2ll(req.attr("end"));
		arh_per = s2ll(req.attr("per"));
	    }
	}

	//> One request check and prepare
	int trcPer = owner().trcPer*1000000;
	if(owner().tTimeCurent && trcPer && owner().valArch.empty() &&
	    (!arh_per || (vmax(arh_per,wantPer) >= trcPer && (tTime-valEnd())/vmax(arh_per,vmax(wantPer,trcPer)) < 2)))
	{
	    XMLNode req("get");
	    req.setAttr("path",addr()+"/%2fserv%2fval")->
		setAttr("tm",ll2s(tTime))->
		setAttr("tm_grnd","0");
	    if(mod->cntrIfCmd(req,user,false)) return;

	    int64_t lst_tm = (s2ll(req.attr("tm"))/wantPer)*wantPer;
	    if(lst_tm >= valEnd())
	    {
		double curVal = (req.text() == EVAL_STR) ? EVAL_REAL : s2r(req.text());
		if((val_tp == TFld::Boolean && curVal == EVAL_BOOL) || (val_tp == TFld::Integer && curVal == EVAL_INT) || isinf(curVal))
		    curVal = EVAL_REAL;
		if(valEnd() && (lst_tm-valEnd())/vmax(wantPer,trcPer) > 2) vals.push_back(SHg(lst_tm-trcPer,EVAL_REAL));
		else if((lst_tm-valEnd()) >= wantPer) vals.push_back(SHg(lst_tm,curVal));
		else if((lst_tm == valEnd() && curVal != EVAL_REAL) || vals[vals.size()-1].val == EVAL_REAL) vals[vals.size()-1].val = curVal;
		else if(curVal != EVAL_REAL)
		{
		    int s_k = lst_tm-wantPer*(lst_tm/wantPer), n_k = trcPer;
		    vals[vals.size()-1].val = (vals[vals.size()-1].val*s_k+curVal*n_k)/(s_k+n_k);
		}
		while(vals.size() > bufLim) vals.pop_front();
	    }
	    return;
	}
    }
    else	//Data direct into address field by searilised XML string or horizontal line
	try
	{
	    if(addr().compare(0,5,"data:") == 0) req.load(addr().substr(5));
	    else if(addr().compare(0,5,"line:") == 0)
		req.setAttr("vtp", i2s(TFld::Real))->
		    setAttr("tm", ll2s(tTime))->
		    setAttr("tm_grnd", ll2s(tTimeGrnd))->
		    setAttr("per", ll2s(wantPer))->
		    setText("0 "+addr().substr(5));

	    val_tp  = req.attr("vtp").size() ? s2i(req.attr("vtp")) : TFld::Real;
	    arh_beg = s2ll(req.attr("tm_grnd"));
	    arh_end = s2ll(req.attr("tm"));
            arh_per = s2ll(req.attr("per"));
        }
        catch(TError) { arh_per = arh_beg = arh_end = 0; return; }

    if(!arh_per) return;

    //> Correct request to archive border
    wantPer	= (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime	= vmin(tTime, arh_end);
    tTimeGrnd	= vmax(tTimeGrnd, arh_beg);	//For prevent possible cycling

    //> Clear data at time error
    if(tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer) vals.clear();
    if(tTime <= tTimeGrnd) return;

    //> Check for request to present in buffer data
    if(tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer) return;

    //> Correcting request to present data
    if(valEnd() && tTime > valEnd())		tTimeGrnd = valEnd()+wantPer;//1;
    else if(valBeg() && tTimeGrnd < valBeg())	tTime = valBeg()-wantPer;//1;

    //> Get values data
    int64_t	bbeg, bend, bper, bbeg_prev = tTime;
    int		curPos, prevPos, maxPos;
    double      curVal = EVAL_REAL, prevVal;
    string      svl, curValS;
    vector<SHg> buf;
    bool	toEnd = (tTimeGrnd >= valEnd());
    int		endBlks = 0;

    m1:
    if(!isDataDir)
    {
	req.clear()->
	    setAttr("arch", arch)->
	    setAttr("path", addr()+"/%2fserv%2fval")->
	    setAttr("tm", ll2s(tTime))->
	    setAttr("tm_grnd", ll2s(tTimeGrnd))->
	    setAttr("per", ll2s(wantPer))->
	    setAttr("mode", "1")->
	    setAttr("real_prec", "6")->
	    setAttr("round_perc", "0");//r2s(100/(float)owner().height));

	if(mod->cntrIfCmd(req,user,false)) return;
    }

    //> Get data buffer parameters
    bbeg = s2ll(req.attr("tm_grnd"));
    bend = s2ll(req.attr("tm"));
    bper = s2ll(req.attr("per"));

    if(bbeg <= 0 || bend <= 0 || bper <= 0 || bbeg > bend || req.text().empty()) return;

    prevPos = 0, prevVal = EVAL_REAL, maxPos = (bend-bbeg)/bper;
    buf.clear();
    for(int v_off = 0, var_off = 0; true; )
    {
	if((svl=TSYS::strLine(req.text(),0,&v_off)).size())
	{
	    var_off = 0;
	    curPos = s2i(TSYS::strParse(svl,0," ",&var_off,true));
	    curVal = s2r((curValS=TSYS::strParse(svl,0," ",&var_off,true)));
	    if(curValS == EVAL_STR || (val_tp == TFld::Boolean && curVal == EVAL_BOOL) ||
				      (val_tp == TFld::Integer && curVal == EVAL_INT) || isinf(curVal))
		curVal = EVAL_REAL;
	}
	else curPos = maxPos+1;
	if(curPos < 0 || curPos > (maxPos+1)) break;	//Out of range exit
	for( ; prevPos < curPos; prevPos++) buf.push_back(SHg(bbeg+prevPos*bper,prevVal));
	if(prevPos > maxPos) break;	//Normal exit
	prevVal = curVal;
    }

    //> Append buffer to values deque
    if(toEnd)
    {
	vals.insert(vals.end()-endBlks, buf.begin(), buf.end());
	while(vals.size() > bufLim) vals.pop_front();
	endBlks += buf.size();
    }
    else
    {
	vals.insert(vals.begin(), buf.begin(), buf.end());
	while(vals.size() > bufLim) vals.pop_back();
    }

    //> Check for archive jump
    if(!isDataDir && arch.empty() && (bbeg-tTimeGrnd)/bper && bper < bbeg_prev)
    {
	bbeg_prev = bper;
	tTime = bbeg-bper;
	goto m1;
    }
}

void VCADiagram::TrendObj::loadSpectrumData( const string &user, bool full )
{
    loadTrendsData(user, full);

    if(!valBeg() || !valEnd()) return;

#if HAVE_FFTW3_H
    if(fftOut) { delete fftOut; fftN = 0; }

    int64_t tSize	= (int64_t)(1e6*owner().tSize);
    int64_t tTime	= owner().tTime;
    int64_t tTimeGrnd	= tTime - tSize;
    int64_t workPer	= tSize/(int)(owner().width+0.5);

    tTimeGrnd = vmax(tTimeGrnd,valBeg());
    tTime = vmin(tTime,valEnd());

    if((fftN=vmax(0,(tTime-tTimeGrnd)/workPer)) == 0) return;
    double fftIn[fftN];
    fftOut = (fftw_complex*)malloc(sizeof(fftw_complex)*(fftN/2+1));

    int fftFirstPos = -1, fftLstPos = -1;
    for(unsigned a_pos = val(tTimeGrnd); a_pos < val().size() && val()[a_pos].tm <= tTime; a_pos++)
    {
	int fftPos = (val()[a_pos].tm-tTimeGrnd)/workPer;
	if(fftPos >= fftN) break;
	if(val()[a_pos].val == EVAL_REAL) continue;
	if(fftFirstPos < 0) fftFirstPos = fftPos;

	if(fftLstPos == fftPos) fftIn[fftPos-fftFirstPos] = (fftIn[fftPos-fftFirstPos]+val()[a_pos].val)/2;
	else fftIn[fftPos-fftFirstPos] = val()[a_pos].val;

	for( ; fftLstPos >= 0 && (fftLstPos+1) < fftPos; fftLstPos++)
	    fftIn[fftLstPos-fftFirstPos+1] = fftIn[fftLstPos-fftFirstPos];
	fftLstPos = fftPos;
    }

    fftN = fftLstPos-fftFirstPos;
    if(fftN < 20) { delete fftOut; fftOut = NULL; fftN = 0; return; }

    fftw_plan p = fftw_plan_dft_r2c_1d(fftN, fftIn, fftOut, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
#endif
}

//*************************************************
//* VCADocument				   *
//*************************************************
VCADocument::VCADocument( const string &iid ) : VCAObj(iid)
{

}

void VCADocument::setAttrs( XMLNode &node, const string &user )
{
    for(unsigned i_a = 0; i_a < node.childSize(); i_a++)
    {
	XMLNode *req_el = node.childGet(i_a);
	if(req_el->name() != "el")	continue;
	switch(s2i(req_el->attr("p")))
	{
	    case A_DocTmpl: case A_DocDoc:
	    {
		if(TSYS::strNoSpace(req_el->text()).empty())	break;
		const char *XHTML_entity =
		    "<!DOCTYPE xhtml [\n"
		    "  <!ENTITY nbsp \"&#160;\" >\n"
		    "]>\n";
		XMLNode xproc("body");
		try
		{
		    xproc.load(string(XHTML_entity)+req_el->text(), true, Mess->charset());
		    req_el->setText(xproc.save(XMLNode::Clean, Mess->charset()));
		}
		catch(TError err)
		{ mess_err(mod->nodePath().c_str(),_("Document '%s' parsing is error: %s"),path().c_str(),err.mess.c_str()); }
		break;
	    }
	}
    }
}


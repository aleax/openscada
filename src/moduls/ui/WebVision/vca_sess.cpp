
//OpenSCADA system module UI.WebVision file: vca_sess.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
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

#include <tsys.h>


#include "web_vision.h"
#include "vca_sess.h"

using namespace WebVision;
						    
//*************************************************
//* VCASess                                       *
//*************************************************
VCASess::VCASess( const string &iid ) : m_id(iid)
{
    lst_ses_req = time(NULL);
    id_objs	= grpAdd("obj_");
}

void VCASess::postDisable( int flag )
{
    TCntrNode::postDisable(flag);

    //- Disconnect/delete session -
    XMLNode req("disconnect");
    req.setAttr("path","/%2fserv%2f0")->setAttr("sess",id());
    mod->cntrIfCmd(req,"root");
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
	ses.page = mod->pgHead()+"<SCRIPT>\n"+mod->VCAjs+"\n</SCRIPT>\n"+mod->pgTail();
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
    else if( wp_com == "pgOpen" && first_lev.empty() )
    {
        prmEl = ses.prm.find("tm");
        XMLNode req("openlist");
        req.setAttr("path",ses.url+"/%2fserv%2f0")->
	    setAttr("tm",(prmEl!=ses.prm.end())?prmEl->second:"0");
	mod->cntrIfCmd(req,ses.user);
	ses.page = req.save();
        ses.page = mod->httpHead("200 OK",ses.page.size(),"text/xml")+ses.page;
    }
    //- Page and widget attributes request -
    else if( wp_com == "attrs" )
    {
        prmEl = ses.prm.find("tm");
        XMLNode req("get");
        req.setAttr("path",ses.url+"/%2fserv%2f0")->
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
	    req.clear()->setAttr("path",req.text()+"/%2finclwdg%2fwdg");
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
    		req.clear()->setAttr("path",ses.url+"/%2fserv%2f0");
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
    if( wp_com == "attrs" )
    {
	XMLNode req("set");
	req.load(ses.content);
	req.setAttr("path",ses.url+"/%2fserv%2f0");
	mod->cntrIfCmd(req,ses.user);
    }    
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
VCAElFigure::VCAElFigure( const string &iid ) : VCAObj(iid)
{

}

Point VCAElFigure::ROTATE(const Point pnt, double alpha)
{
    Point rotate_pnt;
    rotate_pnt=Point (pnt.x*cos((alpha*M_PI)/180)-pnt.y*sin((alpha*M_PI)/180),
                        pnt.x*sin((alpha*M_PI)/180)+pnt.y*cos((alpha*M_PI)/180));
    return  rotate_pnt;
}

//- unrotation of the point -
Point VCAElFigure::UNROTATE(const Point pnt, double alpha, double a, double b)
{
    Point unrotate_pnt;
    unrotate_pnt=Point ((pnt.x-a)*cos((alpha*M_PI)/180)-(pnt.y-b)*sin((alpha*M_PI)/180),
                           -(pnt.x-a)*sin((alpha*M_PI)/180)-(pnt.y-b)*cos((alpha*M_PI)/180));
    return  unrotate_pnt;
}

//- getting the point of the arc, using t as parameter and a,b as radiuses -
Point VCAElFigure::ARC(double t,double a,double b)
{
    Point arc_pnt;  
    arc_pnt=Point(a*cos(t*M_PI*2),-b*sin(t*M_PI*2));
    return arc_pnt;
}

double VCAElFigure::ABS(double var)
{
    if(var<0)
        return -var;
    else
        return var;
}

double VCAElFigure::Bezier_DeltaT(Point p1,Point p2, Point p3, Point p4)
{
    double t=0,dx_dt,dy_dt;
    double max=ABS(-3*pow((1-t),2)*p1.x-6*t*(1-t)*p2.x+6*t*p3.x+3*t*t*p4.x); 
    do
    {
        dx_dt=ABS(-3*pow((1-t),2)*p1.x-6*t*(1-t)*p2.x+6*t*p3.x+3*t*t*p4.x);
        if (max<dx_dt) max=dx_dt;
        dy_dt=ABS(-3*pow((1-t),2)*p1.y-6*t*(1-t)*p2.y+6*t*p3.y+3*t*t*p4.y);
        if (max<dy_dt) max=dy_dt;
        t+=0.1;
    }
    while(t<1);
    double delta=1/max;
    return delta;
}

Point VCAElFigure::Bezier(double t,Point p1,Point p2, Point p3, Point p4)
{
    Point bezier_pnt;  
    bezier_pnt=Point(pow((1-t),3)*p1.x+3*t*pow((1-t),2)*p2.x+3*t*t*(1-t)*p3.x+t*t*t*p4.x,
                     pow((1-t),3)*p1.y+3*t*pow((1-t),2)*p2.y+3*t*t*(1-t)*p3.y+t*t*t*p4.y);
    return bezier_pnt;
}


double VCAElFigure::Angle(const Point p1,const Point p2,const Point p3,const Point p4)
{
    double dx1=(p2.x-p1.x);
    double dx2=(p4.x-p3.x);
    double dy1=(p2.y-p1.y);
    double dy2=(p4.y-p3.y);
    double cos_line = (dx1*dx2 + dy1*dy2) / (Length(p1,p2)*Length(p3,p4));
    double rad = 0;
    if (cos_line >= -1.0 && cos_line <= 1.0) rad = acos( cos_line );
    return rad * 180 / M_PI;
}

double VCAElFigure::Length(const Point pt1, const Point pt2)
{
    double x = pt2.x - pt1.x;
    double y = pt2.y - pt1.y;
    return sqrt(x*x + y*y);
}

void VCAElFigure::Paint_Fill( gdImage* im, InundationItem in_item )
{
    int fill_clr=gdImageColorAllocate(im,(ui8)(in_item.P_color>>16),(ui8)(in_item.P_color>>8),(ui8)in_item.P_color);
    int white = gdImageColorAllocate(im, 255, 255, 255);  
    if(in_item.P_center.x!=0 && in_item.P_center.y!=0)
    {
        gdImageFill(im, (int) (in_item.P_center.x+0.5), (int) (in_item.P_center.y+0.5), fill_clr);
        gdImageSetPixel(im, (int) (in_item.P_center.x+0.5), (int) (in_item.P_center.y+0.5), white);
    }
}


void VCAElFigure::Paint_Figure( gdImage* im, ShapeItem item )
{
    int clr_el, clr_el_line;
    double t;
    double arc_a, arc_b, t_start, t_end,arc_a_small,arc_b_small;
    if (item.type==2)
        if(item.border_width==0)
    {
        clr_el = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
        arc_a=Length(Point(TSYS::realRound((pnts)[item.n5].x,2,true),TSYS::realRound((pnts)[item.n5].y,2,true)),Point(TSYS::realRound((pnts)[item.n3].x,2,true),TSYS::realRound((pnts)[item.n3].y,2,true)));
        arc_b=Length(Point(TSYS::realRound((pnts)[item.n3].x,2,true),TSYS::realRound((pnts)[item.n3].y,2,true)),Point(TSYS::realRound((pnts)[item.n4].x,2,true),TSYS::realRound((pnts)[item.n4].y,2,true)));
        t_start=item.ctrlPos4.x;
        t_end=item.ctrlPos4.y;
        gdImageSetThickness(im, item.width);
        t=t_start;
            do
            {
                gdImageLine(im,   (int)TSYS::realRound((pnts)[item.n3].x+ROTATE(ARC(t,arc_a,arc_b),item.ang).x,2,true),
                                (int)TSYS::realRound((pnts)[item.n3].y-ROTATE(ARC(t,arc_a,arc_b),item.ang).y,2,true),
                               (int)TSYS::realRound((pnts)[item.n3].x+ROTATE(ARC(t+0.00277777777778,arc_a,arc_b),item.ang).x,2,true),
                                (int)TSYS::realRound((pnts)[item.n3].y-ROTATE(ARC(t+0.00277777777778,arc_a,arc_b),item.ang).y,2,true),clr_el);
                t+=0.00277777777778;
            }
            while (t<t_end);
    }
    else
    {
        Point el_p1=(pnts)[item.n1];
        Point el_p2=(pnts)[item.n2];
        Point el_p3=(pnts)[item.n3];
        Point el_p4=(pnts)[item.n4];
        Point el_p5=(pnts)[item.n5];
        Point el_p6=item.ctrlPos4;
        double el_width=item.width;
        double el_ang=item.ang;
        double el_border_width=item.border_width;
        gdImageSetThickness(im, item.border_width);
        clr_el = gdImageColorAllocate(im,(ui8)(item.borderColor>>16),(ui8)(item.borderColor>>8),(ui8)item.borderColor);
        clr_el_line = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
        arc_a=Length(Point(TSYS::realRound(el_p5.x,2,true),TSYS::realRound(el_p5.y,2,true)),Point(TSYS::realRound(el_p3.x,2,true),TSYS::realRound(el_p3.y,2,true)))
                +el_width/2+el_border_width;
        arc_b=Length(Point(TSYS::realRound(el_p3.x,2,true),TSYS::realRound(el_p3.y,2,true)),Point(TSYS::realRound(el_p4.x,2,true),TSYS::realRound(el_p4.y,2,true)))+el_width/2+el_border_width;
        arc_a_small=arc_a-el_width-el_border_width;
        arc_b_small=arc_b-el_width-el_border_width;
        t_start=el_p6.x;
        t_end=el_p6.y;
        t=t_start;
        do
        {
            gdImageLine(im, (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t,arc_a,arc_b),el_ang).x, 2, true),
                               (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t,arc_a,arc_b),el_ang).y, 2, true),
                                (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t+0.00277777777778,arc_a,arc_b),el_ang).x, 2, true),
                                 (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t+0.00277777777778,arc_a,arc_b),el_ang).y, 2, true),clr_el);
            t+=0.00277777777778;
        }
        while (t<t_end);
        gdImageLine(im, (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t_end,arc_a,arc_b),el_ang).x, 2, true),
                           (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t_end,arc_a,arc_b),el_ang).y, 2, true),
                          (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).x, 2, true),
                           (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t_end,arc_a_small,arc_b_small),el_ang).y, 2, true),clr_el);
        t=t_start;
        
        do
        {
            gdImageLine(im, (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).x, 2, true),
                               (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t,arc_a_small,arc_b_small),el_ang).y, 2, true),
                                (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t+0.00277777777778,arc_a_small,arc_b_small),el_ang).x, 2, true),
                                 (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t+0.00277777777778,arc_a_small,arc_b_small),el_ang).y, 2, true),clr_el);
            t+=0.00277777777778;
        }
        while (t<t_end);
        gdImageLine(im, (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t_start,arc_a,arc_b),el_ang).x, 2, true),
                           (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t_start,arc_a,arc_b),el_ang).y, 2, true),
                            (int)TSYS::realRound(el_p3.x+ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).x, 2, true),
                             (int)TSYS::realRound(el_p3.y-ROTATE(ARC(t_start,arc_a_small,arc_b_small),el_ang).y, 2, true),clr_el);
        Point p_center=Point (TSYS::realRound(el_p3.x+ROTATE(ARC((t_end+t_start)/2,arc_a-el_width/2,arc_b-el_width/2),el_ang).x, 2, true),
                        TSYS::realRound(el_p3.y-ROTATE(ARC((t_end+t_start)/2,arc_a-el_width/2,arc_b-el_width/2),el_ang).y, 2, true));
        gdImageFill(im, (int) (p_center.x+0.5), (int) (p_center.y+0.5), clr_el_line);
    }
    if (item.type==3)
        if(item.border_width==0)
        {
            double delta=Bezier_DeltaT((pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]);
            clr_el = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
            t_start=0;
            t_end=1;
            gdImageSetThickness(im, item.width);
            t=t_start;
            do
            {
                gdImageLine(im,   (int)TSYS::realRound(Bezier(t,(pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]).x,2,true),
                        (int)TSYS::realRound(Bezier(t,(pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]).y,2,true),
                         (int)TSYS::realRound(Bezier(t+delta,(pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]).x,2,true),
                          (int)TSYS::realRound(Bezier(t+delta,(pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]).y,2,true),clr_el);
                t+=delta;
            }
            while (t<t_end);
        }
        else
        {
            double el_width=item.width;
            double el_border_width=item.border_width;
            clr_el = gdImageColorAllocate(im,(ui8)(item.borderColor>>16),(ui8)(item.borderColor>>8),(ui8)item.borderColor);
            clr_el_line = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
            Point p1=UNROTATE((pnts)[item.n1], item.ang, (pnts)[item.n1].x, (pnts)[item.n1].y);
            Point p2=UNROTATE((pnts)[item.n3], item.ang, (pnts)[item.n1].x, (pnts)[item.n1].y);
            Point p3=UNROTATE((pnts)[item.n4], item.ang, (pnts)[item.n1].x, (pnts)[item.n1].y);
            Point p4=UNROTATE((pnts)[item.n2], item.ang, (pnts)[item.n1].x, (pnts)[item.n1].y);
            double delta=Bezier_DeltaT((pnts)[item.n1],(pnts)[item.n3],(pnts)[item.n4],(pnts)[item.n2]);
            gdImageSetThickness(im, item.border_width);

            t_start=0;
            t_end=1;
            t=t_start;
            do
            {
                gdImageLine(im,(int)TSYS::realRound((pnts)[item.n1].x+ROTATE(Bezier(t,
                            Point(p1.x,p1.y+el_width/2+el_border_width/2),
                            Point(p2.x,p2.y+(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y+(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y+(el_width/2+el_border_width/2))),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-ROTATE(Bezier(t,
                            Point(p1.x,p1.y+el_width/2+el_border_width/2),
                            Point(p2.x,p2.y+(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y+(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y+(el_width/2+el_border_width/2))),item.ang).y,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].x+ROTATE(Bezier(t+delta,
                            Point(p1.x,p1.y+el_width/2+el_border_width/2),
                            Point(p2.x,p2.y+(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y+(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y+(el_width/2+el_border_width/2))),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-ROTATE(Bezier(t+delta,
                            Point(p1.x,p1.y+el_width/2+el_border_width/2),
                            Point(p2.x,p2.y+(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y+(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y+(el_width/2+el_border_width/2))),item.ang).y,2,true),clr_el);
                t+=delta;
            }
            while (t<t_end);
            t=t_start;
            do
            {
                gdImageLine(im,(int)TSYS::realRound((pnts)[item.n1].x+ROTATE(Bezier(t,
                            Point(p1.x,p1.y-(el_width/2+el_border_width/2)),
                            Point(p2.x,p2.y-(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y-(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y-(el_width/2+el_border_width/2))),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-ROTATE(Bezier(t,
                            Point(p1.x,p1.y-(el_width/2+el_border_width/2)),
                            Point(p2.x,p2.y-(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y-(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y-(el_width/2+el_border_width/2))),item.ang).y,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].x+ROTATE(Bezier(t+delta,
                            Point(p1.x,p1.y-(el_width/2+el_border_width/2)),
                            Point(p2.x,p2.y-(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y-(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y-(el_width/2+el_border_width/2))),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-ROTATE(Bezier(t+delta,
                            Point(p1.x,p1.y-(el_width/2+el_border_width/2)),
                            Point(p2.x,p2.y-(el_width/2+el_border_width/2)),
                            Point (p3.x,p3.y-(el_width/2+el_border_width/2)),
                            Point (p4.x, p4.y-(el_width/2+el_border_width/2))),item.ang).y,2,true),clr_el);
                t+=delta;
            }
            while (t<t_end);
            gdImageLine(im,(int)TSYS::realRound((pnts)[item.n1].x+
                            ROTATE(Point(p1.x,p1.y+el_width/2+el_border_width/2),item.ang).x,2,true),
                           (int)TSYS::realRound((pnts)[item.n1].y-
                            ROTATE(Point(p1.x,p1.y+el_width/2+el_border_width/2),item.ang).y,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].x+
                            ROTATE(Point(p1.x,p1.y-(el_width/2+el_border_width/2)),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-
                            ROTATE(Point(p1.x,p1.y-(el_width/2+el_border_width/2)),item.ang).y,2,true),clr_el);

            gdImageLine(im,(int)TSYS::realRound((pnts)[item.n1].x+
                           ROTATE(Point(p4.x,p4.y+el_width/2+el_border_width/2),item.ang).x,2,true),
                           (int)TSYS::realRound((pnts)[item.n1].y-
                           ROTATE(Point(p4.x,p4.y+el_width/2+el_border_width/2),item.ang).y,2,true),
                           (int)TSYS::realRound((pnts)[item.n1].x+
                           ROTATE(Point(p4.x,p4.y-(el_width/2+el_border_width/2)),item.ang).x,2,true),
                           (int)TSYS::realRound((pnts)[item.n1].y-
                           ROTATE(Point(p4.x,p4.y-(el_width/2+el_border_width/2)),item.ang).y,2,true),clr_el);
            
            
            Point p_center=Point ((int)TSYS::realRound((pnts)[item.n1].x+ROTATE(Bezier(0.5,p1,p2,p3,p4),item.ang).x,2,true),
                            (int)TSYS::realRound((pnts)[item.n1].y-ROTATE(Bezier(0.5,p1,p2,p3,p4),item.ang).y,2,true));
            gdImageFill(im, (int) (p_center.x+0.5), (int) (p_center.y+0.5), clr_el_line);
        }
    if (item.type==1)
        if(item.border_width==0)
        {
            clr_el = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
            gdImageSetThickness(im, item.width);
            gdImageLine(im,(int)TSYS::realRound((pnts)[item.n1].x,2,true),(int)TSYS::realRound((pnts)[item.n1].y,2,true),
                        (int)TSYS::realRound((pnts)[item.n2].x,2,true),(int)TSYS::realRound((pnts)[item.n2].y,2,true),clr_el);
        }
        else
        {
            clr_el = gdImageColorAllocate(im,(ui8)(item.borderColor>>16),(ui8)(item.borderColor>>8),(ui8)item.borderColor);
            clr_el_line = gdImageColorAllocate(im,(ui8)(item.lineColor>>16),(ui8)(item.lineColor>>8),(ui8)item.lineColor);
            gdImageSetThickness(im, item.border_width);
            Point el_p1=(pnts)[item.n1];
            Point el_p2=(pnts)[item.n2];
            double el_border_width=(double)item.border_width/2;
            double el_width=item.width;
            double el_ang=item.ang;
            gdImageLine(im,(int)TSYS::realRound(el_p1.x + ROTATE(Point(-el_border_width,-(el_width/2+el_border_width)),el_ang).x, 2, true),
                        (int)TSYS::realRound(el_p1.y-ROTATE(Point(-el_border_width,-(el_width/2+el_border_width)),el_ang).y, 2, true),
                         (int)TSYS::realRound(el_p1.x+ROTATE(Point(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).x, 2, true),
                          (int)TSYS::realRound(el_p1.y-ROTATE(Point(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).y, 2, true),clr_el);
            
            gdImageLine(im,(int)TSYS::realRound(el_p1.x+ROTATE(Point(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).x, 2, true),
                          (int)TSYS::realRound(el_p1.y-ROTATE(Point(Length(el_p2,el_p1)+el_border_width,-(el_width/2+el_border_width)),el_ang).y, 2, true),
                           (int)TSYS::realRound(el_p1.x+ROTATE(Point(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).x, 2, true),
                            (int)TSYS::realRound(el_p1.y-ROTATE(Point(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).y, 2, true),clr_el);
            
            
            gdImageLine(im, (int)TSYS::realRound(el_p1.x+ROTATE(Point(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).x, 2, true),
                          (int)TSYS::realRound(el_p1.y-ROTATE(Point(Length(el_p2,el_p1)+el_border_width,(el_width/2+el_border_width)),el_ang).y, 2, true),
                           (int)TSYS::realRound(el_p1.x+ROTATE(Point(-el_border_width,(el_width/2+el_border_width)),el_ang).x, 2, true),
                            (int)TSYS::realRound(el_p1.y-ROTATE(Point(-el_border_width,(el_width/2+el_border_width)),el_ang).y, 2, true),clr_el);
            
            gdImageLine(im, (int)TSYS::realRound(el_p1.x+ROTATE(Point(-el_border_width,(el_width/2+el_border_width)),el_ang).x, 2, true),
                          (int)TSYS::realRound(el_p1.y-ROTATE(Point(-el_border_width,(el_width/2+el_border_width)),el_ang).y, 2, true),
                           (int)TSYS::realRound(el_p1.x + ROTATE(Point(-el_border_width,-(el_width/2+el_border_width)),el_ang).x, 2, true),
                            (int)TSYS::realRound(el_p1.y-ROTATE(Point(-el_border_width,-(el_width/2+el_border_width)),el_ang).y, 2, true),clr_el);
            double x_center=((pnts)[item.n1].x+(pnts)[item.n2].x)/2;
            double y_center=((pnts)[item.n1].y+(pnts)[item.n2].y)/2;
            gdImageFill(im, (int) (x_center+0.5), (int) (y_center+0.5), clr_el_line);
        }
}

void VCAElFigure::getReq( SSess &ses )
{
    //- Prepare picture -
    gdImagePtr im = gdImageCreate(width,height);
    gdImageFilledRectangle(im,0,0,width-1,height-1,gdImageColorAllocateAlpha(im,0,0,0,127));
    for (int i=0; i<shapeItems.size(); i++)
        Paint_Figure(im, shapeItems[i]);
    for (int i=0; i<inundationItems.size(); i++)
        Paint_Fill(im, inundationItems[i]);
    
    //- Get image and transfer it -
    int img_sz;
    char *img_ptr = (char *)gdImagePngPtr(im, &img_sz);
    ses.page.assign(img_ptr,img_sz);
    ses.page = mod->httpHead("200 OK",ses.page.size(),"image/png")+ses.page;
    
    gdFree(img_ptr);
    gdImageDestroy(im);
}

void VCAElFigure::postReq( SSess &ses )
{

}

void VCAElFigure::setAttrs( XMLNode &node, const string &user )
{
    XMLNode *req_el;
    Point StartMotionPos;
    Point EndMotionPos;
    Point CtrlMotionPos_1;
    Point CtrlMotionPos_2;
    Point CtrlMotionPos_3;
    Point CtrlMotionPos_4;
    double t_start, t_end, a, b, ang_t, ang;
    int MotionWidth;
    //shapeItems.clear();
    rel_list=false;
    for( int i_a = 0; i_a < node.childSize(); i_a++ )
    {
	req_el = node.childGet(i_a);
	int uiPrmPos = atoi(req_el->attr("pos").c_str());
        
        //printf("PrmPos=%i\n",uiPrmPos);
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
            case 20:	//lineWdth
                lineWdth = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 21:	//lineClr
                lineClr =  mod->colorParse(req_el->text());
                rel_list = true;
                break;
            case 22:	//lineDecor
                lineDecor = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 23:	//bordWdth
                bordWdth = atoi(req_el->text().c_str());
                rel_list = true;
                break;
            case 24:	//bordClr
                bordClr = mod->colorParse(req_el->text());
                rel_list = true;
                break;
            case 25:	//fillClr
                fillClr = mod->colorParse(req_el->text());
                rel_list = true;
                break;
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
                    int pnt  = (uiPrmPos-30)/2;
                    int patr = (uiPrmPos-30)%2;
                    int pval  = atoi(req_el->text().c_str());
                    Point pnt_ = (pnts)[pnt];
                    if( patr == 0 ) pnt_.x=pval;
                    else pnt_.y=pval;
                    (pnts)[pnt] = pnt_;
                }
	}
    }
    if( rel_list)
    {
        string sel;
        int  p[5];
        int width;
        int bord_width;
        int color;
        int bord_color;
        double t_start, t_end, a, b, ang_t, ang;
        int MotionWidth;
        Point ip[5];
        shapeItems.clear();
        for( int off = 0; (sel=TSYS::strSepParse(elLst,0,'\n',&off)).size(); )
        {
            int el_off = 0;
            string el = TSYS::strSepParse(sel,0,':',&el_off);
            if( el == "line" )
            {
                //-- Reading anf setting attributes for the current line --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width=lineWdth;
                color = mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( !color) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=bordWdth;
                bord_color= mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (!bord_color)  bord_color=bordClr; 
                //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 2; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                if( ip[0].y<=ip[1].y )
                    ang=360-Angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                else
                    ang=Angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                shapeItems.push_back( ShapeItem(p[0],p[1],-1,-1,-1,Point(0,0), ang,
                                      color,bord_color,width,bord_width,1));
            }
            if( el == "arc" )
            {
                //-- Reading anf setting attributes for the current arc --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[2]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[3]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[4]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width=lineWdth;
                color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( !color ) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=bordWdth;
                bord_color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (!bord_color)  bord_color=bordClr;
                  //-- Reading coordinates for the points of the line --
                for( int i_p = 0; i_p < 5; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                StartMotionPos=ip[0];
                EndMotionPos=ip[1];
                CtrlMotionPos_1=ip[2];
                CtrlMotionPos_2=ip[3];
                CtrlMotionPos_3=ip[4];
                MotionWidth=width;
                if (CtrlMotionPos_3.y<=CtrlMotionPos_1.y) 
                    ang=Angle(CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y));
                else ang=360-Angle(CtrlMotionPos_1, CtrlMotionPos_3, CtrlMotionPos_1, Point(CtrlMotionPos_1.x+10,CtrlMotionPos_1.y));
                  
                a=Length(CtrlMotionPos_3, CtrlMotionPos_1);
                b=Length(CtrlMotionPos_2, CtrlMotionPos_1);

                CtrlMotionPos_2=Point(CtrlMotionPos_1.x+ROTATE(ARC(0.25,a,b),ang).x,
                                      CtrlMotionPos_1.y-ROTATE(ARC(0.25,a,b),ang).y);
                StartMotionPos=UNROTATE(StartMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y);
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
                EndMotionPos=UNROTATE(EndMotionPos,ang,CtrlMotionPos_1.x,CtrlMotionPos_1.y);
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


                shapeItems.push_back(ShapeItem(p[0],p[1],p[2],p[3],p[4],CtrlMotionPos_4,ang,color,bord_color,width,bord_width,2));
            }
            if( el == "bezier" )
            {
                //-- Reading anf setting attributes for the current arc --
                p[0]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[1]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[2]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                p[3]  = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                width = atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if( !width ) width=lineWdth;
                color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if( !color) color=lineClr;
                bord_width=atoi(TSYS::strSepParse(sel,0,':',&el_off).c_str());
                if (!bord_width) bord_width=bordWdth;
                bord_color =  mod->colorParse(TSYS::strSepParse(sel,0,':',&el_off));
                if (!bord_color)  bord_color=bordClr;  
                for( int i_p = 0; i_p < 4; i_p++ )
                    ip[i_p] = (pnts)[p[i_p]];
                if( ip[0].y<=ip[1].y ) ang=360-Angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                else ang=Angle(ip[0], ip[1], ip[0], Point(ip[0].x+10,ip[0].y));
                shapeItems.push_back( ShapeItem(p[0], p[1], p[2], p[3],-1,Point(0,0),ang,color,bord_color,width,bord_width,3));
            }
            
            if( el == "fill" )
            {
                int fl_pnt=atoi(TSYS::strSepParse(sel,1,':').c_str());
                int fl_color=mod->colorParse(TSYS::strSepParse(sel,2,':'));
                string fl_img=TSYS::strSepParse(sel,3,':');
                Point p_center= (pnts)[fl_pnt]; 
                inundationItems.push_back(InundationItem(p_center, fl_color));
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
	    long long tTimeGrnd = tTime - (int)(tSize*1000000.);
	    if( curTime >= (tTime-2*(long long)trcPer*1000000) || curTime <= tTimeGrnd )
		setCursor(tTime,ses.user);
        }
    }

    int mrkHeight = 0;
    int clr_grid, clr_mrk;						//Colors

    //-- Get generic parameters --
    int parNum     = trnds.size();                         		//Parameter's number
    int tSz        = (int)(tSize*1000000.);       			//Trends size (us)
    long long tEnd = tTime;                     			//Trends end point (us)
    long long tPict = tEnd;
    long long tBeg = tEnd - tSz;                                      	//Trends begin point (us)
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
    int tArX = 1, tArY = 1, 				//Curves of trends area rect
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
    long long aVend;                    //Corrected for allow data the trend end point
    long long aVbeg;                    //Corrected for allow data the trend begin point
    long long hDiv = 1, hDivBase = 1;   //Horisontal scale divisor

    int hmax_ln = tArW/((sclHor&0x2)?40:15);
    if( hmax_ln >= 2 )
    {
        int hvLev = 0;
        long long hLen = tEnd - tBeg;
        if( hLen/86400000000ll >= 2 )    { hvLev = 5; hDivBase = hDiv = 86400000000ll; } //Days
        else if( hLen/3600000000ll >= 2 ){ hvLev = 4; hDivBase = hDiv =  3600000000ll; } //Hours
        else if( hLen/60000000 >= 2 )    { hvLev = 3; hDivBase = hDiv =    60000000; }   //Minutes
        else if( hLen/1000000 >= 2 )     { hvLev = 2; hDivBase = hDiv =     1000000; }   //Seconds
        else if( hLen/1000 >= 2 )        { hvLev = 1; hDivBase = hDiv =        1000; }   //Milliseconds
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
            struct tm *ttm, ttm1;
            char lab_tm[50], lab_dt[50];
            //---- Draw generic grid line ----
	    gdImageLine(im,tArX,tArY+tArH,tArX+tArW,tArY+tArH,clr_grid);
            //---- Draw full trend's data and time to the trend end position ----
	    int begMarkBrd = -1;
            int endMarkBrd = tArX+tArW;
            if( sclHor&0x2 )
            {
                tm_t = tPict/1000000;
        	ttm = localtime(&tm_t);
		snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900);
                if( ttm->tm_sec == 0 && tPict%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm->tm_hour,ttm->tm_min);
                else if( tPict%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
                else snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%g",ttm->tm_hour,ttm->tm_min,(float)ttm->tm_sec+(float)(tPict%1000000)/1e6);
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
            	    ttm = localtime(&tm_t);
                    int chLev = -1;
                    if( !first_m )
                    {
                        if( ttm->tm_mon > ttm1.tm_mon || ttm->tm_year > ttm1.tm_year )  chLev = 5;
                        else if( ttm->tm_mday > ttm1.tm_mday )  chLev = 4;
                        else if( ttm->tm_hour > ttm1.tm_hour )  chLev = 3;
                        else if( ttm->tm_min > ttm1.tm_min )    chLev = 2;
                        else if( ttm->tm_sec > ttm1.tm_sec )    chLev = 1;
                    }
		    //Check for data present
                    lab_dt[0] = lab_tm[0] = 0;
                    if( hvLev == 5 || chLev >= 4 )                                      //Date
                	(chLev>=5 || chLev==-1) ? snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900) :
				     snprintf(lab_dt,sizeof(lab_dt),"%d",ttm->tm_mday);
                    if( (hvLev == 4 || hvLev == 3 || ttm->tm_min) && !ttm->tm_sec )     //Hours and minuts
			snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm->tm_hour,ttm->tm_min);
                    else if( (hvLev == 2 || ttm->tm_sec) && !(i_h%1000000) )            //Seconds
                	(chLev>=2 || chLev==-1) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec) :
				     snprintf(lab_tm,sizeof(lab_tm),"%ds",ttm->tm_sec);
                    else if( hvLev <= 1 || i_h%1000000 )                                //Milliseconds
                        (chLev>=2 || chLev==-1) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%g",ttm->tm_hour,ttm->tm_min,(float)ttm->tm_sec+(float)(i_h%1000000)/1e6) :
		        (chLev>=1) ? snprintf(lab_tm,sizeof(lab_tm),"%gs",(float)ttm->tm_sec+(float)(i_h%1000000)/1e6) :
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
		    memcpy((char*)&ttm1,(char*)ttm,sizeof(tm));
		    first_m = false;
		}
                //---- Next ----
		if( i_h >= tPict )       break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tPict )        i_h = tPict;
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
	    if( vsMax == -3e300 ) 	{ vsMax = 1.0; vsMin = 0.0; }
	    else if( vsMax == vsMin )   { vsMax += 1.0; vsMin -= 1.0; }
	}
        else { vsMax = trnds[0].bordU(); vsMin = trnds[0].bordL(); }
    }
    
    float vmax_ln = tArH/20;
    if( vmax_ln >= 2 )
    {
        double vDiv = 1.;
        double v_len = vsMax - vsMin;
        while(v_len > vmax_ln)  { vDiv*=10.; v_len/=10.; }
        while(v_len < vmax_ln/10){ vDiv/=10.; v_len*=10.; }
        vsMin = floor(vsMin/vDiv)*vDiv;
        vsMax = ceil(vsMax/vDiv)*vDiv;
        while(((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv/=2;

	//--- Draw vertical grid and markers ---
        if( sclVer&0x3 )
        {
            char lab_vl[50];
	    gdImageLine(im,tArX,tArY,tArX,tArH,clr_grid);
            for(double i_v = vsMin; i_v <= vsMax; i_v+=vDiv)
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
	bool end_vl = false;
        double curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
        int    curPos, averPos = 0, prevPos = 0;
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

}

void VCADiagram::setAttrs( XMLNode &node, const string &user )
{
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
                if( parNum == trnds.size() )  break;		
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
			case 0: trnds[trndN].setAddr(req_el->text());         		break;	//addr
	    		case 1: trnds[trndN].setBordL(atof(req_el->text().c_str()));  	break;	//bordL
	    		case 2: trnds[trndN].setBordU(atof(req_el->text().c_str())); 	break;	//bordU
			case 3: trnds[trndN].setColor(mod->colorParse(req_el->text()));	break;	//color
			case 4: trnds[trndN].setCurVal(atof(req_el->text().c_str())); 	break;	//value
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
    long long tTimeGrnd = tTime - (int)(tSize*1000000.);
    curTime = vmax(vmin(itm,tTime),tTimeGrnd);

    XMLNode req("set");
    req.setAttr("path",id()+"/%2fserv%2f0");
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
    int tSize   = (int)(owner().tSize*1000000.);
    long long tTime     = owner().tTime;
    long long tTimeGrnd = tTime - tSize;
    int wantPer = tSize/(int)(owner().width+0.5);
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
        req.setAttr("arch",arch)->setAttr("path",addr()+"/%2fserv%2f0");
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
        req.setAttr("path",addr()+"/%2fserv%2f0")->
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
    wantPer   = (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime     = vmin(tTime,arh_end);
    tTimeGrnd = vmax(tTimeGrnd,arh_beg);
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
    deque<SHg>::iterator bufEndOff = vals.end();
    XMLNode req("get");
    m1: req.clear()->
	    setAttr("arch",arch)->
	    setAttr("path",addr()+"/%2fserv%2f0")->
    	    setAttr("tm",TSYS::ll2str(tTime))->
            setAttr("tm_grnd",TSYS::ll2str(tTimeGrnd))->
            setAttr("per",TSYS::ll2str(wantPer))->
            setAttr("mode","1")->
            setAttr("real_prec","4")->
            setAttr("round_perc","1");
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
    if( bbeg >= valEnd() )
    {
        vals.insert(bufEndOff,buf.begin(),buf.end());
	while( vals.size() > 2000 )     vals.pop_front();
        bufEndOff = vals.end()-buf.size();
    }
    else if( bend <= valBeg() )
    {
        vals.insert(vals.begin(),buf.begin(),buf.end());
        while( vals.size() > 2000 )     vals.pop_back();
    }
    //- Check for archive jump -
    if( arch.empty() && (bbeg-tTimeGrnd)/bper ) { tTime = bbeg-bper; goto m1; }
}

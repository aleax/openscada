
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

#include <gd.h>
#include <gdfonts.h>
#include <gdfontt.h>

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
	    if( req.text() == "Diagram" )
	    {
		objAdd( new VCADiagram(ses.url) );
		new_obj = true;
	    }
	    if( new_obj )
	    {
		//-- Request new object's attributes --
    		req.clear()->setAttr("path",ses.url+"/%2fserv%2f0");
		mod->cntrIfCmd(req,ses.user);
		objAt(ses.url).at().setAttrs(req,ses.user);
	    }
	}
	if( objPresent(ses.url) )	objAt(ses.url).at().getReq(ses);
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
    if( rld ) for( int i_p = 0; i_p < trnds.size(); i_p++ ) trnds[i_p].loadData(ses.user);		        

    int mrkHeight = 0;
    int clr_grid, clr_mrk;						//Colors

    //-- Get generic parameters --
    int parNum     = trnds.size();                         		//Parameter's number
    int tSz        = (int)(tSize*1000000.);       			//Trends size (us)
    long long tEnd = tTime;                     			//Trends end point (us)
    long long tBeg = tEnd - tSz;                                      	//Trends begin point (us)
    if( !parNum || tSz <= 0 ) return;

    //- Prepare picture -
    gdImagePtr im = gdImageCreate(width,height);
    gdImageFilledRectangle(im,0,0,width-1,height-1,gdImageColorAllocateAlpha(im,0,0,0,127));

    //-- Make decoration and prepare trends area --
    int tArX = 1, tArY = 1, 				//Curves of trends area rect
	tArW = width-2*(geomMargin+bordWidth+1),
	tArH = height-2*(geomMargin+bordWidth+1);

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
		    
    int hmax_ln = tArW/30;
    if( hmax_ln >= 2 )
    {
        int hvLev = 0;
        long long hLen = tEnd - tBeg;
        if( hLen/86400000000ll > 2 )    { hvLev = 5; hDivBase = hDiv = 86400000000ll; } //Days
        else if( hLen/3600000000ll > 2 ){ hvLev = 4; hDivBase = hDiv =  3600000000ll; } //Hours
        else if( hLen/60000000 > 2 )    { hvLev = 3; hDivBase = hDiv =    60000000; }   //Minutes
        else if( hLen/1000000 > 2 )     { hvLev = 2; hDivBase = hDiv =     1000000; }   //Seconds
        else if( hLen/1000 > 2 )        { hvLev = 1; hDivBase = hDiv =        1000; }   //Milliseconds
        while( hLen/hDiv > hmax_ln )    hDiv *= 10;
        while( hLen/hDiv < hmax_ln/2 && !((hDiv/2)%hDivBase) )  hDiv/=2;
        if( hmax_ln >= 4 && trcPer )
        {
            tEnd = hDiv*(tEnd/hDiv+1);
            tBeg = tEnd-hLen;
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
            int endMarkBrd = tArX+tArW;
            if( sclHor&0x2 )
            {
                tm_t = tEnd/1000000;
        	ttm = localtime(&tm_t);
		snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900);
                if( ttm->tm_sec == 0 && tEnd%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm->tm_hour,ttm->tm_min);
                else if( tEnd%1000000 == 0 )
		    snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec);
                else snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d.%d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec,tEnd%1000000);
		gdImageString(im,gdFontTiny,tArX+tArW-gdFontTiny->w*strlen(lab_dt),tArY+tArH+3+gdFontTiny->h,(unsigned char *)lab_dt,clr_mrk);
		gdImageString(im,gdFontTiny,tArX+tArW-gdFontTiny->w*strlen(lab_tm),tArY+tArH+3,(unsigned char *)lab_tm,clr_mrk);
		endMarkBrd = vmin(tArX+tArW-gdFontTiny->w*strlen(lab_dt),tArX+tArW-gdFontTiny->w*strlen(lab_tm));
            }
	    
            //---- Draw grid and/or markers ----
            bool first_m = true;
            for( long long i_h = tBeg; true; )
            {
                //---- Draw grid ----
                int h_pos = tArX+tArW*(i_h-tBeg)/(tEnd-tBeg);
                if( sclHor&0x1 ) gdImageLine(im,h_pos,tArY,h_pos,tArY+tArH,clr_grid);
                else gdImageLine(im,h_pos,tArY+tArH-3,h_pos,tArY+tArH+3,clr_grid);
		//---- Draw markers ----
                if( sclHor&0x2 && !(i_h%hDiv) && i_h != tEnd )
                {
                    tm_t = i_h/1000000;
            	    ttm = localtime(&tm_t);
                    int chLev = 0;
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
                	(chLev>=5) ? snprintf(lab_dt,sizeof(lab_dt),"%d-%02d-%d",ttm->tm_mday,ttm->tm_mon+1,ttm->tm_year+1900) :
				     snprintf(lab_dt,sizeof(lab_dt),"%d",ttm->tm_mday);
                    if( (hvLev == 4 || hvLev == 3 || ttm->tm_min) && !ttm->tm_sec )     //Hours and minuts
			snprintf(lab_tm,sizeof(lab_tm),"%d:%02d",ttm->tm_hour,ttm->tm_min);
                    else if( (hvLev == 2 || ttm->tm_sec) && !(i_h%1000000) )            //Seconds
                	(chLev>=2) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec) :
				     snprintf(lab_tm,sizeof(lab_tm),"%ds",ttm->tm_sec);
                    else if( hvLev <= 1 || i_h%1000000 )                                //Milliseconds
                        (chLev>=2) ? snprintf(lab_tm,sizeof(lab_tm),"%d:%02d:%02d.%d",ttm->tm_hour,ttm->tm_min,ttm->tm_sec,i_h%1000000) :
		        (chLev>=1) ? snprintf(lab_tm,sizeof(lab_tm),"%d.%ds",ttm->tm_sec,i_h%1000000) :
				     snprintf(lab_tm,sizeof(lab_tm),"%gms",(double)(i_h%1000000)/1000.);
                    int wdth;
                    if( lab_tm[0] )
                    {
			wdth = gdFontTiny->w*strlen(lab_tm)/2;
			if( (h_pos+wdth) < endMarkBrd )
			    gdImageString(im,gdFontTiny,vmax(h_pos-wdth,0),tArY+tArH+3,(unsigned char *)lab_tm,clr_mrk);
		    }
		    if( lab_dt[0] )
		    {
			wdth = gdFontTiny->w*strlen(lab_dt)/2;
			if( (h_pos+wdth) < endMarkBrd )
			    gdImageString(im,gdFontTiny,vmax(h_pos-wdth,0),tArY+tArH+3+gdFontTiny->h,(unsigned char *)lab_dt,clr_mrk);
		    }
		    memcpy((char*)&ttm1,(char*)ttm,sizeof(tm));
		    first_m = false;
		}
                //---- Next ----
		if( i_h >= tEnd )       break;
		i_h = (i_h/hDiv)*hDiv + hDiv;
		if( i_h > tEnd )        i_h = tEnd;
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

	    if( aVbeg >= aVend ) return;
            //----- Calc value borders -----
            vsMax = -3e300, vsMin = 3e300;
    	    for( int ipos = trnds[0].val(aVbeg); ipos < trnds[0].val().size() && trnds[0].val()[ipos].tm <= aVend; ipos++ )
	    {
	        if( trnds[0].val()[ipos].val == EVAL_REAL ) continue;
	        vsMin  = vmin(vsMin,trnds[0].val()[ipos].val);
	        vsMax  = vmax(vsMax,trnds[0].val()[ipos].val);
	    }
	    if( vsMax == -3e300 ) { vsMax = 1; vsMin = 0; }
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

        //--- Prepare border for percent trend ---
        float bordL = trnds[i_t].bordL();
        float bordU = trnds[i_t].bordU();
	if( vsPerc && bordL >= bordU )
	{
	    bordU = -3e300, bordL = 3e300;
	    for( int ipos = aPosBeg; ipos < trnds[i_t].val().size() && trnds[i_t].val()[ipos].tm <= aVend; ipos++ )
	    {
	        if( trnds[i_t].val()[ipos].val == EVAL_REAL ) continue;
	        bordL = vmin(bordL,trnds[i_t].val()[ipos].val);
	        bordU = vmax(bordU,trnds[i_t].val()[ipos].val);
	    }
            float vMarg = (bordU-bordL)/10;
            bordL-= vMarg;
            bordU+= vMarg;
        }
	
        //--- Draw trend ---
        double curVl, averVl = EVAL_REAL, prevVl = EVAL_REAL;
        int    curPos, averPos = 0, prevPos = 0;
        long long curTm, averTm = 0, averLstTm = 0;
        for( int a_pos = aPosBeg; true; a_pos++ )
        {
            curTm = trnds[i_t].val()[a_pos].tm;
            if( a_pos < trnds[i_t].val().size() && trnds[i_t].val()[a_pos].tm <= aVend )
            {
                curVl = trnds[i_t].val()[a_pos].val;
                if( vsPerc && curVl != EVAL_REAL )
                {
                    curVl = 100.*(curVl-bordL)/(bordU-bordL);
                    curVl = (curVl>100) ? 100 : (curVl<0) ? 0 : curVl;
                }
                curPos = tArX+tArW*(curTm-tBeg)/(tEnd-tBeg);
            }else curPos = 0;
    	    //Square Average
            if( averPos == curPos )
            {
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
    	    arh_per = arh_beg = arh_end = 0;
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
    if( arch.empty() && (bbeg-tTimeGrnd)/bper ) { tTime = bbeg-1; goto m1; }
}

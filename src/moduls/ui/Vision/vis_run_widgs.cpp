
//OpenSCADA system module UI.Vision file: vis_run_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2007 by Roman Savochenko                                *
 *   rom_as@diyaorg.dp.ua                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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

#include <QEvent>
#include <QKeyEvent>
#include <QApplication>

#include <tsys.h>

#include "tvision.h"
#include "vis_shapes.h"
#include "vis_run.h"
#include "vis_run_widgs.h"

using namespace VISION;

//****************************************
//* Shape widget view runtime mode       *
//****************************************
RunWdgView::RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent ) :
    WdgView(iwid,ilevel,(QMainWindow*)mainWind,parent), reqtm(1)
{
    int endElSt = iwid.rfind("/");
    if( endElSt == string::npos ) return;
    string lstEl = iwid.substr(endElSt+1);
    if( lstEl.size() > 4 && lstEl.substr(0,4) == "wdg_" ) setObjectName(lstEl.substr(4).c_str());
    if( lstEl.size() > 3 && lstEl.substr(0,3) == "pg_" )  setObjectName(lstEl.substr(3).c_str());
    curDiv = vmax(1,1000/mainWin()->period());
}

RunWdgView::~RunWdgView( )
{

}

string RunWdgView::user( )
{
    return mainWin()->user();
}

VisRun *RunWdgView::mainWin( )
{
    return (VisRun *)WdgView::mainWin();
}

string RunWdgView::pgGrp( )
{
    return dc().value("pgGrp").toString().toAscii().data();
}

string RunWdgView::pgOpenSrc( )
{
    return dc().value("pgOpenSrc").toString().toAscii().data();
}

void RunWdgView::setPgOpenSrc( const string &vl )
{
    dc()["pgOpenSrc"] = vl.c_str();
    attrSet("pgOpenSrc",vl,26);
}

int RunWdgView::cntrIfCmd( XMLNode &node, bool glob )
{
    return mainWin()->cntrIfCmd(node,glob);
}

WdgView *RunWdgView::newWdgItem( const string &iwid )
{
    return new RunWdgView(iwid,wLevel()+1,mainWin(),this);
}

void RunWdgView::update( unsigned cnt, int div_max, const string &wpath )
{
    if( !wpath.empty() )
    {
	int off = 0;	
	RunWdgView *wdg = findChild<RunWdgView*>(TSYS::pathLev(wpath,0,true,&off).c_str());
	if( wdg ) wdg->update(1,0,wpath.substr(off));
	return;
    }
    
    //- Request to widget for last attributes -    
    if( !div_max || !((cnt+(unsigned)wLevel())%curDiv) )
    {
	bool change = false;
	XMLNode *req_el;
	XMLNode req("get");
	req.setAttr("path",id()+"/%2fserv%2f0")->
	    setAttr("tm",TSYS::uint2str(cnt?reqtm:0));	
	if( !cntrIfCmd(req) )
	{
	    if( !cnt )	setAllAttrLoad(true);
    	    for( int i_el = 0; i_el < req.childSize(); i_el++ )
	    {
		req_el = req.childGet(i_el);
		if( attrSet("",req_el->text(),atoi(req_el->attr("pos").c_str())) )
		    change = true;
	    }
	    if( !cnt )
	    {
		setAllAttrLoad(false);
		attrSet("","load",-1);
	    }
    	    reqtm = strtoul(req.attr("tm").c_str(),0,10);
	    
	}
	//-- Update divider --
	if( curDiv > 1 && change ) 		curDiv--;
	if( curDiv < div_max && !change )	curDiv++;
    }
    
    //- Call childs for update -
    if( div_max || !cnt )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<RunWdgView*>(children().at(i_c)) && ((RunWdgView*)children().at(i_c))->isEnabled() )
    		((RunWdgView*)children().at(i_c))->update(cnt,div_max);
}

bool RunWdgView::attrSet( const string &attr, const string &val, int uiPrmPos )
{
    bool rez = WdgView::attrSet( attr, val, uiPrmPos );
    
    switch(uiPrmPos)
    {
	case -2:        //focus
	    if( (bool)atoi(val.c_str()) == hasFocus() )      break;
	    if( (bool)atoi(val.c_str()) ) setFocus(Qt::OtherFocusReason);
	    break;
    }
    return rez;
}

bool RunWdgView::event( QEvent *event )
{
    if( WdgView::event(event) || (shape&&shape->event(this,event)) )	return true;

    //- Key events process for send to model -
    string mod_ev;
    switch( event->type() )
    {
	case QEvent::KeyPress:
	    mod_ev = "key_pres";
	case QEvent::KeyRelease:
     	{
	    //printf("TEST 00: %s: %d\n",id().c_str(),event->type());	
	    QKeyEvent *key = (QKeyEvent*)event;
	    if( key->key() == Qt::Key_Tab ) { mod_ev = ""; break; }
	    if( mod_ev.empty() ) mod_ev = "key_rels";
	    if( QApplication::keyboardModifiers()&Qt::ControlModifier )	mod_ev+="Ctrl";	    
	    if( QApplication::keyboardModifiers()&Qt::AltModifier )	mod_ev+="Alt";	    	    
	    if( QApplication::keyboardModifiers()&Qt::ShiftModifier )	mod_ev+="Shift";
	    switch(key->key())
	    {
		case Qt::Key_Escape:	mod_ev+="Esc";		break;
		case Qt::Key_Backspace:	mod_ev+="BackSpace";  	break;
		case Qt::Key_Return:	mod_ev+="Return";	break;
		case Qt::Key_Enter:	mod_ev+="Enter";       	break;
		case Qt::Key_Insert:	mod_ev+="Insert";	break;
		case Qt::Key_Delete:	mod_ev+="Delete";	break;
		case Qt::Key_Pause:	mod_ev+="Pause";	break;
		case Qt::Key_Print:	mod_ev+="Print";        break;
		//case Qt::Key_SysReq:	mod_ev+="SysReq";	break;
		//case Qt::Key_Clear:	mod_ev+="Clear";	break;
		case Qt::Key_Home:	mod_ev+="Home";		break;
		case Qt::Key_End:	mod_ev+="End";		break;
		case Qt::Key_Left:	mod_ev+="Left";		break;
		case Qt::Key_Up:	mod_ev+="Up";		break;
		case Qt::Key_Right:	mod_ev+="Right";	break;
		case Qt::Key_Down:	mod_ev+="Down";		break;
		case Qt::Key_PageUp:	mod_ev+="PageUp";	break;
		case Qt::Key_PageDown:	mod_ev+="PageDown";	break;
		case Qt::Key_F1:	mod_ev+="F1";		break;
		case Qt::Key_F2:	mod_ev+="F2";		break;		
		case Qt::Key_F3:	mod_ev+="F3";		break;		
		case Qt::Key_F4:	mod_ev+="F4";		break;		
		case Qt::Key_F5:	mod_ev+="F5";		break;		
		case Qt::Key_F6:	mod_ev+="F6";		break;		
		case Qt::Key_F7:	mod_ev+="F7";		break;		
		case Qt::Key_F8:	mod_ev+="F8";		break;		
		case Qt::Key_F9:	mod_ev+="F9";		break;		
		case Qt::Key_F10:	mod_ev+="F10";		break;		
		case Qt::Key_F11:	mod_ev+="F11";		break;		
		case Qt::Key_F12:	mod_ev+="F12";		break;		
		case Qt::Key_F13:	mod_ev+="F13";		break;		
		case Qt::Key_F14:	mod_ev+="F14";		break;
		case Qt::Key_F15:	mod_ev+="F15";		break;		
		case Qt::Key_F16:	mod_ev+="F16";		break;		
		case Qt::Key_F17:	mod_ev+="F17";		break;		
		case Qt::Key_F18:	mod_ev+="F18";		break;		
		case Qt::Key_F19:	mod_ev+="F19";		break;		
		case Qt::Key_F20:	mod_ev+="F20";		break;		
		case Qt::Key_F21:	mod_ev+="F21";		break;		
		case Qt::Key_F22:	mod_ev+="F22";		break;		
		case Qt::Key_F23:	mod_ev+="F23";		break;		
		case Qt::Key_F24:	mod_ev+="F24";		break;		
		case Qt::Key_F25:	mod_ev+="F25";		break;		
		case Qt::Key_F26:	mod_ev+="F26";		break;		
		case Qt::Key_F27:	mod_ev+="F27";		break;
		case Qt::Key_F28:	mod_ev+="F28";		break;		
		case Qt::Key_F29:	mod_ev+="F29";		break;		
		case Qt::Key_F30:	mod_ev+="F30";		break;		
		case Qt::Key_F31:	mod_ev+="F31";		break;		
		case Qt::Key_F32:	mod_ev+="F32";		break;		
		case Qt::Key_F33:	mod_ev+="F33";		break;		
		case Qt::Key_F34:	mod_ev+="F34";		break;		
		case Qt::Key_F35:	mod_ev+="F35";		break;
		case Qt::Key_Space:	mod_ev+="Space";	break;
		case Qt::Key_Apostrophe:mod_ev+="Apostrophe";	break;
		case Qt::Key_Asterisk:	mod_ev+="Asterisk";	break;
		case Qt::Key_Plus:	mod_ev+="Plus";     	break;
		case Qt::Key_Comma:	mod_ev+="Comma";   	break;		
		case Qt::Key_Minus:	mod_ev+="Minus";	break;
		case Qt::Key_Period:	mod_ev+="Period";	break;
		case Qt::Key_Slash:	mod_ev+="Slash";       	break;
		case Qt::Key_0:		mod_ev+="0";		break;
		case Qt::Key_1:		mod_ev+="1";            break;
		case Qt::Key_2:		mod_ev+="2";            break;
		case Qt::Key_3:		mod_ev+="3";            break;
		case Qt::Key_4:		mod_ev+="4";            break;
		case Qt::Key_5:		mod_ev+="5";            break;
		case Qt::Key_6:		mod_ev+="6";            break;
		case Qt::Key_7:		mod_ev+="7";            break;
		case Qt::Key_8:		mod_ev+="8";            break;
		case Qt::Key_9:		mod_ev+="9";            break;
		case Qt::Key_Semicolon:	mod_ev+="Semicolon";	break;
		case Qt::Key_Equal:	mod_ev+="Equal";	break;
		case Qt::Key_A:		mod_ev+="A";            break;
		case Qt::Key_B:		mod_ev+="B";            break;
		case Qt::Key_C:		mod_ev+="C";            break;
		case Qt::Key_D:		mod_ev+="D";            break;
		case Qt::Key_E:		mod_ev+="E";            break;
		case Qt::Key_F:		mod_ev+="F";            break;
		case Qt::Key_G:		mod_ev+="G";            break;
		case Qt::Key_H:		mod_ev+="H";            break;
		case Qt::Key_I:		mod_ev+="I";            break;
		case Qt::Key_J:		mod_ev+="J";            break;
		case Qt::Key_K:		mod_ev+="K";            break;
		case Qt::Key_L:		mod_ev+="L";            break;
		case Qt::Key_M:		mod_ev+="M";            break;
		case Qt::Key_N:		mod_ev+="N";            break;
		case Qt::Key_O:		mod_ev+="O";            break;
		case Qt::Key_P:		mod_ev+="P";            break;
		case Qt::Key_Q:		mod_ev+="Q";            break;
		case Qt::Key_R:		mod_ev+="R";            break;
		case Qt::Key_S:		mod_ev+="S";            break;
		case Qt::Key_T:		mod_ev+="T";            break;
		case Qt::Key_U:		mod_ev+="U";            break;
		case Qt::Key_V:		mod_ev+="V";            break;
		case Qt::Key_W:		mod_ev+="W";            break;
		case Qt::Key_X:		mod_ev+="X";            break;
		case Qt::Key_Y:		mod_ev+="Y";            break;
		case Qt::Key_Z:		mod_ev+="Z";            break;
		case Qt::Key_BracketLeft: 	mod_ev+="BracketLeft";	break;
		case Qt::Key_Backslash:	mod_ev+="BackSlash";	break;
		case Qt::Key_BracketRight: 	mod_ev+="BracketRight";	break;
		case Qt::Key_QuoteLeft:	mod_ev+="QuoteLeft";	break;		
		default:		mod_ev="";		break;		    
		//    printf("TEST 30: Key %d\n",key->key());
		//    break;
	    }
	    break;
	}
	case QEvent::MouseButtonPress:
	    mod_ev = "key_mousePres";
	case QEvent::MouseButtonRelease:
	{
	    if( mod_ev.empty() ) mod_ev = "key_mouseRels";
	    QMouseEvent *mouse = (QMouseEvent*)event;
	    switch(mouse->button())
	    {
		case Qt::LeftButton: 	mod_ev+="Left";	break;
		case Qt::RightButton:	mod_ev+="Right";break;
		case Qt::MidButton:	mod_ev+="Midle";break;
	    }
	    break;
	}
	case QEvent::MouseButtonDblClick:	
	    mod_ev = "key_mouseDblClick";
	    break;
	case QEvent::FocusIn:
	    attrSet("focus","1");
	    mod_ev = "ws_FocusIn";
	    break;
	case QEvent::FocusOut:
	    attrSet("focus","0");
	    mod_ev = "ws_FocusOut";
	    break;
    }
    if( !mod_ev.empty() ) 
    { 
	attrSet("event",mod_ev);	
	return true; 
    }
    return QWidget::event(event);
}
					    
//****************************************
//* Shape page view runtime mode         *
//****************************************
RunPageView::RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent ) :
    RunWdgView(iwid,0,mainWind,parent), wx_scale(1.0), wy_scale(1.0)
{

}

RunPageView::~RunPageView( )
{

}

float RunPageView::xScale( bool full )
{
    if( full ) return mainWin()->xScale()*WdgView::xScale();
    return WdgView::xScale();
}

float RunPageView::yScale( bool full )
{
    if( full ) return mainWin()->yScale()*WdgView::yScale();
    return WdgView::yScale();
}

RunPageView *RunPageView::parent( )
{
    return qobject_cast<RunPageView*>(parentWidget());
}

RunPageView *RunPageView::findOpenPage( const string &ipg )
{
    //- Self check -
    if( id() == ipg ) return this;
    //- Check to included widgets -
    for( int i_ch = 0; i_ch < children().size(); i_ch++ )
        if( !qobject_cast<RunPageView*>(children().at(i_ch)) &&
        	((RunWdgView*)children().at(i_ch))->root() == "Box" &&
        	((RunWdgView*)children().at(i_ch))->pgOpenSrc() == ipg.c_str() )    
    	    return (RunPageView*)children().at(i_ch);
    //- Put checking to childs -
    for( int i_ch = 0; i_ch < children().size(); i_ch++ )
	if( qobject_cast<RunPageView*>(children().at(i_ch)) )
	{
	    RunPageView *pg = ((RunPageView*)children().at(i_ch))->findOpenPage(ipg);
	    if( pg ) return pg;
	}

    return NULL;
}

bool RunPageView::callPage( const string &pg_it, const string &pgGrp, const string &pgSrc )
{
    //printf("TEST 00: %s => %s\n",id().c_str(),pg_it.c_str());
    //- Check for set include page -
    for( int i_ch = 0; i_ch < children().size(); i_ch++ )
        if( !pgGrp.empty() && !qobject_cast<RunPageView*>(children().at(i_ch)) &&
		((RunWdgView *)children().at(i_ch))->root() == "Box" &&
	    	((RunWdgView*)children().at(i_ch))->pgGrp() == pgGrp )
	{
	    string pg_it_prev = ((RunWdgView*)children().at(i_ch))->pgOpenSrc();
	    if( pg_it != pg_it_prev )
	    {
		if( !pg_it_prev.empty() ) mainWin()->wAttrSet(pg_it_prev,"pgOpen","0");
		((RunWdgView*)children().at(i_ch))->setPgOpenSrc(pg_it);
	    }
	    return true;
        }
    //- Check for open child page -
    if( pgSrc == id().c_str() )	{ pgOpen( pg_it ); return true; }
    //- Put checking to self include pages -
    for( int i_ch = 0; i_ch < children().size(); i_ch++ )
        if( qobject_cast<RunPageView*>(children().at(i_ch)) &&
		((RunPageView *)children().at(i_ch))->callPage(pg_it,pgGrp,pgSrc)) 
	    return true;
    //- Unknown and empty source pages open as master page child windows -
    if( !parent() )	{ pgOpen( pg_it ); return true; }
    
    return false;
}

RunPageView *RunPageView::pgOpen( const string &ipg )
{
    RunPageView *pg = new RunPageView(ipg,mainWin(),this);
    pg->setAttribute(Qt::WA_DeleteOnClose);
    pg->setWindowFlags(Qt::Sheet);
    pg->load("");
    pg->moveF(QPointF(mapToGlobal(pos()).x()+sizeF().width()/2-pg->sizeF().width()/2,
    	     mapToGlobal(pos()).y()+sizeF().height()/2-pg->sizeF().height()/2));
    pg->show( );
    
    return pg;
}

void RunPageView::closeEvent( QCloseEvent *event )
{
    //-- Send close command --
    attrSet("pgOpen","0");
}

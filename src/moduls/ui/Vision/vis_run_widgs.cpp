
//OpenSCADA system module UI.Vision file: vis_run_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2007-2008 by Roman Savochenko                           *
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
#include <QPainter>

#include <tsys.h>

#include "tvision.h"
#include "vis_shapes.h"
#include "vis_run.h"
#include "vis_run_widgs.h"

using namespace VISION;

//*************************************************
//* Shape widget view runtime mode                *
//*************************************************
RunWdgView::RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent, Qt::WindowFlags f ) :
    WdgView(iwid,ilevel,(QMainWindow*)mainWind,parent,f), reqtm(1), mPermCntr(false), mPermView(true)
{
    int endElSt = iwid.rfind("/");
    if( endElSt == string::npos ) return;
    string lstEl = iwid.substr(endElSt+1);
    if( lstEl.size() > 4 && lstEl.substr(0,4) == "wdg_" ) setObjectName(lstEl.substr(4).c_str());
    if( lstEl.size() > 3 && lstEl.substr(0,3) == "pg_" )  setObjectName(lstEl.substr(3).c_str());
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
    return property("pgGrp").toString().toAscii().data();
}

string RunWdgView::pgOpenSrc( )
{
    return property("pgOpenSrc").toString().toAscii().data();
}

void RunWdgView::setPgOpenSrc( const string &vl )
{
    setProperty("pgOpenSrc",vl.c_str());
    attrSet("pgOpenSrc",vl,3);
}

int RunWdgView::cntrIfCmd( XMLNode &node, bool glob )
{
    return mainWin()->cntrIfCmd(node,glob);
}

WdgView *RunWdgView::newWdgItem( const string &iwid )
{
    return new RunWdgView(iwid,wLevel()+1,mainWin(),this);
}

void RunWdgView::update( bool full, const string &wpath, bool all )
{
    if( !wpath.empty() )
    {
	int off = 0;
	RunWdgView *wdg = findChild<RunWdgView*>(TSYS::pathLev(wpath,0,true,&off).c_str());
	if( wdg && !qobject_cast<RunPageView*>(wdg) ) wdg->update(false,wpath.substr(off));
	return;
    }

    //- Request to widget for last attributes -
    bool change = false;
    XMLNode *req_el;
    XMLNode req("get");
    req.setAttr("path",id()+"/%2fserv%2fattr")->
	setAttr("tm",TSYS::uint2str(full?0:reqtm));
    if( !cntrIfCmd(req) )
    {
	if( full )	setAllAttrLoad(true);
	for( int i_el = 0; i_el < req.childSize(); i_el++ )
	{
	    req_el = req.childGet(i_el);
	    if( attrSet("",req_el->text(),atoi(req_el->attr("pos").c_str())) )
		change = true;
	}
	if( full )
	{
	    setAllAttrLoad(false);
	    attrSet("","load",-1);
	    //- Childs update for permition change -
	    childsUpdate(true);
	    orderUpdate();
	    QWidget::update();
	}
	reqtm = strtoul(req.attr("tm").c_str(),0,10);
    }

    //- Call childs for update -
    if( full || all )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<RunWdgView*>(children().at(i_c)) && !qobject_cast<RunPageView*>(children().at(i_c)) &&
		    ((RunWdgView*)children().at(i_c))->isEnabled() )
		((RunWdgView*)children().at(i_c))->update(full,"",all);
}

void RunWdgView::childsUpdate( bool newLoad )
{
    XMLNode req("get");

    string b_nm = id();
    req.setAttr("path",id()+"/%2fwdg%2fcfg%2fpath")->setAttr("resLink","1");
    if( !cntrIfCmd(req) ) b_nm = req.text();

    vector<string> lst;
    req.clear()->setAttr("path",b_nm+"/%2finclwdg%2fwdg")->setAttr("chkUserPerm","1");

    if( !cntrIfCmd(req) )
	for( int i_el = 0; i_el < req.childSize(); i_el++ )
	    lst.push_back(req.childGet(i_el)->attr("id"));

    //- Delete child widgets -
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	if( !qobject_cast<RunWdgView*>(children().at(i_c)) || qobject_cast<RunPageView*>(children().at(i_c)) ) continue;
	int i_l;
	for( i_l = 0; i_l < lst.size(); i_l++ )
	if( qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+lst[i_l]) )
	    break;
	if( i_l >= lst.size() ) children().at(i_c)->deleteLater(); //delete children().at(i_c--);
    }

    //- Create new child widget -
    for( int i_l = 0; i_l < lst.size(); i_l++ )
    {
	int i_c;
	for( i_c = 0; i_c < children().size(); i_c++ )
	if( qobject_cast<WdgView*>(children().at(i_c)) &&
		qobject_cast<WdgView*>(children().at(i_c))->id() == (b_nm+"/wdg_"+lst[i_l]) )
	    break;
	if( i_c >= children().size() )
	{
	    WdgView *nwdg = newWdgItem(b_nm+"/wdg_"+lst[i_l]);
	    if( nwdg )
	    {
		nwdg->show();
		if( newLoad ) nwdg->load("");
	    }
	}
    }
}

void RunWdgView::orderUpdate( )
{
    //- Same order of included widgets is update -
    WdgView *lw = NULL;
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	WdgView *cw = qobject_cast<RunWdgView*>(children().at(i_c));
	if( !cw || qobject_cast<RunPageView*>(cw) ) continue;
	//- Change order -
	if( lw && (cw->z() < lw->z()) )
	{
	    cw->stackUnder(lw);
	    i_c = -1;
	    lw = NULL;
	}
	else lw = cw;
    }

    //- Update tab order -
    RunWdgView *prev_aw = NULL;
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	RunWdgView *cw = qobject_cast<RunWdgView*>(children().at(i_c));
	if( !cw || !(mod->getFocusedWdg(cw)->focusPolicy()&Qt::TabFocus) ) continue;
	if( prev_aw )	QWidget::setTabOrder( mod->getFocusedWdg(prev_aw), mod->getFocusedWdg(cw) );
	prev_aw = cw;
    }
}

bool RunWdgView::attrSet( const string &attr, const string &val, int uiPrmPos )
{
    bool rez = WdgView::attrSet( attr, val, uiPrmPos );

    switch(uiPrmPos)
    {
	case -2:	//focus
	    if( (bool)atoi(val.c_str()) == hasFocus() )      break;
	    if( (bool)atoi(val.c_str()) ) setFocus(Qt::OtherFocusReason);
	    return true;
	case -3:	//perm
	    setPermCntr( atoi(val.c_str())&SEQ_WR );
	    setPermView( atoi(val.c_str())&SEQ_RD );
	    return true;
	case 4:		//pgGrp
	    setProperty("pgGrp",val.c_str());
	    return true;
	case 3:		//pgOpenSrc
	    setProperty("pgOpenSrc",val.c_str());
	    return true;
    }

    return rez;
}

string RunWdgView::resGet( const string &res )
{
    if( res.empty() )	return "";
    string ret = mainWin( )->cacheResGet(res);
    if( ret.empty() && !(ret=WdgView::resGet(res)).empty() )
	mainWin( )->cacheResSet(res,ret);

    return ret;
}

bool RunWdgView::event( QEvent *event )
{
    //- Paint message about access denied -
    if( event->type() == QEvent::Paint && !permView() )
    {
	QPainter pnt(this);
	//-- Fill page and draw border --
	pnt.fillRect(rect(),QBrush(QColor("black"),Qt::Dense4Pattern));
	pnt.setPen(QPen(QBrush(QColor("black")),1));
	pnt.drawRect(rect().adjusted(0,0,-1,-1));
	//-- Draw message --
	QTextOption to;
	pnt.setPen(QColor("red"));
	to.setAlignment(Qt::AlignCenter);
	to.setWrapMode(QTextOption::WordWrap);
	pnt.drawText(rect(),QString(_("Widget: '%1'.\nView access is no permited.")).arg(id().c_str()),to);
	return true;
    }

    if( WdgView::event(event) || (shape&&shape->event(this,event)) )	return true;

    //- Key events process for send to model -
    string mod_ev;
    switch( event->type() )
    {
	case QEvent::Paint:	return true;
	case QEvent::KeyPress:
	    mod_ev = "key_pres";
	case QEvent::KeyRelease:
	    //printf("TEST 00: %s: %d\n",id().c_str(),event->type());
	    if( ((QKeyEvent*)event)->key() == Qt::Key_Tab ) { mod_ev = ""; break; }
	    if( mod_ev.empty() ) mod_ev = "key_rels";
	    if( QApplication::keyboardModifiers()&Qt::ControlModifier )	mod_ev+="Ctrl";
	    if( QApplication::keyboardModifiers()&Qt::AltModifier )	mod_ev+="Alt";
	    if( QApplication::keyboardModifiers()&Qt::ShiftModifier )	mod_ev+="Shift";
	    if( ((QKeyEvent*)event)->nativeScanCode() )
		attrSet("event",mod_ev+"SC#"+TSYS::int2str(((QKeyEvent*)event)->nativeScanCode(),TSYS::Hex));
	    switch(((QKeyEvent*)event)->key())
	    {
		case Qt::Key_Escape:	mod_ev+="Esc";		break;
		case Qt::Key_Backspace:	mod_ev+="BackSpace";	break;
		case Qt::Key_Return:	mod_ev+="Return";	break;
		case Qt::Key_Enter:	mod_ev+="Enter";	break;
		case Qt::Key_Insert:	mod_ev+="Insert";	break;
		case Qt::Key_Delete:	mod_ev+="Delete";	break;
		case Qt::Key_Pause:	mod_ev+="Pause";	break;
		case Qt::Key_Print:	mod_ev+="Print";	break;
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
		case Qt::Key_Plus:	mod_ev+="Plus";		break;
		case Qt::Key_Comma:	mod_ev+="Comma";	break;
		case Qt::Key_Minus:	mod_ev+="Minus";	break;
		case Qt::Key_Period:	mod_ev+="Period";	break;
		case Qt::Key_Slash:	mod_ev+="Slash";	break;
		case Qt::Key_0:		mod_ev+="0";		break;
		case Qt::Key_1:		mod_ev+="1";		break;
		case Qt::Key_2:		mod_ev+="2";		break;
		case Qt::Key_3:		mod_ev+="3";		break;
		case Qt::Key_4:		mod_ev+="4";		break;
		case Qt::Key_5:		mod_ev+="5";		break;
		case Qt::Key_6:		mod_ev+="6";		break;
		case Qt::Key_7:		mod_ev+="7";		break;
		case Qt::Key_8:		mod_ev+="8";		break;
		case Qt::Key_9:		mod_ev+="9";		break;
		case Qt::Key_Semicolon:	mod_ev+="Semicolon";	break;
		case Qt::Key_Equal:	mod_ev+="Equal";	break;
		case Qt::Key_A:		mod_ev+="A";		break;
		case Qt::Key_B:		mod_ev+="B";		break;
		case Qt::Key_C:		mod_ev+="C";		break;
		case Qt::Key_D:		mod_ev+="D";		break;
		case Qt::Key_E:		mod_ev+="E";		break;
		case Qt::Key_F:		mod_ev+="F";		break;
		case Qt::Key_G:		mod_ev+="G";		break;
		case Qt::Key_H:		mod_ev+="H";		break;
		case Qt::Key_I:		mod_ev+="I";		break;
		case Qt::Key_J:		mod_ev+="J";		break;
		case Qt::Key_K:		mod_ev+="K";		break;
		case Qt::Key_L:		mod_ev+="L";		break;
		case Qt::Key_M:		mod_ev+="M";		break;
		case Qt::Key_N:		mod_ev+="N";		break;
		case Qt::Key_O:		mod_ev+="O";		break;
		case Qt::Key_P:		mod_ev+="P";		break;
		case Qt::Key_Q:		mod_ev+="Q";		break;
		case Qt::Key_R:		mod_ev+="R";		break;
		case Qt::Key_S:		mod_ev+="S";		break;
		case Qt::Key_T:		mod_ev+="T";		break;
		case Qt::Key_U:		mod_ev+="U";		break;
		case Qt::Key_V:		mod_ev+="V";		break;
		case Qt::Key_W:		mod_ev+="W";		break;
		case Qt::Key_X:		mod_ev+="X";		break;
		case Qt::Key_Y:		mod_ev+="Y";		break;
		case Qt::Key_Z:		mod_ev+="Z";		break;
		case Qt::Key_BracketLeft:	mod_ev+="BracketLeft";	break;
		case Qt::Key_Backslash:	mod_ev+="BackSlash";	break;
		case Qt::Key_BracketRight:	mod_ev+="BracketRight";	break;
		case Qt::Key_QuoteLeft:	mod_ev+="QuoteLeft";	break;
		default:
		    mod_ev+="#"+TSYS::int2str(((QKeyEvent*)event)->key(),TSYS::Hex);
		    break;
	    }
	    attrSet("event",mod_ev);
	    return true;
	case QEvent::MouseButtonPress:
	    mod_ev = "key_mousePres";
	case QEvent::MouseButtonRelease:
	    if( mod_ev.empty() ) mod_ev = "key_mouseRels";
	    switch(((QMouseEvent*)event)->button())
	    {
		case Qt::LeftButton:	mod_ev+="Left";	break;
		case Qt::RightButton:	mod_ev+="Right";break;
		case Qt::MidButton:	mod_ev+="Midle";break;
	    }
	    attrSet("event",mod_ev);
	    return true;
	case QEvent::MouseButtonDblClick:	attrSet("event","key_mouseDblClick");	return true;
	case QEvent::FocusIn:	attrSet("focus","1");	attrSet("event","ws_FocusIn");	return true;
	case QEvent::FocusOut:	attrSet("focus","0");	attrSet("event","ws_FocusOut");	return true;
    }
    return QWidget::event(event);
}

//*************************************************
//* Shape page view runtime mode                  *
//*************************************************
RunPageView::RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent, Qt::WindowFlags f ) :
    RunWdgView(iwid,0,mainWind,parent,f), wx_scale(1.0), wy_scale(1.0)
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
    {
	if( qobject_cast<RunPageView*>(children().at(i_ch)) )
	{
	    RunPageView *pg = ((RunPageView*)children().at(i_ch))->findOpenPage(ipg);
	    if( pg ) return pg;
	    continue;
	}
	if( !qobject_cast<RunWdgView*>(children().at(i_ch)) )	continue;
	RunWdgView *rwdg = (RunWdgView*)children().at(i_ch);
	if( rwdg->root() == "Box" && rwdg->pgOpenSrc() == ipg && !rwdg->property("inclPg").toString().isEmpty() )
	    return (RunPageView*)TSYS::str2addr(rwdg->property("inclPg").toString().toAscii().data());
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
		if( !pg_it_prev.empty() )
		{
		    XMLNode req("close");
		    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",pg_it_prev);
		    mainWin()->cntrIfCmd(req);
		    //mainWin()->wAttrSet(pg_it_prev,"pgOpen","0");
		}
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
    if( !parent() )		{ pgOpen( pg_it ); return true; }

    return false;
}

RunPageView *RunPageView::pgOpen( const string &ipg )
{
    RunPageView *pg = new RunPageView(ipg,mainWin(),this);
    pg->setAttribute(Qt::WA_DeleteOnClose);
    pg->setWindowFlags(Qt::Sheet);
    pg->setWindowTitle(mainWin()->windowTitle());
    pg->load("");
    pg->moveF(QPointF(mapToGlobal(pos()).x()+sizeF().width()/2-pg->sizeF().width()/2,
	     mapToGlobal(pos()).y()+sizeF().height()/2-pg->sizeF().height()/2));

    return pg;
}

void RunPageView::closeEvent( QCloseEvent *event )
{
    //-- Send close command --
    XMLNode req("close");
    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",id());
    mainWin()->cntrIfCmd(req);

    //-- Close included pages --
    for( int i_ch = 0; i_ch < children().size(); i_ch++ )
	if( !qobject_cast<RunPageView*>(children().at(i_ch)) && ((RunWdgView *)children().at(i_ch))->root() == "Box" &&
		!((RunWdgView*)children().at(i_ch))->pgOpenSrc().empty() )
	{
	    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",((RunWdgView*)children().at(i_ch))->pgOpenSrc());
	    mainWin()->cntrIfCmd(req);
	}
}

//*********************************************
//* Play sound thread for RunTime session     *
//*********************************************
SndPlay::SndPlay( QObject *parent ) : QThread(parent)
{

}

VisRun *SndPlay::mainWin( )
{
    return (VisRun *)parent();
}

void SndPlay::run( )
{
    if( mPlayData.empty() )	return;

    int comPos = 0;
    string com = mod->playCom( );
    string srcFile = "/var/tmp/oscadaPlayTmp_"+mainWin()->workSess( );

    //- Put source file name to command -
    bool srcToPipe = false;
    if( (comPos=com.find("%f")) != string::npos )
	com.replace( comPos, 2, srcFile.c_str() );
    else srcToPipe = true;

    //- Write play data to file -
    if( !srcToPipe )
    {
	FILE *fp = fopen( srcFile.c_str(), "w" );
	if( !fp )	{ mPlayData.clear(); return; }
	fwrite( mPlayData.data(), 1, mPlayData.size(), fp );
	fclose(fp);
    }

    //- Call play command -
    FILE *fp = popen( com.c_str(), "w" );
    if( !fp )		{ mPlayData.clear(); return; }
    //- Write data to pipe -
    if( srcToPipe )	fwrite( mPlayData.data(), mPlayData.size(), 1, fp );
    pclose(fp);
    if( !srcToPipe )	remove( srcFile.c_str() );

    mPlayData.clear();
};



//OpenSCADA module UI.Vision file: vis_run_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2007-2019 by Roman Savochenko, <roman@oscada.org>       *
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
#include <QComboBox>
#include <QStatusBar>
#include <QDesktopWidget>
#include <QScrollBar>

#include "vis_run.h"
#include "vis_run_widgs.h"
#include "vis_shapes.h"

#undef _
#define _(mess) mod->I18N(mess, mainWin()->lang().c_str())

using namespace VISION;

//*************************************************
//* Shape widget view runtime mode                *
//*************************************************
RunWdgView::RunWdgView( const string &iwid, int ilevel, VisRun *mainWind, QWidget* parent, Qt::WindowFlags f ) :
    WdgView(iwid,ilevel,(QMainWindow*)mainWind,parent,f), mPermCntr(false), mPermView(true)
{
    size_t endElSt = iwid.rfind("/");
    if(endElSt == string::npos) return;
    string lstEl = iwid.substr(endElSt+1);
    if(lstEl.size() > 4 && lstEl.substr(0,4) == "wdg_") setObjectName(lstEl.substr(4).c_str());
    if(lstEl.size() > 3 && lstEl.substr(0,3) == "pg_")  setObjectName(lstEl.substr(3).c_str());

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:RunWdgView", 1);
}

RunWdgView::~RunWdgView( )
{
    //Child widgets remove before
    childsClear();

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:RunWdgView", -1);
}

string RunWdgView::name( )	{ return windowTitle().isEmpty() ? mainWin()->wAttrGet(id(),"name") : windowTitle().toStdString(); }

string RunWdgView::user( )	{ return mainWin()->user(); }

VisRun *RunWdgView::mainWin( )	{ return (VisRun *)WdgView::mainWin(); }

void RunWdgView::resizeF( const QSizeF &size )
{
    RunPageView *holdPg = dynamic_cast<RunPageView*>(this);
    RunWdgView *cntW = NULL;
    if(holdPg && !holdPg->property("cntPg").toString().isEmpty())
	cntW = (RunWdgView*)TSYS::str2addr(holdPg->property("cntPg").toString().toStdString());
    else if(!holdPg && root() == "Box" && (holdPg=((ShapeBox::ShpDt*)shpData)->inclPg)) cntW = this;

    //Hard resize for main and external windows
    if(holdPg && !cntW)	resize((mWSize=size).toSize());
    else WdgView::resizeF(size);

    if(holdPg && cntW) {
	QAbstractScrollArea *sa = (cntW->root() == "Box") ? ((ShapeBox::ShpDt*)cntW->shpData)->inclScrl : NULL;
	bool wHold = (holdPg->sizeOrigF().width()*holdPg->xScale() <= cntW->sizeOrigF().width()*cntW->xScale());
	bool hHold = (holdPg->sizeOrigF().height()*holdPg->yScale() <= cntW->sizeOrigF().height()*cntW->yScale());

	holdPg->setMaximumSize(wHold ? cntW->size().width() : 1000000, hHold ? cntW->size().height() : 1000000);	//Needed to hide the spare scroll bar on big scale rates

	if(cntW != this) {
	    QSize holdB = QSize(cntW->size().width()  - ((sa&&sa->verticalScrollBar()&&holdPg->size().height()>cntW->size().height())?sa->verticalScrollBar()->size().width():0),
				cntW->size().height() - ((sa&&sa->horizontalScrollBar()&&holdPg->size().width()>cntW->size().width())?sa->horizontalScrollBar()->size().height():0));
	    holdPg->setMinimumSize(wHold ? holdB.width() : holdPg->size().width(), hHold ? holdB.height() : holdPg->size().height());
	}
	else holdPg->resize(QSize(holdPg->sizeOrigF().width()*holdPg->xScale(),holdPg->sizeOrigF().height()*holdPg->yScale()));
    }
}

string RunWdgView::pgGrp( )	{ return property("pgGrp").toString().toStdString(); }

string RunWdgView::pgOpenSrc( )	{ return property("pgOpenSrc").toString().toStdString(); }

void RunWdgView::setPgOpenSrc( const string &vl )
{
    setProperty("pgOpenSrc", vl.c_str());
    attrSet("pgOpenSrc", vl, A_PG_OPEN_SRC, true);
}

int RunWdgView::cntrIfCmd( XMLNode &node, bool glob )	{ return mainWin()->cntrIfCmd(node,glob); }

WdgView *RunWdgView::newWdgItem( const string &iwid )	{ return new RunWdgView(iwid,wLevel()+1,mainWin(),this); }

void RunWdgView::update( bool full, XMLNode *aBr, bool FullTree )
{
    bool reqBrCr = false;
    if(!aBr) {
	aBr = new XMLNode("get");
	aBr->setAttr("path", id()+"/%2fserv%2fattrBr")->
	    setAttr("tm", u2s(full?0:mainWin()->reqTm()))->setAttr("FullTree", FullTree?"1":"0");
	cntrIfCmd(*aBr);
	reqBrCr = true;
    }

    if(full)	setAllAttrLoad(true);
    for(unsigned i_el = 0; i_el < aBr->childSize(); i_el++) {
	XMLNode *cN = aBr->childGet(i_el);
	if(cN->name() == "el") attrSet(cN->attr("id"), cN->text(), s2i(cN->attr("p")));
    }
    if(full) {
	setAllAttrLoad(false);
	attrSet("", "load", A_COM_LOAD);
    }

    //Delete child widgets check
    if(full || FullTree)
	for(int iC = 0, iL = 0; iC < children().size(); iC++) {
	    if(!qobject_cast<RunWdgView*>(children().at(iC)) || qobject_cast<RunPageView*>(children().at(iC))) continue;
	    for(iL = 0; iL < (int)aBr->childSize(); iL++)
		if(aBr->childGet(iL)->name() == "w" &&
			((WdgView*)children().at(iC))->id() == (id()+"/wdg_"+aBr->childGet(iL)->attr("id")))
		    break;
	    if(iL >= (int)aBr->childSize()) children().at(iC)->deleteLater();
	}

    //Create new child widget
    for(int iL = 0, iC = 0; iL < (int)aBr->childSize(); iL++) {
	if(aBr->childGet(iL)->name() != "w") continue;

	for(iC = 0; iC < children().size(); iC++)
	    if(qobject_cast<RunWdgView*>(children().at(iC)) && !qobject_cast<RunPageView*>(children().at(iC)) &&
		    ((RunWdgView*)children().at(iC))->id() == (id()+"/wdg_"+aBr->childGet(iL)->attr("id")))
	    {
		((RunWdgView*)children().at(iC))->update(full,aBr->childGet(iL),FullTree);
		break;
	    }
	if(iC < children().size()) continue;
	WdgView *nwdg = newWdgItem(id()+"/wdg_"+aBr->childGet(iL)->attr("id"));
	nwdg->show();
	nwdg->load("");
    }

    if(full) {
	orderUpdate();
	QWidget::update();
    }

    if(reqBrCr) delete aBr;
}

void RunWdgView::shapeList( const string &snm, vector<string> &ls )
{
    if(shape && snm == shape->id())	ls.push_back(id());

    for(int iC = 0; iC < children().size(); iC++)
	if(qobject_cast<RunWdgView*>(children().at(iC)) && !qobject_cast<RunPageView*>(children().at(iC)) &&
		((RunWdgView*)children().at(iC))->isEnabled())
	    ((RunWdgView*)children().at(iC))->shapeList(snm,ls);
}

RunWdgView *RunWdgView::findOpenWidget( const string &iwdg )
{
    //Self check
    if(id() == iwdg) return this;
    //Check to included widgets
    RunWdgView *wdg;
    for(int iCh = 0; iCh < children().size(); iCh++)
	if(qobject_cast<RunWdgView*>(children().at(iCh)) && !qobject_cast<RunPageView*>(children().at(iCh)) &&
		((RunWdgView*)children().at(iCh))->isEnabled() && (wdg=((RunWdgView*)children().at(iCh))->findOpenWidget(iwdg)))
	    return wdg;

    return NULL;
}

void RunWdgView::orderUpdate( )
{
    WdgView::orderUpdate( );

    //Update tab order
    RunWdgView *prev_aw = NULL;
    for(int iC = 0; iC < children().size(); iC++) {
	RunWdgView *cw = qobject_cast<RunWdgView*>(children().at(iC));
	if(!cw || !(mod->getFocusedWdg(cw)->focusPolicy()&Qt::TabFocus)) continue;
	if(prev_aw) QWidget::setTabOrder(mod->getFocusedWdg(prev_aw), mod->getFocusedWdg(cw));
	prev_aw = cw;
    }
}

bool RunWdgView::attrSet( const string &attr, const string &val, int uiPrmPos, bool toModel )
{
    bool rez = WdgView::attrSet(attr, val, uiPrmPos, toModel);

    switch(uiPrmPos) {
	case A_COM_FOCUS:
	    if((bool)s2i(val) == hasFocus())	break;
	    if((bool)s2i(val))	setFocus(Qt::OtherFocusReason);
	    return true;
	case A_PERM:
	    setPermCntr(s2i(val)&SEC_WR);
	    setPermView(s2i(val)&SEC_RD);
	    return true;
	case A_NO_ID:
	    // User's status line items
	    if(attr == "statLine")		mainWin()->usrStatus(val, dynamic_cast<RunPageView*>(this));
	    else if(attr == "runWin" && !mainWin()->isResizeManual && (!mainWin()->masterPg() || this == mainWin()->masterPg())) {
		switch(s2i(val)) {
		    case 0:
			if(s2i(SYS->cmdOpt("showWin")))	break;
			mainWin()->aFullScr()->setChecked(false);
			mainWin()->setWindowState(Qt::WindowNoState);
			break;
		    case 1:
			if(s2i(SYS->cmdOpt("showWin")) > 1) break;
			mainWin()->aFullScr()->setChecked(false);
			mainWin()->setWindowState(Qt::WindowMaximized);
			break;
		    case 2:
			mainWin()->aFullScr()->setChecked(true);
			break;
		}
	    }
	    else if(attr == "keepAspectRatio")	mainWin()->setKeepAspectRatio(s2i(val));
	    else if(attr == "stBarNoShow")	mainWin()->statusBar()->setVisible(!s2i(val));
	    else if(attr == "winPosCntrSave")	mainWin()->setWinPosCntrSave(s2i(val));
	    else if(attr == "userSetVis") {
		if(val.size() && mainWin()->user() != TSYS::strDecode(TSYS::strParse(val,0,":"),TSYS::Custom) &&
		    val != property("userSetVis").toString().toStdString())
		{
		    setProperty("userSetVis", QString(val.c_str()));
		    mainWin()->userSel(val);
		} else setProperty("userSetVis", QString(val.c_str()));
	    }
	    else if(attr.find("notifyVis"+mod->modId()) == 0)
		mainWin()->ntfReg(s2i(attr.substr(9+mod->modId().size())), val, id(), true);
	    else if(attr.find("notify") == 0)	mainWin()->ntfReg(s2i(attr.substr(6)), val, id(), false);
	    else break;
	    return true;
	case A_PG_NAME:	setWindowTitle(val.c_str());	break;
	case A_PG_OPEN_SRC: setProperty("pgOpenSrc", val.c_str());	return true;
	case A_PG_GRP:	setProperty("pgGrp", val.c_str());		return true;
	case A_EN:	setProperty("isVisible", s2i(val) && (permView() || dynamic_cast<RunPageView*>(this)));	return true;
	case A_ACTIVE:	setProperty("active",(bool)s2i(val));	return true;
	case A_GEOM_Z:
	    if(!allAttrLoad() && !dynamic_cast<RunPageView*>(this)) {
		RunWdgView *wdg = qobject_cast<RunWdgView*>(parentWidget());
		if(wdg) { wdg->orderUpdate(); wdg->QWidget::update(); }
	    }
	    return true;
	case A_TIP_STATUS:
	    if(val.size() && mainWin()->masterPg() == this)
		mainWin()->statusBar()->showMessage(val.c_str(), 10000);
	    return true;
	case A_CTX_MENU: setProperty("contextMenu", val.c_str());	return true;
    }

    return rez;
}

string RunWdgView::resGet( const string &res )
{
    if(res.empty())	return "";
    string ret = mainWin()->cacheResGet(res);
    if(ret.empty() && !(ret=WdgView::resGet(res)).empty())
	mainWin()->cacheResSet(res,ret);

    return ret;
}

bool RunWdgView::isVisible( QPoint pos )
{
    if(!shape || !shape->needToVisibleCheck())	return true;
    if(!QRect(QPoint(),size()).contains(pos))	return false;

    //Clear background and draw transparent
    QPalette pltSave, plt;
    pltSave = plt = palette();
    plt.setBrush(QPalette::Window, QColor(0,0,0,0));
    setPalette(plt);

    //Grab widget and check it for no zero
#if QT_VERSION >= 0x050000
    bool rez = grab().toImage().pixel(pos);
#else
    bool rez = QPixmap::grabWidget(this).toImage().pixel(pos);
#endif

    setPalette(pltSave);

    return rez;
}

bool RunWdgView::event( QEvent *event )
{
    bool trToUnderlay = false;

    //Force event's process
    switch(event->type()) {
	case QEvent::Paint:
	    if(permView())	break;
	    //Paint message about access denied
	    if(dynamic_cast<RunPageView*>(this)) {
		QPainter pnt(this);
		// Fill page and draw border
		pnt.fillRect(rect(),QBrush(QColor("black"),Qt::Dense4Pattern));
		pnt.setPen(QPen(QBrush(QColor("black")),1));
		pnt.drawRect(rect().adjusted(0,0,-1,-1));
		// Draw message
		QTextOption to;
		pnt.setPen(QColor("red"));
		to.setAlignment(Qt::AlignCenter);
		to.setWrapMode(QTextOption::WordWrap);
		if(rect().width() > 500 && rect().height() > 100) {
		    QFont cfnt = pnt.font();
		    cfnt.setPointSize(16);
		    pnt.setFont(cfnt);
		}
		pnt.drawText(rect(),QString(_("Page: '%1'.\nNo access to view.")).arg(id().c_str()),to);
	    }
	    return true;
	case QEvent::MouseButtonPress:	trToUnderlay = true;	break;
	case QEvent::MouseButtonRelease:
	    if(((QMouseEvent*)event)->button() == Qt::RightButton && !property("contextMenu").toString().isEmpty() &&
		property("active").toBool() && permCntr() && isVisible(mapFromGlobal(cursor().pos())))
	    {
		QAction *actTmp;
		QMenu popup;
		string sln;
		for(int off = 0; (sln=TSYS::strSepParse(property("contextMenu").toString().toStdString(),0,'\n',&off)).size(); ) {
		    actTmp = new QAction(TSYS::strSepParse(sln,0,':').c_str(),this);
		    actTmp->setWhatsThis(TSYS::strSepParse(sln,1,':').c_str());
		    popup.addAction(actTmp);
		}
		if(!popup.isEmpty()) {
		    actTmp = popup.exec(QCursor::pos());
		    if(actTmp && !actTmp->whatsThis().isEmpty()) attrSet("event", "usr_"+actTmp->whatsThis().toStdString(), A_NO_ID, true);
		    popup.clear();
		    return true;
		}
	    }
	    trToUnderlay = true;
	    break;
	case QEvent::MouseButtonDblClick: trToUnderlay = true;	break;
	case QEvent::ToolTip:
	    if(isVisible(mapFromGlobal(cursor().pos()))) break;
	    trToUnderlay = true;
	    break;
	case QEvent::StatusTip:
	    if(isVisible(mapFromGlobal(cursor().pos()))) {
		mainWin()->statusBar()->showMessage(statusTip(), 10000);
		return true;
	    }
	    trToUnderlay = true;
	    break;
	default: break;
    }

    //Call to shape for event process
    if(WdgView::event(event) || (shape&&shape->event(this,event)))	return true;

    //Key events process for send to model
    string mod_ev, evs;
    AttrValS attrs;
    if(property("active").toBool() && permCntr())
    switch(event->type()) {
	case QEvent::Paint:	return true;
	case QEvent::KeyPress:
	    mod_ev = "key_pres";
	case QEvent::KeyRelease:
	    if(((QKeyEvent*)event)->key() == Qt::Key_Tab) { mod_ev = ""; break; }
	    if(mod_ev.empty()) mod_ev = "key_rels";
	    if(QApplication::keyboardModifiers()&Qt::ControlModifier)	mod_ev += "Ctrl";
	    if(QApplication::keyboardModifiers()&Qt::AltModifier)	mod_ev += "Alt";
	    if(QApplication::keyboardModifiers()&Qt::ShiftModifier)	mod_ev += "Shift";
	    if(((QKeyEvent*)event)->nativeScanCode()) evs = mod_ev+"SC#"+i2s(((QKeyEvent*)event)->nativeScanCode(),TSYS::Hex);
	    switch(((QKeyEvent*)event)->key()) {
		case Qt::Key_Escape:	mod_ev += "Esc";	break;
		case Qt::Key_Backspace:	mod_ev += "BackSpace";	break;
		case Qt::Key_Return:	mod_ev += "Return";	break;
		case Qt::Key_Enter:	mod_ev += "Enter";	break;
		case Qt::Key_Insert:	mod_ev += "Insert";	break;
		case Qt::Key_Delete:	mod_ev += "Delete";	break;
		case Qt::Key_Pause:	mod_ev += "Pause";	break;
		case Qt::Key_Print:	mod_ev += "Print";	break;
		//case Qt::Key_SysReq:	mod_ev += "SysReq";	break;
		//case Qt::Key_Clear:	mod_ev += "Clear";	break;
		case Qt::Key_Home:	mod_ev += "Home";	break;
		case Qt::Key_End:	mod_ev += "End";	break;
		case Qt::Key_Left:	mod_ev += "Left";	break;
		case Qt::Key_Up:	mod_ev += "Up";		break;
		case Qt::Key_Right:	mod_ev += "Right";	break;
		case Qt::Key_Down:	mod_ev += "Down";	break;
		case Qt::Key_PageUp:	mod_ev += "PageUp";	break;
		case Qt::Key_PageDown:	mod_ev += "PageDown";	break;
		case Qt::Key_F1:	mod_ev += "F1";		break;
		case Qt::Key_F2:	mod_ev += "F2";		break;
		case Qt::Key_F3:	mod_ev += "F3";		break;
		case Qt::Key_F4:	mod_ev += "F4";		break;
		case Qt::Key_F5:	mod_ev += "F5";		break;
		case Qt::Key_F6:	mod_ev += "F6";		break;
		case Qt::Key_F7:	mod_ev += "F7";		break;
		case Qt::Key_F8:	mod_ev += "F8";		break;
		case Qt::Key_F9:	mod_ev += "F9";		break;
		case Qt::Key_F10:	mod_ev += "F10";	break;
		case Qt::Key_F11:	mod_ev += "F11";	break;
		case Qt::Key_F12:	mod_ev += "F12";	break;
		case Qt::Key_F13:	mod_ev += "F13";	break;
		case Qt::Key_F14:	mod_ev += "F14";	break;
		case Qt::Key_F15:	mod_ev += "F15";	break;
		case Qt::Key_F16:	mod_ev += "F16";	break;
		case Qt::Key_F17:	mod_ev += "F17";	break;
		case Qt::Key_F18:	mod_ev += "F18";	break;
		case Qt::Key_F19:	mod_ev += "F19";	break;
		case Qt::Key_F20:	mod_ev += "F20";	break;
		case Qt::Key_F21:	mod_ev += "F21";	break;
		case Qt::Key_F22:	mod_ev += "F22";	break;
		case Qt::Key_F23:	mod_ev += "F23";	break;
		case Qt::Key_F24:	mod_ev += "F24";	break;
		case Qt::Key_F25:	mod_ev += "F25";	break;
		case Qt::Key_F26:	mod_ev += "F26";	break;
		case Qt::Key_F27:	mod_ev += "F27";	break;
		case Qt::Key_F28:	mod_ev += "F28";	break;
		case Qt::Key_F29:	mod_ev += "F29";	break;
		case Qt::Key_F30:	mod_ev += "F30";	break;
		case Qt::Key_F31:	mod_ev += "F31";	break;
		case Qt::Key_F32:	mod_ev += "F32";	break;
		case Qt::Key_F33:	mod_ev += "F33";	break;
		case Qt::Key_F34:	mod_ev += "F34";	break;
		case Qt::Key_F35:	mod_ev += "F35";	break;
		case Qt::Key_Space:	mod_ev += "Space";	break;
		case Qt::Key_Apostrophe:mod_ev += "Apostrophe";	break;
		case Qt::Key_Asterisk:	mod_ev += "Asterisk";	break;
		case Qt::Key_Plus:	mod_ev += "Plus";	break;
		case Qt::Key_Comma:	mod_ev += "Comma";	break;
		case Qt::Key_Minus:	mod_ev += "Minus";	break;
		case Qt::Key_Period:	mod_ev += "Period";	break;
		case Qt::Key_Slash:	mod_ev += "Slash";	break;
		case Qt::Key_0:		mod_ev += "0";		break;
		case Qt::Key_1:		mod_ev += "1";		break;
		case Qt::Key_2:		mod_ev += "2";		break;
		case Qt::Key_3:		mod_ev += "3";		break;
		case Qt::Key_4:		mod_ev += "4";		break;
		case Qt::Key_5:		mod_ev += "5";		break;
		case Qt::Key_6:		mod_ev += "6";		break;
		case Qt::Key_7:		mod_ev += "7";		break;
		case Qt::Key_8:		mod_ev += "8";		break;
		case Qt::Key_9:		mod_ev += "9";		break;
		case Qt::Key_Semicolon:	mod_ev += "Semicolon";	break;
		case Qt::Key_Equal:	mod_ev += "Equal";	break;
		case Qt::Key_A:		mod_ev += "A";		break;
		case Qt::Key_B:		mod_ev += "B";		break;
		case Qt::Key_C:		mod_ev += "C";		break;
		case Qt::Key_D:		mod_ev += "D";		break;
		case Qt::Key_E:		mod_ev += "E";		break;
		case Qt::Key_F:		mod_ev += "F";		break;
		case Qt::Key_G:		mod_ev += "G";		break;
		case Qt::Key_H:		mod_ev += "H";		break;
		case Qt::Key_I:		mod_ev += "I";		break;
		case Qt::Key_J:		mod_ev += "J";		break;
		case Qt::Key_K:		mod_ev += "K";		break;
		case Qt::Key_L:		mod_ev += "L";		break;
		case Qt::Key_M:		mod_ev += "M";		break;
		case Qt::Key_N:		mod_ev += "N";		break;
		case Qt::Key_O:		mod_ev += "O";		break;
		case Qt::Key_P:		mod_ev += "P";		break;
		case Qt::Key_Q:		mod_ev += "Q";		break;
		case Qt::Key_R:		mod_ev += "R";		break;
		case Qt::Key_S:		mod_ev += "S";		break;
		case Qt::Key_T:		mod_ev += "T";		break;
		case Qt::Key_U:		mod_ev += "U";		break;
		case Qt::Key_V:		mod_ev += "V";		break;
		case Qt::Key_W:		mod_ev += "W";		break;
		case Qt::Key_X:		mod_ev += "X";		break;
		case Qt::Key_Y:		mod_ev += "Y";		break;
		case Qt::Key_Z:		mod_ev += "Z";		break;
		case Qt::Key_BracketLeft: mod_ev += "BracketLeft"; break;
		case Qt::Key_Backslash:	mod_ev += "BackSlash";	break;
		case Qt::Key_BracketRight: mod_ev += "BracketRight"; break;
		case Qt::Key_QuoteLeft:	mod_ev += "QuoteLeft";	break;
		default:
		    mod_ev += "#"+i2s(((QKeyEvent*)event)->key(), TSYS::Hex);
		    break;
	    }
	    evs += (evs.size()?"\n":"")+mod_ev;
	    attrs.push_back(std::make_pair("event",evs));
	    attrsSet(attrs);
	    return true;
	case QEvent::MouseButtonPress:
	    mod_ev = "key_mousePres";
	case QEvent::MouseButtonRelease:
	    if(mod_ev.empty()) mod_ev = "key_mouseRels";
	    switch(((QMouseEvent*)event)->button()) {
		case Qt::LeftButton:	mod_ev += "Left";	break;
		case Qt::RightButton:	mod_ev += "Right";	break;
		case Qt::MidButton:	mod_ev += "Midle";	break;
		default: break;
	    }
	    if(isVisible(mapFromGlobal(cursor().pos()))) {
		if(event->type() == QEvent::MouseButtonPress && !hasFocus()) setFocus(Qt::MouseFocusReason);
		attrSet("event", mod_ev, A_NO_ID, true);
		return true;
	    }
	    break;
	case QEvent::MouseButtonDblClick:
	    if(isVisible(mapFromGlobal(cursor().pos()))) {
		attrSet("event", "key_mouseDblClick", A_NO_ID, true);
		return true;
	    }
	    break;
	case QEvent::FocusIn:	mainWin()->setFocus(id());	return true;
	    /*attrs.push_back(std::make_pair("focus","1"));
	    attrs.push_back(std::make_pair("event","ws_FocusIn"));
	    attrsSet(attrs);
	    return true;
	case QEvent::FocusOut:
	    attrs.push_back(std::make_pair("focus","0"));
	    attrs.push_back(std::make_pair("event","ws_FocusOut"));
	    attrsSet(attrs);
	    return true;*/
	default: break;
    }

    //Try put mouse event to next level widget into this container
    if(!qobject_cast<RunPageView*>(this) && trToUnderlay) {
	//(event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::MouseButtonDblClick))
	bool isOk = false;
	QPoint curp = parentWidget()->mapFromGlobal(cursor().pos());
	for(int iC = parentWidget()->children().size()-1; iC >= 0; iC--) {
	    RunWdgView *curwdg = qobject_cast<RunWdgView*>(parentWidget()->children().at(iC));
	    if(!curwdg) continue;
	    if(curwdg == this) isOk = true;
	    else if(isOk && curwdg->geometry().contains(curp)) {
		RunWdgView *wdg = curwdg;
		curp = wdg->mapFromGlobal(cursor().pos());
		for(int iCr = wdg->children().size()-1; iCr >= 0; iCr--) {
		    curwdg = qobject_cast<RunWdgView*>(wdg->children().at(iCr));
		    if(curwdg && curwdg->geometry().contains(curp)) {
			wdg = curwdg;
			iCr = wdg->children().size();
			curp = wdg->mapFromGlobal(cursor().pos());
		    }
		}
		return QApplication::sendEvent(wdg, event);
	    }
	}
	return QApplication::sendEvent(parentWidget(), event);
    }

    return QWidget::event(event);
}

//*************************************************
//* Shape page view runtime mode                  *
//*************************************************
RunPageView::RunPageView( const string &iwid, VisRun *mainWind, QWidget* parent, Qt::WindowFlags f ) :
    RunWdgView(iwid, 0, mainWind, parent, f), wx_scale(1), wy_scale(1)
{
    if(!mainWind->master_pg) mainWind->master_pg = this;

    //resize(50, 50);
    load("");

    //Restore external window position
    string xPos, yPos;
    if(mainWin()->winPosCntrSave()) {
	if(f == Qt::Tool && (xPos=mainWin()->wAttrGet(id(),i2s(mainWin()->screen())+"geomX",true)).size() &&
		(yPos=mainWin()->wAttrGet(id(),i2s(mainWin()->screen())+"geomY",true)).size())
	    move(s2i(xPos), s2i(yPos));
	else if(abs(posF().x()) || abs(posF().y())) move(posF().x(), posF().y());
    }

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:RunPageView", 1);
}

RunPageView::~RunPageView( )
{
    //Child widgets remove before
    childsClear();

    if(mess_lev() == TMess::Debug) SYS->cntrIter("UI:Vision:RunPageView", -1);
}

float RunPageView::xScale( bool full )	{ return full ? mainWin()->xScale()*WdgView::xScale() : WdgView::xScale(); }

float RunPageView::yScale( bool full )	{ return full ? mainWin()->yScale()*WdgView::yScale() : WdgView::yScale(); }

RunPageView *RunPageView::parent( )	{ return qobject_cast<RunPageView*>(parentWidget()); }

RunPageView *RunPageView::findOpenPage( const string &ipg )
{
    RunPageView *pg;

    //Self check
    if(id() == ipg) return this;

    //Check to included widgets
    for(int iCh = 0; iCh < children().size(); iCh++) {
	if(qobject_cast<RunPageView*>(children().at(iCh))) {
	    pg = ((RunPageView*)children().at(iCh))->findOpenPage(ipg);
	    if(pg) return pg;
	    continue;
	}
	if(!qobject_cast<RunWdgView*>(children().at(iCh)))	continue;
	RunWdgView *rwdg = (RunWdgView*)children().at(iCh);
	if(rwdg->property("isVisible").toBool() && rwdg->root() == "Box") {
	    if(rwdg->pgOpenSrc() == ipg && !rwdg->property("inclPg").toString().isEmpty())
		return (RunPageView*)TSYS::str2addr(rwdg->property("inclPg").toString().toStdString());
	    if(((ShapeBox::ShpDt*)rwdg->shpData)->inclPg) {
		pg = ((ShapeBox::ShpDt*)rwdg->shpData)->inclPg->findOpenPage(ipg);
		if(pg) return pg;
	    }
	}
    }

    return NULL;
}

bool RunPageView::callPage( const string &pg_it, const string &pgGrp, const string &pgSrc )
{
    //Check for set include page
    for(int iCh = 0; iCh < children().size(); iCh++)
	if(!pgGrp.empty() && !qobject_cast<RunPageView*>(children().at(iCh)) &&
		((RunWdgView*)children().at(iCh))->property("isVisible").toBool() && ((RunWdgView*)children().at(iCh))->root() == "Box")
	{
	    if(((RunWdgView*)children().at(iCh))->pgGrp() == pgGrp) {
		string pg_it_prev = ((RunWdgView*)children().at(iCh))->pgOpenSrc();
		if(pg_it != pg_it_prev) {
		    if(!pg_it_prev.empty()) {
			XMLNode req("close");
			req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",pg_it_prev);
			mainWin()->cntrIfCmd(req);
		    }
		    ((RunWdgView*)children().at(iCh))->setPgOpenSrc(pg_it);
		}
		return true;
	    }
	    if(((ShapeBox::ShpDt*)((RunWdgView*)children().at(iCh))->shpData)->inclPg &&
		    ((ShapeBox::ShpDt*)((RunWdgView*)children().at(iCh))->shpData)->inclPg->callPage(pg_it,pgGrp,pgSrc))
		return true;
        }
    //Put checking to self include pages
    for(int iCh = 0; iCh < children().size(); iCh++)
	if(qobject_cast<RunPageView*>(children().at(iCh)) && ((RunPageView *)children().at(iCh))->callPage(pg_it,pgGrp,pgSrc))
	    return true;
    //Check for open child page or for unknown and empty source pages open as master page child windows
    if((pgGrp.empty() && pgSrc == id()) || this == mainWin()->master_pg) {
	RunPageView *pg = new RunPageView(pg_it, mainWin(), this, Qt::Tool);
	//pg->setAttribute(Qt::WA_DeleteOnClose);
	//pg->load("");
	//pg->moveF(QCursor::pos());
	//pg->moveF(QPointF(mapToGlobal(pos()).x()+sizeF().width()/2-pg->sizeF().width()/2,
	//		  mapToGlobal(pos()).y()+sizeF().height()/2-pg->sizeF().height()/2));
	pg->setMinimumSize(pg->frameGeometry().size());
	pg->setMaximumSize(pg->frameGeometry().size());
	pg->setWindowState(pg->windowState() | Qt::WindowActive);

	// Get the page name
	/*XMLNode req("get");
	req.setAttr("path",pg->id()+"/%2fwdg%2fcfg%2fname");
	if( !mainWin()->cntrIfCmd(req) ) pg->setWindowTitle(req.text().c_str());
	else pg->setWindowTitle(mainWin()->windowTitle());*/
	if(pg->windowTitle().isEmpty()) pg->setWindowTitle(mainWin()->windowTitle());

	return true;
    }

    return false;
}

void RunPageView::closeEvent( QCloseEvent *event )
{
    //Save curent position
    if(mainWin()->winPosCntrSave()) {
	mainWin()->wAttrSet(id(), i2s(mainWin()->screen())+"geomX", i2s(pos().x()), true);
	mainWin()->wAttrSet(id(), i2s(mainWin()->screen())+"geomY", i2s(pos().y()), true);
    }

    //Send close command
    XMLNode req("close");
    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",id());
    mainWin()->cntrIfCmd(req);

    //Close included pages
    /*for(int iCh = 0; iCh < children().size(); iCh++)
	if(!qobject_cast<RunPageView*>(children().at(iCh)) && ((RunWdgView *)children().at(iCh))->root() == "Box" &&
		!((RunWdgView*)children().at(iCh))->pgOpenSrc().empty())
	{
	    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fserv%2fpg")->setAttr("pg",((RunWdgView*)children().at(iCh))->pgOpenSrc());
	    mainWin()->cntrIfCmd(req);
	}*/

    //Freeing the notificators configuration for the page
    mainWin()->ntfReg(-1, "", id());
}

void RunPageView::toPgCache( )
{
    //Freeing the notificators configuration for the page
    mainWin()->ntfReg(-1, "", id());
}

//*********************************************
//* Status bar styles                         *
//*********************************************
StylesStBar::StylesStBar( int styleId, QWidget *parent ) : QLabel(parent), mStyle(-1)
{
    setStyle(styleId);
}

VisRun *StylesStBar::mainWin( )	{ return (VisRun *)window(); }

void StylesStBar::setStyle( int istl, const string &nm )
{
    mStyle = istl;
    if(mStyle < 0) setText(_("<Disabled>"));
    else if(!nm.empty()) setText(nm.c_str());
    else {
	XMLNode req("get");
	req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fobj%2fcfg%2fstLst");
	mainWin()->cntrIfCmd(req);
	for(unsigned iS = 0; iS < req.childSize(); iS++)
	    if(s2i(req.childGet(iS)->attr("id")) == istl)
		setText(req.childGet(iS)->text().c_str());
    }
}

bool StylesStBar::styleSel( )
{
    //Get syles list
    XMLNode req("get");
    req.setAttr("path","/ses_"+mainWin()->workSess()+"/%2fobj%2fcfg%2fstLst");
    mainWin()->cntrIfCmd(req);

    if(req.childSize() <= 1) return false;

    InputDlg dlg(this, mainWin()->windowIcon(), _("Select your style from the list."), _("Selecting a style"), false, false, mainWin()->lang());
    QLabel *lab = new QLabel(_("Style:"),&dlg);
    lab->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    dlg.edLay()->addWidget(lab, 0, 0);
    QComboBox *stls = new QComboBox(&dlg);
    dlg.edLay()->addWidget(stls, 0, 1);
    for(unsigned iS = 0, iSls = 0; iS < req.childSize(); iS++) {
	if(s2i(req.childGet(iS)->attr("id")) < 0) continue;
	stls->addItem(req.childGet(iS)->text().c_str(), s2i(req.childGet(iS)->attr("id")));
	if(s2i(req.childGet(iS)->attr("id")) == style()) stls->setCurrentIndex(iSls);
	iSls++;
    }
    dlg.resize(300, 120);
    if(dlg.exec() == QDialog::Accepted && stls->currentIndex() >= 0) {
	setStyle(stls->itemData(stls->currentIndex()).toInt(), stls->itemText(stls->currentIndex()).toStdString());
	emit styleChanged();
	return true;
    }

    return false;
}

bool StylesStBar::event( QEvent *event )
{
    if(event->type() == QEvent::MouseButtonDblClick)	styleSel();
    return QLabel::event(event);
}

//*********************************************
//* User's status bar item                    *
//*********************************************
UserItStBar::UserItStBar( QWidget *parent ) : QLabel(parent)
{

}

bool UserItStBar::event( QEvent *event )
{
    string mod_ev, objId = objectName().toStdString();
    if(objId.compare(0,4,"usr_") == 0) objId = objId.substr(4);
    VisRun *w = dynamic_cast<VisRun*>(parentWidget()->window());

    switch(event->type()) {
	case QEvent::MouseButtonPress:
	    mod_ev = "key_mousePres";
	case QEvent::MouseButtonRelease:
	    if(mod_ev.empty()) mod_ev = "key_mouseRels";
	    switch(((QMouseEvent*)event)->button()) {
		case Qt::LeftButton:	mod_ev += "Left";	break;
		case Qt::RightButton:	mod_ev += "Right";	break;
		case Qt::MidButton:	mod_ev += "Midle";	break;
		default: break;
	    }
	    if(w && w->masterPg()) { w->masterPg()->attrSet("event", mod_ev+":/stIt_"+objId, A_NO_ID, true); return true; }
	    break;
	case QEvent::MouseButtonDblClick:
	    if(w && w->masterPg()) { w->masterPg()->attrSet("event", "key_mouseDblClick:/stIt_"+objId, A_NO_ID, true); return true; }
	    break;
	default: break;
    }

    return QLabel::event(event);
}

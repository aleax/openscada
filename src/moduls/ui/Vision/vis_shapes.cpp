
//OpenSCADA system module UI.Vision file: vis_shapes.cpp
/***************************************************************************
 *   Copyright (C) 2007-2017 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>

#include <algorithm>

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QMovie>
#include <QBuffer>
#include <QPicture>
#include <QTimer>
#include <QKeyEvent>
#include <QTableWidget>
#include <QDateTime>
#include <QToolTip>
#include <QScrollBar>
#include <QHeaderView>
#if QT_VERSION < 0x050000
#include <QPlastiqueStyle>
#else
#include <QCommonStyle>
#include <qdrawutil.h>
#endif

#include <tsys.h>
#include "tvision.h"
#include "vis_devel.h"
#include "vis_run.h"
#include "vis_run_widgs.h"
#include "vis_devel_widgs.h"
#include "vis_shapes.h"

#ifdef HAVE_PHONON
#ifdef HAVE_PHONON_VIDEOPLAYER
#include <phonon/VideoPlayer>
#include <phonon/VideoWidget>
#else
#include <Phonon/VideoPlayer>
#include <Phonon/VideoWidget>
#endif
#endif


using namespace VISION;

//*************************************************
//* Widget shape abstract object                  *
//*************************************************
WdgShape::WdgShape( const string &iid ) : m_id(iid)
{

}

bool WdgShape::event( WdgView *view, QEvent *event )
{
    switch(event->type()) {
	case QEvent::Paint:
	    if(qobject_cast<DevelWdgView*>(view)) {
		QPainter pnt( view );
		pnt.setWindow(view->rect());
		pnt.drawImage(view->rect(),QImage(":/images/attention.png"));
		event->accept();
		view->setToolTip(QString(_("Widget's shape '%1' is not implemented yet!")).arg(id().c_str()));
	    }
	    return true;
	default: break;
    }

    return false;
}

void WdgShape::borderDraw( QPainter &pnt, QRect dA, QPen bpen, int bordStyle )
{
    int bordWidth = bpen.width();
    if(bordStyle && bordWidth)
    switch(bordStyle) {
	case FBRD_DOT:
	    bpen.setStyle(Qt::DotLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case FBRD_DASH:
	    bpen.setStyle(Qt::DashLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case FBRD_SOL:
	    bpen.setStyle(Qt::SolidLine);
	    pnt.setPen(bpen);
	    pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    break;
	case FBRD_DBL:
	    bpen.setStyle(Qt::SolidLine);
	    if(bordWidth/3) {
		int brdLnSpc = bordWidth-2*(bordWidth/3);
		bordWidth/=3;
		bpen.setWidth(bordWidth);
		pnt.setPen(bpen);
		pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
		pnt.drawRect(dA.adjusted(bordWidth+brdLnSpc+bordWidth/2,bordWidth+brdLnSpc+bordWidth/2,
			-bordWidth-brdLnSpc-bordWidth/2-bordWidth%2,-bordWidth-brdLnSpc-bordWidth/2-bordWidth%2));
	    }
	    else {
	        pnt.setPen(bpen);
	        pnt.drawRect(dA.adjusted(bordWidth/2,bordWidth/2,-bordWidth/2-bordWidth%2,-bordWidth/2-bordWidth%2));
	    }
	    break;
	case FBRD_GROOVE: {
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadeRect(&pnt,dA,plt,true,bordWidth/2);
	    break;
	}
	case FBRD_RIDGE: {
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadeRect(&pnt,dA,plt,false,bordWidth/2);
	    break;
	}
	case FBRD_INSET: {
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadePanel(&pnt,dA,plt,true,bordWidth);
	    break;
	}
	case FBRD_OUTSET: {
	    QPalette plt;
	    plt.setColor(QPalette::Light,bpen.color().lighter(150));
	    plt.setColor(QPalette::Dark,bpen.color().lighter(50));
	    qDrawShadePanel(&pnt,dA,plt,false,bordWidth);
	    break;
	}
    }
}

bool WdgShape::attrSet( WdgView *view, int uiPrmPos, const string &val, const string &attr )	{ return false; }

QFont WdgShape::getFont( const string &val, float fsc, bool pixSize, const QFont &defFnt )
{
    QFont rez = defFnt;

    char family[101]; family[0] = 0; //strcpy(family,"Arial");
    int size = -1, bold = -1, italic = -1, underline = -1, strike = -1;
    sscanf(val.c_str(), "%100s %d %d %d %d %d", family, &size, &bold, &italic, &underline, &strike);
    if(strlen(family)) rez.setFamily(QString(family).replace(QRegExp("_")," "));
    if(size >= 0) {
	if(pixSize) rez.setPixelSize((int)(fsc*(float)size));
	else rez.setPointSize((int)(fsc*(float)size));
    }
    if(bold >= 0)	rez.setBold(bold);
    if(italic >= 0)	rez.setItalic(italic);
    if(underline >= 0)	rez.setUnderline(underline);
    if(strike >= 0)	rez.setStrikeOut(strike);

    return rez;
}

QColor WdgShape::getColor( const string &val )
{
    QColor res_color;
    size_t fPs = val.find("-");
    if(fPs == string::npos) res_color = QColor(val.c_str());
    else {
	res_color = QColor(val.substr(0,fPs).c_str());
	res_color.setAlpha(s2i(val.substr(fPs+1)));
    }
    return res_color;
}

//============ Support widget's shapes ============

//*************************************************
//* Form element shape widget                     *
//*************************************************
ShapeFormEl::ShapeFormEl( ) : WdgShape("FormEl")
{

}

void ShapeFormEl::init( WdgView *w )
{
    w->shpData = new ShpDt();
    new QVBoxLayout(w);
}

void ShapeFormEl::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeFormEl::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    DevelWdgView *devW = qobject_cast<DevelWdgView*>(w);
    RunWdgView   *runW = qobject_cast<RunWdgView*>(w);

    bool rel_cfg = false;	//Reload configuration

    shD->evLock = true;
    switch(uiPrmPos) {
	case A_COM_LOAD:	rel_cfg = true;	break;
	case A_EN:
	    if(!runW)	break;
	    shD->en = (bool)s2i(val);
	    runW->setVisible(shD->en && runW->permView());
	    if(shD->elType >= 0) shD->addrWdg->setVisible(shD->en && runW->permView());
	    break;
	case A_ACTIVE:
	    if(!runW)	break;
	    shD->active = (bool)s2i(val);
	    setActive(w, shD->active && runW->permCntr());
	    break;
	case A_GEOM_W: case A_GEOM_X_SC: rel_cfg = (shD->elType==F_TABLE);	break;
	case A_GEOM_MARGIN:
	    w->layout()->setMargin(s2i(val));
	    rel_cfg = (shD->elType==F_BUTTON);
	    break;
	case A_FormElType:
	    if(shD->elType == s2i(val)) break;
	    shD->elType = s2i(val);
	    shD->setType = false;
	    rel_cfg = true;
	    break;
	case A_FormElValue:
	    //if(shD->value != val)	//For prevent to possibility the value difference into the widget and the model.
					//Check for equal into setValue()!
	    setValue(w, val);
	    break;
	case A_FormElName:
	    shD->name = TSYS::strEncode(val, TSYS::ShieldSimb);
	    if(!shD->setType) break;
	    switch(shD->elType) {
		case F_CHECK_BOX:	((QCheckBox*)shD->addrWdg)->setText(shD->name.c_str());		break;
		case F_BUTTON:		((QPushButton*)shD->addrWdg)->setText(shD->name.c_str());	break;
		case F_TREE:
		    ((QTreeWidget*)shD->addrWdg)->setHeaderLabels(QStringList() << shD->name.c_str());
		    ((QTreeWidget*)shD->addrWdg)->headerItem()->setHidden(!shD->name.size());
		    break;
	    }
	    break;
	case A_FormElMixP1:
	    rel_cfg = true;
	    switch(shD->elType) {
		case F_LINE_ED:	shD->view = s2i(val);		break;
		case F_TEXT_ED:	shD->opt1 = s2i(val);	break;
		case F_BUTTON:	shD->img = val;		break;
		case F_COMBO: case F_LIST: case F_TREE: case F_TABLE: shD->items = val;	break;
		case F_SLIDER: case F_SCROLL_BAR: shD->cfg = val;		break;
		default: rel_cfg = false;
	    }
	    break;
	case A_FormElMixP2:
	    rel_cfg = true;
	    switch(shD->elType) {
		case F_LINE_ED:	shD->cfg = val;		break;
		case F_BUTTON:	shD->color = val;	break;
		case F_LIST:	shD->opt1 = s2i(val);	break;
		default: rel_cfg = false;
	    }
	    break;
	case A_FormElMixP3: shD->mode = s2i(val); rel_cfg = true;	break;
	case A_FormElFont: shD->font = getFont(val); rel_cfg = true;	break;
	case A_FormElMixP4: shD->colorText = val; rel_cfg = true;	break;
    }
    if(rel_cfg && !w->allAttrLoad()) {
	bool mk_new = false;
	Qt::Alignment wAlign = 0;
	QFont elFnt = shD->font;
	if(elFnt.pixelSize() > 0) elFnt.setPixelSize((int)((float)elFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true))));

	switch(shD->elType) {
	    case F_LINE_ED: {
		LineEdit *wdg = (LineEdit*)shD->addrWdg;
		if(!wdg || !qobject_cast<LineEdit*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new LineEdit(w, LineEdit::Text, !shD->mode);
		    if(runW) connect(wdg, SIGNAL(apply()), this, SLOT(lineAccept()));
		    mk_new = true;
		}
		//View
		LineEdit::LType tp = LineEdit::Text;
		switch(shD->view) {
		    case FL_TEXT:	tp = LineEdit::Text;	break;
		    case FL_COMBO:	tp = LineEdit::Combo;	break;
		    case FL_INTEGER:	tp = LineEdit::Integer;	break;
		    case FL_REAL:	tp = LineEdit::Real;	break;
		    case FL_TIME:	tp = LineEdit::Time;	break;
		    case FL_DATE:	tp = LineEdit::Date;	break;
		    case FL_DATE_TM:	tp = LineEdit::DateTime;break;
		}
		if(wdg->type() != tp) { wdg->setType(tp); mk_new = true; }
		wdg->setCfg(shD->cfg.c_str());	//Cfg
		wdg->setFont(elFnt);		//Font
		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_TEXT_ED: {
		TextEdit *wdg = (TextEdit*)shD->addrWdg;
		if(!wdg || !qobject_cast<TextEdit*>(wdg) || (runW && wdg->lang != ((VisRun*)runW->window())->lang())) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new TextEdit(w, !shD->mode);
		    if(runW) connect(wdg, SIGNAL(apply()), this, SLOT(textAccept()));
		    mk_new = true;
		}
		wdg->setFont(elFnt);		//Font
		setValue(w, shD->value, true);	//Value
		wdg->workWdg()->setLineWrapMode(shD->opt1 ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);	//WordWrap
		break;
	    }
	    case F_CHECK_BOX: {
		QCheckBox *wdg = (QCheckBox*)shD->addrWdg;
		if(!wdg || !qobject_cast<QCheckBox*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QCheckBox("test", w);
		    if(runW) connect(wdg, SIGNAL(stateChanged(int)), this, SLOT(checkChange(int)));
		    mk_new = true;
		}
		wdg->setFont(elFnt);		//Font
		wdg->setText(shD->name.c_str());//Name
		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_BUTTON: {
		QPushButton *wdg = (QPushButton*)shD->addrWdg;
		if(!wdg || !qobject_cast<QPushButton*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QPushButton("test", w);
#if QT_VERSION < 0x050000
		    wdg->setStyle(new QPlastiqueStyle());
#else
		    wdg->setStyle(new QCommonStyle());
#endif
		    wdg->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
		    if(runW) {
			connect(wdg, SIGNAL(pressed()), this, SLOT(buttonPressed()));
			connect(wdg, SIGNAL(released()), this, SLOT(buttonReleased()));
			connect(wdg, SIGNAL(toggled(bool)), this, SLOT(buttonToggled(bool)));
		    }
		    mk_new = true;
		}
		wdg->setText(shD->name.c_str());	//Name
		//Img
		QImage img;
		string backimg = w->resGet(shD->img);
		if(!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size())) {
		    int icSzW = w->width() - w->layout()->margin();
		    int icSzH = w->height() - w->layout()->margin();
		    img = img.scaled(w->width()-w->layout()->margin(), w->height()-w->layout()->margin(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

		    wdg->setIconSize(QSize(icSzW,icSzH));
		    wdg->setIcon(QPixmap::fromImage(img));
		} else wdg->setIcon(QPixmap());
		//Color
		QPalette plt;
		QColor clr = getColor(shD->color);
		if(clr.isValid())	plt.setColor(QPalette::Button, clr);
		clr = getColor(shD->colorText);
		if(clr.isValid())	plt.setColor(QPalette::ButtonText, clr);
		wdg->setPalette(plt);
		wdg->setFont(elFnt);				//Font
		setValue(w, shD->value, true);			//Value
		break;
	    }
	    case F_COMBO: {
		QComboBox *wdg = (QComboBox*)shD->addrWdg;
		if(!wdg || !qobject_cast<QComboBox*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QComboBox(w);
		    if(runW) connect(wdg, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(comboChange(const QString&)));
		    mk_new = true;
		}
		//Font
		wdg->setFont(elFnt);
		//Items
		wdg->clear();
		wdg->addItems(QString(shD->items.c_str()).split("\n"));
		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_LIST: {
		QListWidget *wdg = (QListWidget*)shD->addrWdg;
		if(!wdg || !qobject_cast<QListWidget*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QListWidget(w);
		    if(runW) connect(wdg, SIGNAL(itemSelectionChanged()), this, SLOT(listChange()));
		    mk_new = true;
		}
		wdg->setSelectionMode(shD->opt1?QAbstractItemView::ExtendedSelection:QAbstractItemView::SingleSelection);
		//Items
		wdg->clear();
		wdg->addItems(QString(shD->items.c_str()).split("\n"));

		wdg->setFont(elFnt);		//Font
		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_TREE: {
		QTreeWidget *wdg = (QTreeWidget*)shD->addrWdg;
		if(!wdg || !qobject_cast<QTreeWidget*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QTreeWidget(w);
		    wdg->setAlternatingRowColors(true);
		    wdg->setSelectionMode(QAbstractItemView::SingleSelection);
		    wdg->setSelectionBehavior(QAbstractItemView::SelectRows);
		    wdg->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		    wdg->setHeaderLabels(QStringList() << shD->name.c_str());
		    wdg->headerItem()->setHidden(!shD->name.size());
		    if(runW) connect(wdg, SIGNAL(itemSelectionChanged()), this, SLOT(treeChange()));
		    mk_new = true;
		}
		//Items
		wdg->clear();
		string ipath, item;
		QTreeWidgetItem *cur_it = NULL, *t_it = NULL;
		shD->addrWdg->blockSignals(true);
		QFont noSelFnt = elFnt;
		noSelFnt.setItalic(true);
		for(int off = 0; (ipath=TSYS::strLine(shD->items,0,&off)).size(); ) {
		    for(int off1 = 0, lev = 0; (item=TSYS::pathLev(ipath,0,true,&off1)).size(); lev++)
			if(lev == 0) {
			    cur_it = NULL;
			    for(int iR = 0; !cur_it && iR < wdg->topLevelItemCount(); iR++)
				if(wdg->topLevelItem(iR)->text(0) == item.c_str()) cur_it = wdg->topLevelItem(iR);
			    if(!cur_it) {
				cur_it = new QTreeWidgetItem(0);
				cur_it->setText(0, item.c_str());
				cur_it->setFlags(cur_it->flags()&(~Qt::ItemIsSelectable));
				cur_it->setData(0, Qt::FontRole, noSelFnt);
				wdg->addTopLevelItem(cur_it);
			    }
			}
			else {
			    t_it = NULL;
			    for(int iR = 0; !t_it && iR < cur_it->childCount(); iR++)
				if(cur_it->child(iR)->text(0) == item.c_str())	t_it = cur_it->child(iR);
			    if(!t_it) {
				t_it = new QTreeWidgetItem(cur_it);
				t_it->setText(0, item.c_str());
				t_it->setFlags(cur_it->flags()&(~Qt::ItemIsSelectable));
				t_it->setData(0, Qt::FontRole, noSelFnt);
			    }
			    cur_it = t_it;
			}
		    if(cur_it) {
			cur_it->setFlags(cur_it->flags()|Qt::ItemIsSelectable);
			cur_it->setData(0, Qt::FontRole, elFnt);
			cur_it->setData(0, Qt::UserRole, ipath.c_str());
		    }
		}
		shD->addrWdg->blockSignals(false);

		wdg->setFont(elFnt);		//Font
		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_TABLE: {
		QTableWidget *wdg = (QTableWidget*)shD->addrWdg;
		if(!wdg || !qobject_cast<QTableWidget*>(wdg)) {
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = new QTableWidget(w);
		    wdg->setAlternatingRowColors(true);
		    wdg->setSelectionMode(QAbstractItemView::SingleSelection);
		    wdg->setSelectionBehavior(QAbstractItemView::SelectItems);
		    wdg->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
		    if(runW) {
			connect(wdg, SIGNAL(itemSelectionChanged()), this, SLOT(tableSelectChange()));
			connect(wdg, SIGNAL(cellChanged(int,int)), this, SLOT(tableChange(int,int)));
		    }
		    mk_new = true;
		}

		wdg->setFont(elFnt);		//Font

		//Items
		shD->addrWdg->blockSignals(true);
		XMLNode tX("tbl");
		bool hdrPresent = false, colsWdthFit = false;
		int maxCols = 0, maxRows = 0;
		try { tX.load(shD->items); } catch(...) { }
		if(tX.name() != "tbl") {
		    wdg->clear();
		    wdg->horizontalHeader()->setVisible(false);
		    wdg->verticalHeader()->setVisible(false);
		}
		else {
		    if(wdg->isSortingEnabled()) {
			wdg->setSortingEnabled(false);
			//wdg->clear();
			//wdg->setColumnCount(0);
			wdg->setRowCount(0);
		    }
		    string wVl, rClr, rClrTxt, rFnt;
		    int sortCol = 0;
		    // Items
		    for(unsigned iR = 0, iRR = 0, iCh = 0; iCh < tX.childSize() || (int)iR < wdg->rowCount(); iCh++) {
			XMLNode *tR = (iCh < tX.childSize()) ? tX.childGet(iCh) : NULL;
			bool isH = false;
			QTableWidgetItem *hit = NULL, *tit = NULL;
			if(tR && !((isH=(tR->name()=="h")) || tR->name() == "r")) continue;
			if(!isH && (int)iR >= wdg->rowCount()) wdg->setRowCount(iR+1);
			if(!isH && tR) { rClr = tR->attr("color"); rClrTxt = tR->attr("colorText"); rFnt = tR->attr("font"); }
			for(unsigned iC = 0, iCR = 0, iCh1 = 0; (tR && iCh1 < tR->childSize()) ||
								    (int)iC < wdg->columnCount(); iCh1++)
			{
			    XMLNode *tC = (tR && iCh1 < tR->childSize()) ? tR->childGet(iCh1) : NULL;
			    if(tC && (int)iC >= wdg->columnCount()) wdg->setColumnCount(iC+1);
			    if(!(hit=wdg->horizontalHeaderItem(iC))) wdg->setHorizontalHeaderItem(iC, (hit=new QTableWidgetItem()));
			    if(isH) {	//Header process
				hit->setText(tC?tC->text().c_str():"");
				if(tC) {
				    if((wVl=tC->attr("width")).size()) {
					int wdthCel = fmax(1, s2i(wVl));
					hit->setData(Qt::UserRole,
					    (wVl.find("%") == wVl.size()-1) ? w->size().width()*wdthCel/100 : wdthCel*w->xScale(true));
				    } else hit->setData(Qt::UserRole, QVariant());
				    hit->setData(Qt::UserRole+1, (bool)s2i(tC->attr("edit")));
				    hit->setData(Qt::UserRole+2, ((wVl=tC->attr("color")).size()) ? QString::fromStdString(wVl) : QVariant());
				    hit->setData(Qt::UserRole+3, ((wVl=tC->attr("colorText")).size()) ? QString::fromStdString(wVl) : QVariant());
				    hit->setData(Qt::UserRole+4, ((wVl=tC->attr("font")).size()) ? QString::fromStdString(wVl) : QVariant());
				    if((wVl=tC->attr("sort")).size())	{ sortCol = iC+1; if(!s2i(wVl)) sortCol *= -1; }
				}
			    }
			    else {	//Rows content process
				if(!(tit=wdg->item(iR,iC))) wdg->setItem(iR, iC, (tit=new QTableWidgetItem()));
				// Value
				QVariant v;
				if(tC)
				    switch(tC->name()[0]) {
					case 'b': v = (bool)s2i(tC->text());	break;
					case 'i': v = s2ll(tC->text());		break;
					case 'r': v = s2r(tC->text());		break;
					default: v = QString::fromStdString(tC->text()); break;
				    }
				tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
				tit->setData(Qt::DisplayRole, QVariant()); tit->setData(Qt::DisplayRole, v);
				// Back color
				if((tC && (wVl=tC->attr("color")).size()) || (wVl=hit->data(Qt::UserRole+2).toString().toStdString()).size() ||
					(wVl=rClr).size())
				    tit->setData(Qt::BackgroundRole, QColor(wVl.c_str()));
				else tit->setData(Qt::BackgroundRole, QVariant());
				// Text font and color
				if((tC && (wVl=tC->attr("colorText")).size()) || (wVl=hit->data(Qt::UserRole+3).toString().toStdString()).size() ||
					(wVl=rClrTxt).size())
				    tit->setData(Qt::ForegroundRole, QColor(wVl.c_str()));
				else tit->setData(Qt::ForegroundRole, QVariant());
				if((tC && (wVl=tC->attr("font")).size()) || (wVl=hit->data(Qt::UserRole+4).toString().toStdString()).size() ||
					(wVl=rFnt).size())
				    tit->setData(Qt::FontRole, getFont(wVl.c_str(),vmin(w->xScale(true),w->yScale(true)),true,elFnt));
				else tit->setData(Qt::FontRole, QVariant());
				// Cell image
				QImage img;
				if(tC && (wVl=w->resGet(tC->attr("img"))).size() && img.loadFromData((const uchar*)wVl.data(),wVl.size()))
				    tit->setData(Qt::DecorationRole, QPixmap::fromImage(img));
				else tit->setData(Qt::DecorationRole, QVariant());
				// Modify set
				if(hit->data(Qt::UserRole+1).toBool() || (tC && s2i(tC->attr("edit"))))
				    tit->setFlags(tit->flags()|Qt::ItemIsEditable);
				tit->setData(Qt::UserRole+1, iC);
				tit->setData(Qt::UserRole+2, iR);
			    }
			    if(tC)	{ ++iCR; maxCols = vmax(maxCols, (int)iCR); }
			    iC++;
			}
			if(!isH) {
			    if(tR)	{ ++iRR; maxRows = vmax(maxRows, (int)iRR); }
			    iR++;
			}
			else hdrPresent = true;
		    }

		    // Generic properties set
		    int keyID = s2i(tX.attr("keyID"));
		    wdg->setProperty("sel", (wVl=tX.attr("sel")).c_str());
		    if(wVl == "row") {
			wdg->setSelectionBehavior(QAbstractItemView::SelectRows);
			keyID = vmin(keyID, maxCols-1);
		    }
		    else if(wVl == "col") {
			wdg->setSelectionBehavior(QAbstractItemView::SelectColumns);
			keyID = vmin(keyID, maxRows-1);
		    }
		    else wdg->setSelectionBehavior(QAbstractItemView::SelectItems);
		    wdg->setProperty("keyID", keyID);

		    colsWdthFit = s2i(tX.attr("colsWdthFit"));

		    wdg->horizontalHeader()->setVisible(tX.attr("hHdrVis").size()?s2i(tX.attr("hHdrVis")):hdrPresent);
		    wdg->verticalHeader()->setVisible(s2i(tX.attr("vHdrVis")));
		    if(s2i(tX.attr("sortEn")) || sortCol) {
			wdg->setSortingEnabled(true);
			wdg->sortItems((sortCol?abs(sortCol)-1:0), ((sortCol>=0)?Qt::AscendingOrder:Qt::DescendingOrder));
		    }
		}
		wdg->setColumnCount(maxCols);
		wdg->setRowCount(maxRows);
		if(maxCols > 1) wdg->resizeColumnsToContents();
		if(colsWdthFit && maxRows) {
		    int averWdth = w->size().width()/maxCols;
		    int fullColsWdth = 0, niceForceColsWdth = 0, busyCols = 0, tVl;
		    //Count width params
		    for(int iC = 0; iC < wdg->columnCount(); iC++) {
			fullColsWdth += wdg->columnWidth(iC);
			if(wdg->horizontalHeaderItem(iC) && (tVl=wdg->horizontalHeaderItem(iC)->data(Qt::UserRole).toInt())) {
			    niceForceColsWdth += tVl;
			    wdg->setColumnWidth(iC, tVl);
			}
			else if(wdg->columnWidth(iC) <= averWdth)	niceForceColsWdth += wdg->columnWidth(iC);
			else busyCols++;
		    }
		    //Set busyCols
		    if(fullColsWdth > w->size().width() && busyCols) {
			int busyColsWdth = (w->size().width()-niceForceColsWdth)/busyCols;
			for(int iC = 0; iC < wdg->columnCount(); iC++)
			    if(wdg->columnWidth(iC) > averWdth && wdg->columnWidth(iC) > busyColsWdth)
				wdg->setColumnWidth(iC, busyColsWdth);
		    }
		    wdg->resizeRowsToContents();
		}
		wdg->horizontalHeader()->setStretchLastSection(colsWdthFit);
		shD->addrWdg->blockSignals(false);

		setValue(w, shD->value, true);	//Value
		break;
	    }
	    case F_SLIDER: case F_SCROLL_BAR: {
		QAbstractSlider *wdg = (QAbstractSlider*)shD->addrWdg;
		if(!wdg || (shD->elType == F_SLIDER && !qobject_cast<QSlider*>(wdg)) ||
			   (shD->elType == F_SCROLL_BAR && !qobject_cast<QScrollBar*>(wdg)))
		{
		    if(wdg) wdg->deleteLater();
		    shD->addrWdg = wdg = (shD->elType == F_SLIDER ? (QAbstractSlider*)new QSlider(w) : (QAbstractSlider*)new QScrollBar(w));
		    if(shD->elType == F_SLIDER) ((QSlider*)wdg)->setTickPosition(QSlider::TicksBothSides);
		    wdg->setTracking(false);
		    if(runW) connect(wdg, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
		    mk_new = true;
		}
		int cfgOff = 0;
		if(s2i(TSYS::strSepParse(shD->cfg,0,':',&cfgOff))) {
		    wdg->setOrientation(Qt::Vertical);
		    wAlign = Qt::AlignHCenter;
		}
		else {
		    wdg->setOrientation(Qt::Horizontal);
		    wAlign = Qt::AlignVCenter;
		}
		wdg->blockSignals(true);
		wdg->setMinimum(s2i(TSYS::strSepParse(shD->cfg,0,':',&cfgOff)));
		wdg->setMaximum(s2i(TSYS::strSepParse(shD->cfg,0,':',&cfgOff)));
		wdg->setSingleStep(s2i(TSYS::strSepParse(shD->cfg,0,':',&cfgOff)));
		wdg->setPageStep(s2i(TSYS::strSepParse(shD->cfg,0,':',&cfgOff)));
		wdg->blockSignals(false);
		setValue(w, shD->value, true);
		break;
	    }
	}
	shD->setType = true;
	if(mk_new) {
	    // Install event's filter and disable focus
	    eventFilterSet(w, shD->addrWdg, true);
	    w->setFocusProxy(shD->addrWdg);
	    if(devW) setFocus(w, shD->addrWdg, false, devW);
	    if(runW) setActive(w, shD->active && runW->permCntr());
	    shD->addrWdg->setVisible(shD->en);
	    // Fix widget
	    ((QVBoxLayout*)w->layout())->addWidget(shD->addrWdg);
	}
	if(wAlign) ((QVBoxLayout*)w->layout())->setAlignment(shD->addrWdg, wAlign);
    }

    shD->evLock = false;

    return true;
}

void ShapeFormEl::setActive( WdgView *w, bool val )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if(!shD->setType)	return;
    switch(shD->elType) {
	case F_LINE_ED:	((LineEdit*)shD->addrWdg)->setReadOnly(!val);			break;
	case F_TEXT_ED:	((TextEdit*)shD->addrWdg)->workWdg()->setReadOnly(!val);	break;
	case F_TABLE:	break;
	default: shD->addrWdg->setEnabled(val);
    }
    setFocus(w, shD->addrWdg, val);
}

void ShapeFormEl::setValue( WdgView *w, const string &val, bool force )
{
    RunWdgView	*runW = qobject_cast<RunWdgView*>(w);
    ShpDt *shD = (ShpDt*)w->shpData;

    shD->value = val;
    if(!shD->addrWdg || (!force && !shD->setType)) return;

    switch(shD->elType) {
	case F_LINE_ED:
	    if(!((LineEdit*)shD->addrWdg)->isEdited()) ((LineEdit*)shD->addrWdg)->setValue(val.c_str());
	    break;
	case F_TEXT_ED: ((TextEdit*)shD->addrWdg)->setText(val.c_str());	break;
	case F_CHECK_BOX: ((QCheckBox*)shD->addrWdg)->setChecked(s2i(val));	break;
	case F_BUTTON: {
	    QPushButton *wdg = (QPushButton*)shD->addrWdg;
	    wdg->setCheckable(false);
	    wdg->setAutoRepeat(false);
	    if(wdg->menu()) { wdg->menu()->deleteLater(); wdg->setMenu(NULL); }
	    switch(shD->mode) {
		case FBT_STD: {
		    unsigned delay = 0, interv = 0;
		    sscanf(shD->value.c_str(), "%u-%u", &delay, &interv);
		    if(delay && interv) {
			wdg->setAutoRepeat(true);
			wdg->setAutoRepeatDelay(delay);
			wdg->setAutoRepeatInterval(interv);
		    }
		    break;
		}
		case FBT_CHECK:
		    wdg->setCheckable(true);		//Checkable
		    wdg->setChecked(s2i(shD->value));	//Value
		    break;
		case FBT_MENU: {
		    QAction *cur_act = NULL;
		    QMenu *menu = new QMenu(wdg), *menuNext;
		    wdg->setMenu(menu);
		    while(wdg->actions().size()) wdg->removeAction(wdg->actions()[0]);
		    //Fill menu
		    string ipath, item;
		    for(int off = 0; (ipath=TSYS::strLine(shD->value,0,&off)).size(); ) {
			string treeItPath = "";
			menu = menuNext = wdg->menu();
			for(int off1 = 0, lev = 0; (item=TSYS::pathLev(ipath,0,true,&off1)).size(); lev++) {
			    treeItPath += "/"+item;
			    if(menuNext) menu = menuNext;
			    else {
				menu = new QMenu(menu);
				cur_act->setMenu(menu);
				connect(menu->menuAction(), SIGNAL(triggered()), this, SLOT(buttonMenuTrig()));
			    }

			    cur_act = NULL;
			    for(int i_a = 0; i_a < menu->actions().size(); i_a++)
				if(menu->actions()[i_a]->text() == item.c_str()) cur_act = menu->actions()[i_a];
			    if(!cur_act) {
				cur_act = new QAction(item.c_str(), wdg);
				connect(cur_act, SIGNAL(triggered()), this, SLOT(buttonMenuTrig()));
				cur_act->setData(treeItPath.c_str());
				menu->addAction(cur_act);
			    }
			    menuNext = cur_act->menu();
			}
		    }
		    break;
		}
		case FBT_SAVE:
		    if(!runW)	break;
		    shD->opt1 = true;
		    QTimer::singleShot(100, wdg, SIGNAL(released()));
		    break;
	    }
	    break;
	}
	case F_COMBO:
	    if(((QComboBox*)shD->addrWdg)->currentText().toStdString() == val) break;
	    if(((QComboBox*)shD->addrWdg)->findText(val.c_str()) < 0) ((QComboBox*)shD->addrWdg)->addItem(val.c_str());
	    ((QComboBox*)shD->addrWdg)->setCurrentIndex(((QComboBox*)shD->addrWdg)->findText(val.c_str()));
	    break;
	case F_LIST: {
	    QListWidget *wdg = (QListWidget*)shD->addrWdg;

	    while(wdg->selectedItems().size()) wdg->selectedItems()[0]->setSelected(false);

	    string vl;
	    for(int off = 0, cnt = 0; (vl=TSYS::strLine(val,0,&off)).size(); cnt++) {
		QList<QListWidgetItem *> its = wdg->findItems(vl.c_str(), Qt::MatchExactly);
		if(!its.size() && vl.size()) { wdg->addItem(vl.c_str()); its = wdg->findItems(vl.c_str(), Qt::MatchExactly); }
		if(its.size()) {
		    wdg->setCurrentItem(its[0], QItemSelectionModel::Select);
		    if(cnt == 0) wdg->scrollToItem(its[0]);
		}
	    }

	    break;
	}
	case F_TREE: {
	    QTreeWidget *wdg = (QTreeWidget*)shD->addrWdg;
	    QTreeWidgetItem *cur_it = NULL;
	    string item;
	    for(int off = 0, lev = 0; (item=TSYS::pathLev(val,0,true,&off)).size(); lev++)
		if(lev == 0) {
		    for(int iR = 0; !cur_it && iR < wdg->topLevelItemCount(); iR++)
			if(wdg->topLevelItem(iR)->text(0) == item.c_str()) cur_it = wdg->topLevelItem(iR);
		    if(!cur_it)	break;
		}
		else {
		    bool findOK = false;
		    for(int iR = 0; !findOK && iR < cur_it->childCount(); iR++)
			if(cur_it->child(iR)->text(0) == item.c_str())	{ cur_it = cur_it->child(iR); findOK = true; }
		    if(!findOK)	{ cur_it = NULL; break; }
		}
	    while(wdg->selectedItems().size())	wdg->selectedItems()[0]->setSelected(false);
	    if(cur_it) {
		shD->addrWdg->blockSignals(true);
		cur_it->setSelected(true);
		shD->addrWdg->blockSignals(false);
		wdg->scrollToItem(cur_it);
		//Expand all parents for visible selected
		//for(int i_l = 0; cur_it; i_l++) { if(i_l > 0) cur_it->setExpanded(true); cur_it = cur_it->parent(); }
	    }
	    break;
	}
	case F_TABLE: {
	    QTableWidget *wdg = (QTableWidget*)shD->addrWdg;
	    QList<QTableWidgetItem*> foundIts = wdg->findItems(val.c_str(), Qt::MatchExactly);
	    QTableWidgetItem *selIt = NULL, *selItPrev = wdg->currentItem();
	    switch(wdg->selectionBehavior()) {
		case QAbstractItemView::SelectRows:
		    for(QList<QTableWidgetItem*>::iterator iIt = foundIts.begin(); !selIt && iIt != foundIts.end(); ++iIt)
			if((*iIt)->data(Qt::UserRole+1).toInt() == wdg->property("keyID").toInt()) selIt = *iIt;
		    break;
		case QAbstractItemView::SelectColumns:
		    for(QList<QTableWidgetItem*>::iterator iIt = foundIts.begin(); !selIt && iIt != foundIts.end(); ++iIt)
			if((*iIt)->data(Qt::UserRole+2).toInt() == wdg->property("keyID").toInt()) selIt = *iIt;
		    break;
		default: {
		    if(wdg->property("sel").toString() == "cell") selIt = foundIts.length() ? foundIts.front() : NULL;
		    else selIt = wdg->item(s2i(TSYS::strParse(val,0,":")), s2i(TSYS::strParse(val,1,":")));
		    break;
		}
	    }
	    shD->addrWdg->blockSignals(true);
	    if(selIt) {
		wdg->setCurrentItem(selIt);
		if(selIt != selItPrev) wdg->scrollToItem(selIt);
	    }
	    else wdg->setCurrentItem(NULL);
	    shD->addrWdg->blockSignals(false);
	    break;
	}
	case F_SLIDER: case F_SCROLL_BAR:
	    shD->addrWdg->blockSignals(true);
	    ((QAbstractSlider*)shD->addrWdg)->setValue(s2i(val));
	    shD->addrWdg->blockSignals(false);
	    break;
    }
}

bool ShapeFormEl::event( WdgView *w, QEvent *event )
{
    if(qobject_cast<RunWdgView*>(w))
	switch(event->type()) {
	    case QEvent::Hide: {
		ShpDt *shD = (ShpDt*)w->shpData;
		switch(shD->elType) {
		    case F_LINE_ED:
			((LineEdit*)shD->addrWdg)->setValue(((LineEdit*)shD->addrWdg)->value());
			break;
		    case F_TEXT_ED:
			((TextEdit*)shD->addrWdg)->setText(((TextEdit*)shD->addrWdg)->text());
			break;
		}
		break;
	    }
	    default: break;
	}

    return false;
}

bool ShapeFormEl::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if(qobject_cast<DevelWdgView*>(w))
	switch(event->type()) {
	    case QEvent::Enter:
	    case QEvent::Leave:		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::MouseButtonDblClick:
	    case QEvent::ContextMenu:
		QApplication::sendEvent(w,event);
		return true;
	    default:	break;
	}
    else {
	AttrValS attrs;
	switch(event->type()) {
	    case QEvent::FocusIn:
		if(!w->hasFocus()) break;
		attrs.push_back(std::make_pair("focus","1"));
		attrs.push_back(std::make_pair("event","ws_FocusIn"));
		w->attrsSet(attrs);
		break;
	    case QEvent::FocusOut:
		if(w->hasFocus()) break;
		attrs.push_back(std::make_pair("focus","0"));
		attrs.push_back(std::make_pair("event","ws_FocusOut"));
		w->attrsSet(attrs);
		break;
	    case QEvent::MouseButtonDblClick:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease: QApplication::sendEvent(w, event);	break;
	    default:	break;
	}
    }

    return false;
}

void ShapeFormEl::lineAccept( )
{
    LineEdit *el   = (LineEdit*)sender();
    WdgView  *view = (WdgView *)el->parentWidget();

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",el->value().toStdString()));
    attrs.push_back(std::make_pair("event","ws_LnAccept"));
    view->attrsSet(attrs);
}

void ShapeFormEl::textAccept( )
{
    TextEdit *el   = (TextEdit*)sender();
    WdgView  *view = (WdgView *)el->parentWidget();

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",el->text().toStdString()));
    attrs.push_back(std::make_pair("event","ws_TxtAccept"));
    view->attrsSet(attrs);
}

void ShapeFormEl::checkChange(int st)
{
    WdgView *view = (WdgView *)((QCheckBox*)sender())->parentWidget();

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",i2s(st)));
    attrs.push_back(std::make_pair("event","ws_ChkChange"));
    view->attrsSet(attrs);
}

void ShapeFormEl::buttonPressed( )
{
    WdgView *w = (WdgView *)((QPushButton*)sender())->parentWidget();
    switch(((ShpDt*)w->shpData)->mode) {
	case FBT_STD:
	case FBT_SAVE: w->attrSet("event", "ws_BtPress", A_NO_ID, true);	break;
    }
}

void ShapeFormEl::buttonReleased( )
{
    RunWdgView *w = (RunWdgView *)((QPushButton*)sender())->parentWidget();
    ShpDt *shD = (ShpDt*)w->shpData;

    switch(shD->mode) {
	case FBT_STD: w->attrSet("event", "ws_BtRelease", A_NO_ID, true);	break;
	case FBT_SAVE: {
	    if(!shD->opt1) {	//The event from timer
		w->attrSet("event", "ws_BtRelease", A_NO_ID, true);
		break;
	    }
	    shD->opt1 = false;

	    int off = 0;
	    string  fHead	= TSYS::strLine(shD->value, 0, &off);
	    string  fCtx	= shD->value.substr(off);
	    off = 0;
	    string  fTmpl	= TSYS::strParse(fHead, 0, "|", &off),
		    fTitle	= TSYS::strParse(fHead, 0, "|", &off),
		    fDefFile	= TSYS::strParse(fHead, 0, "|", &off);
	    if(fTmpl.empty())	break;
	    if(fTitle.empty())	fTitle = _("Save file");
	    QString fn = w->mainWin()->getFileName(fTitle.c_str(), fDefFile.c_str(), fTmpl.c_str(), QFileDialog::AcceptSave);
	    if(fn.size()) {
		QFile file(fn);
		if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
		    mod->postMess(mod->nodePath().c_str(),
			QString(_("Open file '%1' is fail: %2")).arg(fn).arg(file.errorString()), TVision::Error);
		    break;
		}
		if(file.write(fCtx.data(),fCtx.size()) != (int)fCtx.size())
		    mod->postMess(mod->nodePath().c_str(),
			QString(_("Write data to file '%1' is fail: %2")).arg(fn).arg(file.errorString()), TVision::Error);
	    }
	    w->attrSet("value", "", A_NO_ID, true);	//Clear previous
	    break;
	}
	case FBT_LOAD: {
	    int off = 0;
	    string  fHead	= TSYS::strLine(shD->value, 0, &off);
	    string  fCtx	= shD->value.substr(off);
	    off = 0;
	    string  fTmpl	= TSYS::strParse(fHead, 0, "|", &off),
		    fTitle	= TSYS::strParse(fHead, 0, "|", &off),
		    fDefFile	= TSYS::strParse(fHead, 0, "|", &off);
	    if(fTmpl.empty())	break;
	    if(fTitle.empty())	fTitle = _("Load file");
	    QString fn = w->mainWin()->getFileName(fTitle.c_str(), fDefFile.c_str(), fTmpl.c_str(), QFileDialog::AcceptOpen);
	    if(!fn.size()) break;
	    QFile file(fn);
	    if(!file.open(QFile::ReadOnly)) {
		mod->postMess(mod->nodePath().c_str(), QString(_("Open file '%1' is fail: %2")).arg(fn).arg(file.errorString()), TVision::Error);
		break;
	    }
	    if(file.size() >= USER_FILE_LIMIT) {
		mod->postMess(mod->nodePath().c_str(), QString(_("Loadable file '%1' is too large.")).arg(fn), TVision::Error);
		break;
	    }
	    QByteArray data = file.readAll();

	    AttrValS attrs;
	    attrs.push_back(std::make_pair("event","ws_BtLoad"));
	    attrs.push_back(std::make_pair("value",fHead+"\n"+string(data.data(),data.size())));
	    w->attrsSet(attrs);
	    break;
	}
    }
}

void ShapeFormEl::buttonToggled( bool val )
{
    WdgView *w = (WdgView *)((QPushButton*)sender())->parentWidget();
    if(((ShpDt*)w->shpData)->evLock)	return;
    AttrValS attrs;
    attrs.push_back(std::make_pair("event", string("ws_BtToggleChange\n")+(val?"ws_BtPress":"ws_BtRelease")));
    attrs.push_back(std::make_pair("value", i2s(val)));
    w->attrsSet(attrs);
}

void ShapeFormEl::buttonMenuTrig( )
{
    QAction *act = (QAction*)sender();
    WdgView *w = (WdgView *)act->parentWidget()->parentWidget();
    w->attrSet("event", "ws_BtMenu="+act->data().toString().toStdString(), A_NO_ID, true);
}

void ShapeFormEl::comboChange( const QString &val )
{
    WdgView *w = (WdgView *)((QWidget*)sender())->parentWidget();
    if(((ShpDt*)w->shpData)->evLock)	return;

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",val.toStdString()));
    attrs.push_back(std::make_pair("event","ws_CombChange"));
    w->attrsSet(attrs);
}

void ShapeFormEl::listChange( )
{
    QListWidget *el = (QListWidget*)sender();
    WdgView     *w  = (WdgView *)el->parentWidget();

    if(((ShpDt*)w->shpData)->evLock) return;

    string vl = "";
    for(int iS = 0; iS < el->selectedItems().size(); iS++)
	vl += (vl.size()?"\n":"") + el->selectedItems()[iS]->text().toStdString();

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",vl));
    attrs.push_back(std::make_pair("event","ws_ListChange"));
    w->attrsSet(attrs);
}

void ShapeFormEl::treeChange( )
{
    QTreeWidget *el = (QTreeWidget*)sender();
    WdgView	*w  = (WdgView*)el->parentWidget();

    if(((ShpDt*)w->shpData)->evLock || !el->selectedItems().size()) return;

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",el->selectedItems()[0]->data(0,Qt::UserRole).toString().toStdString()));
    attrs.push_back(std::make_pair("event","ws_TreeChange"));
    w->attrsSet(attrs);
}

void ShapeFormEl::tableSelectChange( )
{
    QTableWidget *el = (QTableWidget*)sender();
    RunWdgView   *w  = (RunWdgView*)el->parentWidget();

    if(((ShpDt*)w->shpData)->evLock || !el->selectedItems().size() || !(((ShpDt*)w->shpData)->active && w->permCntr())) return;

    AttrValS attrs;
    QTableWidgetItem *wIt = el->selectedItems()[0];
    string value = wIt->text().toStdString();
    switch(el->selectionBehavior()) {
	case QAbstractItemView::SelectRows:
	    value = wIt->tableWidget()->item(wIt->row()/*data(Qt::UserRole+2).toInt()*/, el->property("keyID").toInt())->text().toStdString();
	    break;
	case QAbstractItemView::SelectColumns:
	    value = wIt->tableWidget()->item(el->property("keyID").toInt(), wIt->column()/*data(Qt::UserRole+1).toInt()*/)->text().toStdString();
	    break;
	default:
	    if(el->property("sel").toString() != "cell")
		value = i2s(wIt->row()/*data(Qt::UserRole+2).toInt()*/)+":"+i2s(wIt->column()/*data(Qt::UserRole+1).toInt()*/);
	    break;
    }

    //Events prepare
    attrs.push_back(std::make_pair("value",value));
    attrs.push_back(std::make_pair("event","ws_TableChangeSel"));
    w->attrsSet(attrs);
}

void ShapeFormEl::tableChange( int row, int col )
{
    QTableWidget *el = (QTableWidget*)sender();
    RunWdgView   *w  = (RunWdgView*)el->parentWidget();

    if(((ShpDt*)w->shpData)->evLock || !el->selectedItems().size() || !(((ShpDt*)w->shpData)->active && w->permCntr())) return;

    QTableWidgetItem *wIt = el->item(row,col);
    QVariant val = wIt->data(Qt::DisplayRole);
    if(val.type() == QVariant::Bool) val = val.toInt();

    //Events prepare
    AttrValS attrs;
    attrs.push_back(std::make_pair("set",val.toString().toStdString()));
    attrs.push_back(std::make_pair("event",TSYS::strMess("ws_TableEdit_%d_%d",wIt->data(Qt::UserRole+1).toInt(),wIt->data(Qt::UserRole+2).toInt())));
    w->attrsSet(attrs);
}

void ShapeFormEl::sliderMoved( int val )
{
    QAbstractSlider *el = (QAbstractSlider*)sender();
    WdgView	    *w  = (WdgView *)el->parentWidget();

    AttrValS attrs;
    attrs.push_back(std::make_pair("value",i2s(val)));
    attrs.push_back(std::make_pair("event","ws_SliderChange"));
    w->attrsSet(attrs);
}

void ShapeFormEl::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if(en) wdg->installEventFilter(view);
    else   wdg->removeEventFilter(view);

    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC)))
	    eventFilterSet(view,(QWidget*)wdg->children().at(iC),en);
}

void ShapeFormEl::setFocus( WdgView *w, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();

    //Set up current widget
    if(en) {
	if(isFocus && !devel) wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
    }
    else {
	if(wdg->focusPolicy() != Qt::NoFocus) {
	    wdg->setWindowIconText(QString::number((int)wdg->focusPolicy()));
	    wdg->setFocusPolicy(Qt::NoFocus);
	}
	if(devel) wdg->setMouseTracking(true);
    }

    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC)))
	    setFocus(w,(QWidget*)wdg->children().at(iC),en,devel);
}

//************************************************
//* Text element shape widget                    *
//************************************************
ShapeText::ShapeText( ) : WdgShape("Text")	{ }

void ShapeText::init( WdgView *w )	{ w->shpData = new ShpDt(); }

void ShapeText::destroy( WdgView *w )	{ delete (ShpDt*)w->shpData; }

bool ShapeText::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    bool up = true,		//Update view checking
	 reform = false;	//Text reformation

    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos) {
	case A_COM_LOAD: up = reform = true;	break;
	case A_EN:
	    if(!qobject_cast<RunWdgView*>(w))	{ up = false; break; }
	    shD->en = (bool)s2i(val);
	    w->setVisible(s2i(val) && ((RunWdgView*)w)->permView());
	    break;
	case A_ACTIVE:
	    if(!qobject_cast<RunWdgView*>(w))	break;
	    shD->active = (bool)s2i(val);
	    w->setFocusPolicy((s2i(val)&&((RunWdgView*)w)->permCntr()) ? Qt::StrongFocus : Qt::NoFocus);
	    break;
	case A_GEOM_MARGIN: shD->geomMargin = s2i(val); up = true;	break;
	case A_BackColor: {
	    shD->backGrnd.setColor(getColor(val));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if(!brsh.color().isValid()) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle(brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern);
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);

	    up = true;
	    break;
	}
	case A_BackImg: {
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage((!backimg.empty()&&img.loadFromData((const uchar*)backimg.data(),backimg.size()))?img:QImage());

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle(!brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern);
	    plt.setBrush(QPalette::Base,brsh);
	    w->setPalette(plt);

	    up = true;
	    break;
	}
	case A_BordWidth: shD->border.setWidth(s2i(val)); up = true;	break;
	case A_BordColor: shD->border.setColor(getColor(val)); up = true;break;
	case A_BordStyle: shD->bordStyle = s2i(val); up = true;	break;
	case A_TextFont: shD->font = val; up = true;			break;
	case A_TextColor: shD->color = getColor(val);			break;
	case A_TextOrient: shD->orient = s2i(val);			break;
	case A_TextWordWrap:
	    if(s2i(val)) shD->text_flg |= Qt::TextWordWrap; else shD->text_flg &= (~Qt::TextWordWrap);
	    break;
	case A_TextAlignment:
	    shD->text_flg &= ~(Qt::AlignLeft|Qt::AlignRight|Qt::AlignHCenter|Qt::AlignJustify|Qt::AlignTop|Qt::AlignBottom|Qt::AlignVCenter);
	    switch(s2i(val)&0x3) {
		case 0: shD->text_flg |= Qt::AlignLeft;		break;
		case 1: shD->text_flg |= Qt::AlignRight;	break;
		case 2: shD->text_flg |= Qt::AlignHCenter;	break;
		case 3: shD->text_flg |= Qt::AlignJustify;	break;
	    }
	    switch(s2i(val)>>2) {
		case 0: shD->text_flg |= Qt::AlignTop;		break;
		case 1: shD->text_flg |= Qt::AlignBottom;	break;
		case 2: shD->text_flg |= Qt::AlignVCenter;	break;
	    }
	    break;
	case A_TextText:
	    if(shD->text_tmpl == val.c_str())	break;
	    shD->text_tmpl = val;
	    reform = true;
	    break;
	case A_TextNumbArg: {
	    int numbArg = s2i(val);
	    while((int)shD->args.size() < numbArg) shD->args.push_back(ArgObj());
	    while((int)shD->args.size() > numbArg) shD->args.pop_back();
	    reform = true;
	    break;
	}
	default:
	    //Individual arguments process
	    if(uiPrmPos >= A_TextArs) {
		int argN = (uiPrmPos-A_TextArs)/A_TextArsSz;
		if(argN >= (int)shD->args.size()) break;
		if((uiPrmPos%A_TextArsSz) == A_TextArsVal || (uiPrmPos%A_TextArsSz) == A_TextArsTp) {
		    QVariant gval = shD->args[argN].val();
		    int tp = (gval.type()==QVariant::Double) ? 1 : ((gval.type()==QVariant::String) ? 2 : 0);
		    if((uiPrmPos%A_TextArsSz) == A_TextArsVal)	gval = val.c_str();
		    if((uiPrmPos%A_TextArsSz) == A_TextArsTp)	tp = s2i(val);
		    switch(tp) {
			case FT_INT:  shD->args[argN].setVal(gval.toInt());	break;
			case FT_REAL: shD->args[argN].setVal(gval.toDouble());	break;
			case FT_STR:  shD->args[argN].setVal(gval.toString());	break;
		    }
		}
		if((uiPrmPos%A_TextArsSz) == A_TextArsCfg) shD->args[argN].setCfg(val.c_str());
		reform = true;
	    }else up = false;
    }

    //Text reformation
    if(reform && !w->allAttrLoad()) {
	QString text = shD->text_tmpl.c_str();
	for(unsigned i_a = 0; i_a < shD->args.size(); i_a++) {
	    switch(shD->args[i_a].val().type()) {
		case QVariant::String:
		    text = text.arg(shD->args[i_a].val().toString(), vmax(-1000,vmin(1000,s2i(shD->args[i_a].cfg()))));
		    break;
		case QVariant::Double: {
		    int off = 0;
		    int wdth = s2i(TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off));
		    string form = TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off);
		    int prec = s2i(TSYS::strSepParse(shD->args[i_a].cfg(),0,';',&off));
		    text = text.arg(shD->args[i_a].val().toDouble(),vmax(-1000,vmin(1000,wdth)),form.empty()?0:form[0],vmax(0,prec),' ');
		    break;
		}
		default:
		    text = text.arg(shD->args[i_a].val().toInt(), vmax(-1000,vmin(1000,s2i(shD->args[i_a].cfg()))));
		    break;
	    }
	}
	if(text != shD->text.c_str())	{ shD->text = text.toStdString(); up = true; }
    }

    if(up && !w->allAttrLoad() && uiPrmPos != -1) w->update();

    return up;
}

bool ShapeText::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    if(!shD->en) return false;
    switch(event->type()) {
	case QEvent::Paint: {
	    QPainter pnt(w);

	    QRect dA = w->rect();	// Decoration draw area

	    //Draw decoration
	    if(shD->backGrnd.color().isValid())		pnt.fillRect(dA, shD->backGrnd.color());
	    if(!shD->backGrnd.textureImage().isNull())	pnt.fillRect(dA, shD->backGrnd.textureImage());

	    //Draw border
	    if(shD->border.width()) {
		borderDraw(pnt, dA, shD->border, shD->bordStyle);
		dA.adjust(shD->border.width(), shD->border.width(), -shD->border.width(), -shD->border.width());
	    }
	    dA.adjust(shD->geomMargin, shD->geomMargin, -shD->geomMargin, -shD->geomMargin);

	    //Text translation
	    pnt.translate(w->rect().width()/2, w->rect().height()/2);
	    pnt.rotate(shD->orient);

	    //Calc whidth and hight draw rect at rotate
	    double rad_angl  = fabs(M_PI*(double)shD->orient/180);
	    double rect_rate = 1/(fabs(cos(rad_angl))+fabs(sin(rad_angl)));
	    int wdth  = (int)(rect_rate*dA.size().width()+fabs(sin(rad_angl))*(dA.size().height()-dA.size().width()));
	    int heigt = (int)(rect_rate*dA.size().height()+fabs(sin(rad_angl))*(dA.size().width()-dA.size().height()));
	    dA = QRect(QPoint(-wdth/2,-heigt/2), QSize(wdth,heigt));

	    //Draw text
	    pnt.setPen(shD->color);
	    pnt.setFont(getFont(shD->font,vmin(w->xScale(true),w->yScale(true))));
	    pnt.drawText(dA, shD->text_flg, shD->text.c_str());

	    event->accept( );
	    return true;
	}
	default: break;
    }
    return false;
}

//************************************************
//* Media view shape widget                      *
//************************************************
#ifdef HAVE_PHONON
using namespace Phonon;
#endif

ShapeMedia::ShapeMedia( ) : WdgShape("Media")
{

}

void ShapeMedia::init( WdgView *w )
{
    w->shpData = new ShpDt();
    new QVBoxLayout(w);
}

void ShapeMedia::destroy( WdgView *w )
{
    clear(w);
    delete (ShpDt*)w->shpData;
}

void ShapeMedia::clear( WdgView *w )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    //Clear label widget's elements
    QLabel *lab = dynamic_cast<QLabel*>(shD->addrWdg);
    if(lab) {
	if(lab->movie()) {
	    if(lab->movie()->device()) delete lab->movie()->device();
	    delete lab->movie();
	    lab->clear();
	}
#ifdef HAVE_PHONON
	if(shD->mediaType == FM_FULL_VIDEO) { lab->deleteLater(); shD->addrWdg = NULL; }
#endif
    }

#ifdef HAVE_PHONON
    VideoPlayer *player = dynamic_cast<VideoPlayer*>(shD->addrWdg);
    if(player && (shD->mediaType == FM_IMG || shD->mediaType == FM_ANIM)) { player->deleteLater(); shD->addrWdg = NULL; }
#endif
}

void ShapeMedia::mediaFinished( )
{
    WdgView *w = (WdgView*)((QWidget*)sender())->parentWidget();
    ShpDt *shD = (ShpDt*)w->shpData;

#ifdef HAVE_PHONON
    VideoPlayer *player = dynamic_cast<VideoPlayer*>(shD->addrWdg);
    AttrValS attrs;
    if(shD->videoRoll && player) player->play();
    else attrs.push_back(std::make_pair("play","0"));
    attrs.push_back(std::make_pair("event","ws_MediaFinished"));
    w->attrsSet(attrs);
#endif
}

bool ShapeMedia::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    QLabel *lab;
#ifdef HAVE_PHONON
    VideoPlayer *player;
#endif

    bool up = true, reld_cfg = false;
    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos) {
	case A_COM_LOAD: reld_cfg = true;	break;
	case A_EN:
	    if(!qobject_cast<RunWdgView*>(w))	{ up = false; break; }
	    shD->en = (bool)s2i(val);
	    w->setVisible(shD->en && ((RunWdgView*)w)->permView());
	    break;
	case A_ACTIVE:
	    if(!qobject_cast<RunWdgView*>(w))	break;
	    shD->active = s2i(val);
	    if(shD->addrWdg) shD->addrWdg->setMouseTracking(shD->active && ((RunWdgView*)w)->permCntr());
	    w->setMouseTracking(shD->active && ((RunWdgView*)w)->permCntr());
	    break;
	case A_GEOM_MARGIN:
	    shD->geomMargin = s2i(val);
	    w->layout()->setMargin(shD->geomMargin);
	    break;
	case A_BackColor: {
	    shD->backGrnd.setColor(getColor(val));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if(!brsh.color().isValid()) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle( brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern );
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case A_BackImg: {
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if(!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size()))
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle(!brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern);
	    plt.setBrush(QPalette::Background,brsh);
	    w->setPalette(plt);
	    break;
	}
	case A_BordWidth: shD->border.setWidth(s2i(val));	break;
	case A_BordColor: shD->border.setColor(getColor(val));	break;
	case A_BordStyle: shD->bordStyle = s2i(val);		break;
	case A_MediaSrc:
	    if(shD->mediaSrc == val)	break;
	    shD->mediaSrc = val;
	    reld_cfg = true;
	    break;
	case A_MediaFit:
	    shD->mediaFit = (bool)s2i(val);
	    lab = dynamic_cast<QLabel*>(shD->addrWdg);
	    if(lab) lab->setScaledContents(shD->mediaFit);
	    break;
	case A_MediaType:
	    if(shD->mediaType == s2i(val))	break;
	    shD->mediaType = s2i(val);
	    reld_cfg = true;
	    break;
	case A_MediaAreas: {
	    int numbMAr = s2i(val);
	    while((int)shD->maps.size() < numbMAr) shD->maps.push_back(MapArea());
	    while((int)shD->maps.size() > numbMAr) shD->maps.pop_back();
	    break;
	}
	case A_MediaSpeedPlay:
	    switch(shD->mediaType) {
		case FM_IMG: case FM_ANIM: shD->mediaSpeed = s2i(val);	break;
		case FM_FULL_VIDEO: shD->videoPlay = (bool)s2i(val);	break;
	    }
	    if((lab=dynamic_cast<QLabel*>(shD->addrWdg))) {
		if(!lab->movie()) break;
		if(shD->mediaSpeed <= 1) lab->movie()->stop();
		else {
		    lab->movie()->setSpeed(shD->mediaSpeed);
		    lab->movie()->start();
		}
	    }
#ifdef HAVE_PHONON
	    else if((player=dynamic_cast<VideoPlayer*>(shD->addrWdg))) {
		if(shD->videoPlay) {
		    player->play();
		    w->attrSet("size", r2s(player->totalTime()), A_NO_ID, true);
		}
		else player->stop();
	    }
#endif
	    break;
#ifdef HAVE_PHONON
	case A_MediaRoll: shD->videoRoll = (bool)s2i(val);	break;
	case A_MediaPause:
	    shD->videoPause = (bool)s2i(val);
	    if((player=dynamic_cast<VideoPlayer*>(shD->addrWdg))) {
		if(shD->videoPause && !player->isPaused()) player->pause();
		if(!shD->videoPause && player->isPaused()) player->play();
	    }
	    break;
	case A_MediaSeek:
	    if(shD->videoSeek == s2r(val)) break;
	    shD->videoSeek = s2r(val);
	    if((player=dynamic_cast<VideoPlayer*>(shD->addrWdg))) player->seek(shD->videoSeek);
	    break;
	case A_MediaVolume:
	    if(shD->audioVolume == s2r(val)) break;
	    shD->audioVolume = s2r(val);
	    if((player=dynamic_cast<VideoPlayer*>(shD->addrWdg))) player->setVolume(shD->audioVolume);
	    break;
#endif
	default:
	    //Individual arguments process
	    if(uiPrmPos >= A_MediaArs) {
		int areaN = (uiPrmPos-A_MediaArs)/A_MediaArsSz;
		if(areaN >= (int)shD->maps.size()) break;
		switch((uiPrmPos-A_MediaArs)%A_MediaArsSz) {
		    case A_MediaArShape: shD->maps[areaN].shp = s2i(val);	break;
		    case A_MediaArCoord: {
			string stmp;
			shD->maps[areaN].pnts.clear();
			for(int ncrd = 0, pos = 0; (stmp=TSYS::strSepParse(val,0,',',&ncrd)).size(); pos++)
			    if(!(pos%2)) shD->maps[areaN].pnts.push_back(QPoint(s2i(stmp),0));
			    else	 shD->maps[areaN].pnts[shD->maps[areaN].pnts.size()-1].setY(s2i(stmp));
		    }
		    case A_MediaArTitle: shD->maps[areaN].title = val;	break;
		}
	    }
    }

    if(reld_cfg && !w->allAttrLoad()) {
	bool mk_new = false;
	switch(shD->mediaType) {
	    case FM_IMG: {
		QImage img;
		//Free movie data, if set
		clear(w);
		//Create label widget
		if(!shD->addrWdg) { shD->addrWdg = new QLabel(w); mk_new = true; }
		//Set new image
		lab = dynamic_cast<QLabel*>(shD->addrWdg);
		if(!lab) break;
		lab->setAlignment(Qt::AlignCenter);
		string sdata = w->resGet(shD->mediaSrc);
		if(!sdata.empty() && img.loadFromData((const uchar*)sdata.data(),sdata.size())) {
		    lab->setPixmap(QPixmap::fromImage(img.scaled(
			(int)((float)img.width()*w->xScale(true)),
			(int)((float)img.height()*w->yScale(true)),
			Qt::KeepAspectRatio,Qt::SmoothTransformation)));
		    lab->setScaledContents(shD->mediaFit);
		}
		else lab->setPixmap(QPixmap());
		break;
	    }
	    case FM_ANIM: {
		//Clear previous movie data
		clear(w);
		//Create label widget
		if(!shD->addrWdg) { shD->addrWdg = new QLabel(w); mk_new = true; }
		//Set new data
		lab = dynamic_cast<QLabel*>(shD->addrWdg);
		if(!lab) break;
		lab->setAlignment(Qt::AlignCenter);
		string sdata = w->resGet(shD->mediaSrc);
		if(sdata.size()) {
		    QBuffer *buf = new QBuffer(w);
		    buf->setData(sdata.data(), sdata.size());
		    buf->open(QIODevice::ReadOnly);
		    lab->setMovie(new QMovie(buf));

		    //Play speed set
		    if(shD->mediaSpeed <= 1) lab->movie()->stop();
		    else {
			lab->movie()->setSpeed(shD->mediaSpeed);
			lab->movie()->start();
		    }
		    //Fit set
		    lab->setScaledContents( shD->mediaFit );
		    if(!shD->mediaFit && lab->movie()->jumpToNextFrame()) {
			QImage img = lab->movie()->currentImage();
			lab->movie()->setScaledSize(QSize((int)((float)img.width()*w->xScale(true)),(int)((float)img.height()*w->yScale(true))));
		    }
		}else lab->setText("");
		break;
	    }
#ifdef HAVE_PHONON
	    case FM_FULL_VIDEO: {
		//Clear previous movie data
		clear(w);
		//Create player widget
		if(!shD->addrWdg) {
		    shD->addrWdg = new VideoPlayer(Phonon::VideoCategory, w);
		    connect(shD->addrWdg, SIGNAL(finished()), this, SLOT(mediaFinished()));
		    ((VideoPlayer*)shD->addrWdg)->videoWidget()->installEventFilter(w);
		    mk_new = true;
		}
		//Set new data
		player = dynamic_cast<VideoPlayer*>(shD->addrWdg);
		if(!player) break;
		MediaSource mSrc;
		if(player->isPlaying()) { player->stop(); player->seek(0); }
		//Try play local file
		if(shD->mediaSrc.compare(0,5,"file:") == 0) mSrc = MediaSource(QUrl(shD->mediaSrc.c_str()));
		    //mSrc = MediaSource(QString(shD->mediaSrc.substr(5).c_str()));
		//Try play Stream by URL
		else if(shD->mediaSrc.compare(0,7,"stream:") == 0)
		    mSrc = MediaSource(QUrl(shD->mediaSrc.substr(7).c_str()));
		//Try remote VCAEngine resource at last
		if(shD->mediaSrc.size() && (mSrc.type() == MediaSource::Invalid || mSrc.type() == MediaSource::Empty)) {
		    string sdata = w->resGet(shD->mediaSrc);
		    if(sdata.size()) {
			string tfile = TSYS::path2sepstr(w->id(),'_');
			int tfid = open(tfile.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0664);
			if(tfid >= 0) {
			    if(write(tfid, sdata.data(), sdata.size()) != (ssize_t)sdata.size())
				mod->postMess(mod->nodePath().c_str(), QString(_("Write file '%1' is fail.")).arg(tfile.c_str()), TVision::Error);
			    close(tfid);
			    mSrc = MediaSource(QUrl(("file:"+tfile).c_str()));
			    //mSrc = MediaSource(QString(tfile.c_str()));
			}
		    }
		}
		if(mSrc.type() != MediaSource::Invalid && mSrc.type() != MediaSource::Empty) {
		    player->load(mSrc);
		    if(shD->videoPlay) {
			player->play();
			w->attrSet("size", r2s(player->totalTime()), A_NO_ID, true);
		    }
		    else player->stop();
		    if(shD->videoPause) player->pause();
		    player->seek(shD->videoSeek);
		    player->setVolume(shD->audioVolume);
		}
		break;
	    }
#endif
	}
	if(mk_new) {
	    w->setMouseTracking(qobject_cast<DevelWdgView*>(w) || (shD->active && ((RunWdgView*)w)->permCntr()));
	    shD->addrWdg->setMouseTracking(qobject_cast<DevelWdgView*>(w) || (shD->active && ((RunWdgView*)w)->permCntr()));
	    shD->addrWdg->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	    ((QVBoxLayout*)w->layout())->addWidget(shD->addrWdg);
	}
    }

    if(up && !w->allAttrLoad( ) && uiPrmPos != -1) w->update();

    return up;
}

bool ShapeMedia::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if(!shD->en) return false;

    switch(event->type()) {
	case QEvent::Paint: {
	    QPainter pnt(w);

	    //Prepare draw area
	    QRect dA = w->rect().adjusted(0,0,-2*shD->geomMargin,-2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));

	    //Draw decoration
	    if(shD->backGrnd.color().isValid()) pnt.fillRect(dA,shD->backGrnd.color());
	    if(!shD->backGrnd.textureImage().isNull()) pnt.fillRect(dA,shD->backGrnd.textureImage());

	    //Draw border
	    borderDraw(pnt, dA, shD->border, shD->bordStyle);

	    return true;
	}
	case QEvent::MouseMove: {
	    Qt::CursorShape new_shp = Qt::ArrowCursor;
	    for(unsigned i_a = 0; i_a < shD->maps.size(); i_a++)
		if(shD->maps[i_a].containsPoint(w->mapFromGlobal(w->cursor().pos()))) {
		    new_shp = Qt::PointingHandCursor;
		    if(!shD->maps[i_a].title.empty()) QToolTip::showText(w->cursor().pos(),shD->maps[i_a].title.c_str());
		    break;
		}

	    if(new_shp != w->cursor().shape()) w->setCursor(new_shp);

	    return true;
	}
	case QEvent::MouseButtonPress: {
	    string sev;
	    for(unsigned i_a = 0; i_a < shD->maps.size(); i_a++)
	        if(shD->maps[i_a].containsPoint(w->mapFromGlobal(w->cursor().pos())))
	        { sev = "ws_MapAct"+i2s(i_a); break; }
	    if(!sev.empty()) {
		switch(((QMouseEvent*)event)->button()) {
		    case Qt::LeftButton:	sev += "Left";	break;
		    case Qt::RightButton:	sev += "Right";	break;
		    case Qt::MidButton:		sev += "Midle";	break;
		    default: return false;
		}
		w->attrSet("event", sev, A_NO_ID, true);
		//return true;	//For common Press event produce
	    }
	    break;
	}
	default: break;
    }

    return false;
}

bool ShapeMedia::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
#ifdef HAVE_PHONON
    VideoPlayer *player = dynamic_cast<VideoPlayer*>(((ShpDt*)w->shpData)->addrWdg);
    if(player && player->videoWidget() == object)
	if(event->type() == QEvent::MouseButtonDblClick && ((QMouseEvent*)event)->button() == Qt::LeftButton) {
	    player->videoWidget()->setFullScreen(!player->videoWidget()->isFullScreen());
	    return true;
	}
#endif

    return false;
}

//* Map areas structure                           *
//*************************************************
bool ShapeMedia::MapArea::containsPoint( const QPoint & point )
{
    switch(shp) {
	case FM_RECT:
	    if(pnts.size() < 2) return false;
	    return QRect(pnts[0],pnts[1]).contains(point);
	case FM_POLY:
	    return QPolygon(pnts).containsPoint(point, Qt::OddEvenFill);
	case FM_CIRCLE: {
	    if(pnts.size() < 2) return false;
	    QPoint work = point-pnts[0];
	    return (pow(pow((float)work.x(),2)+pow((float)work.y(),2),0.5) < pnts[1].x());
	}
    }
    return false;
}

//*************************************************
//* Diagram view shape widget                     *
//*************************************************
ShapeDiagram::ShapeDiagram( ) : WdgShape("Diagram")
{

}

void ShapeDiagram::init( WdgView *w )
{
    w->shpData = new ShpDt();

    //Init tracing timer
    ((ShpDt*)w->shpData)->trcTimer = new QTimer(w);
    connect(((ShpDt*)w->shpData)->trcTimer, SIGNAL(timeout()), this, SLOT(tracing()));

    //Activate vizualizer specific attributes
    RunWdgView *rw = qobject_cast<RunWdgView*>(w);
    if(rw) {
	XMLNode reqSpc("activate"); reqSpc.setAttr("path", rw->id()+"/%2fserv%2fattr%2fsclWin");
	rw->mainWin()->cntrIfCmd(reqSpc);
    }
}

void ShapeDiagram::destroy( WdgView *w )
{
    ((ShpDt*)w->shpData)->trcTimer->stop();

    delete (ShpDt*)w->shpData;
}

bool ShapeDiagram::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    bool up = false,		//Repaint diagram picture
	 make_pct = false;	//Remake diagram picture
    int  reld_tr_dt = 0;	//Reload trend's data (1-reload addons, 2-full reload)

    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos) {
	case A_NO_ID:
	    if(attr == "sclWin") shD->sclWin = s2i(val);
	    break;
	case A_COM_LOAD: up = make_pct = true; reld_tr_dt = 2;	break;
	case A_COM_FOCUS: up = ((bool)s2i(val) != w->hasFocus()); break;
	case A_EN:
	    if(!qobject_cast<RunWdgView*>(w))	break;
	    shD->en = (bool)s2i(val);
	    w->setVisible(shD->en && ((RunWdgView*)w)->permView());
	    up = true;
	    break;
	case A_ACTIVE:
	    shD->active = (bool)s2i(val);
	    if(!qobject_cast<RunWdgView*>(w))	break;
	    w->setFocusPolicy((shD->active&&((RunWdgView*)w)->permCntr())?Qt::StrongFocus:Qt::NoFocus);
	    break;
	case A_GEOM_W: case A_GEOM_H: make_pct = true;	break;
	case A_GEOM_MARGIN: shD->geomMargin = s2i(val); make_pct = true; break;
	case A_BackColor: {
	    shD->backGrnd.setColor(getColor(val));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setColor(shD->backGrnd.color());
	    if(!brsh.color().isValid()) brsh.setColor(QPalette().brush(QPalette::Background).color());
	    brsh.setStyle(brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern);
	    plt.setBrush(QPalette::Background, brsh);
	    w->setPalette(plt);
	    up = true;
	    break;
	}
	case A_BackImg: {
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if(!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size()))
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Background);
	    brsh.setTextureImage(img);
	    brsh.setStyle(!brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern);
	    plt.setBrush(QPalette::Background, brsh);
	    w->setPalette(plt);
	    up = true;
	    break;
	}
	case A_BordWidth: shD->border.setWidth(s2i(val)); make_pct = true;	break;
	case A_BordColor: shD->border.setColor(getColor(val)); up = true;	break;
	case A_BordStyle: shD->bordStyle = s2i(val); up = true;			break;
	case A_DiagramTrcPer:
	    shD->trcPer = s2r(val);
	    if(shD->trcPer < 0.01) { shD->trcPer = 0; shD->trcTimer->stop(); }
	    else shD->trcTimer->start(vmax(qobject_cast<RunWdgView*>(w)?0.01:1,shD->trcPer)*1000);
	    break;
	case A_DiagramType: shD->type = s2i(val); reld_tr_dt = 2;		break;
	case A_DiagramTSek:
	    shD->tTimeCurent = false;
	    if(s2ll(val) == 0) {
		shD->tTime = (int64_t)time(NULL)*1000000;
		shD->tTimeCurent = true;
	    } else shD->tTime = s2ll(val)*1000000 + shD->tTime%1000000;
	    reld_tr_dt = 1;
	    break;
	case A_DiagramTUSek: shD->tTime = 1000000ll*(shD->tTime/1000000)+s2ll(val); reld_tr_dt = 1;	break;
	case A_DiagramTSize: shD->tSize = vmax(1e-3,s2r(val)); reld_tr_dt = 2;	break;
	case A_DiagramCurSek:
	    if((shD->curTime/1000000) == s2i(val)) break;
	    shD->curTime = s2ll(val)*1000000 + shD->curTime%1000000;
	    shD->holdCur = (shD->curTime >= (shD->tTime-(shD->pictRect.width()?(int64_t)(3*1e6*shD->tSize)/shD->pictRect.width():0)));	//No more 3 pixels
	    //shD->holdCur = (shD->curTime >= shD->tTime);
	    up = true;
	    break;
	case A_DiagramCurUSek:
	    if((shD->curTime%1000000) == s2i(val)) break;
	    shD->curTime = 1000000ll*(shD->curTime/1000000)+s2ll(val);
	    shD->holdCur = (shD->curTime>=shD->tTime);
	    up = true;
	    break;
	case A_DiagramCurColor: shD->curColor = getColor(val); up = true;	break;
	case A_DiagramSclColor: shD->sclColor = getColor(val); make_pct = true;	break;
	case A_DiagramSclHor: shD->sclHor = s2i(val); make_pct = true;		break;
	case A_DiagramSclVer: shD->sclVer = s2i(val); make_pct = true;		break;
	case A_DiagramSclMarkColor: shD->sclMarkColor = getColor(val); make_pct = true;	break;
	case A_DiagramSclMarkFont: shD->sclMarkFont = getFont(val); make_pct = true;	break;
	case A_DiagramValArch: if(shD->valArch != val) { shD->valArch = val; reld_tr_dt = 2; }	break;
	case A_DiagramParNum: {
	    int parNum = s2i(val);
	    while((int)shD->prms.size() < parNum) shD->prms.push_back(TrendObj(w));
	    while((int)shD->prms.size() > parNum) shD->prms.pop_back();
	    make_pct = true;
	    break;
	}
	case A_DiagramSclVerScl:
	    if(shD->sclVerScl != s2r(val)) { shD->sclVerScl = s2r(val); make_pct = true; }
	    break;
	case A_DiagramSclVerSclOff:
	    if(shD->sclVerSclOff != s2r(val)) { shD->sclVerSclOff = s2r(val); make_pct = true; }
	    break;
	case A_DiagramSclHorScl:
	    if(shD->sclHorScl != s2r(val)) { shD->sclHorScl = s2r(val); make_pct = true; }
	    break;
	case A_DiagramSclHorSclOff:
	    if(shD->sclHorSclOff != s2r(val)) { shD->sclHorSclOff = s2r(val); make_pct = true; }
	    break;
	case A_DiagramValsForPix:
	    if(shD->valsForPix == vmin(10,vmax(0,s2i(val))))	break;
	    shD->valsForPix = vmin(10,vmax(0,s2i(val)));
	    reld_tr_dt = 2;
	    break;
	case A_DiagramSclHorPer: shD->sclHorPer = vmax(0, s2r(val))*1e6; make_pct = true;	break;
	default:
	    //Individual trend's attributes process
	    if(uiPrmPos >= A_DiagramTrs) {
		int trndN = (uiPrmPos-A_DiagramTrs)/A_DiagramTrsSz;
		if(trndN >= (int)shD->prms.size()) break;
		make_pct = true;
		switch(uiPrmPos%A_DiagramTrsSz) {
		    case A_DiagramTrAddr: shD->prms[trndN].setAddr(val);		break;
		    case A_DiagramTrBordL: shD->prms[trndN].setBordL(s2r(val));		break;
		    case A_DiagramTrBordU: shD->prms[trndN].setBordU(s2r(val));		break;
		    case A_DiagramTrClr: shD->prms[trndN].setColor(getColor(val));	break;
		    case A_DiagramTrVal:
			shD->prms[trndN].setCurVal((val==EVAL_STR) ? EVAL_REAL : s2r(val));
			make_pct = false;
			break;
		    case A_DiagramTrScl: shD->prms[trndN].setScale(s2i(val));		break;
		    case A_DiagramTrWdth: shD->prms[trndN].setWidth(s2i(val));		break;
		    case A_DiagramTrProp: make_pct = false;				break;
		}
	    }
    }

    if(!w->allAttrLoad()) {
	if(reld_tr_dt)	{ loadData(w,reld_tr_dt==2); make_pct = true; }
	if(make_pct)	{ makePicture(w); up = true; }
	if(up && uiPrmPos != -1) {
	    w->update();
	    setCursor(w, shD->curTime);
	}
    }

    return (reld_tr_dt || make_pct || up);
}

void ShapeDiagram::loadData( WdgView *w, bool full )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    XMLNode req("set");
    req.setAttr("path",w->id()+"/%2fserv%2fattr");
    for(unsigned iP = 0; iP < shD->prms.size(); iP++) {
	shD->prms[iP].loadData(full);
	if(shD->prms[iP].arh_beg && shD->prms[iP].arh_end)
	    req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dprop",iP))->setText(TSYS::strMess("%.15g:%.15g:%.15g",
		(double)shD->prms[iP].arh_beg*1e-6,(double)shD->prms[iP].arh_end*1e-6,(double)shD->prms[iP].arh_per*1e-6));
    }
    if(req.childSize()) w->cntrIfCmd(req);
}

void ShapeDiagram::makePicture( WdgView *w )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if(!shD->en) return;
    switch(shD->type) {
	case FD_TRND:	makeTrendsPicture(w);	break;
	case FD_SPECTR:	makeSpectrumPicture(w);	break;
	case FD_XY:	makeXYPicture(w);	break;
    }
}

void ShapeDiagram::makeXYPicture( WdgView *w )
{
    int64_t d_cnt = 0;
    QPen grdPen, mrkPen;
    int  mrkHeight = 0, mrkWidth = 0;

    ShpDt *shD = (ShpDt*)w->shpData;

    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();

    //Prepare picture
    shD->pictObj = QImage(w->rect().size(),QImage::Format_ARGB32_Premultiplied);
    shD->pictObj.fill(0);

    QPainter pnt(&shD->pictObj);

    //Get generic parameters
    int64_t tSize = (int64_t)(1e6*shD->tSize);				//Trends size (us)
    int64_t tEnd  = shD->tTime;						//Trends end point (us)
    if(shD->tTimeCurent) tEnd = shD->arhEnd(shD->tTime);
    int64_t tPict = tEnd;
    int64_t tBeg  = tEnd - tSize;					//Trends begin point (us)
    if(shD->prms.empty() || tSize <= 0) return;
    int sclHor = shD->sclHor;						//Horisontal scale mode
    int sclVer = shD->sclVer;						//Vertical scale mode

    //Trends' area rect definition
    QRect tAr	= w->rect().adjusted(1, 1, -2*(shD->geomMargin+shD->border.width()+1), -2*(shD->geomMargin+shD->border.width()+1));

    //Main scales definition
    if(sclHor&FD_GRD_MARKS || sclVer&FD_GRD_MARKS) {
	// Set grid pen
	grdPen.setColor(shD->sclColor);
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth(vmax(1,(int)rRnd(vmin(w->xScale(true),w->yScale(true)))));

	// Set markers font and color
	if(sclHor&FD_MARKS || sclVer&FD_MARKS) {
	    mrkPen.setColor(shD->sclMarkColor);
	    QFont mrkFnt = shD->sclMarkFont;
	    mrkFnt.setPixelSize((double)mrkFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true)));
	    pnt.setFont(mrkFnt);
	    mrkHeight = pnt.fontMetrics().height() - pnt.fontMetrics().descent();
	    mrkWidth = pnt.fontMetrics().width("000000");
	    if(sclHor&FD_MARKS) {
		if(tAr.height() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclHor &= ~(FD_MARKS);
		else tAr.adjust(0, 0, 0, -mrkHeight);
	    }
	    if(sclVer&FD_MARKS && tAr.width() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclVer &= ~(FD_MARKS);
	}
    }

    //Calc vertical scale for main and individual
    int64_t	aVbeg, aVend;		//Corrected for allow data the trend begin and end point
    bool	vsPerc = true,		//Vertical scale percent mode
		isLog = sclVer&FD_LOG,	//Logarithmic scale
		isScale = (fabs(shD->sclVerSclOff) > 1 || fabs(shD->sclVerScl-100) > 1);
    double	curVl, vsMax = -3e300, vsMin = 3e300;	//Trend's vertical scale border

    // Get main scale for non individual parameters
    int prmInGrp = 0, prmGrpLast = -1;
    for(unsigned iP = 0, mainPerc = false; iP < shD->prms.size(); iP += 2) {
	TrendObj &cP = shD->prms[iP];
	if(!cP.val().size() || !cP.color().isValid() || (iP+1) >= shD->prms.size() ||
	    !shD->prms[iP+1].val().size() || !shD->prms[iP+1].color().isValid())	continue;

	cP.adjU = -3e300; cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL() && cP.valTp() != TFld::Boolean) {
	    // Check trend for valid data
	    aVbeg = vmax(tBeg, cP.valBeg());
	    aVend = vmin(tEnd, cP.valEnd());

	    if(aVbeg >= aVend) return;
	    // Calc value borders
	    bool end_vl = false;
	    int ipos = cP.val(aVbeg);
	    if(ipos && cP.val()[ipos].tm > aVbeg) ipos--;
	    while(true) {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend) end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL) {
		    curVl = cP.val()[ipos].val;
		    cP.adjL = vmin(cP.adjL, curVl); cP.adjU = vmax(cP.adjU, curVl);
		}
		ipos++;
	    }
	    if(cP.adjU == -3e300)	{ cP.adjU = 1; cP.adjL = 0; }
	    else if((cP.adjU-cP.adjL) < 1e-30 && fabs(cP.adjU) < 1e-30)	{ cP.adjU += 0.5; cP.adjL -= 0.5; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001) {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	else if(cP.bordU() <= cP.bordL() && cP.valTp() == TFld::Boolean) { cP.adjU = 1.5; cP.adjL = -0.5; }
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclVer|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS) continue;

	//  Check for value border allow
	if(!mainPerc && (vsMin > vsMax || vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = iP;
    }

    // Check for individual parameters and for possibility to merge it to group or create new for no group
    int prmIndiv = 0, prmIndivSc = -1;
    vector<int>	prmsInd;
    for(unsigned iP = 0; iP < shD->prms.size(); iP += 2) {
	TrendObj &cP = shD->prms[iP];
	cP.isIndiv = false;
	if(!cP.val().size() || !cP.color().isValid() || !(cP.wScale&FD_GRD_MARKS) || (iP+1) >= shD->prms.size() ||
	    !shD->prms[iP+1].val().size() || !shD->prms[iP+1].color().isValid()) continue;

	//  Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (vsMin < vsMax && vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2)) &&
	    (cP.mScale&FD_LOG) == (sclVer&FD_LOG))
	{
	    vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = iP;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = iP;
	else prmsInd.push_back(iP);
	if(cP.mScale&FD_LOG) {
	    cP.adjU = log10(vmax(1e-100,cP.adjU)); cP.adjL = log10(vmax(1e-100,cP.adjL));
	    if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.0001) {
		double wnt_dp = 0.0001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	if(isScale) {	//Vertical scale and offset apply
	    float vsDif = cP.adjU - cP.adjL;
	    cP.adjU += shD->sclVerSclOff*vsDif/100;		cP.adjL += shD->sclVerSclOff*vsDif/100;
	    cP.adjU += (shD->sclVerScl*vsDif/100-vsDif)/2;	cP.adjL -= (shD->sclVerScl*vsDif/100-vsDif)/2;
	}
    }
    if(prmInGrp)	prmsInd.push_back(-1);
    if(prmIndivSc >= 0)	prmsInd.push_back(prmIndivSc);

    // Final main scale adapting
    if(vsMin > vsMax) { vsPerc = true; vsMax = 100; vsMin = isLog ? pow(10,vmin(0,2-(tAr.height()/150))) : 0; }
    else vsPerc = false;
    if(isLog) {
	vsMax = log10(vmax(1e-100,vsMax)); vsMin = log10(vmax(1e-100,vsMin));
	if((vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.0001) {
	    double wnt_dp = 0.0001*fabs(vsMin+(vsMax-vsMin)/2) - (vsMax-vsMin);
	    vsMin -= wnt_dp/2; vsMax += wnt_dp/2;
	}
    }
    if(isScale) {	//Vertical scale and offset apply
	float vsDif = vsMax - vsMin;
	vsMax += shD->sclVerSclOff*vsDif/100; vsMin += shD->sclVerSclOff*vsDif/100;
	vsMax += (shD->sclVerScl*vsDif/100-vsDif)/2; vsMin -= (shD->sclVerScl*vsDif/100-vsDif)/2;
    }

    //Draw main and individual vertical scales
    float vmax_ln = tAr.height() / ((sclVer&FD_MARKS)?(2*mrkHeight):(int)(15*vmin(w->xScale(true),w->yScale(true))));
    for(unsigned iP = 0; vmax_ln >= 2 && iP < prmsInd.size(); iP++) {	//prmsInd[i]=-1 - for main scale
	bool	isLogT, vsPercT;
	char	sclVerT;
	QPen	grdPenT = grdPen;
	double	vsMinT, vsMaxT;
	double	vDiv = 1;
	if(prmsInd[iP] < 0) {	//Main scale process
	    // Draw environment
	    vsPercT = vsPerc;
	    isLogT = isLog;
	    sclVerT = sclVer;
	    grdPenT.setColor(shD->sclColor);
	    mrkPen.setColor(shD->sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0) {	//Set color for single parameter in main group
		grdPenT.setColor(shD->prms[prmGrpLast].color());
		mrkPen.setColor(shD->prms[prmGrpLast].color());
	    }
	    // Rounding
	    double v_len = vsMax - vsMin;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv; }
	    while(!isLogT && ((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = vsMin; vsMaxT = vsMax;
	}
	else {	//Individual scale process
	    TrendObj &cP = shD->prms[prmsInd[iP]];
	    // Draw environment
	    vsPercT = false;
	    isLogT = cP.mScale&FD_LOG;
	    sclVerT = cP.wScale;
	    grdPenT.setColor(cP.color());
	    mrkPen.setColor(cP.color());
	    // Rounding
	    double v_len = cP.adjU - cP.adjL;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv; }
	    while(!isLogT && ((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(iP < (prmsInd.size()-1))	sclVerT &= ~(FD_GRD);	//Hide grid for no last scale

	//Draw vertical grid and markers
	int markWdth = 0;
	if(sclVerT&FD_GRD_MARKS) {
	    string labVal;
	    pnt.setPen(grdPenT);
	    pnt.drawLine(tAr.x()-1, tAr.y(), tAr.x()-1, tAr.height());
	    for(double iV = ceil(vsMinT/vDiv)*vDiv; (vsMaxT-iV)/vDiv > -0.1; iV += vDiv) {
		int v_pos = tAr.y() + tAr.height() - (int)((double)tAr.height()*(iV-vsMinT)/(vsMaxT-vsMinT));
		if(sclVerT&FD_GRD) { pnt.setPen(grdPen); pnt.drawLine(tAr.x(), v_pos, tAr.x()+tAr.width(), v_pos); }
		else { pnt.setPen(grdPenT); pnt.drawLine(tAr.x()-3, v_pos, tAr.x()+3, v_pos); }

		if(sclVerT&FD_MARKS) {
		    bool isPerc = vsPercT && ((vsMaxT-iV-vDiv)/vDiv <= -0.1);
		    bool isMax = (v_pos-1-mrkHeight) < tAr.y();
		    pnt.setPen(mrkPen);
		    labVal = TSYS::strMess("%0.5g",(isLogT?pow(10,iV):iV)) + (isPerc?" %":"");
		    pnt.drawText(tAr.x()+2, v_pos-1+(isMax?mrkHeight:0), labVal.c_str());
		    markWdth = vmax(markWdth, pnt.fontMetrics().width(labVal.c_str()));
		}
	    }
	}
	if(iP < (prmsInd.size()-1)) tAr.adjust((markWdth?(markWdth+5):0), 0, 0, 0);
    }
    mrkPen.setColor(shD->sclMarkColor);	//Restore mark color

    //Calc horizontal scale for main and individual
    int64_t	aHbeg, aHend;		//Corrected for allow data the trend begin and end point
    bool	hsPerc = true,		//Horizontal scale percent mode
		isHLog = sclHor&FD_LOG,	//Logarithmic horizontal scale
		isHScale = (fabs(shD->sclHorSclOff) > 1 || fabs(shD->sclHorScl-100) > 1);
    double	hsMax = -3e300, hsMin = 3e300;	//Trend's horizontal scale border

    // Get main scale for non individual parameters
    prmInGrp = 0, prmGrpLast = -1;
    for(unsigned iP = 1, mainPerc = false; iP < shD->prms.size(); iP += 2) {
	TrendObj &cP = shD->prms[iP];
	if(!cP.val().size() || !cP.color().isValid() || !shD->prms[iP-1].val().size() || !shD->prms[iP-1].color().isValid())	continue;

	cP.adjU = -3e300; cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL() && cP.valTp() != TFld::Boolean) {
	    // Check trend for valid data
	    aHbeg = vmax(tBeg, cP.valBeg());
	    aHend = vmin(tEnd, cP.valEnd());

	    if(aHbeg >= aHend) return;
	    // Calc value borders
	    bool end_vl = false;
	    int ipos = cP.val(aHbeg);
	    if(ipos && cP.val()[ipos].tm > aHbeg) ipos--;
	    while(true) {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aHend) end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL) {
		    curVl = cP.val()[ipos].val;
		    cP.adjL = vmin(cP.adjL, curVl); cP.adjU = vmax(cP.adjU, curVl);
		}
		ipos++;
	    }
	    if(cP.adjU == -3e300)	{ cP.adjU = 1; cP.adjL = 0; }
	    else if((cP.adjU-cP.adjL) < 1e-30 && fabs(cP.adjU) < 1e-30)	{ cP.adjU += 0.5; cP.adjL -= 0.5; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001) {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	else if(cP.bordU() <= cP.bordL() && cP.valTp() == TFld::Boolean) { cP.adjU = 1.5; cP.adjL = -0.5; }
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclHor|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS) continue;

	//  Check for value border allow
	if(!mainPerc && (hsMin > hsMax || vmax(fabs((hsMax-cP.adjL)/(hsMax-hsMin)-1),fabs((cP.adjU-hsMin)/(hsMax-hsMin)-1)) < 0.2))
	{ hsMin = vmin(hsMin, cP.adjL); hsMax = vmax(hsMax, cP.adjU); }
	else { hsMax = -3e300; hsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = iP;
    }

    // Check for individual parameters and for possibility to merge it to group or create new for no group
    prmIndiv = 0, prmIndivSc = -1;
    prmsInd.clear();
    for(unsigned iP = 1; iP < shD->prms.size(); iP += 2) {
	TrendObj &cP = shD->prms[iP];
	cP.isIndiv = false;
	if(!cP.val().size() || !cP.color().isValid() || !(cP.wScale&FD_GRD_MARKS) ||
	    !shD->prms[iP-1].val().size() || !shD->prms[iP-1].color().isValid()) continue;

	//  Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (hsMin < hsMax && vmax(fabs((hsMax-cP.adjL)/(hsMax-hsMin)-1),fabs((cP.adjU-hsMin)/(hsMax-hsMin)-1)) < 0.2)) &&
	    (cP.mScale&FD_LOG) == (sclHor&FD_LOG))
	{
	    hsMin = vmin(hsMin, cP.adjL); hsMax = vmax(hsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = iP;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = iP;
	else prmsInd.push_back(iP);
	if(cP.mScale&FD_LOG) {
	    cP.adjU = log10(vmax(1e-100,cP.adjU)); cP.adjL = log10(vmax(1e-100,cP.adjL));
	    if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.0001) {
		double wnt_dp = 0.0001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	if(isHScale) {	//horizontal scale and offset apply
	    float hsDif = cP.adjU - cP.adjL;
	    cP.adjU += shD->sclHorSclOff*hsDif/100;		cP.adjL += shD->sclHorSclOff*hsDif/100;
	    cP.adjU += (shD->sclHorScl*hsDif/100-hsDif)/2;	cP.adjL -= (shD->sclHorScl*hsDif/100-hsDif)/2;
	}
    }
    if(prmInGrp)	prmsInd.push_back(-1);
    if(prmIndivSc >= 0)	prmsInd.push_back(prmIndivSc);

    // Final main scale adapting
    if(hsMin > hsMax) { hsPerc = true; hsMax = 100; hsMin = isHLog ? pow(10,vmin(0,2-(tAr.width()/150))) : 0; }
    else hsPerc = false;
    if(isHLog) {
	hsMax = log10(vmax(1e-100,hsMax)); hsMin = log10(vmax(1e-100,hsMin));
	if((hsMax-hsMin) / fabs(hsMin+(hsMax-hsMin)/2) < 0.0001) {
	    double wnt_dp = 0.0001*fabs(hsMin+(hsMax-hsMin)/2) - (hsMax-hsMin);
	    hsMin -= wnt_dp/2; hsMax += wnt_dp/2;
	}
    }
    if(isHScale) {	//Horizontal scale and offset apply
	float hsDif = hsMax - hsMin;
	hsMax += shD->sclHorSclOff*hsDif/100; hsMin += shD->sclHorSclOff*hsDif/100;
	hsMax += (shD->sclHorScl*hsDif/100-hsDif)/2; hsMin -= (shD->sclHorScl*hsDif/100-hsDif)/2;
    }

    // Draw main and individual horizontal scales
    float hmax_ln = tAr.width() / (int)((sclHor&FD_MARKS)?mrkWidth:15*vmin(w->xScale(true),w->yScale(true)));
    for(unsigned iP = 0; hmax_ln >= 2 && iP < prmsInd.size(); iP++) {	//prmsInd[i]=-1 - for main scale
	bool	isLogT, hsPercT;
	char	sclHorT;
	QPen	grdPenT = grdPen;
	double	hsMinT, hsMaxT;
	double	hDiv = 1;
	if(prmsInd[iP] < 0) {	//Main scale process
	    // Draw environment
	    hsPercT = hsPerc;

	    isLogT = isHLog;
	    sclHorT = sclHor;
	    grdPenT.setColor(shD->sclColor);
	    mrkPen.setColor(shD->sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0) {	//Set color for single parameter in main group
		grdPenT.setColor(shD->prms[prmGrpLast].color());
		mrkPen.setColor(shD->prms[prmGrpLast].color());
	    }
	    // Rounding
	    double h_len = hsMax - hsMin;
	    while(h_len > hmax_ln)		{ hDiv *= 10; h_len /= 10; }
	    while(h_len && h_len < hmax_ln/10)	{ hDiv /= 10; h_len *= 10; }
	    if(!isHScale) { hsMin = floor(hsMin/hDiv)*hDiv; hsMax = ceil(hsMax/hDiv)*hDiv; }
	    while(!isLogT && ((hsMax-hsMin)/hDiv) < hmax_ln/2) hDiv /= 2;
	    hsMinT = hsMin; hsMaxT = hsMax;
	}
	else {	//Individual scale process
	    TrendObj &cP = shD->prms[prmsInd[iP]];
	    // Draw environment
	    hsPercT = false;
	    isLogT = cP.mScale&FD_LOG;
	    sclHorT = cP.wScale;
	    grdPenT.setColor(cP.color());
	    mrkPen.setColor(cP.color());
	    // Rounding
	    double h_len = cP.adjU - cP.adjL;
	    while(h_len > hmax_ln)		{ hDiv *= 10; h_len /= 10; }
	    while(h_len && h_len < hmax_ln/10)	{ hDiv /= 10; h_len *= 10; }
	    if(!isHScale) { cP.adjL = floor(cP.adjL/hDiv)*hDiv; cP.adjU = ceil(cP.adjU/hDiv)*hDiv; }
	    while(!isLogT && ((cP.adjU-cP.adjL)/hDiv) < hmax_ln/2) hDiv /= 2;
	    hsMinT = cP.adjL; hsMaxT = cP.adjU;
	}
	if(iP < (prmsInd.size()-1))	sclHorT &= ~(FD_GRD);	//Hide grid for no last scale

	//Draw horizontal grid and markers
	if(sclHorT&FD_GRD_MARKS) {
	    string labVal;
	    int begMarkBrd = -5, endMarkBrd = tAr.x() + tAr.width() + 5;
	    pnt.setPen(grdPenT);
	    pnt.drawLine(tAr.x(), tAr.y()+tAr.height()+1, tAr.x()+tAr.width(), tAr.y()+tAr.height()+1);
	    for(double iH = ceil(hsMinT/hDiv)*hDiv; (hsMaxT-iH)/hDiv > -0.1; iH += hDiv) {
		int h_pos = tAr.x() + (int)((double)tAr.width()*(iH-hsMinT)/(hsMaxT-hsMinT));
		if(sclHorT&FD_GRD) { pnt.setPen(grdPen); pnt.drawLine(h_pos, tAr.y(), h_pos, tAr.y()+tAr.height()); }
		else { pnt.setPen(grdPenT); pnt.drawLine(h_pos, tAr.y()+tAr.height()-3, h_pos, tAr.y()+tAr.height()+3); }

		if(sclHorT&FD_MARKS) {
		    pnt.setPen(mrkPen);
		    bool isPerc = hsPercT && ((hsMaxT-iH-hDiv)/hDiv <= -0.1);
		    labVal = TSYS::strMess("%0.5g",(isLogT?pow(10,iH):iH)) + (isPerc?" %":"");
		    int wdth = pnt.fontMetrics().width(labVal.c_str());
		    int tpos = vmax(0, vmin(endMarkBrd-3-wdth,h_pos-wdth/2));
		    if((tpos+wdth) <= (endMarkBrd-3) && tpos >= (begMarkBrd+3)) {
			pnt.drawText(tpos, tAr.y()+tAr.height()+mrkHeight, labVal.c_str());
			begMarkBrd = vmax(begMarkBrd, tpos+wdth);
		    }
		}
	    }
	}
	if(iP < (prmsInd.size()-1)) tAr.adjust(0, (mrkHeight?(mrkHeight+5):0), 0, 0);
    }
    mrkPen.setColor(shD->sclMarkColor);	//Restore mark color

    //Draw curves
    for(unsigned iT = 0; (iT+1) < shD->prms.size(); iT += 2) {
	TrendObj &cP = shD->prms[iT], &cPX = shD->prms[iT+1];

	// Set trend's pen
	QPen trpen(cP.color());
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(vmax(1,vmin(10,(int)rRnd(cP.width()*vmin(w->xScale(true),w->yScale(true))))));
#if QT_VERSION < 0x050000
	if(cP.valTp() != TFld::Boolean && trpen.width() > 1) trpen.setCapStyle(Qt::RoundCap);
#else
	trpen.setCapStyle(Qt::FlatCap);
#endif
	pnt.setRenderHint(QPainter::Antialiasing, (trpen.width()>=2));
	pnt.setPen(trpen);

	// Prepare generic parameters
	aVbeg = vmax(tBeg, vmax(cP.valBeg(),cPX.valBeg()));
	aVend = vmin(tEnd, vmin(cP.valEnd(),cPX.valEnd()));

	if(aVbeg >= aVend || !cP.color().isValid() || !cPX.color().isValid()) continue;
	int aPosBeg = cP.val(aVbeg);
	if(aPosBeg && cP.val()[aPosBeg].tm > aVbeg) aPosBeg--;
	bool vsPercT = cP.isIndiv ? false : vsPerc;
	bool isLogT = cP.isIndiv ? (cP.wScale&FD_LOG) : isLog;
	double vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	double vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	int aPosBegX = cPX.val(aVbeg);
	if(aPosBegX && cPX.val()[aPosBegX].tm > aVbeg) aPosBegX--;
	bool hsPercT = cPX.isIndiv ? false : hsPerc;
	bool isHLogT = cPX.isIndiv ? (cPX.wScale&FD_LOG) : isHLog;
	double hsMaxT = cPX.isIndiv ? cPX.adjU : hsMax;
	double hsMinT = cPX.isIndiv ? cPX.adjL : hsMin;

	// Y: Prepare border for percent trend, ONLY!
	float bordL = cP.bordL();
	float bordU = cP.bordU();
	if(vsPercT && bordL >= bordU) {
	    bordU = -3e300, bordL = 3e300;
	    bool end_vl = false;
	    for(int ipos = aPosBeg; true; ipos++) {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend) end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL) {
		    bordL = vmin(bordL, cP.val()[ipos].val);
		    bordU = vmax(bordU, cP.val()[ipos].val);
		}
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	// X: Prepare XY data buffer and prepare border for percent trend, ONLY!
	float xBordL = cPX.bordL();
	float xBordU = cPX.bordU();
	vector< pair<double,double> > dBuf;
	{
	    bool xNeedRngChk = (hsPercT && xBordL >= xBordU);
	    if(xNeedRngChk) xBordU = -3e300, xBordL = 3e300;
	    bool end_vl = false;
	    for(int ipos = aPosBegX, iVpos = 0; true; ipos++) {
		if(ipos >= (int)cPX.val().size() || end_vl)	break;
		if(cPX.val()[ipos].tm >= aVend) end_vl = true;
		if(cPX.val()[ipos].val != EVAL_REAL) {
		    if((iVpos=cP.val(cPX.val()[ipos].tm)) < (int)cP.val().size() && cP.val()[iVpos].val != EVAL_REAL)
			dBuf.push_back(pair<double,double>(cPX.val()[ipos].val,cP.val()[iVpos].val));
		    if(xNeedRngChk) {
			xBordL = vmin(xBordL, cPX.val()[ipos].val);
			xBordU = vmax(xBordU, cPX.val()[ipos].val);
		    }
		}
	    }
	    if(xNeedRngChk) {
		float vMarg = (xBordU-xBordL)/10;
		xBordL -= vMarg;
		xBordU += vMarg;
	    }
	}

	// Draw curve
	int c_vpos, c_hpos, c_vposPrev = -1, c_hposPrev = -1;
	double curVl, curVlX;
	for(vector< pair<double,double> >::iterator iD = dBuf.begin(); iD != dBuf.end(); ++iD) {
	    curVl = vsPercT ? 100*(iD->second-bordL)/(bordU-bordL) : iD->second;
	    curVlX = hsPercT ? 100*(iD->first-xBordL)/(xBordU-xBordL) : iD->first;
	    c_vpos = tAr.y() + tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,curVl)):curVl)-vsMinT)/(vsMaxT-vsMinT))));
	    c_hpos = tAr.x() + (int)((double)tAr.width()*vmax(0,vmin(1,((isHLogT?log10(vmax(1e-100,curVlX)):curVlX)-hsMinT)/(hsMaxT-hsMinT))));
	    if(c_vposPrev < 0 || c_hposPrev < 0) pnt.drawPoint(c_hpos, c_vpos);
	    else if(c_hposPrev != c_hpos || c_vposPrev != c_vpos) pnt.drawLine(c_hposPrev, c_vposPrev, c_hpos, c_vpos);
	    c_hposPrev = c_hpos; c_vposPrev = c_vpos;
	}

	// Draw curent point
	int iVpos = cP.val(aVend);
	int iVposX = cPX.val(aVend);
	if(iVpos < (int)cP.val().size() && iVposX < (int)cPX.val().size() && cP.val()[iVpos].val != EVAL_REAL && cPX.val()[iVposX].val != EVAL_REAL)
	{
	    curVl = vsPercT ? 100*(cP.val()[iVpos].val-bordL)/(bordU-bordL) : cP.val()[iVpos].val;
	    curVlX = hsPercT ? 100*(cPX.val()[iVposX].val-xBordL)/(xBordU-xBordL) : cPX.val()[iVposX].val;
	    c_vpos = tAr.y() + tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,curVl)):curVl)-vsMinT)/(vsMaxT-vsMinT))));
	    c_hpos = tAr.x() + (int)((double)tAr.width()*vmax(0,vmin(1,((isHLogT?log10(vmax(1e-100,curVlX)):curVlX)-hsMinT)/(hsMaxT-hsMinT))));
	    pnt.drawLine(c_hpos-trpen.width()*5, c_vpos-trpen.width()*5, c_hpos+trpen.width()*5, c_vpos+trpen.width()*5);
	    pnt.drawLine(c_hpos-trpen.width()*5, c_vpos+trpen.width()*5, c_hpos+trpen.width()*5, c_vpos-trpen.width()*5);

	    AttrValS attrs;
	    attrs.push_back(std::make_pair(TSYS::strMess("prm%dval",iT),r2s(cP.val()[iVpos].val,6)));
	    attrs.push_back(std::make_pair(TSYS::strMess("prm%dval",iT+1),r2s(cPX.val()[iVposX].val,6)));
	    attrs.push_back(std::make_pair("curSek",i2s(aVend/1000000)));
	    attrs.push_back(std::make_pair("curUSek",i2s(aVend%1000000)));
	    w->attrsSet(attrs);
	}
    }

    shD->pictRect = tAr;
    shD->tPict = tPict;

    if(mess_lev() == TMess::Debug) mess_debug(mod->nodePath().c_str(), _("Trend build: %f ms."), 1e-3*(TSYS::curTime()-d_cnt));
}

void ShapeDiagram::makeSpectrumPicture( WdgView *w )
{
    QPen grdPen, mrkPen;
    int  mrkHeight = 0, mrkWidth = 0;

    ShpDt *shD = (ShpDt*)w->shpData;

    //Prepare picture
    shD->pictObj = QImage(w->rect().size(), QImage::Format_ARGB32_Premultiplied);
    shD->pictObj.fill(0);

    QPainter pnt(&shD->pictObj);

    //Get generic parameters
    int64_t tSize = (int64_t)(1e6*shD->tSize);				//Time size (us)
    if(shD->prms.empty() || tSize <= 0) return;
    int sclHor = shD->sclHor;						//Horisontal scale mode
    int sclVer = shD->sclVer;						//Vertical scale mode

    //Trends' area rect definition
    QRect tAr  = w->rect().adjusted(1,1,-2*(shD->geomMargin+shD->border.width()+1),-2*(shD->geomMargin+shD->border.width()+1));	//Curves of spectrum area rect

    //Main scales definition
    if(sclHor&FD_GRD_MARKS || sclVer&FD_GRD_MARKS) {
	// Set grid pen
	grdPen.setColor(shD->sclColor);
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth(vmax(1,rRnd(vmin(w->xScale(true),w->yScale(true)))));
	// Set markers font and color
	if(sclHor&FD_MARKS || sclVer&FD_MARKS) {
	    mrkPen.setColor(shD->sclMarkColor);
	    QFont mrkFnt = shD->sclMarkFont;
	    mrkFnt.setPixelSize((double)mrkFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true)));
	    pnt.setFont(mrkFnt);
	    mrkHeight = pnt.fontMetrics().height() - pnt.fontMetrics().descent();
	    mrkWidth = pnt.fontMetrics().width("000000");
	    if(sclHor&FD_MARKS) {
		if(tAr.height() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclHor &= ~(FD_MARKS);
		else tAr.adjust(0,0,0,-mrkHeight);
	    }
	    if(sclVer&FD_MARKS && tAr.width() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclVer &= ~(FD_MARKS);
	}
    }

#if HAVE_FFTW3_H
    //Calc vertical scale for main and individual
    double curVl, vsMax = -3e300, vsMin = 3e300;//Trend's vertical scale border
    bool   vsPerc = true;			//Vertical scale percent mode
    bool isScale = (fabs(shD->sclVerSclOff) > 1 || fabs(shD->sclVerScl-100) > 1);

    // Get main scale for non individual parameters
    int prmInGrp = 0, prmGrpLast = -1;
    for(unsigned iP = 0, mainPerc = false; iP < shD->prms.size(); iP++) {
	TrendObj &cP = shD->prms[iP];
	if(!cP.fftN || !cP.color().isValid())	continue;

	cP.adjU = -3e300; cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL()) {
	    //  Calc value borders
	    double vlOff = cP.fftOut[0][0]/cP.fftN;
	    for(int iV = 1; iV < (cP.fftN/2+1); iV++) {
		curVl = vlOff+pow(pow(cP.fftOut[iV][0],2)+pow(cP.fftOut[iV][1],2),0.5)/(cP.fftN/2+1);
		cP.adjL = vmin(cP.adjL, curVl); cP.adjU = vmax(cP.adjU, curVl);
	    }
	    if(cP.adjU == cP.adjL)	{ cP.adjU += 1.0; cP.adjL -= 1.0; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001) {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2;
		cP.adjU += wnt_dp/2;
	    }
	}
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclVer|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS) continue;

	// Check for value border allow
	if(!mainPerc && (vsMin > vsMax || vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = iP;
    }

    // Check for individual parameters and for possibility to merge it to group or create new for no group
    int prmIndiv = 0;
    int prmIndivSc = -1;
    vector<int> prmsInd;
    for(unsigned iP = 0; iP < shD->prms.size(); iP++) {
	TrendObj &cP = shD->prms[iP];
	cP.isIndiv = false;
	if(!cP.fftN || !cP.color().isValid() || !(cP.wScale&FD_GRD_MARKS)) continue;
	// Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (vsMin < vsMax && vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))) {
	    vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = iP;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = iP;
	else prmsInd.push_back(iP);
	if(isScale) {	//Vertical scale and offset apply
	    float vsDif = cP.adjU - cP.adjL;
	    cP.adjU += shD->sclVerSclOff*vsDif/100;             cP.adjL += shD->sclVerSclOff*vsDif/100;
	    cP.adjU += (shD->sclVerScl*vsDif/100-vsDif)/2;      cP.adjL -= (shD->sclVerScl*vsDif/100-vsDif)/2;
	}
    }
    if(prmInGrp) prmsInd.push_back(-1);
    if(prmIndivSc >= 0) prmsInd.push_back(prmIndivSc);

    // Final main scale adapting
    if(vsMin > vsMax) { vsPerc = true; vsMax = 100; vsMin = 0; }
    else vsPerc = false;
    if(isScale) {	//Vertical scale and offset apply
	float vsDif = vsMax - vsMin;
	vsMax += shD->sclVerSclOff*vsDif/100; vsMin += shD->sclVerSclOff*vsDif/100;
	vsMax += (shD->sclVerScl*vsDif/100-vsDif)/2; vsMin -= (shD->sclVerScl*vsDif/100-vsDif)/2;
    }

    //Draw main and individual vertical scales
    double vmax_ln = tAr.height() / ((sclVer&FD_MARKS)?(2*mrkHeight):(int)(15*vmin(w->xScale(true),w->yScale(true))));
    for(unsigned iP = 0; vmax_ln >= 2 && iP < prmsInd.size(); iP++) {	//prmsInd[i]=-1 - for main scale
	bool	vsPercT;
	char	sclVerT;
	QPen	grdPenT = grdPen;
	double	vsMinT, vsMaxT;
	double	vDiv = 1;
	if(prmsInd[iP] < 0) {	//Main scale process
	    // Draw environment
	    vsPercT = vsPerc;
	    sclVerT = sclVer;
	    grdPenT.setColor(shD->sclColor);
	    mrkPen.setColor(shD->sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0) {	//Set color for single parameter in main group
		grdPenT.setColor(shD->prms[prmGrpLast].color());
		mrkPen.setColor(shD->prms[prmGrpLast].color());
	    }
	    // Rounding
	    double v_len = vsMax - vsMin;
	    while(v_len > vmax_ln)      { vDiv *= 10; v_len /= 10; }
	    while(v_len < vmax_ln/10)   { vDiv /= 10; v_len *= 10; }
	    if(!isScale)        { vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv; }
	    while(((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = vsMin; vsMaxT = vsMax;
	}
	else {	//Individual scale process
	    TrendObj &cP = shD->prms[prmsInd[iP]];
	    // Draw environment
	    vsPercT = false;
	    sclVerT = cP.wScale;
	    grdPenT.setColor(cP.color());
	    mrkPen.setColor(cP.color());
	    // Rounding
	    double v_len = cP.adjU - cP.adjL;
	    while(v_len > vmax_ln)	{ vDiv *= 10; v_len /= 10; }
	    while(v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale)		{ cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv; }
	    while(((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(iP < (prmsInd.size()-1))	sclVerT &= ~(FD_GRD);	//Hide grid for no last scale

	// Draw vertical grid and markers
	int markWdth = 0;
	if(sclVerT&FD_GRD_MARKS) {
	    string labVal;
	    pnt.setPen(grdPenT);
	    pnt.drawLine(tAr.x()-1, tAr.y(), tAr.x()-1, tAr.height());
	    for(double iV = ceil(vsMinT/vDiv)*vDiv; (vsMaxT-iV)/vDiv > -0.1; iV += vDiv) {
		int v_pos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(iV-vsMinT)/(vsMaxT-vsMinT));
		if(sclVerT&FD_GRD) { pnt.setPen(grdPen); pnt.drawLine(tAr.x(), v_pos, tAr.x()+tAr.width(), v_pos); }
		else { pnt.setPen(grdPenT); pnt.drawLine(tAr.x()-3, v_pos, tAr.x()+3, v_pos); }

		if(sclVerT&FD_MARKS) {
		    bool isPerc = vsPercT && ((vsMaxT-iV-vDiv)/vDiv <= -0.1);
		    bool isMax = (v_pos-1-mrkHeight) < tAr.y();
		    pnt.setPen(mrkPen);
		    labVal = TSYS::strMess("%0.5g",iV)+(isPerc?" %":"");
		    pnt.drawText(tAr.x()+2, v_pos-1+(isMax?mrkHeight:0), labVal.c_str());
		    markWdth = vmax(markWdth, pnt.fontMetrics().width(labVal.c_str()));
		}
	    }
	}
	if(iP < (prmsInd.size()-1)) tAr.adjust((markWdth?(markWdth+5):0), 0, 0, 0);
    }
    mrkPen.setColor(shD->sclMarkColor); //Restore mark color
#endif

    //Calc horizontal scale
    int fftN = w->size().width();
    double fftBeg = 1e6/(double)tSize;			//Minimum frequency or maximum period time (s)
    double fftEnd = (double)fftN*fftBeg/2;		//Maximum frequency or minimum period time (s)
    double hDiv = 1;					//Horisontal scale divisor
    int hmax_ln = tAr.width() / (int)((sclHor&FD_MARKS)?mrkWidth:15*vmin(w->xScale(true),w->yScale(true)));
    if(hmax_ln >= 2) {
	double hLen = fftEnd-fftBeg;
	while(hLen/hDiv > hmax_ln)	hDiv *= 10;
	while(hLen/hDiv < hmax_ln/10)	hDiv /= 10;
	fftBeg = floor(10*fftBeg/hDiv)*hDiv/10;
	fftEnd = ceil(10*fftEnd/hDiv)*hDiv/10;
	while(((fftEnd-fftBeg)/hDiv) < hmax_ln/2) hDiv/=2;

	// Draw horisontal grid and markers
	if(sclHor&FD_GRD_MARKS) {
	    string labH;
	    double labDiv = 1;
	    if(fftEnd > 1000) labDiv = 1000;
	    //  Draw generic grid line
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(),tAr.y()+tAr.height(),tAr.x()+tAr.width(),tAr.y()+tAr.height());
	    //  Draw full trend's data and time to the trend end position
	    int begMarkBrd = -5;
	    int endMarkBrd = tAr.x()+tAr.width();
	    if(sclHor&FD_MARKS) {
		pnt.setPen(mrkPen);
		labH = TSYS::strMess("%0.5g",fftEnd/labDiv)+((labDiv==1000)?_("kHz"):_("Hz"));

		int markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().width(labH.c_str());
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+mrkHeight,labH.c_str());
	    }
	    //  Draw grid and/or markers
	    for(double i_h = fftBeg; (fftEnd-i_h)/hDiv > -0.1; i_h += hDiv) {
		//   Draw grid
		pnt.setPen(grdPen);
		int h_pos = tAr.x()+(int)((double)tAr.width()*(i_h-fftBeg)/(fftEnd-fftBeg));
		if(sclHor&FD_GRD) pnt.drawLine(h_pos, tAr.y(), h_pos, tAr.y()+tAr.height());
		else pnt.drawLine(h_pos, tAr.y()+tAr.height()-3, h_pos, tAr.y()+tAr.height()+3);

		if(sclHor&FD_MARKS) {
		    pnt.setPen(mrkPen);
		    labH = TSYS::strMess("%0.5g", i_h/labDiv);
		    int wdth = pnt.fontMetrics().width(labH.c_str());
		    int tpos = vmax(h_pos-wdth/2, 0);
		    if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3))
			pnt.drawText(tpos, tAr.y()+tAr.height()+mrkHeight, labH.c_str());
		    begMarkBrd = vmax(begMarkBrd,tpos+wdth);
		}
	    }
	}
    }

#if HAVE_FFTW3_H
    // Draw trends
    for(unsigned iT = 0; iT < shD->prms.size(); iT++) {
	TrendObj &cP = shD->prms[iT];
	if(!cP.fftN || !cP.color().isValid()) continue;

	//  Set trend's pen
	QPen trpen(cP.color());
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(vmax(1,vmin(10,(int)rRnd(cP.width()*vmin(w->xScale(true),w->yScale(true))))));
#if QT_VERSION < 0x050000
	if(trpen.width() > 1) trpen.setCapStyle(Qt::RoundCap);
#else
	trpen.setCapStyle(Qt::FlatCap);
#endif
	pnt.setRenderHint(QPainter::Antialiasing, (trpen.width()>=2));
	pnt.setPen(trpen);

	//  Prepare generic parameters
	double vlOff = cP.fftOut[0][0]/cP.fftN;
	double fftDt = (1e6/(double)tSize)*(double)w->size().width()/cP.fftN;

	bool vsPercT = cP.isIndiv ? false : vsPerc;
	double vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	double vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	//  Prepare border for percent trend
	double bordL = cP.bordL();
	double bordU = cP.bordU();
	if(vsPercT && bordL >= bordU) {
	    bordU = -3e300, bordL = 3e300;
	    for(int iV = 1; iV < (cP.fftN/2+1); iV++) {
		curVl = vlOff + pow(pow(cP.fftOut[iV][0],2)+pow(cP.fftOut[iV][1],2),0.5)/(cP.fftN/2+1);
		bordL = vmin(bordL, curVl);
		bordU = vmax(bordU, curVl);
	    }
	    double vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	//  Draw trend
	double prevVl = EVAL_REAL;
	int curPos = 0, prevPos = 0;
	for(int iV = 1; iV < (cP.fftN/2+1); iV++) {
	    curVl = vlOff + pow(pow(cP.fftOut[iV][0],2)+pow(cP.fftOut[iV][1],2),0.5)/(cP.fftN/2+1);
	    if(vsPercT) curVl = 100*(curVl-bordL)/(bordU-bordL);
	    curPos = tAr.x() + (int)((double)tAr.width()*(fftDt*iV-fftBeg)/(fftEnd-fftBeg));

	    int c_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,(curVl-vsMinT)/(vsMaxT-vsMinT))));
	    if(prevVl == EVAL_REAL) pnt.drawPoint(curPos, c_vpos);
	    else {
		int c_vpos_prv = tAr.y() + tAr.height() - (int)((double)tAr.height()*vmax(0,vmin(1,(prevVl-vsMinT)/(vsMaxT-vsMinT))));
		pnt.drawLine(prevPos, c_vpos_prv, curPos, c_vpos);
	    }
	    prevPos = curPos;
	    prevVl = curVl;
	}

	//  Update value on cursor
	if(shD->active && shD->tTimeCurent && shD->trcPer) {
	    double curFrq = vmax(vmin(1e6/(double)shD->curTime,fftEnd),fftBeg);
	    curPos = (int)(curFrq/fftDt);
	    if(curPos >= 1 && curPos < (cP.fftN/2+1)) {
		double val = cP.fftOut[0][0]/cP.fftN + pow(pow(cP.fftOut[curPos][0],2)+pow(cP.fftOut[curPos][1],2),0.5)/(cP.fftN/2+1);
		w->attrSet(TSYS::strMess("prm%dval",iT), r2s(val,6), A_DiagramTrs+A_DiagramTrVal+A_DiagramTrsSz*iT, true);
	    }
	}
    }
#endif

    shD->fftBeg = fftBeg;
    shD->fftEnd = fftEnd;
    shD->pictRect = tAr;
}

void ShapeDiagram::makeTrendsPicture( WdgView *w )
{
    int64_t d_cnt = 0;
    QPen grdPen, mrkPen;
    int  mrkHeight = 0, mrkWidth = 0;

    ShpDt *shD = (ShpDt*)w->shpData;

    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();

    //Prepare picture
    shD->pictObj = QImage(w->rect().size(),QImage::Format_ARGB32_Premultiplied);
    shD->pictObj.fill(0);

    QPainter pnt(&shD->pictObj);

    //Get generic parameters
    int64_t tSize = (int64_t)(1e6*shD->tSize);				//Trends size (us)
    int64_t tEnd  = shD->tTime;						//Trends end point (us)
    if(shD->tTimeCurent) tEnd = shD->arhEnd(shD->tTime);
    int64_t tPict = tEnd;
    int64_t tBeg  = tEnd - tSize;					//Trends begin point (us)
    if(shD->prms.empty() || tSize <= 0) return;
    int sclHor = shD->sclHor;						//Horisontal scale mode
    int sclVer = shD->sclVer;						//Vertical scale mode

    //Trends' area rect definition
    QRect tAr  = w->rect().adjusted(1,1,-2*(shD->geomMargin+shD->border.width()+1),-2*(shD->geomMargin+shD->border.width()+1));

    //Main scales definition
    if(sclHor&FD_GRD_MARKS || sclVer&FD_GRD_MARKS) {
	// Set grid pen
	grdPen.setColor(shD->sclColor);
	grdPen.setStyle(Qt::SolidLine);
	grdPen.setWidth(vmax(1,(int)rRnd(vmin(w->xScale(true),w->yScale(true)))));

	// Set markers font and color
	if(sclHor&FD_MARKS || sclVer&FD_MARKS) {
	    mrkPen.setColor(shD->sclMarkColor);
	    QFont mrkFnt = shD->sclMarkFont;
	    mrkFnt.setPixelSize((double)mrkFnt.pixelSize()*vmin(w->xScale(true),w->yScale(true)));
	    pnt.setFont(mrkFnt);
	    mrkHeight = pnt.fontMetrics().height()-pnt.fontMetrics().descent();
	    mrkWidth = pnt.fontMetrics().width("000000");
	    if(sclHor&FD_MARKS) {
		if(tAr.height() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclHor &= ~(FD_MARKS);
		else tAr.adjust(0,0,0,-2*mrkHeight);
	    }
	    if(sclVer&FD_MARKS && tAr.width() < (int)(100*vmin(w->xScale(true),w->yScale(true)))) sclVer &= ~(FD_MARKS);
	}
    }

    //Calc vertical scale for main and individual
    int64_t	aVend;			//Corrected for allow data the trend end point
    int64_t	aVbeg;			//Corrected for allow data the trend begin point
    bool	vsPerc = true;		//Vertical scale percent mode
    bool	isLog = sclVer&FD_LOG;	//Logarithmic scale
    double	curVl, vsMax = -3e300, vsMin = 3e300;	//Trend's vertical scale border
    bool	isScale = (fabs(shD->sclVerSclOff) > 1 || fabs(shD->sclVerScl-100) > 1);

    // Get main scale for non individual parameters
    int prmInGrp = 0, prmGrpLast = -1;
    for(unsigned iP = 0, mainPerc = false; iP < shD->prms.size(); iP++) {
	TrendObj &cP = shD->prms[iP];
	if(!cP.val().size() || !cP.color().isValid())	continue;

	cP.adjU = -3e300; cP.adjL = 3e300;
	if(cP.bordU() <= cP.bordL() && cP.valTp() != 0) {
	    // Check trend for valid data
	    aVbeg = vmax(tBeg, cP.valBeg());
	    aVend = vmin(tEnd, cP.valEnd());

	    if(aVbeg >= aVend) return;
	    // Calc value borders
	    bool end_vl = false;
	    int ipos = cP.val(aVbeg);
	    if(ipos && cP.val()[ipos].tm > aVbeg) ipos--;
	    while(true) {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend) end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL) {
		    curVl = cP.val()[ipos].val;
		    cP.adjL = vmin(cP.adjL,curVl); cP.adjU = vmax(cP.adjU,curVl);
		}
		ipos++;
	    }
	    if(cP.adjU == -3e300)	{ cP.adjU = 1.0; cP.adjL = 0.0; }
	    else if((cP.adjU-cP.adjL) < 1e-30 && fabs(cP.adjU) < 1e-30)	{ cP.adjU += 0.5; cP.adjL -= 0.5; }
	    else if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.001) {
		double wnt_dp = 0.001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	else if(cP.bordU() <= cP.bordL() && cP.valTp() == 0)	{ cP.adjU = 1.5; cP.adjL = -0.5; }
	else { cP.adjU = cP.bordU(); cP.adjL = cP.bordL(); }

	cP.wScale = cP.mScale&(sclVer|FD_LOG);
	if(cP.wScale&FD_GRD_MARKS) continue;

	//  Check for value border allow
	if(!mainPerc && (vsMin > vsMax || vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2))
	{ vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU); }
	else { vsMax = -3e300; vsMin = 3e300; mainPerc = true; }

	prmInGrp++; prmGrpLast = iP;
    }

    // Check for individual parameters and for possibility to merge it to group or create new for no group
    int prmIndiv = 0;
    int prmIndivSc = -1;
    vector<int>	prmsInd;
    for(unsigned iP = 0; iP < shD->prms.size(); iP++) {
	TrendObj &cP = shD->prms[iP];
	cP.isIndiv = false;
	if(!cP.val().size() || !cP.color().isValid() || !(cP.wScale&FD_GRD_MARKS)) continue;
	//  Check for include to present or create new group and exclude from individual
	if((!prmInGrp || (vsMin < vsMax && vmax(fabs((vsMax-cP.adjL)/(vsMax-vsMin)-1),fabs((cP.adjU-vsMin)/(vsMax-vsMin)-1)) < 0.2)) &&
	    (cP.mScale&FD_LOG) == (sclVer&FD_LOG))
	{
	    vsMin = vmin(vsMin, cP.adjL); vsMax = vmax(vsMax, cP.adjU);
	    prmInGrp++; prmGrpLast = iP;
	    continue;
	}
	cP.isIndiv = true;
	prmIndiv++;
	if(prmIndivSc < 0 && cP.mScale&FD_GRD) prmIndivSc = iP;
	else prmsInd.push_back(iP);
	if(cP.mScale&FD_LOG) {
	    cP.adjU = log10(vmax(1e-100,cP.adjU)); cP.adjL = log10(vmax(1e-100,cP.adjL));
	    if((cP.adjU-cP.adjL) / fabs(cP.adjL+(cP.adjU-cP.adjL)/2) < 0.0001) {
		double wnt_dp = 0.0001*fabs(cP.adjL+(cP.adjU-cP.adjL)/2)-(cP.adjU-cP.adjL);
		cP.adjL -= wnt_dp/2; cP.adjU += wnt_dp/2;
	    }
	}
	if(isScale) {	//Vertical scale and offset apply
	    float vsDif = cP.adjU - cP.adjL;
	    cP.adjU += shD->sclVerSclOff*vsDif/100;		cP.adjL += shD->sclVerSclOff*vsDif/100;
	    cP.adjU += (shD->sclVerScl*vsDif/100-vsDif)/2;	cP.adjL -= (shD->sclVerScl*vsDif/100-vsDif)/2;
	}
    }
    if(prmInGrp)	prmsInd.push_back(-1);
    if(prmIndivSc >= 0)	prmsInd.push_back(prmIndivSc);

    // Final main scale adapting
    if(vsMin > vsMax) { vsPerc = true; vsMax = 100; vsMin = isLog ? pow(10,vmin(0,2-(tAr.height()/150))) : 0; }
    else vsPerc = false;
    if(isLog) {
	vsMax = log10(vmax(1e-100,vsMax)); vsMin = log10(vmax(1e-100,vsMin));
	if((vsMax-vsMin) / fabs(vsMin+(vsMax-vsMin)/2) < 0.0001) {
	    double wnt_dp = 0.0001*fabs(vsMin+(vsMax-vsMin)/2)-(vsMax-vsMin);
	    vsMin -= wnt_dp/2; vsMax += wnt_dp/2;
	}
    }
    if(isScale) {	//Vertical scale and offset apply
	float vsDif = vsMax - vsMin;
	vsMax += shD->sclVerSclOff*vsDif/100; vsMin += shD->sclVerSclOff*vsDif/100;
	vsMax += (shD->sclVerScl*vsDif/100-vsDif)/2; vsMin -= (shD->sclVerScl*vsDif/100-vsDif)/2;
    }

    //Draw main and individual vertical scales
    float vmax_ln = tAr.height() / ((sclVer&FD_MARKS)?(2*mrkHeight):(int)(15*vmin(w->xScale(true),w->yScale(true))));
    for(unsigned iP = 0; vmax_ln >= 2 && iP < prmsInd.size(); iP++) {	//prmsInd[i]=-1 - for main scale
	bool	isLogT, vsPercT;
	char	sclVerT;
	QPen	grdPenT = grdPen;
	double	vsMinT, vsMaxT;
	double	vDiv = 1;
	if(prmsInd[iP] < 0) {	//Main scale process
	    // Draw environment
	    vsPercT = vsPerc;
	    isLogT = isLog;
	    sclVerT = sclVer;
	    grdPenT.setColor(shD->sclColor);
	    mrkPen.setColor(shD->sclMarkColor);
	    if(prmInGrp == 1 && prmGrpLast >= 0) {	//Set color for single parameter in main group
		grdPenT.setColor(shD->prms[prmGrpLast].color());
		mrkPen.setColor(shD->prms[prmGrpLast].color());
	    }
	    // Rounding
	    double v_len = vsMax - vsMin;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { vsMin = floor(vsMin/vDiv)*vDiv; vsMax = ceil(vsMax/vDiv)*vDiv; }
	    while(!isLogT && ((vsMax-vsMin)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = vsMin; vsMaxT = vsMax;
	}
	else {	//Individual scale process
	    TrendObj &cP = shD->prms[prmsInd[iP]];
	    // Draw environment
	    vsPercT = false;
	    isLogT = cP.mScale&FD_LOG;
	    sclVerT = cP.wScale;
	    grdPenT.setColor(cP.color());
	    mrkPen.setColor(cP.color());
	    // Rounding
	    double v_len = cP.adjU - cP.adjL;
	    while(v_len > vmax_ln)		{ vDiv *= 10; v_len /= 10; }
	    while(v_len && v_len < vmax_ln/10)	{ vDiv /= 10; v_len *= 10; }
	    if(!isScale) { cP.adjL = floor(cP.adjL/vDiv)*vDiv; cP.adjU = ceil(cP.adjU/vDiv)*vDiv; }
	    while(!isLogT && ((cP.adjU-cP.adjL)/vDiv) < vmax_ln/2) vDiv /= 2;
	    vsMinT = cP.adjL; vsMaxT = cP.adjU;
	}
	if(iP < (prmsInd.size()-1))	sclVerT &= ~(FD_GRD);	//Hide grid for no last scale

	//Draw vertical grid and markers
	int markWdth = 0;
	if(sclVerT&FD_GRD_MARKS) {
	    string labVal;
	    pnt.setPen(grdPenT);
	    pnt.drawLine(tAr.x()-1, tAr.y(), tAr.x()-1, tAr.height());
	    for(double iV = ceil(vsMinT/vDiv)*vDiv; (vsMaxT-iV)/vDiv > -0.1; iV += vDiv) {
		int v_pos = tAr.y()+tAr.height()-(int)((double)tAr.height()*(iV-vsMinT)/(vsMaxT-vsMinT));
		if(sclVerT&FD_GRD) { pnt.setPen(grdPen); pnt.drawLine(tAr.x(), v_pos, tAr.x()+tAr.width(), v_pos); }
		else { pnt.setPen(grdPenT); pnt.drawLine(tAr.x()-3, v_pos, tAr.x()+3, v_pos); }

		if(sclVerT&FD_MARKS) {
		    bool isPerc = vsPercT && ((vsMaxT-iV-vDiv)/vDiv <= -0.1);
		    bool isMax = (v_pos-1-mrkHeight) < tAr.y();
		    pnt.setPen(mrkPen);
		    labVal = TSYS::strMess("%0.5g",(isLogT?pow(10,iV):iV)) + (isPerc?" %":"");
		    pnt.drawText(tAr.x()+2, v_pos-1+(isMax?mrkHeight:0), labVal.c_str());
		    markWdth = vmax(markWdth, pnt.fontMetrics().width(labVal.c_str()));
		}
	    }
	}
	if(iP < (prmsInd.size()-1)) tAr.adjust((markWdth?(markWdth+5):0), 0, 0, 0);
    }
    mrkPen.setColor(shD->sclMarkColor);	//Restore mark color

    //Calc horizontal scale
    int64_t hDiv = 1;	//Horisontal scale divisor
    int hmax_ln = tAr.width() / (int)((sclHor&FD_MARKS)?mrkWidth:15*vmin(w->xScale(true),w->yScale(true)));
    if(hmax_ln >= 2) {
	int hvLev = 0;
	int64_t hLen = tEnd - tBeg;
	if(hLen/86400000000ll >= 2)	{ hvLev = 5; hDiv = 86400000000ll; }	//Days
	else if(hLen/3600000000ll >= 2)	{ hvLev = 4; hDiv =  3600000000ll; }	//Hours
	else if(hLen/60000000 >= 2)	{ hvLev = 3; hDiv =    60000000ll; }	//Minutes
	else if(hLen/1000000 >= 2)	{ hvLev = 2; hDiv =     1000000ll; }	//Seconds
	else if(hLen/1000 >= 2)		{ hvLev = 1; hDiv =        1000ll; }	//Milliseconds
	while(hLen/hDiv > hmax_ln)	hDiv *= 10;
	while(hLen/hDiv < hmax_ln/2)	hDiv /= 2;

	if(shD->sclHorPer > 0 && (hLen/shD->sclHorPer) > 2 && (tAr.width()/(hLen/shD->sclHorPer)) > 15)	hDiv = shD->sclHorPer;

	if((hLen/hDiv) >= 5 && shD->trcPer) {
	    tPict = hDiv*(tEnd/hDiv+1);
	    tBeg = tPict-hLen;
	}

	// Draw horisontal grid and markers
	if(sclHor&FD_GRD_MARKS) {
	    time_t tm_t = 0;
	    struct tm ttm, ttm1 = ttm;
	    string lab_tm, lab_dt;

	    localtime_r(&tm_t, &ttm);
	    int64_t UTChourDt = (int64_t)ttm.tm_hour*3600000000ll;

	    //  Draw generic grid line
	    pnt.setPen(grdPen);
	    pnt.drawLine(tAr.x(), tAr.y()+tAr.height(), tAr.x()+tAr.width(), tAr.y()+tAr.height());
	    //  Draw full trend's data and time to the trend end position
	    int begMarkBrd = -5;
	    int endMarkBrd = tAr.x()+tAr.width();
	    if(sclHor&FD_MARKS) {
		pnt.setPen(mrkPen);
		tm_t = tPict/1000000;
		localtime_r(&tm_t,&ttm);
		lab_dt = TSYS::strMess("%d-%02d-%d",ttm.tm_mday,ttm.tm_mon+1,ttm.tm_year+1900);
		if(ttm.tm_sec == 0 && tPict%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		else if(tPict%1000000 == 0) lab_tm = TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
		else lab_tm = TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(tPict%1000000)/1e6);
		int markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().width(lab_tm.c_str());
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+mrkHeight,lab_tm.c_str());
		markBrd = tAr.x()+tAr.width()-pnt.fontMetrics().width(lab_dt.c_str());
		endMarkBrd = vmin(endMarkBrd,markBrd);
		pnt.drawText(markBrd,tAr.y()+tAr.height()+2*mrkHeight,lab_dt.c_str());
	    }
	    //  Draw grid and/or markers
	    bool first_m = true;
	    for(int64_t i_h = tBeg; true; ) {
		//   Draw grid
		pnt.setPen(grdPen);
		int h_pos = tAr.x()+tAr.width()*(i_h-tBeg)/(tPict-tBeg);
		if(sclHor&FD_GRD) pnt.drawLine(h_pos, tAr.y(), h_pos, tAr.y()+tAr.height());
		else pnt.drawLine(h_pos, tAr.y()+tAr.height()-3, h_pos, tAr.y()+tAr.height()+3);

		if(sclHor&FD_MARKS && !((i_h+UTChourDt)%hDiv) && i_h != tPict) {
		    tm_t = i_h/1000000;
		    localtime_r(&tm_t,&ttm);
		    int chLev = -1;
		    if(!first_m) {
			if(ttm.tm_mon > ttm1.tm_mon || ttm.tm_year > ttm1.tm_year) chLev = 5;
			else if(ttm.tm_mday > ttm1.tm_mday)	chLev = 4;
			else if(ttm.tm_hour > ttm1.tm_hour)	chLev = 3;
			else if(ttm.tm_min > ttm1.tm_min)	chLev = 2;
			else if(ttm.tm_sec > ttm1.tm_sec)	chLev = 1;
			else chLev = 0;
		    }

		    //Check for data present
		    lab_dt.clear(), lab_tm.clear();
		    //Date
		    if(hvLev == 5 || chLev >= 4)
			lab_dt = TSYS::strMess(((chLev>=5 || chLev==-1)?"%d-%02d-%d":"%d"), ttm.tm_mday, ttm.tm_mon+1, ttm.tm_year+1900);
		    //Hours and minuts
		    if((hvLev == 4 || hvLev == 3 || ttm.tm_hour || ttm.tm_min) && !ttm.tm_sec)
			lab_tm = TSYS::strMess("%d:%02d",ttm.tm_hour,ttm.tm_min);
		    //Seconds
		    else if((hvLev == 2 || ttm.tm_sec) && !(i_h%1000000))
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%02d",ttm.tm_hour,ttm.tm_min,ttm.tm_sec) : TSYS::strMess(_("%ds"),ttm.tm_sec);
		    //Milliseconds
		    else if(hvLev <= 1 || i_h%1000000)
			lab_tm = (chLev>=2 || chLev==-1) ? TSYS::strMess("%d:%02d:%g",ttm.tm_hour,ttm.tm_min,(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
				 (chLev>=1) ? TSYS::strMess(_("%gs"),(float)ttm.tm_sec+(float)(i_h%1000000)/1e6) :
					      TSYS::strMess(_("%gms"),(double)(i_h%1000000)/1000.);
		    int wdth, tpos, endPosTm = 0, endPosDt = 0;
		    pnt.setPen(mrkPen);
		    if(lab_tm.size()) {
			wdth = pnt.fontMetrics().width(lab_tm.c_str());
			tpos = vmax(h_pos-wdth/2,0);
			if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3)) {
			    pnt.drawText(tpos, tAr.y()+tAr.height()+mrkHeight, lab_tm.c_str());
			    endPosTm = tpos+wdth;
			}
		    }
		    if(lab_dt.size()) {
			wdth = pnt.fontMetrics().width(lab_dt.c_str());
			tpos = vmax(h_pos-wdth/2,0);
			if((tpos+wdth) < (endMarkBrd-3) && tpos > (begMarkBrd+3)) {
			    pnt.drawText(tpos, tAr.y()+tAr.height()+2*mrkHeight, lab_dt.c_str());
			    endPosDt = tpos+wdth;
			}
		    }
		    begMarkBrd = vmax(begMarkBrd, vmax(endPosTm,endPosDt));
		    memcpy((char*)&ttm1, (char*)&ttm, sizeof(tm));
		    first_m = false;
		}
		//   Next
		if(i_h >= tPict) break;
		i_h = ((i_h+UTChourDt)/hDiv)*hDiv + hDiv - UTChourDt;
		if(i_h > tPict)	i_h = tPict;
	    }
	}
    }

    //Draw trends
    for(unsigned iT = 0; iT < shD->prms.size(); iT++) {
	TrendObj &cP = shD->prms[iT];

	// Set trend's pen
	QPen trpen(cP.color());
	trpen.setStyle(Qt::SolidLine);
	trpen.setWidth(vmax(1,vmin(10,(int)rRnd(cP.width()*vmin(w->xScale(true),w->yScale(true))))));
#if QT_VERSION < 0x050000
	if(cP.valTp() != 0 && trpen.width() > 1) trpen.setCapStyle(Qt::RoundCap);
#else
	trpen.setCapStyle(Qt::FlatCap);
#endif
	pnt.setRenderHint(QPainter::Antialiasing, (trpen.width()>=2));
	pnt.setPen(trpen);

	// Prepare generic parameters
	aVbeg = vmax(tBeg, cP.valBeg());
	aVend = vmin(tEnd, cP.valEnd());

	if(aVbeg >= aVend || !cP.color().isValid()) continue;
	int aPosBeg = cP.val(aVbeg);
	if(aPosBeg && cP.val()[aPosBeg].tm > aVbeg) aPosBeg--;
	bool vsPercT = cP.isIndiv ? false : vsPerc;
	bool isLogT = cP.isIndiv ? (cP.wScale&FD_LOG) : isLog;
	double vsMaxT = cP.isIndiv ? cP.adjU : vsMax;
	double vsMinT = cP.isIndiv ? cP.adjL : vsMin;

	// Prepare border for percent trend, ONLY!
	float bordL = cP.bordL();
	float bordU = cP.bordU();
	if(vsPercT && bordL >= bordU) {
	    bordU = -3e300, bordL = 3e300;
	    bool end_vl = false;
	    int ipos = aPosBeg;
	    while(true) {
		if(ipos >= (int)cP.val().size() || end_vl)	break;
		if(cP.val()[ipos].tm >= aVend) end_vl = true;
		if(cP.val()[ipos].val != EVAL_REAL) {
		    bordL = vmin(bordL, cP.val()[ipos].val);
		    bordU = vmax(bordU, cP.val()[ipos].val);
		}
		ipos++;
	    }
	    float vMarg = (bordU-bordL)/10;
	    bordL -= vMarg;
	    bordU += vMarg;
	}

	// Draw trend
	bool end_vl = false;
	double curVl = EVAL_REAL, averVl = EVAL_REAL, prevVl = EVAL_REAL;
	int    curPos, averPos = 0, prevPos = 0, c_vpos, z_vpos = 0;
	int64_t curTm = 0, averTm = 0, averLstTm = 0;
	for(int a_pos = aPosBeg; true; a_pos++) {
	    if(a_pos < (int)cP.val().size() && !end_vl) {
		curTm = vmin(aVend,vmax(aVbeg,cP.val()[a_pos].tm));
		curVl = cP.val()[a_pos].val;
		if(vsPercT && curVl != EVAL_REAL) curVl = 100*(curVl-bordL)/(bordU-bordL);
		if(isnan(curVl)) curVl = EVAL_REAL;
		curPos = tAr.x()+tAr.width()*(curTm-tBeg)/(tPict-tBeg);
	    } else curPos = 0;
	    if(!curPos || cP.val()[a_pos].tm >= aVend) end_vl = true;

	    //Square Average
	    if(averPos == curPos) {
		if(!(2*curTm-averTm-averLstTm)) continue;
		if(averVl == EVAL_REAL) averVl = curVl;
		else if(curVl != EVAL_REAL)
		    averVl = (averVl*(double)(curTm-averTm)+curVl*(double)(curTm-averLstTm))/((double)(2*curTm-averTm-averLstTm));
		averLstTm = curTm;
		continue;
	    }

	    //Write point and line
	    if(averVl != EVAL_REAL) {
		if(cP.valTp() == 0)
		    z_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,((vsPercT?(100.*(0-bordL)/(bordU-bordL)):0)-vsMinT)/(vsMaxT-vsMinT))));
		c_vpos = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,averVl)):averVl)-vsMinT)/(vsMaxT-vsMinT))));
		if(prevVl == EVAL_REAL) {
		    if(cP.valTp() != 0) pnt.drawPoint(averPos,c_vpos);
		    else pnt.drawLine(averPos,z_vpos,averPos,vmin(z_vpos-trpen.width(),c_vpos));
		}
		else {
		    int c_vpos_prv = tAr.y()+tAr.height()-(int)((double)tAr.height()*vmax(0,vmin(1,((isLogT?log10(vmax(1e-100,prevVl)):prevVl)-vsMinT)/(vsMaxT-vsMinT))));
		    if(cP.valTp() != 0) pnt.drawLine(prevPos,c_vpos_prv,averPos,c_vpos);
		    else
			for(int sps = prevPos+1; sps <= averPos; sps++)
			    pnt.drawLine(sps,z_vpos,sps,vmin(z_vpos-trpen.width(),c_vpos));
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

    shD->pictRect = tAr;
    shD->tPict = tPict;

    if(mess_lev() == TMess::Debug) mess_debug(mod->nodePath().c_str(), _("Trend build: %f ms."), 1e-3*(TSYS::curTime()-d_cnt));
}

void ShapeDiagram::tracing( )
{
    WdgView *w = (WdgView*)((QTimer*)sender())->parent();
    ShpDt *shD = (ShpDt*)w->shpData;

    if(!w->isEnabled()) return;

    int64_t trcPer = shD->trcPer*1e6;
    if(shD->tTimeCurent)shD->tTime = (int64_t)time(NULL)*1000000;
    else if(shD->tTime)	shD->tTime += trcPer;
    loadData(w);
    makePicture(w);

    //Trace cursors value
    if(shD->tTimeCurent) shD->tTime = shD->arhEnd(shD->tTime);	//Force for cursor limit to real data
    if(shD->type == 0 && shD->active && (shD->holdCur || shD->curTime <= (shD->tPict-(int64_t)(1e6*shD->tSize))))
	setCursor(w, shD->tTime);
    w->update();
}

bool ShapeDiagram::event( WdgView *w, QEvent *event )
{
    int64_t d_cnt = 0;
    ShpDt *shD = (ShpDt*)w->shpData;

    if(!shD->en) return false;

    //Process event
    switch(event->type()) {
	case QEvent::Paint: {
	    if(mess_lev() == TMess::Debug) d_cnt = TSYS::curTime();
	    QPainter pnt(w);

	    // Draw decoration
	    QRect dA = w->rect().adjusted(0, 0, -2*shD->geomMargin, -2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));

	    if(shD->backGrnd.color().isValid()) pnt.fillRect(dA,shD->backGrnd.color());
	    if(!shD->backGrnd.textureImage().isNull()) pnt.fillRect(dA,shD->backGrnd.textureImage());

	    // Draw border
	    borderDraw(pnt, dA, shD->border, shD->bordStyle);

	    // Trend's picture
	    pnt.drawImage(shD->border.width(), shD->border.width(), shD->pictObj);

	    // Draw focused border
	    if(w->hasFocus()) qDrawShadeRect(&pnt,dA.x(),dA.y(),dA.width(),dA.height(),w->palette());

	    // Draw cursor
	    int curPos = -1;
	    if(shD->type == FD_TRND && shD->active) {
		int64_t tTimeGrnd = shD->tPict - (int64_t)(1e6*shD->tSize);
		int64_t curTime = vmax(vmin(shD->curTime,shD->tPict), tTimeGrnd);
		if(curTime && tTimeGrnd && shD->tPict && (curTime >= tTimeGrnd || curTime <= shD->tPict))
		    curPos = shD->pictRect.x() + shD->pictRect.width()*(curTime-tTimeGrnd)/(shD->tPict-tTimeGrnd);
	    }
	    else if(shD->type == FD_SPECTR && shD->active) {
		float curFrq = vmax(vmin(1e6/(float)shD->curTime,shD->fftEnd),shD->fftBeg);
		curPos = shD->pictRect.x() + (int)(shD->pictRect.width()*(curFrq-shD->fftBeg)/(shD->fftEnd-shD->fftBeg));
	    }
	    if(curPos >= 0 && (curPos-shD->pictRect.x()) <= shD->pictRect.width()) {
		QPen curpen(shD->curColor);
		curpen.setWidth(1);
		pnt.setPen(curpen);
		pnt.drawLine(curPos, shD->pictRect.y(), curPos,shD->pictRect.y()+shD->pictRect.height());
	    }

	    // Draw selection window
	    if(shD->sclWin && shD->fMoveHold && shD->type == FD_TRND && shD->active) {
		pnt.setPen("white");
		pnt.drawRect(QRect(shD->startPnt,shD->holdPnt));
		QPen pen(QColor("black")); pen.setStyle(Qt::DashLine); pnt.setPen(pen);
		pnt.drawRect(QRect(shD->startPnt,shD->holdPnt));
	    }

	    if(mess_lev() == TMess::Debug) mess_debug(mod->nodePath().c_str(), _("Trend draw: %f ms."), 1e-3*(TSYS::curTime()-d_cnt));

	    return true;
	}
	case QEvent::KeyPress: {
	    QKeyEvent *key = static_cast<QKeyEvent*>(event);

	    switch(key->key()) {
		case Qt::Key_Left: case Qt::Key_Right:
		    if(!shD->active) break;
		    if(shD->type == 0) {
			int64_t tTimeGrnd = shD->tPict - (int64_t)(1e6*shD->tSize);
			int64_t curTime = vmax(vmin(shD->curTime,shD->tPict),tTimeGrnd);
			setCursor(w, curTime+((key->key()==Qt::Key_Left)?-1:1)*(shD->tTime-tTimeGrnd)/shD->pictRect.width());
		    }
		    else if(shD->type == 1) {
			float curFrq = vmax(vmin(1e6/(float)shD->curTime,shD->fftEnd),shD->fftBeg);
			setCursor(w, (int64_t)(1e6/(curFrq+((key->key()==Qt::Key_Left)?-1:1)*(shD->fftEnd-shD->fftBeg)/shD->pictRect.width())));
		    }
		    w->update();
		    return true;
	    }
	    break;
	}
	case QEvent::MouseButtonPress: {
	    if(!shD->active || !w->hasFocus()) break;
	    QPoint curp = w->mapFromGlobal(w->cursor().pos());
	    if(curp.x() < shD->pictRect.x() || curp.x() > (shD->pictRect.x()+shD->pictRect.width())) break;

	    //Hold processing
	    if((static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton && shD->type == FD_TRND)
		shD->holdPnt = shD->startPnt = curp;

	    break;
	}
	case QEvent::MouseButtonRelease:
	    if(shD->fMoveHold) {
		shD->fMoveHold = false;
		w->setCursor(Qt::ArrowCursor);
		AttrValS attrs;
		if(shD->sclWin) {
		    //Calcullate the time scale
		    int64_t tTimeGrnd = shD->tPict - (int64_t)(1e6*shD->tSize);
		    int64_t tP1 = tTimeGrnd + (shD->tPict-tTimeGrnd)*(shD->startPnt.x()-shD->pictRect.x())/shD->pictRect.width();
		    int64_t tP2 = tTimeGrnd + (shD->tPict-tTimeGrnd)*(shD->holdPnt.x()-shD->pictRect.x())/shD->pictRect.width();
		    tTimeGrnd = vmin(tP1, tP2); tP1 = vmax(tP1, tP2);
		    attrs.push_back(std::make_pair("tSek",i2s(tP1/1000000)));
		    attrs.push_back(std::make_pair("tUSek",i2s(tP1%1000000)));
		    attrs.push_back(std::make_pair("tSize",r2s(1e-6*(tP1-tTimeGrnd))));
		    attrs.push_back(std::make_pair("trcPer","0"));

		    //Calculate the vertical scale
		    float vScl = fabs(shD->startPnt.y()-shD->holdPnt.y())/shD->pictRect.height();
		    float vOff = ((float)vmin(shD->startPnt.y(),shD->holdPnt.y())/shD->pictRect.height() + vScl/2 - 0.5)*shD->sclVerScl;
		    attrs.push_back(std::make_pair("sclVerScl",r2s(shD->sclVerScl*vScl)));
		    attrs.push_back(std::make_pair("sclVerSclOff",r2s(shD->sclVerSclOff-vOff)));
		    w->attrsSet(attrs);
		}
		else if(shD->fMoveTmCh) {
		    attrs.push_back(std::make_pair("tSek",i2s(shD->tTime/1000000)));
		    attrs.push_back(std::make_pair("tUSek",i2s(shD->tTime%1000000)));
		    attrs.push_back(std::make_pair("trcPer","0"));
		    w->attrsSet(attrs);
		}
	    }
	    else {
		//Cursor set
		QPoint curp = w->mapFromGlobal(w->cursor().pos());
		if(curp.x() < shD->pictRect.x() || curp.x() > (shD->pictRect.x()+shD->pictRect.width())) break;
		switch(shD->type) {
		    case FD_TRND: {
			int64_t tTimeGrnd = shD->tPict - (int64_t)(1e6*shD->tSize);
			setCursor(w, tTimeGrnd + (shD->tPict-tTimeGrnd)*(curp.x()-shD->pictRect.x())/shD->pictRect.width());
			break;
		    }
		    case FD_SPECTR:
			setCursor(w, (int64_t)(1e6/(shD->fftBeg+(shD->fftEnd-shD->fftBeg)*(curp.x()-shD->pictRect.x())/shD->pictRect.width())));
			break;
		    default: break;
		}
		w->update();
	    }
	    break;
	case QEvent::MouseMove: {
	    if(shD->type != FD_TRND) break;
	    QMouseEvent *mev = (QMouseEvent*)event;
	    QPoint dP = mev->pos() - shD->holdPnt;
	    if(shD->fMoveHold || ((mev->buttons()&Qt::LeftButton) && dP.manhattanLength() >= QApplication::startDragDistance())) {
		if(!shD->fMoveHold) { w->setCursor(shD->sclWin?Qt::CrossCursor:Qt::ClosedHandCursor); shD->fMoveTmCh = false; }
		shD->fMoveHold = true;
		shD->holdPnt = mev->pos();
		if(shD->sclWin) w->update();
		else if(dP.x() > 0 || (dP.x() < 0 && !shD->tTimeCurent)) {
		    if(shD->trcPer) w->attrSet("trcPer", "0", A_DiagramTrcPer);
		    shD->tTimeCurent = false;

		    int64_t toTm = vmax(0, vmin((int64_t)time(NULL)*1000000,shD->tTime-((int64_t)(1e6*shD->tSize)/shD->pictRect.width())*dP.x()));
		    if(toTm != shD->tTime) {
			shD->tTime = toTm; shD->fMoveTmCh = true;
			//Load by parameters after moving drop.
			makePicture(w);
			w->update();
		    }
		}
	    }
	    if(shD->fMoveHold && !(((QMouseEvent*)event)->buttons()&Qt::LeftButton)) { shD->fMoveHold = false; w->setCursor(Qt::ArrowCursor); }
	}
	default: break;
    }

    return false;
}

void ShapeDiagram::setCursor( WdgView *w, int64_t itm )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    if(shD->type == FD_TRND) {
	int64_t tTimeGrnd = shD->tTime - (int64_t)(1e6*shD->tSize);
	int64_t curTime   = vmax(vmin(itm,shD->tTime), tTimeGrnd);

	shD->curTime = curTime;
	shD->holdCur = (curTime==shD->tTime);

	XMLNode req("set");
	req.setAttr("path",w->id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(i2s(curTime/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(i2s(curTime%1000000));

	//Update trend's current values
	for(unsigned iP = 0; iP < shD->prms.size(); iP++) {
	    int vpos = shD->prms[iP].val(curTime);
	    double val = EVAL_REAL;
	    if(!(!shD->prms[iP].val().size() || curTime < shD->prms[iP].valBeg( ) ||
		(!shD->holdCur && vpos >= (int)shD->prms[iP].val().size())))
	    {
		vpos = vmax(0,vmin((int)shD->prms[iP].val().size()-1,vpos));
		if(vpos && shD->prms[iP].val()[vpos].tm > curTime) vpos--;
		val = shD->prms[iP].val()[vpos].val;
	    }
	    if(val != shD->prms[iP].curVal()) {
		req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",iP))->setText(r2s(val,6));
		shD->prms[iP].setCurVal(val);
	    }
	}
	w->cntrIfCmd(req);
    }
    else if(shD->type == FD_SPECTR) {
	float curFrq = vmax(vmin(1e6/(float)itm,shD->fftEnd),shD->fftBeg);
	shD->curTime = 1e6/curFrq;

	XMLNode req("set");
	req.setAttr("path",w->id()+"/%2fserv%2fattr");
	req.childAdd("el")->setAttr("id","curSek")->setText(i2s(shD->curTime/1000000));
	req.childAdd("el")->setAttr("id","curUSek")->setText(i2s(shD->curTime%1000000));

#if HAVE_FFTW3_H
	//Update trend's current values
	for(unsigned iP = 0; iP < shD->prms.size(); iP++) {
	    if(!shD->prms[iP].fftN) continue;
	    float fftDt = (1/shD->tSize)*(float)w->size().width()/shD->prms[iP].fftN;
	    int vpos = (int)(curFrq/fftDt);
	    double val = EVAL_REAL;
	    if(vpos >= 1 && vpos < (shD->prms[iP].fftN/2+1))
		val = shD->prms[iP].fftOut[0][0]/shD->prms[iP].fftN +
		    pow(pow(shD->prms[iP].fftOut[vpos][0],2)+pow(shD->prms[iP].fftOut[vpos][1],2),0.5)/(shD->prms[iP].fftN/2+1);
	    req.childAdd("el")->setAttr("id",TSYS::strMess("prm%dval",iP))->setText(r2s(val,6));
	    shD->prms[iP].setCurVal(val);
	}
#endif
	w->cntrIfCmd(req);
    }
}

//* Trend object's class                         *
//************************************************
ShapeDiagram::TrendObj::TrendObj( WdgView *iview ) : adjL(3e300), adjU(-3e300), isIndiv(false), wScale(0),
#if HAVE_FFTW3_H
    fftN(0), fftOut(NULL),
#endif
    mBordLow(0), mBordUp(0), mCurvl(EVAL_REAL), mWidth(1), mScale(0),
    arh_per(0), arh_beg(0), arh_end(0), val_tp(0), view(iview)
{
    loadData();
}

ShapeDiagram::TrendObj::~TrendObj( )
{
#if HAVE_FFTW3_H
    if(fftOut) { delete fftOut; fftN = 0; }
#endif
}

int64_t ShapeDiagram::TrendObj::valBeg( )	{ return vals.empty() ? 0 : vals.front().tm; }

int64_t ShapeDiagram::TrendObj::valEnd( )	{ return vals.empty() ? 0 : vals.back().tm; }

int ShapeDiagram::TrendObj::val( int64_t tm )
{
    unsigned iP = 0;
    for(unsigned d_win = vals.size()/2; d_win > 10; d_win/=2)
	if(tm > vals[iP+d_win].tm) iP += d_win;
    for( ; iP < vals.size(); iP++)
	if(vals[iP].tm >= tm) return iP;
    return vals.size();
}

void ShapeDiagram::TrendObj::setAddr( const string &vl )
{
    if(vl == mAddr) return;
    mAddr = vl;
    loadData(true);
}

void ShapeDiagram::TrendObj::loadData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;
    switch(shD->type) {
	case FD_TRND:
	case FD_XY:	loadTrendsData(full);	break;
	case FD_SPECTR:	loadSpectrumData(full);	break;
    }
}

void ShapeDiagram::TrendObj::loadTrendsData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;

    int64_t tSize	= (int64_t)(1e6*shD->tSize);
    int64_t tTime	= shD->tTime;
    int64_t tTimeGrnd	= tTime - tSize;
    int64_t wantPer	= tSize/(view->size().width()*shD->valsForPix);
    unsigned bufLim	= 2*view->size().width()*shD->valsForPix;
    XMLNode req("get");

    //Clear trend for empty address and for full reload data
    if(full || addr().empty()) {
	arh_per = arh_beg = arh_end = 0;
	val_tp = TFld::Boolean;
	vals.clear();
	if(addr().empty()) return;
    }

    bool isDataDir = (addr().compare(0,5,"data:") == 0 || addr().compare(0,5,"line:") == 0);

    if(!isDataDir) {	//From archive by address
	// Get archive parameters
	if(!arh_per || tTime > arh_end || shD->tTimeCurent) {
	    XMLNode req("info");
	    req.setAttr("arch",shD->valArch)->setAttr("path",addr()+"/%2fserv%2fval");
	    if(view->cntrIfCmd(req,true) || (val_tp=s2i(req.attr("vtp"))) == TFld::String || val_tp == TFld::Object)
	    { arh_per = arh_beg = arh_end = 0; return; }
	    else {
		val_tp  = s2i(req.attr("vtp"));
		arh_beg = s2ll(req.attr("beg"));
		arh_end = s2ll(req.attr("end"));
		arh_per = s2ll(req.attr("per"));

		//  Correct to real data
		if(shD->tTimeCurent) tTimeGrnd = (tTime=shD->arhEnd(tTime)) - tSize;
	    }
	}

	// One request check and prepare
	int trcPer = shD->trcPer*1e6;
	if(shD->tTimeCurent && trcPer && shD->valArch.empty() &&
	    (!arh_per || (vmax(arh_per,wantPer) >= trcPer && (tTime-valEnd())/vmax(arh_per,vmax(wantPer,trcPer)) < 2)))
	{
	    XMLNode req("get");
	    req.setAttr("path", addr()+"/%2fserv%2fval")->
		setAttr("tm", ll2s(tTime))->
		setAttr("tm_grnd", "0");
	    if(view->cntrIfCmd(req,true))	return;

	    int64_t lstTm = (s2ll(req.attr("tm"))/wantPer)*wantPer,
		    valEnd_ = (valEnd()/wantPer)*wantPer;
	    if(lstTm && lstTm >= valEnd_) {
		double curVal = (req.text() == EVAL_STR) ? EVAL_REAL : s2r(req.text());
		if((val_tp == TFld::Boolean && curVal == EVAL_BOOL) || (val_tp == TFld::Integer && curVal == EVAL_INT) || isinf(curVal))
		    curVal = EVAL_REAL;
		if(valEnd_ && (lstTm-valEnd_)/vmax(wantPer,trcPer) > 2) vals.push_back(SHg(lstTm-trcPer,EVAL_REAL));
		else if((lstTm-valEnd_) >= wantPer) vals.push_back(SHg(lstTm,curVal));
		else if((lstTm == valEnd_ && curVal != EVAL_REAL) || vals[vals.size()-1].val == EVAL_REAL) vals[vals.size()-1].val = curVal;
		else if(curVal != EVAL_REAL) {
		    int s_k = lstTm-wantPer*(lstTm/wantPer), n_k = trcPer;
		    vals[vals.size()-1].val = (vals[vals.size()-1].val*s_k+curVal*n_k)/(s_k+n_k);
		}
		while(vals.size() > bufLim)	vals.pop_front();
	    }
	    return;
	}
    }
    else	//Data direct into address field by searilised XML string or horizontal line
	try {
	    if(addr().compare(0,5,"data:") == 0) {
		req.load(addr().substr(5));
		bool inSec = s2i(req.attr("s"));
		arh_beg = s2ll(req.attr("tm_grnd")) * (inSec?1000000ll:1ll);
		if(!arh_beg) arh_beg = tTimeGrnd;
		req.setAttr("tm_grnd", ll2s(arh_beg));
		arh_end = s2ll(req.attr("tm")) * (inSec?1000000ll:1ll);
		if(!arh_end) arh_end = tTime;
		req.setAttr("tm", ll2s(arh_end));
		arh_per = s2ll(req.attr("per")) * (inSec?1000000ll:1ll);
		req.setAttr("per", ll2s(arh_per));
	    }
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
	} catch(TError&) { arh_per = arh_beg = arh_end = 0; return; }

    if(!arh_per) return;

    //Correct request to archive border
    wantPer   = (vmax(wantPer,arh_per)/arh_per)*arh_per;
    tTime     = vmin(tTime, arh_end);
    tTimeGrnd = vmax(tTimeGrnd, arh_beg);	//For prevent possible cycling

    //Clear data at time error
    if(tTime <= tTimeGrnd || tTimeGrnd/wantPer > valEnd()/wantPer || tTime/wantPer < valBeg()/wantPer) vals.clear();
    if(tTime <= tTimeGrnd) return;

    //Check for request to present in buffer data
    if(tTime/wantPer <= valEnd()/wantPer && tTimeGrnd/wantPer >= valBeg()/wantPer) return;

    //Correcting request to present data
    if(valEnd() && tTime > valEnd())		tTimeGrnd = valEnd()+wantPer;//1;
    else if(valBeg() && tTimeGrnd < valBeg())	tTime = valBeg()-wantPer;//1;

    //Get values data
    int64_t	bbeg, bend, bper, bbeg_prev = tTime;
    int		curPos, prevPos, maxPos;
    double	curVal = EVAL_REAL, prevVal;
    string	svl, curValS;
    vector<SHg>	buf;
    bool	toEnd = (tTimeGrnd >= valEnd());
    int		endBlks = 0;

    m1:
    if(!isDataDir) {
	req.clear()->
	    setAttr("arch", shD->valArch)->
	    setAttr("path", addr()+"/%2fserv%2fval")->
	    setAttr("tm", ll2s(tTime))->
	    setAttr("tm_grnd", ll2s(tTimeGrnd))->
	    setAttr("per", ll2s(wantPer))->
	    setAttr("mode", "1")->
	    setAttr("real_prec", "6")->
	    setAttr("round_perc", "0");//r2s(100/(float)view->size().height()));

	if(view->cntrIfCmd(req,true)) return;
    }

    //Get data buffer parameters
    bbeg = s2ll(req.attr("tm_grnd"));
    bend = s2ll(req.attr("tm"));
    bper = s2ll(req.attr("per"));
    bool toAprox = s2i(req.attr("aprox"));

    if(bbeg <= 0 || bend <= 0 || bper <= 0 || bbeg > bend || req.text().empty()) return;

    prevPos = 0, prevVal = EVAL_REAL, maxPos = (bend-bbeg)/bper;
    buf.clear();
    for(int v_off = 0, var_off = 0; true; ) {
	if((svl=TSYS::strLine(req.text(),0,&v_off)).size()) {
	    var_off = 0;
	    curPos = s2i(TSYS::strParse(svl,0," ",&var_off,true));
	    curVal = s2r((curValS=TSYS::strParse(svl,0," ",&var_off,true)));
	    if(curValS == EVAL_STR || (val_tp == TFld::Boolean && curVal == EVAL_BOOL) ||
				      (val_tp == TFld::Integer && curVal == EVAL_INT) || isinf(curVal))
		curVal = EVAL_REAL;
	}
	else curPos = maxPos+1;
	if(curPos < 0 || curPos > (maxPos+1)) break;	//Out of range exit
	for(int stPos = prevPos; prevPos < curPos; prevPos++)
	    if(toAprox && prevVal != EVAL_REAL && curVal != EVAL_REAL)
		buf.push_back(SHg(bbeg+prevPos*bper,prevVal+(curVal-prevVal)*(prevPos-stPos)/(curPos-stPos)));
	    else buf.push_back(SHg(bbeg+prevPos*bper,prevVal));
	if(prevPos > maxPos) break;	//Normal exit
	prevVal = curVal;
    }

    //Append buffer to values deque
    if(isDataDir) vals.assign(buf.begin(), buf.end());
    else if(toEnd) {
	vals.insert(vals.end()-endBlks, buf.begin(), buf.end());
	while(vals.size() > bufLim) vals.pop_front();
	endBlks += buf.size();
    }
    else {
	vals.insert(vals.begin(), buf.begin(), buf.end());
	while(vals.size() > bufLim) vals.pop_back();
    }

    //Check for archive jump
    if(!isDataDir && shD->valArch.empty() && (bbeg-tTimeGrnd)/bper && bper < bbeg_prev) {
	bbeg_prev = bper;
	tTime = bbeg-bper;
	goto m1;
    }
}

void ShapeDiagram::TrendObj::loadSpectrumData( bool full )
{
    ShpDt *shD = (ShpDt*)view->shpData;

    loadTrendsData(true);

    if(!valBeg() || !valEnd()) return;

#if HAVE_FFTW3_H
    if(fftOut) { delete fftOut; fftN = 0; }

    int64_t tSize	= (int64_t)(1e6*shD->tSize);
    int64_t tTime	= shD->tTime;
    int64_t tTimeGrnd	= tTime - tSize;
    if(shD->tTimeCurent) tTimeGrnd = (tTime=shD->arhEnd(tTime)) - tSize;
    int64_t workPer	= tSize/view->size().width();

    tTimeGrnd = vmax(tTimeGrnd, valBeg());
    tTime = vmin(tTime, valEnd());

    if((fftN=vmax(0,(tTime-tTimeGrnd)/workPer)) == 0) return;
    double fftIn[fftN];
    fftOut = (fftw_complex*)malloc(sizeof(fftw_complex)*(fftN/2+1));

    int fftFirstPos = -1, fftLstPos = -1;
    for(unsigned a_pos = val(tTimeGrnd); a_pos < val().size() && val()[a_pos].tm <= tTime; a_pos++) {
	int fftPos = (val()[a_pos].tm-tTimeGrnd)/workPer;
	if(fftPos >= fftN) break;
	if(val()[a_pos].val == EVAL_REAL) continue;
	if(fftFirstPos < 0) fftFirstPos = fftPos;

	if(fftLstPos == fftPos) fftIn[fftPos-fftFirstPos] = (fftIn[fftPos-fftFirstPos]+val()[a_pos].val)/2;
	else fftIn[fftPos-fftFirstPos] = val()[a_pos].val;

	for( ; fftLstPos >= 0 && (fftLstPos+1) < fftPos; fftLstPos++ )
	    fftIn[fftLstPos-fftFirstPos+1] = fftIn[fftLstPos-fftFirstPos];
	fftLstPos = fftPos;
    }

    fftN = fftLstPos-fftFirstPos;
    if(fftN < 20) { delete fftOut; fftOut = NULL; fftN = 0; return; }

    fftw_plan p = fftw_plan_dft_r2c_1d( fftN, fftIn, fftOut, FFTW_ESTIMATE );
    fftw_execute(p);
    fftw_destroy_plan(p);
#endif
}

int64_t ShapeDiagram::ShpDt::arhEnd( int64_t def )
{
    int64_t rez = 0, rez1 = 0;
    for(vector<TrendObj>::iterator iP = prms.begin(); iP != prms.end(); ++iP) {
	rez = vmax(rez, iP->arhEnd());
	rez1 = vmax(rez1, iP->arhEnd()+iP->arhPer());	//!!!! For allow curent values update on periodicity by minutes and hours.
    }

    return (rez && rez1 < def) ? rez : def;
}

//************************************************
//* Protocol view shape widget                   *
//************************************************
ShapeProtocol::ShapeProtocol( ) : WdgShape("Protocol")	{ }

void ShapeProtocol::init( WdgView *w )
{
    w->shpData = new ShpDt();

    ShpDt *shD = (ShpDt*)w->shpData;

    //Init main widget
    QVBoxLayout *lay = new QVBoxLayout(w);
    shD->addrWdg = new QTableWidget(w);
    shD->addrWdg->setSelectionBehavior(QAbstractItemView::SelectRows);
    //shD->addrWdg->setSortingEnabled(true);
    eventFilterSet(w, shD->addrWdg, true);
    w->setFocusProxy(shD->addrWdg);
    if(qobject_cast<DevelWdgView*>(w)) setFocus(w, shD->addrWdg, false, true);
    lay->addWidget(shD->addrWdg);
    //Init tracing timer
    shD->trcTimer = new QTimer(w);
    connect(shD->trcTimer, SIGNAL(timeout()), this, SLOT(tracing()));

    //Bckground palette init
    QPalette plt(shD->addrWdg->palette());
    plt.setBrush(QPalette::Background, QPalette().brush(QPalette::Background));
    shD->addrWdg->setPalette(plt);
}

void ShapeProtocol::destroy( WdgView *w )
{
    ((ShpDt*)w->shpData)->trcTimer->stop();

    delete (ShpDt*)w->shpData;
}

bool ShapeProtocol::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    int	reld_dt = 0;	//Reload data ( 1-reload addons, 2-full reload )

    ShpDt *shD = (ShpDt*)w->shpData;

    switch(uiPrmPos) {
	case A_COM_LOAD: reld_dt = 2;	break;
	case A_EN:
	    if(!qobject_cast<RunWdgView*>(w))	break;
	    w->setVisible((bool)s2i(val) && ((RunWdgView*)w)->permView());
	    break;
	case A_ACTIVE:
	    if(!qobject_cast<RunWdgView*>(w)) break;
	    shD->active = (bool)s2i(val) && ((RunWdgView*)w)->permCntr();
	    setFocus(w, shD->addrWdg, shD->active);
	    shD->addrWdg->setSelectionMode(shD->active ? QAbstractItemView::SingleSelection : QAbstractItemView::NoSelection);
//	    shD->addrWdg->setEnabled( shD->active && ((RunWdgView*)w)->permCntr() );
	    break;
	case A_GEOM_MARGIN: w->layout()->setMargin(s2i(val));	break;
	case A_BackColor: {
	    QPalette plt(shD->addrWdg->palette());
	    QBrush brsh = plt.brush(QPalette::Base);
	    brsh.setColor(getColor(val));
	    if(!brsh.color().isValid()) brsh.setColor(QPalette().brush(QPalette::Base).color());
	    brsh.setStyle(brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern);
	    plt.setBrush(QPalette::Base, brsh);
	    shD->addrWdg->setPalette(plt);
	    break;
	}
	case A_BackImg: {
	    QImage img;
	    QPalette plt(shD->addrWdg->palette());
	    QBrush brsh = plt.brush(QPalette::Base);
	    string backimg = w->resGet(val);
	    if(!backimg.empty()) img.loadFromData((const uchar*)backimg.data(),backimg.size());
	    brsh.setTextureImage(img);
	    brsh.setStyle(!brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern);
	    plt.setBrush(QPalette::Base, brsh);
	    shD->addrWdg->setPalette(plt);
	    break;
	}
	case A_ProtFont: shD->addrWdg->setFont(getFont(val,vmin(w->xScale(true),w->yScale(true))));	break;
	case A_ProtHeadVis:
	    shD->addrWdg->horizontalHeader()->setVisible(s2i(val));
	    shD->addrWdg->verticalHeader()->setVisible(s2i(val));
	    break;
	case A_ProtTime: {
	    unsigned int tm = strtoul(val.c_str(), 0, 10);
	    //if(shD->time == tm) break;
	    shD->tTimeCurent = false;
	    if(tm == 0) {
		shD->time = (unsigned int)time(NULL);
		shD->tTimeCurent = true;
	    }else shD->time = tm;
	    reld_dt = 1;
	    break;
	}
	case A_ProtTSize:
	    if(shD->tSize == strtoul(val.c_str(),0,10)) break;
	    shD->tSize = (unsigned int)strtoul(val.c_str(), 0, 10);
	    reld_dt = 1;
	    break;
	case A_ProtTrcPer: {
	    int trcPer = vmax(0, s2i(val));
	    if(shD->trcPer == trcPer) break;
	    shD->trcPer = trcPer;
	    if(trcPer)	shD->trcTimer->start(trcPer*1000);
	    else	shD->trcTimer->stop();
	    break;
	}
	case A_ProtArch: if(shD->arch != val) { shD->arch = val; reld_dt = 2; } break;
	case A_ProtTmpl: if(shD->tmpl != val) { shD->tmpl = val; reld_dt = 2; } break;
	case A_ProtLev:  if(shD->lev != s2i(val)) { shD->lev = s2i(val); reld_dt = 2; } break;
	case A_ProtViewOrd: if(shD->viewOrd != s2i(val)) { shD->viewOrd = s2i(val); reld_dt = 1; } break;
	case A_ProtCol: if(shD->col != val) { shD->col = val; reld_dt = 1; } break;
	case A_ProtItProp: {
	    int itNum = s2i(val);
	    if((int)shD->itProps.size() == itNum) break;
	    while((int)shD->itProps.size() < itNum) shD->itProps.push_back(ShpDt::ItProp());
	    while((int)shD->itProps.size() > itNum) shD->itProps.pop_back();
	    reld_dt = 1;
	    break;
	}
	default:
	    //Item's properties configuration
	    if(uiPrmPos >= A_ProtProps) {
		int itNum = (uiPrmPos-A_ProtProps)/A_ProtPropsSz;
		if(itNum >= (int)shD->itProps.size()) break;
		reld_dt = 1;
		switch(uiPrmPos%A_ProtPropsSz) {
		    case A_ProtPropLev: shD->itProps[itNum].lev = s2i(val);	break;
		    case A_ProtPropTmpl: shD->itProps[itNum].tmpl = val;	break;
		    case A_ProtPropFnt:
			shD->itProps[itNum].font = val.empty() ? shD->addrWdg->font() : getFont(val,vmin(w->xScale(true),w->yScale(true)));
			break;
		    case A_ProtPropClr: shD->itProps[itNum].clr = getColor(val);	break;
		}
	    }
    }

    if(reld_dt && !w->allAttrLoad())	loadData(w, (reld_dt==2));

    return true;
}

void ShapeProtocol::loadData( WdgView *w, bool full )
{
    ShpDt *shD = (ShpDt*)w->shpData;

    unsigned int tTime		= shD->time,
		tTimeGrnd	= tTime - shD->tSize,
		arhBeg		= shD->arhBeg,
		arhEnd		= shD->arhEnd;
    bool	toUp = false,
		isDtChang = false,
		newFill = false,
		isDbl = false;

    //Check table structure
    //Clear columns
    for(int ncl = 0; ncl < shD->addrWdg->columnCount(); ncl++)
	if(shD->col.find(shD->addrWdg->horizontalHeaderItem(ncl)->data(Qt::UserRole).toString().toStdString()) == string::npos) {
	    shD->addrWdg->removeColumn(ncl);
	    ncl--;
	    newFill = true;
	}

    //Get columns indexes
    int c_tm = -1, c_tmu = -1, c_lev = -1, c_cat = -1, c_mess = -1;
    shD->addrWdg->verticalHeader()->setVisible(false);
    string clm;
    for(int c_off = 0; (clm=TSYS::strSepParse(shD->col,0,';',&c_off)).size(); ) {
	string clmNm;
	int ncl;
	for(ncl = 0; ncl < shD->addrWdg->columnCount(); ncl++)
	    if(shD->addrWdg->horizontalHeaderItem(ncl)->data(Qt::UserRole).toString() == clm.c_str())
		break;
	if(clm == "pos")	shD->addrWdg->verticalHeader()->setVisible(true);
	else if(clm == "tm")	{ clmNm = _("Date and time"); c_tm = ncl; }
	else if(clm == "utm")	{ clmNm = _("mcsec"); c_tmu = ncl; }
	else if(clm == "lev")	{ clmNm = _("Level"); c_lev = ncl; }
	else if(clm == "cat")	{ clmNm = _("Category"); c_cat = ncl; }
	else if(clm == "mess")	{ clmNm = _("Message"); c_mess = ncl; }
	if(clmNm.empty() || ncl < shD->addrWdg->columnCount())	continue;

	ncl = shD->addrWdg->columnCount();
	shD->addrWdg->setColumnCount(ncl+1);
	shD->addrWdg->setHorizontalHeaderItem(ncl, new QTableWidgetItem());
	shD->addrWdg->horizontalHeaderItem(ncl)->setText(clmNm.c_str());
	shD->addrWdg->horizontalHeaderItem(ncl)->setData(Qt::UserRole, clm.c_str());
	newFill = true;
    }

    newFill = newFill || !shD->messList.size();

    if(shD->lev < 0) shD->messList.clear();
    else {
	//Clear loaded data
	if(full) {
	    shD->messList.clear();
	    arhBeg = arhEnd = 0;
	}

	//Get archive parameters
	if(!arhBeg || !arhEnd || tTime > arhEnd || shD->tTimeCurent) {
	    XMLNode req("info");
	    req.setAttr("arch",shD->arch)->setAttr("path","/Archive/%2fserv%2fmess");
	    if(w->cntrIfCmd(req,true))	arhBeg = arhEnd = 0;
	    else {
		arhBeg = strtoul(req.attr("beg").c_str(), 0, 10);
		arhEnd = strtoul(req.attr("end").c_str(), 0, 10);
	    }
	}
	if(!arhBeg || !arhEnd)	return;

	//Correct request to archive border
	tTime = shD->tTimeCurent ? arhEnd : vmin(tTime, arhEnd);
	tTimeGrnd = vmax(tTimeGrnd, arhBeg);
    }

    //Clear data at time error
    unsigned int valEnd = 0, valBeg = 0;

    while(shD->messList.size() && (valEnd=shD->messList[0].time) > tTime) { shD->messList.pop_front(); isDtChang = true; }
    while(shD->messList.size() && (valBeg=shD->messList[shD->messList.size()-1].time) < tTimeGrnd) { shD->messList.pop_back(); isDtChang = true; }
    if(shD->messList.empty()) valEnd = valBeg = 0;

    if(tTime < tTimeGrnd || (tTime < valEnd && tTimeGrnd > valBeg)) {
	shD->messList.clear();
	shD->addrWdg->setRowCount(0);
	valEnd = valBeg = 0;
	return;
    }

    //Correct request to present data
    if(shD->time > shD->tmPrev) { if(valEnd) tTimeGrnd = valEnd; toUp = true; }
    else if((shD->time-shD->tSize) < shD->tmGrndPrev) { if(valBeg) tTime = valBeg-1; }
    else return;
    shD->tmPrev = shD->time;
    shD->tmGrndPrev = shD->time-shD->tSize;

    //Get values data
    XMLNode req("get");
    req.clear()->
	setAttr("arch", shD->arch)->
	setAttr("path", "/Archive/%2fserv%2fmess")->
	setAttr("tm", ll2s(tTime))->
	setAttr("tm_grnd", ll2s(tTimeGrnd))->
	setAttr("cat", shD->tmpl)->
	setAttr("lev", i2s(shD->lev));
    if(w->cntrIfCmd(req,true))	return;

    if(toUp)
	for(unsigned iReq = 0; iReq < req.childSize(); iReq++) {
	    XMLNode *rcd = req.childGet(iReq);
	    TMess::SRec mess(strtoul(rcd->attr("time").c_str(),0,10), s2i(rcd->attr("utime")),
		rcd->attr("cat"), (TMess::Type)abs(s2i(rcd->attr("lev"))), rcd->text());

	    // Check for dublicates
	    isDbl = false;
	    for(unsigned iP = 0; !isDbl && iP < shD->messList.size(); iP++) {
		if(mess.time > shD->messList[0].time && iP) break;
		if(shD->messList[iP].utime == mess.utime && shD->messList[iP].level == mess.level &&
			shD->messList[iP].categ == mess.categ && shD->messList[iP].mess == mess.mess) isDbl = true;
	    }
	    if(isDbl) continue;

	    // Insert new row
	    shD->messList.push_front(mess);
	    isDtChang = true;
	}
    else
	for(int iReq = req.childSize()-1; iReq >= 0; iReq--) {
	    XMLNode *rcd = req.childGet(iReq);
	    TMess::SRec mess(strtoul(rcd->attr("time").c_str(),0,10), s2i(rcd->attr("utime")),
		rcd->attr("cat"), (TMess::Type)abs(s2i(rcd->attr("lev"))), rcd->text());

	    // Check for dublicates
	    isDbl = false;
	    for(int iP = shD->messList.size()-1; !isDbl && iP >= 0; iP--) {
		if(mess.time < shD->messList[shD->messList.size()-1].time && iP < ((int)shD->messList.size()-1)) break;
		if(shD->messList[iP].utime == mess.utime && shD->messList[iP].level == mess.level &&
			shD->messList[iP].categ == mess.categ && shD->messList[iP].mess == mess.mess)	isDbl = true;
	    }
	    if(isDbl) continue;

	    // Insert new row
	    shD->messList.push_back(mess);
	    isDtChang = true;
	}

    if(shD->addrWdg->rowCount() == (int)shD->messList.size() && !isDtChang) return;

    //Sort data
    vector< pair<string,int> > sortIts;
    switch(shD->viewOrd&0x3) {
	case FP_ON_TM:
	    for(unsigned i_m = 0; i_m < shD->messList.size(); i_m++)
		sortIts.push_back(pair<string,int>(u2s(shD->messList[i_m].time)+" "+u2s(shD->messList[i_m].utime),i_m));
	    break;
	case FP_ON_LEV:
	    for(unsigned i_m = 0; i_m < shD->messList.size(); i_m++)
		sortIts.push_back(pair<string,int>(i2s(shD->messList[i_m].level),i_m));
	    break;
	case FP_ON_CAT:
	    for(unsigned i_m = 0; i_m < shD->messList.size(); i_m++)
		sortIts.push_back(pair<string,int>(shD->messList[i_m].categ,i_m));
	    break;
	case FP_ON_MESS:
	    for(unsigned i_m = 0; i_m < shD->messList.size(); i_m++)
		sortIts.push_back(pair<string,int>(shD->messList[i_m].mess,i_m));
	    break;
    }
    sort(sortIts.begin(), sortIts.end());
    if(shD->viewOrd&0x4) reverse(sortIts.begin(),sortIts.end());

    //Write to table
    shD->addrWdg->setRowCount(sortIts.size());
    QTableWidgetItem *tit;
    for(unsigned i_m = 0; i_m < sortIts.size(); i_m++) {
	QFont fnt;
	QColor clr, fclr;
	// Check properties
	for(int i_it = 0, lst_lev = -1; i_it < (int)shD->itProps.size(); i_it++)
	    if(shD->messList[sortIts[i_m].second].level >= shD->itProps[i_it].lev && shD->itProps[i_it].lev > lst_lev &&
		TRegExp(shD->itProps[i_it].tmpl, "p").test(shD->messList[sortIts[i_m].second].categ))
	    {
		fnt = shD->itProps[i_it].font;
		clr = shD->itProps[i_it].clr;
		if(shD->messList[sortIts[i_m].second].level == shD->itProps[i_it].lev) break;
		lst_lev = shD->itProps[i_it].lev;
	    }
	if(clr.isValid())
	    fclr = ((0.3*clr.red()+0.59*clr.green()+0.11*clr.blue()) > 128) ? Qt::black : Qt::white;

	for(int iCl = 0; iCl < shD->addrWdg->columnCount(); iCl++) {
	    tit = NULL;
	    if(iCl == c_tm) {
		QDateTime	dtm;
		dtm.setTime_t(shD->messList[sortIts[i_m].second].time);
		shD->addrWdg->setItem(i_m, c_tm, tit=new QTableWidgetItem(dtm.toString("dd.MM.yyyy hh:mm:ss")));
	    }
	    else if(iCl == c_tmu)
		shD->addrWdg->setItem(i_m, c_tmu, tit=new QTableWidgetItem(QString::number(shD->messList[sortIts[i_m].second].utime)));
	    else if(iCl == c_lev)
		shD->addrWdg->setItem(i_m, c_lev, tit=new QTableWidgetItem(QString::number(shD->messList[sortIts[i_m].second].level)));
	    else if(iCl == c_cat)
		shD->addrWdg->setItem(i_m, c_cat, tit=new QTableWidgetItem(shD->messList[sortIts[i_m].second].categ.c_str()));
	    else if(iCl == c_mess)
		shD->addrWdg->setItem(i_m, c_mess, tit=new QTableWidgetItem(shD->messList[sortIts[i_m].second].mess.c_str()));
	    else continue;
	    tit->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	    tit->setData(Qt::FontRole, fnt);
	    tit->setData(Qt::BackgroundRole, clr.isValid() ? clr : QVariant());
	    tit->setData(Qt::ForegroundRole, fclr.isValid() ? fclr : QVariant());
	}
    }

    if(newFill) {
	shD->addrWdg->resizeColumnsToContents();
	//Resize too long columns
	int max_col_sz = vmax(w->size().width()/2, 40);
	for(int iC = 0; iC < shD->addrWdg->columnCount(); iC++)
	    shD->addrWdg->setColumnWidth(iC,vmin(max_col_sz,shD->addrWdg->columnWidth(iC)));
	shD->addrWdg->horizontalHeader()->setStretchLastSection(true);
    }
    shD->addrWdg->resizeRowsToContents();
}

void ShapeProtocol::tracing( )
{
    WdgView *w = (WdgView *)((QTimer*)sender())->parent();
    ShpDt *shD = (ShpDt*)w->shpData;
    if(!w->isEnabled()) return;

    if(shD->tTimeCurent)shD->time = (unsigned int)time(NULL);
    else if(shD->time)	shD->time += shD->trcPer;
    loadData(w);
}

bool ShapeProtocol::event( WdgView *w, QEvent *event )	{ return false; }

bool ShapeProtocol::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if(qobject_cast<DevelWdgView*>(w))
	switch(event->type()) {
	    case QEvent::Enter:
	    case QEvent::Leave:
		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::MouseButtonDblClick:
		QApplication::sendEvent(w,event);
		return true;
	    default: break;
	}
    else {
	AttrValS attrs;
	switch(event->type()) {
	    case QEvent::FocusIn:
		attrs.push_back(std::make_pair("focus","1"));
		attrs.push_back(std::make_pair("event","ws_FocusIn"));
		w->attrsSet(attrs);
		break;
	    case QEvent::FocusOut:
		attrs.push_back(std::make_pair("focus","0"));
		attrs.push_back(std::make_pair("event","ws_FocusOut"));
		w->attrsSet(attrs);
		break;
	    default: break;
	}
    }

    return false;
}

void ShapeProtocol::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if(en)	wdg->installEventFilter(view);
    else	wdg->removeEventFilter(view);
    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC))) {
	    eventFilterSet(view, (QWidget*)wdg->children().at(iC), en);
	    if(en) ((QWidget*)wdg->children().at(iC))->setMouseTracking(true);
	}
}

void ShapeProtocol::setFocus(WdgView *view, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();
    //Set up current widget
    if(en) {
	if(isFocus) wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
    }
    else {
	if(wdg->focusPolicy() != Qt::NoFocus) {
	    wdg->setWindowIconText(QString::number((int)wdg->focusPolicy()));
	    wdg->setFocusPolicy(Qt::NoFocus);
	}
	if(devel) wdg->setMouseTracking(true);
    }

    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC)))
	    setFocus(view, (QWidget*)wdg->children().at(iC), en, devel);
}

//************************************************
//* Document view shape widget                   *
//************************************************
const char *ShapeDocument::XHTML_entity =
    "<!DOCTYPE xhtml [\n"
    "  <!ENTITY nbsp \"&#160;\" >\n"
    "]>\n";

ShapeDocument::ShapeDocument( ) : WdgShape("Document")	{ }

void ShapeDocument::init( WdgView *w )
{
    w->shpData = new ShpDt();
    ShpDt *shD = (ShpDt*)w->shpData;

    QVBoxLayout *lay = new QVBoxLayout(w);
#ifdef HAVE_WEBKIT
    shD->web = new QWebView(w);
    shD->web->setAttribute(Qt::WA_AcceptTouchEvents, false);
#else
    shD->web = new QTextBrowser(w);
#endif

    if(qobject_cast<RunWdgView*>(w)) {
	shD->web->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(shD->web, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(custContextMenu()));
    }

    eventFilterSet(w, shD->web, true);
    w->setFocusProxy(shD->web);
    if(qobject_cast<DevelWdgView*>(w)) setFocus(w, shD->web, false, true);

    lay->addWidget(shD->web);
}

void ShapeDocument::destroy( WdgView *w )
{
    delete (ShpDt*)w->shpData;
}

bool ShapeDocument::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    RunWdgView *runW = qobject_cast<RunWdgView*>(w);

    bool relDoc = false;	//Reload configuration

    switch(uiPrmPos) {
	case A_COM_LOAD: relDoc = true;	break;
	case A_EN:
	    if(!runW)	break;
	    shD->en = (bool)s2i(val);
	    shD->web->setVisible(shD->en && runW->permView());
	    break;
	case A_ACTIVE:
	    if(!runW)	break;
	    shD->active = (bool)s2i(val);
	    setFocus(w, shD->web, shD->active && runW->permCntr());
	    shD->web->setEnabled(shD->active && runW->permCntr());
	    break;
	case A_GEOM_MARGIN: w->layout()->setMargin(s2i(val));	break;
	case A_DocStyle: if(shD->style != val) { shD->style = val; relDoc = true; }	break;
	case A_DocTmpl:
	    if((!shD->doc.empty() && !shD->tmpl) || shD->doc == val) break;
	    shD->doc = val;
	    relDoc = true;
	    shD->tmpl = true;
	    break;
	case A_DocDoc:
	    if(sTrm(val).empty() || shD->doc == val) break;
	    shD->doc = val;
	    relDoc = true;
	    shD->tmpl = false;
	    break;
	case A_DocFont: if(shD->font != val) { shD->font = val; relDoc = true;}		break;
    }
    if(relDoc && !w->allAttrLoad()) {
	shD->web->setFont(getFont(shD->font,vmin(w->xScale(true),w->yScale(true)),false));

#ifdef HAVE_WEBKIT
	QPoint scrollPos;
	if(shD->web->page() && shD->web->page()->mainFrame()) scrollPos = shD->web->page()->mainFrame()->scrollPosition();
#else
	int scrollPos = shD->web->verticalScrollBar()->value();
#endif

	shD->web->setHtml(shD->toHtml().c_str());

#ifdef HAVE_WEBKIT
	if(!scrollPos.isNull() && shD->web->page() && shD->web->page()->mainFrame())
	    shD->web->page()->mainFrame()->setScrollPosition(scrollPos);
#else
	shD->web->verticalScrollBar()->setValue(scrollPos);
#endif
    }

    return true;
}

bool ShapeDocument::event( WdgView *w, QEvent *event )	{ return false; }

bool ShapeDocument::eventFilter( WdgView *w, QObject *object, QEvent *event )
{
    if(qobject_cast<DevelWdgView*>(w))
	switch(event->type()) {
	    case QEvent::Enter:
	    case QEvent::Leave:		return true;
	    case QEvent::MouseMove:
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::ContextMenu:
	    case QEvent::Wheel:
		QApplication::sendEvent(w,event);
		return true;
	    default: break;
	}
    else {
	AttrValS attrs;
	switch(event->type()) {
	    case QEvent::FocusIn:
		attrs.push_back(std::make_pair("focus","1"));
		attrs.push_back(std::make_pair("event","ws_FocusIn"));
		w->attrsSet(attrs);
		break;
	    case QEvent::FocusOut:
		attrs.push_back(std::make_pair("focus","0"));
		attrs.push_back(std::make_pair("event","ws_FocusOut"));
		w->attrsSet(attrs);
		break;
	    default: break;
	}
    }

    return false;
}

void ShapeDocument::eventFilterSet( WdgView *view, QWidget *wdg, bool en )
{
    if(en) wdg->installEventFilter(view);
    else   wdg->removeEventFilter(view);
    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC)))
	    eventFilterSet(view,(QWidget*)wdg->children().at(iC),en);
}

void ShapeDocument::custContextMenu( )
{
    QObject *web = sender();
    RunWdgView *w = dynamic_cast<RunWdgView*>(web->parent());
#ifdef HAVE_WEBKIT
    QMenu *menu = ((QWebView*)web)->page()->createStandardContextMenu();
#else
    QMenu *menu = ((QTextBrowser*)web)->createStandardContextMenu();
#endif
    menu->addSeparator();
    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("print",NULL,true).c_str())) ico_t.load(":/images/print.png");
    QAction *actPrint = new QAction(QPixmap::fromImage(ico_t),_("Print"),this);
    menu->addAction(actPrint);
    if(!ico_t.load(TUIS::icoGet("export",NULL,true).c_str())) ico_t.load(":/images/export.png");
    QAction *actExp = new QAction(QPixmap::fromImage(ico_t),_("Export"),this);
    menu->addAction(actExp);
    QAction *rez = menu->exec(QCursor::pos());
    if(rez == actPrint) w->mainWin()->printDoc(w->id());
    else if(rez == actExp) w->mainWin()->exportDoc(w->id());
    delete menu;
}

void ShapeDocument::setFocus( WdgView *view, QWidget *wdg, bool en, bool devel )
{
    int isFocus = wdg->windowIconText().toInt();
    //Set up current widget
    if(en) {
	if(isFocus && !devel)	wdg->setFocusPolicy((Qt::FocusPolicy)isFocus);
    }
    else {
	if(wdg->focusPolicy() != Qt::NoFocus) {
	    wdg->setWindowIconText(QString::number((int)wdg->focusPolicy()));
	    wdg->setFocusPolicy(Qt::NoFocus);
	}
	if(devel) wdg->setMouseTracking(true);
    }

    //Process childs
    for(int iC = 0; iC < wdg->children().size(); iC++)
	if(qobject_cast<QWidget*>(wdg->children().at(iC)))
	    setFocus(view,(QWidget*)wdg->children().at(iC),en,devel);
}

//Shape node date
string ShapeDocument::ShpDt::toHtml( )
{
    if(!web) return "";

    //Process source document
    // Parse document
    XMLNode xproc("body");
    try{ if(!doc.empty()) xproc.load(string(XHTML_entity)+doc, true, Mess->charset()); }
    catch(TError &err) { mess_err(mod->nodePath().c_str(), _("Document parsing error: %s"), err.mess.c_str()); }

    nodeProcess(&xproc);

    return "<?xml version='1.0' ?>\n"
	"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN'\n"
	"'DTD/xhtml1-transitional.dtd'>\n"
	"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
	"<head>\n"
	"  <meta http-equiv='Content-Type' content='text/html; charset="+Mess->charset()+"'/>\n"
	"  <style type='text/css'>\n"+
	" * { font-family: "+web->font().family().toStdString()+"; "
	    "font-size: "+i2s(web->font().pointSize())+"pt; "+
	    (TSYS::strParse(font,2," ",NULL,true).size()?(string("font-weight: ")+(web->font().bold()?"bold":"normal")+"; "):"")+
	    (TSYS::strParse(font,3," ",NULL,true).size()?(string("font-style: ")+(web->font().bold()?"italic":"normal")+"; "):"")+
	    "}\n"
	" big { font-size: 120%; }\n"+
	" small { font-size: 90%; }\n"+
	" h1 { font-size: 200%; }\n"+
	" h2 { font-size: 150%; }\n"+
	" h3 { font-size: 120%; }\n"+
	" h4 { font-size: 105%; }\n"+
	" h5 { font-size: 95%; }\n"+
	" h6 { font-size: 70%; }\n"+
	" u,b,i { font-size: inherit; }\n"+
	" sup,sub { font-size: 80%; }\n"+
	" th { font-weight: bold; }\n"+style+"</style>\n"
	"</head>\n"+
	xproc.save(XMLNode::Clean, Mess->charset())+
	"</html>";
}

void ShapeDocument::ShpDt::nodeProcess( XMLNode *xcur )
{
    //Delete process instructions
    //xcur->prcInstrClear();

    //Go to include nodes
    for(unsigned iC = 0; iC < xcur->childSize(); ) {
	// Check for special tags
	if(xcur->childGet(iC)->name().compare(0,3,"doc") == 0) { xcur->childDel(iC); continue; }
	nodeProcess(xcur->childGet(iC));
	iC++;
    }
}

#ifndef QT_NO_PRINTER
void ShapeDocument::ShpDt::print( QPrinter * printer )
{
# ifdef HAVE_WEBKIT
    web->print(printer);
# else
    web->document()->print(printer);
# endif
}
#endif

//************************************************
//* User function shape widget                   *
//************************************************
ShapeFunction::ShapeFunction( ) : WdgShape("Function")
{

}

/*bool ShapeFunction::event( WdgView *view, QEvent *event )
{

}*/

//************************************************
//* User element shape widget                    *
//************************************************
ShapeBox::ShapeBox( ) : WdgShape("Box")	{ }

void ShapeBox::init( WdgView *w )	{ w->shpData = new ShpDt(); }

void ShapeBox::destroy( WdgView *w )	{ delete (ShpDt*)w->shpData; }

bool ShapeBox::attrSet( WdgView *w, int uiPrmPos, const string &val, const string &attr )
{
    bool up = true;
    ShpDt *shD = (ShpDt*)w->shpData;
    RunWdgView	*runW = qobject_cast<RunWdgView*>(w);
    RunPageView	*runP = runW ? qobject_cast<RunPageView*>(w) : NULL;

    switch(uiPrmPos) {
	case A_COM_LOAD:
	    up = true;
	    if(runW && shD->inclPg)	shD->inclPg->setMinimumSize(w->size());
	    break;
	case A_COM_FOCUS: break;
	case A_EN:
	    if(!runW)	{ up = false; break; }
	    shD->en = (bool)s2i(val);
	    w->setVisible(shD->en && (runW->permView() || runP));
	    break;
	case A_ACTIVE:
	    if(!runW)	{ up = false; break; }
	    if(s2i(val) && runW->permCntr()) w->setFocusPolicy(Qt::TabFocus /*Qt::StrongFocus*/);
	    else w->setFocusPolicy(Qt::NoFocus);
	    break;
	case A_GEOM_MARGIN:
	    shD->geomMargin = s2i(val);
	    if(w->layout()) w->layout()->setMargin(shD->geomMargin);
	    break;
	case A_BackColor: {
	    shD->backGrnd.setColor(getColor(val));

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Window);
	    brsh.setColor(shD->backGrnd.color());
	    if(!brsh.color().isValid()) brsh.setColor(QPalette().brush(QPalette::Window).color());
	    brsh.setStyle(brsh.textureImage().isNull() ? Qt::SolidPattern : Qt::TexturePattern);
	    plt.setBrush(QPalette::Window, brsh);
	    w->setPalette(plt);

	    if(runP && (!runP->mainWin()->masterPg() || runP->mainWin()->masterPg() == w) && !shD->border.width())
		runP->mainWin()->setPalette(plt);

	    break;
	}
	case A_BackImg: {
	    QImage img;
	    string backimg = w->resGet(val);
	    shD->backGrnd.setTextureImage(QImage());
	    if(!backimg.empty() && img.loadFromData((const uchar*)backimg.data(),backimg.size()))
		shD->backGrnd.setTextureImage(img);

	    QPalette plt(w->palette());
	    QBrush brsh = plt.brush(QPalette::Window);
	    brsh.setTextureImage(img);
	    brsh.setStyle(!brsh.textureImage().isNull() ? Qt::TexturePattern : Qt::SolidPattern);
	    plt.setBrush(QPalette::Window, brsh);
	    w->setPalette(plt);

	    if(runP && (!runP->mainWin()->masterPg() || runP->mainWin()->masterPg() == w) && !shD->border.width())
		runP->mainWin()->setPalette(plt);

	    break;
	}
	case A_BordWidth:
	    shD->border.setWidth(s2i(val));

	    if(runP && (!runP->mainWin()->masterPg() || runP->mainWin()->masterPg() == w))
		runP->mainWin()->setPalette(shD->border.width() ? QPalette() : w->palette());

	    break;
	case A_BordColor: shD->border.setColor(getColor(val));	break;
	case A_BordStyle: shD->bordStyle = s2i(val);		break;
	case A_PG_OPEN_SRC:
	    if(!runW || runP)	{ up = false; break; }

	    //Put previous include widget to page cache
	    if((shD->inclPg && val != shD->inclPg->id()) || (!shD->inclPg && !val.empty())) {
		if(shD->inclPg) {
		    shD->inclPg->setReqTm(shD->inclPg->mainWin()->reqTm());
		    runW->mainWin()->pgCacheAdd(shD->inclPg);
		    shD->inclPg->setEnabled(false);
		    shD->inclPg->setVisible(false);
		    shD->inclPg->toPgCache();
		    shD->inclScrl->takeWidget();
		    shD->inclPg->setParent(NULL);
		    shD->inclPg->wx_scale = shD->inclPg->mainWin()->xScale();
		    shD->inclPg->wy_scale = shD->inclPg->mainWin()->yScale();
		    shD->inclPg = NULL;
		    w->setProperty("inclPg", "");
		}
		// Create new include widget
		if(val.size()) {
		    if(!shD->inclScrl) {
			QGridLayout *wlay = (QGridLayout*)w->layout();
			if(!wlay) wlay = new QGridLayout(w);
			wlay->setMargin(0/*shD->geomMargin*/);
			shD->inclScrl = new QScrollArea(w);
			shD->inclScrl->setFocusPolicy(Qt::NoFocus);
			shD->inclScrl->setFrameShape(QFrame::NoFrame);
			wlay->addWidget(shD->inclScrl);
		    }

		    QLabel *lab = new QLabel(QString(_("Loading page: '%1'.")).arg(val.c_str()), shD->inclPg);
		    lab->setAlignment(Qt::AlignCenter);
		    lab->setWordWrap(true);
		    lab->resize(w->size());
		    QPalette plt = lab->palette();
		    plt.setColor(QPalette::WindowText, shD->border.color());
		    lab->setPalette(plt);
		    lab->setLineWidth(shD->border.width());
		    lab->setFrameShape(QFrame::Box);
		    shD->inclScrl->setWidget(lab);
		    qApp->processEvents();

		    shD->inclPg = runW->mainWin()->pgCacheGet(val);
		    if(shD->inclPg) {
			shD->inclPg->setProperty("cntPg", TSYS::addr2str(w).c_str());
			shD->inclScrl->setWidget(shD->inclPg);
			shD->inclPg->setEnabled(true);
			shD->inclPg->setVisible(true);
			shD->inclPg->setMinimumSize(w->size());
			if(shD->inclPg->wx_scale != shD->inclPg->mainWin()->xScale() ||
				shD->inclPg->wy_scale != shD->inclPg->mainWin()->yScale())
			    shD->inclPg->load("");
			else {
			    unsigned trt = shD->inclPg->mainWin()->reqTm();
			    shD->inclPg->mainWin()->setReqTm(shD->inclPg->reqTm());
			    shD->inclPg->update(false);
			    shD->inclPg->mainWin()->setReqTm(trt);
			}
		    }
		    else {
			shD->inclPg = new RunPageView(val, (VisRun*)w->mainWin(), shD->inclScrl, Qt::SubWindow);
			shD->inclPg->setProperty("cntPg", TSYS::addr2str(w).c_str());
			shD->inclScrl->setWidget(shD->inclPg);
			shD->inclPg->resizeF(shD->inclPg->sizeF());	//To realign
			//shD->inclPg->load("");			//That does into the constructor

			shD->inclPg->setAttribute(Qt::WA_WindowPropagation, true);
		    }
		    w->setProperty("inclPg", TSYS::addr2str(shD->inclPg).c_str());
		}
	    } else up = false;
	    break;
	default: up = false;
    }

    if(up && !w->allAttrLoad() && uiPrmPos != -1) w->update();

    return up;
}

bool ShapeBox::event( WdgView *w, QEvent *event )
{
    ShpDt *shD = (ShpDt*)w->shpData;
    if(!shD->en) return false;

    switch(event->type()) {
	case QEvent::Paint: {
	    if(shD->inclPg) return false;
	    QPainter pnt(w);

	    //Apply margin
	    QRect dA = w->rect().adjusted(0, 0, -2*shD->geomMargin, -2*shD->geomMargin);
	    pnt.setWindow(dA);
	    pnt.setViewport(w->rect().adjusted(shD->geomMargin,shD->geomMargin,-shD->geomMargin,-shD->geomMargin));

	    //Draw background
	    if(shD->backGrnd.color().isValid()) pnt.fillRect(dA, shD->backGrnd.color());
	    if(!shD->backGrnd.textureImage().isNull()) pnt.fillRect(dA, shD->backGrnd.textureImage());

	    //Draw border
	    borderDraw(pnt, dA, shD->border, shD->bordStyle);

	    //Draw focused border
	    //if(w->hasFocus())	qDrawShadeRect(&pnt,dA,w->palette(),false,1);

	    return true;
	}
	//case QEvent::User:
	//    if(shD->inclPg) shD->inclPg->update();
	default: return false;
    }
    return false;
}

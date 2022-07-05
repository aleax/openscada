
//OpenSCADA module UI.QTCfg file: selfwidg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2022 by Roman Savochenko, <roman@oscada.org>       *
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

#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QItemEditorFactory>
#include <QMetaProperty>
#include <QSpinBox>
#include <QCalendarWidget>
#include <QToolTip>
#include <QStatusBar>
#include <QMenu>
#if QT_VERSION < 0x050000
#include <QPlastiqueStyle>
#else
#include <QCommonStyle>
#endif
#include <QScrollBar>
#include <QCompleter>
#include <QDesktopWidget>

#include "qtcfg.h"
#include "tuimod.h"
#include "../QTStarter/lib_qtgen.h"
#include "selfwidg.h"

using namespace OSCADA_QT;
using namespace QTCFG;

//************************************************
//* ListView: List view widget                   *
//************************************************
ListView::ListView( QWidget * parent ) : QListWidget(parent)
{
    setWordWrap(true);
}

QSize ListView::sizeHint( ) const	{ return QSize(QListWidget::sizeHint().width(), QFontMetrics(font()).height()*4); }

QSize ListView::minimumSizeHint( ) const	{ return QSize(QListWidget::minimumSizeHint().width(), QFontMetrics(font()).height()*4); }

//*************************************************
//* ImgView: Image view widget.                   *
//*************************************************
ImgView::ImgView( QWidget * parent, Qt::WindowFlags f, int ihSz, int ivSz ) :
    QWidget(parent,f), hSz(ihSz), vSz(ivSz)
{

}

ImgView::~ImgView( )	{ }

bool ImgView::setImage( const string &imgdata )
{
    bool rez = mImg.loadFromData((const uchar*)imgdata.c_str(), imgdata.size());

    if(rez) {
	mImg = mImg.scaled(QSize(hSz?vmin(hSz,mImg.width()):mImg.width(),
				   vSz?vmin(vSz,mImg.height()):mImg.height()),Qt::KeepAspectRatio);
	setMinimumSize(mImg.width(), mImg.height());
    }
    else {
	mImg = QImage();
	setMinimumSize(200, 40);
    }

    update();

    return rez;
}

void ImgView::paintEvent( QPaintEvent * )
{
    QPainter pnt(this);
    if(mImg.isNull()) {
	pnt.setWindow(0, 0, rect().width(), rect().height());
	pnt.setPen(QColor(255,0,0));
	pnt.setBackground(QBrush(QColor(210,237,234)));
	pnt.drawRect(0, 0, 199, 39);
	pnt.drawText(3, 3, 194, 34, Qt::AlignCenter, _("Picture is not set!"));
    }
    else {
	pnt.setWindow(0, 0, rect().width(), rect().height());
	pnt.drawImage(QPoint(0,0), mImg);
	pnt.setPen(QColor(0,0,255));
	pnt.drawRect(0, 0, mImg.width()-1, mImg.height()-1);
    }
}

//*********************************************************************************************
//* Universal edit line widget. Contain support of: QLineEdit, QSpinBox, QDoubleSpinBox,      *
//* QTimeEdit, QDateEdit and QDateTimeEdit.                                                   *
//*********************************************************************************************
LineEdit::LineEdit( QWidget *parent, LType tp, bool prev_dis ) :
    QWidget(parent), mTp((LineEdit::LType)-1), mPrev(!prev_dis), edFld(NULL), btFld(NULL)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    setType(tp);
}

void LineEdit::viewApplyBt( bool view )
{
    if(view == (bool)btFld) return;

    if(view && !btFld) {
	btFld = new QPushButton(this);
	btFld->setIcon(QIcon(":/images/button_ok.png"));
	btFld->setIconSize(QSize(icoSize(),icoSize()));
	btFld->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
	//btFld->setMaximumWidth(15);
	connect(btFld, SIGNAL(clicked()), this, SLOT(btApply()));
	layout()->addWidget(btFld);
    }
    if(!view && btFld) { btFld->deleteLater(); btFld = NULL; }
}

bool LineEdit::isEdited( )	{ return btFld; }

void LineEdit::setType( LType tp )
{
    if(tp == mTp) return;

    //Delete previous
    if(tp >= 0 && edFld) edFld->deleteLater();	//delete edFld;

    //Create new widget
    switch(tp) {
	case Text:
	    edFld = new QLineEdit(this);
	    connect((QLineEdit*)edFld, SIGNAL(textEdited(const QString&)), SLOT(changed()));
	    break;
	case Integer:
	    edFld = new QSpinBox(this);
	    connect((QSpinBox*)edFld, SIGNAL(valueChanged(int)), SLOT(changed()));
	    break;
	case Real:
	    edFld = new QDoubleSpinBox(this);
	    connect((QDoubleSpinBox*)edFld, SIGNAL(valueChanged(double)), SLOT(changed()));
	    break;
	case Time:
	    edFld = new QTimeEdit(this);
	    connect((QTimeEdit*)edFld, SIGNAL(timeChanged(const QTime&)), SLOT(changed()));
	    break;
	case Date:
	    edFld = new QDateEdit(this);
	    ((QDateEdit*)edFld)->setCalendarPopup(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)edFld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateEdit*)edFld, SIGNAL(dateChanged(const QDate&)), SLOT(changed()));
	    break;
	case DateTime:
	    edFld = new QDateTimeEdit(this);
	    ((QDateTimeEdit*)edFld)->setCalendarPopup(true);
	    ((QDateTimeEdit*)edFld)->calendarWidget()->setGridVisible(true);
	    ((QDateTimeEdit*)edFld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateTimeEdit*)edFld, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(changed()));
	    break;
	case Combo:
	    edFld = new QComboBox(this);
	    ((QComboBox*)edFld)->setEditable(true);
	    connect((QComboBox*)edFld, SIGNAL(editTextChanged(const QString&)), SLOT(changed()));
	    connect((QComboBox*)edFld, SIGNAL(activated(int)), this, SLOT(btApply()));
	    break;
    }
    ((QBoxLayout*)layout())->insertWidget(0, edFld);
    setFocusProxy(edFld);

    mTp = tp;
}

void LineEdit::changed( )
{
    //Enable for apply
    if(mPrev && !btFld) viewApplyBt(true);

    emit valChanged(value());
}

void LineEdit::setValue(const QString &txt)
{
    if(edFld) edFld->blockSignals(true);
    switch(type()) {
	case Text:
	    if(txt == ((QLineEdit*)edFld)->text()) break;
	    ((QLineEdit*)edFld)->setText(txt);
	    ((QLineEdit*)edFld)->setCursorPosition(0);
	    break;
	case Integer:
	    if(txt.toInt() == ((QSpinBox*)edFld)->value()) break;
	    ((QSpinBox*)edFld)->setValue(txt.toInt());
	    break;
	case Real:
	    if(txt.toDouble() == ((QDoubleSpinBox*)edFld)->value()) break;
	    ((QDoubleSpinBox*)edFld)->setValue(txt.toDouble());
	    break;
	case Time:
	    if(QTime().addSecs(txt.toInt()) == ((QTimeEdit*)edFld)->time()) break;
	    ((QTimeEdit*)edFld)->setTime(QTime().addSecs(txt.toInt()));
	    break;
	case Date: case DateTime:
	    if(QDateTime::fromTime_t(txt.toInt()) == ((QDateTimeEdit*)edFld)->dateTime()) break;
	    ((QDateTimeEdit*)edFld)->setDateTime(QDateTime::fromTime_t(txt.toInt()));
	    break;
	case Combo:
	    if(txt == ((QComboBox*)edFld)->currentText()) break;
	    if(((QComboBox*)edFld)->findText(txt) < 0) ((QComboBox*)edFld)->addItem(txt);
	    ((QComboBox*)edFld)->setEditText(txt);
	    break;
    }
    if(edFld) edFld->blockSignals(false);

    mVal = txt;

    if(btFld) viewApplyBt(false);
}

void LineEdit::setCfg( const QString &cfg )
{
    if(edFld) edFld->blockSignals(true);
    switch(type()) {
	case Text:	((QLineEdit*)edFld)->setInputMask(cfg);	break;
	case Integer: {
	    int		minv = 0, maxv = 100, sstep = 1;
	    string	pref, suff;
	    if(!cfg.isEmpty()) {
		minv  = s2i(TSYS::strSepParse(cfg.toStdString(),0,':'));
		maxv  = s2i(TSYS::strSepParse(cfg.toStdString(),1,':'));
		sstep = s2i(TSYS::strSepParse(cfg.toStdString(),2,':'));
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
	    }
	    ((QSpinBox*)edFld)->setRange(minv, maxv);
	    ((QSpinBox*)edFld)->setSingleStep(sstep);
	    ((QSpinBox*)edFld)->setPrefix(pref.c_str());
	    ((QSpinBox*)edFld)->setSuffix(suff.c_str());
	    break;
	}
	case Real: {
	    double minv = 0, maxv = 100, sstep = 1;
	    string pref, suff;
	    int    dec = 2;
	    if(!cfg.isEmpty()) {
		minv  = s2r(TSYS::strSepParse(cfg.toStdString(),0,':'));
		maxv  = s2r(TSYS::strSepParse(cfg.toStdString(),1,':'));
		sstep = s2r(TSYS::strSepParse(cfg.toStdString(),2,':'));
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
		dec   = s2i(TSYS::strSepParse(cfg.toStdString(),5,':'));
	    }
	    ((QDoubleSpinBox*)edFld)->setRange(minv, maxv);
	    ((QDoubleSpinBox*)edFld)->setSingleStep(sstep);
	    ((QDoubleSpinBox*)edFld)->setPrefix(pref.c_str());
	    ((QDoubleSpinBox*)edFld)->setSuffix(suff.c_str());
	    ((QDoubleSpinBox*)edFld)->setDecimals(dec);
	    break;
	}
	case Time: case Date: case DateTime:
	    ((QDateTimeEdit*)edFld)->setDisplayFormat(cfg);
	    break;
	case Combo: {
	    QString ctext = ((QComboBox*)edFld)->currentText();
	    ((QComboBox*)edFld)->clear();
	    ((QComboBox*)edFld)->addItems(cfg.split("\n"));
	    if(((QComboBox*)edFld)->findText(ctext) < 0) ((QComboBox*)edFld)->addItem(ctext);
	    ((QComboBox*)edFld)->setEditText(ctext);
	    if(((QComboBox*)edFld)->completer()) ((QComboBox*)edFld)->completer()->setCaseSensitivity(Qt::CaseSensitive);
	    break;
	}
    }
    if(btFld) viewApplyBt(false);
    if(edFld) edFld->blockSignals(false);
}

QString LineEdit::value( )
{
    switch(type()) {
	case Text:	return ((QLineEdit*)edFld)->text();
	case Integer:	return QString::number(((QSpinBox*)edFld)->value());
	case Real:	return QString::number(((QDoubleSpinBox*)edFld)->value());
	case Time:	return QString::number(QTime().secsTo(((QTimeEdit*)edFld)->time()));
	case Date: case DateTime:
	    return QString::number(((QDateTimeEdit*)edFld)->dateTime().toTime_t());
	case Combo:	return ((QComboBox*)edFld)->currentText();
    }
    return "";
}

void LineEdit::btApply( )
{
    viewApplyBt(false);

    if(mVal == value()) return;
    mVal = value();

    emit valChanged(value());
    emit apply();
}

void LineEdit::btCancel( )
{
    emit cancel();
    setValue(mVal);
}

bool LineEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyRelease && btFld) {
	QKeyEvent *keyEvent = (QKeyEvent*)e;
	if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
	    btFld->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape) {
	    btCancel();
	    return true;
	}
    }
    return QWidget::event(e);
}

//*************************************************
//* TextEdit: Text edit widget.                   *
//*************************************************
TextEdit::TextEdit( QWidget *parent, const char *name, bool prev_dis ) :
    QWidget(parent), isInit(false), isCh(false), sntHgl(NULL), butBox(NULL)
{
    setObjectName(name);

    edFld = new QTextEdit(this);
/*#if QT_VERSION < 0x050000
    edFld->setStyle(new QPlastiqueStyle());	//Force style set for resize allow everywhere
#else
    edFld->setStyle(new QCommonStyle());	//Force style set for resize allow everywhere
#endif*/
    edFld->setContextMenuPolicy(Qt::CustomContextMenu);
#if QT_VERSION >= 0x050A00
    edFld->setTabStopDistance(20);
#else
    edFld->setTabStopWidth(20);
#endif
    edFld->setAcceptRichText(false);
    connect(edFld, SIGNAL(textChanged()), this, SLOT(changed()));
    connect(edFld, SIGNAL(cursorPositionChanged()), this, SLOT(curPosChange()));
    connect(edFld, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ctrTreePopup()));
    edFld->move(0, 0); edFld->resize(size());

    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("find",NULL,true).c_str())) ico_t.load(":/images/find.png");
    actFind = new QAction(QPixmap::fromImage(ico_t), _("Find"), edFld);
    actFind->setShortcut(Qt::CTRL+Qt::Key_F);
    actFind->setShortcutContext(Qt::WidgetShortcut);
    connect(actFind, SIGNAL(triggered()), this, SLOT(find()));
    edFld->addAction(actFind);
    actFindNext = new QAction(_("Find next"), edFld);
    actFindNext->setShortcut(Qt::Key_F3);
    actFindNext->setShortcutContext(Qt::WidgetShortcut);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(find()));
    edFld->addAction(actFindNext);

    if(!prev_dis) {
	butBox = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel, Qt::Horizontal, this);
	QImage ico_t;
	butBox->button(QDialogButtonBox::Apply)->setText(_("Apply"));
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	butBox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
	connect(butBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(btApply()));
	butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	connect(butBox, SIGNAL(rejected()), this, SLOT(btCancel()));
	butBox->setVisible(false);
    }
}

QSize TextEdit::sizeHint( ) const
{
    return QSize(edFld->sizeHint().width(), QFontMetrics(edFld->currentFont()).height()*(mRowCol.height()+2));
					    //2*edFld->currentFont().pointSize()*(mRowCol.height()+1));
}

QSize TextEdit::minimumSizeHint( ) const
{
    return QSize(edFld->minimumSizeHint().width(), QFontMetrics(edFld->currentFont()).height()*(mRowCol.height()+2));
						//2*edFld->currentFont().pointSize()*(mRowCol.height()+1));
}

bool TextEdit::isChanged( )		{ return isCh; /*(butBox && butBox->isVisible());*/ }

QString TextEdit::text( )		{ return edFld->toPlainText(); }

bool TextEdit::hasFocus( ) const	{ return edFld->hasFocus(); }

void TextEdit::setText( const QString &text )
{
    if(!sntHgl || sntHgl->isBuiltInSH) {	//Try the builtin syntax higlihgt
	XMLNode hglO("SnthHgl");
	if(SnthHgl::checkInSnthHgl(text,hglO)) { setSnthHgl(hglO); sntHgl->isBuiltInSH = true; }
    }

    isInit = true;
    edFld->blockSignals(true);	//!!!! The block to prevent the status bar updating and crashes here sometime
    edFld->setPlainText(text);
    edFld->blockSignals(false);
    edFld->document()->setModified(false);
    isInit = false;
    changed();
}

void TextEdit::setSnthHgl( XMLNode nd )
{
    if(!sntHgl)	sntHgl = new SnthHgl(edFld->document());
    sntHgl->setSnthHgl(nd);
}

void TextEdit::setRowsCols( int w, int h )
{
    mRowCol = QSize(w, h);

    if(mRowCol.width()) {
	edFld->setWordWrapMode(QTextOption::WordWrap);
	edFld->setLineWrapMode(QTextEdit::FixedColumnWidth);
	edFld->setLineWrapColumnOrWidth(mRowCol.width());
    }
    else edFld->setLineWrapMode(QTextEdit::NoWrap);
}

void TextEdit::changed( )
{
    if(isInit) return;
    if(butBox) {
	butBox->setVisible((isCh=edFld->document()->isModified()));
	if(butBox->isVisible()) {
	    butBox->move(width()-butBox->width(), height()-butBox->height());
	    edFld->resize(edFld->width(), height()-butBox->height());
	}
    }
    if(edFld->document()->isModified()) emit textChanged(text());
}

void TextEdit::btApply( )
{
    emit textChanged(text());
    butBox->setVisible((isCh=false));
    edFld->document()->setModified(isCh);
    edFld->resize(size());
    emit apply();

    //Try the builtin syntax higlihgt
    if(!sntHgl || sntHgl->isBuiltInSH) {
	XMLNode hglO("SnthHgl");
	if(SnthHgl::checkInSnthHgl(text(),hglO) && (!sntHgl || sntHgl->snthHgl().save() != hglO.save())) {
	    //Activation the syntax rules change
	    int cursorPos = edFld->textCursor().position();
	    int scrollVPos = edFld->verticalScrollBar()->value();
	    int scrollHPos = edFld->horizontalScrollBar()->value();

	    edFld->blockSignals(true);
	    edFld->setPlainText(text());
	    edFld->blockSignals(false);

	    //Cursor position restore
	    QTextCursor tCur = edFld->textCursor(); tCur.setPosition(cursorPos);
	    edFld->setTextCursor(tCur); edFld->ensureCursorVisible();
	    edFld->verticalScrollBar()->setValue(scrollVPos);
	    edFld->horizontalScrollBar()->setValue(scrollHPos);
	}
    }
}

void TextEdit::btCancel( )
{
    butBox->setVisible((isCh=false));
    edFld->document()->setModified(isCh);
    edFld->resize(size());
    emit cancel();
}

void TextEdit::curPosChange( )
{
    ((QMainWindow*)window())->statusBar()->showMessage(QString(_("Cursor = (%1:%2)"))
	.arg(edFld->textCursor().blockNumber()+1).arg(edFld->textCursor().columnNumber()+1),10000);
}

void TextEdit::resizeEvent( QResizeEvent *e )
{
    if(butBox && butBox->isVisible()) {
	butBox->move(width()-butBox->width(), height()-butBox->height());
	edFld->resize(width(), height()-butBox->height());
    }
    else edFld->resize(size());
}

bool TextEdit::event( QEvent *e )
{
    if(butBox && e->type() == QEvent::KeyRelease) {
	QKeyEvent *keyEvent = (QKeyEvent*)e;
	if((keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) && QApplication::keyboardModifiers()&Qt::ControlModifier) {
	    butBox->button(QDialogButtonBox::Apply)->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape) {
	    butBox->button(QDialogButtonBox::Cancel)->animateClick();
	    return true;
	}
    }
    else if(e->type() == QEvent::MouseButtonPress) holdPnt = mapFromGlobal(cursor().pos());
    else if(e->type() == QEvent::MouseMove) {
	QPoint curp = mapFromGlobal(cursor().pos());
	setFixedHeight(vmax(50,size().height()+(curp-holdPnt).y()));
	holdPnt = curp;
    }

    return QWidget::event(e);
}

void TextEdit::ctrTreePopup( )
{
    QMenu *menu = edFld->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(actFind);
    menu->addAction(actFindNext);
    menu->exec(QCursor::pos());
    menu->deleteLater();	//delete menu;
}

void TextEdit::find( )
{
    bool isFind = false;
    int fopt = (QTextDocument::FindFlag)actFind->objectName().section(':',0,0).toInt();
    QString fstr = actFind->objectName().section(':',1);
    if(sender() == actFind) {
	InputDlg dlg(this, actFind->icon(), QString(_("Enter a string to search:")), _("Searching a string"), 0, 0);
	QLineEdit *le = new QLineEdit(fstr, &dlg);
	dlg.edLay->addWidget(le, 0, 0);
	QCheckBox *bw = new QCheckBox(_("Backward"), &dlg);
	if(fopt & QTextDocument::FindBackward) bw->setCheckState(Qt::Checked);
	dlg.edLay->addWidget(bw, 1, 0);
	QCheckBox *cs = new QCheckBox(_("Case sensitively"), &dlg);
	if(fopt & QTextDocument::FindCaseSensitively) cs->setCheckState(Qt::Checked);
	dlg.edLay->addWidget(cs, 2, 0);
	QCheckBox *ww = new QCheckBox(_("Whole words"), &dlg);
	if(fopt & QTextDocument::FindWholeWords) ww->setCheckState(Qt::Checked);
	dlg.edLay->addWidget(ww, 3, 0);
	le->setFocus(Qt::OtherFocusReason);
	dlg.resize(400, 210);
	if(dlg.exec() == QDialog::Accepted && !le->text().isEmpty()) {
	    fopt = (QTextDocument::FindFlag)0;
	    if(bw->checkState() == Qt::Checked) fopt |= QTextDocument::FindBackward;
	    if(cs->checkState() == Qt::Checked) fopt |= QTextDocument::FindCaseSensitively;
	    if(ww->checkState() == Qt::Checked) fopt |= QTextDocument::FindWholeWords;
	    fstr = le->text();
	    isFind = true;
	}
    }
    else if(sender() == actFindNext && !fstr.isEmpty()) isFind = true;

    if(isFind) {
	edFld->find(fstr,(QTextDocument::FindFlag)fopt);
	actFind->setObjectName(QString::number(fopt)+":"+fstr);
    }
}

//************************************************
//* CfgTable: Table view widget                  *
//************************************************
CfgTable::CfgTable( QWidget *parent ) : QTableWidget(parent)
{
/*#if QT_VERSION < 0x050000
    setStyle(new QPlastiqueStyle());	//Force style set for resize allow everywhere
#else
    setStyle(new QCommonStyle());	//Force style set for resize allow everywhere
#endif*/
}

QSize CfgTable::sizeHint( ) const
{
    int rows = property("rows").toInt();
    return QSize(QTableWidget::sizeHint().width(), rows?QFontMetrics(font()).height()*rows+1:QTableWidget::sizeHint().height());
}

void CfgTable::resizeRowsToContentsLim( )
{
    QTableView::resizeRowsToContents();
    for(int iRW = 0; iRW < rowCount(); iRW++)
	setRowHeight(iRW, vmin(rowHeight(iRW), size().height()/1.3));
}

bool CfgTable::event( QEvent *e )
{
    if(e->type() == QEvent::KeyPress) {
	QKeyEvent *key = static_cast<QKeyEvent*>(e);
	bool toUp = false;
	ConfApp *mainW = dynamic_cast<ConfApp *>(window());
	if(mainW && (QApplication::keyboardModifiers()&Qt::ControlModifier) &&
		((toUp=(key->key()==Qt::Key_Up)) || key->key() == Qt::Key_Down)) {
	    try {
		int row = currentRow();
		int r_new = toUp ? row-1 : row+1;

		XMLNode *n_el = SYS->ctrId(mainW->root, TSYS::strDecode(objectName().toStdString(),TSYS::PathEl));
		if(n_el->attr("s_com").find("move") != string::npos && r_new >= 0 && r_new < rowCount()) {
		    string el_path = mainW->selPath + "/" + objectName().toStdString();
		    XMLNode n_el1;
		    n_el1.setAttr("path", el_path);
		    n_el1.setName("move");
		    n_el1.setAttr("row", i2s(row))->setAttr("to", i2s(r_new));
		    mess_info(mod->nodePath().c_str(), _("%s| '%s' moved for the record %d to %d."),
			mainW->user().c_str(), el_path.c_str(), row, r_new);
		    if(mainW->cntrIfCmd(n_el1)) throw TError(n_el1.attr("mcat").c_str(), n_el1.text().c_str());
		    mainW->tblInit = true;
		    item(row,currentColumn())->setSelected(false);
		    for(int iCol = 0; iCol < columnCount(); iCol++) {
			QTableWidgetItem *tIt = takeItem(row, iCol), *tIt2 = takeItem(r_new, iCol);
			setItem(r_new, iCol, tIt); setItem(row, iCol, tIt2);
		    }
		    mainW->tblInit = false;
		}
	    } catch(TError &err) { mod->postMess(err.cat, err.mess, TUIMod::Error, this); }
	}
    }
    else if(e->type() == QEvent::MouseButtonPress)
	holdPnt = mapFromGlobal(cursor().pos());
    else if(e->type() == QEvent::MouseMove) {
	QPoint curp = mapFromGlobal(cursor().pos());
	int hg = vmax(50,size().height()+(curp-holdPnt).y());
	setMinimumHeight(hg); setMaximumHeight(hg);
	holdPnt = curp;
    }

    return QTableWidget::event(e);
}

//*************************************************
//* InputDlg: Id and name input dialog.           *
//*************************************************
InputDlg::InputDlg( QWidget *parent, const QIcon &icon, const QString &mess,
	const QString &ndlg, int with_id, int with_nm, QDialogButtonBox::StandardButtons buttons ) :
		QDialog(parent), mId(NULL), mName(NULL)
{
    setMaximumSize(800, 600);
    setWindowTitle(ndlg);
    setWindowIcon(icon);
    setSizeGripEnabled(true);

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    //Icon label and text message
    QHBoxLayout *intr_lay = new QHBoxLayout;
    intr_lay->setSpacing(6);

    QLabel *icon_lab = new QLabel(this);
    icon_lab->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
    icon_lab->setPixmap(icon.pixmap(48));
    intr_lay->addWidget(icon_lab);

    inpLab = new QLabel(mess, this);
    inpLab->setWordWrap(true);
    intr_lay->addWidget(inpLab);
    dlg_lay->addItem(intr_lay);

    //Id and name fields
    edLay = new QGridLayout;
    if(with_nm || with_id) {
	edLay->setSpacing(6);
	if(with_id) {
	    mIdLab = new QLabel(_("Identifier:"), this);
	    edLay->addWidget(mIdLab, 3, 0);
	    mId = new QLineEdit(this);
	    mId->setMaxLength(with_id);
	    edLay->addWidget(mId, 3, 1);
	}
	if(with_nm) {
	    mNameLab = new QLabel(_("Name:"), this);
	    edLay->addWidget(mNameLab, 4, 0);
	    mName = new QLineEdit(this);
	    mName->setMaxLength(with_nm);
	    edLay->addWidget(mName, 4, 1);
	}
    }

    edLay->addItem(new QSpacerItem(10,0,QSizePolicy::Minimum,QSizePolicy::Expanding), 100, 0, 1, -1);

    dlg_lay->addItem(edLay);

    //Qk and Cancel buttons
    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Raised);
    dlg_lay->addWidget(sep);

    QDialogButtonBox *butBox = new QDialogButtonBox(buttons, Qt::Horizontal, this);
    QImage ico_t;
    if(buttons & QDialogButtonBox::Ok) {
	butBox->button(QDialogButtonBox::Ok)->setText(_("Ok"));
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	butBox->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
	connect(butBox, SIGNAL(accepted()), this, SLOT(accept()));
    }
    if(buttons & QDialogButtonBox::Cancel) {
	butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	connect(butBox, SIGNAL(rejected()), this, SLOT(reject()));
    }
    dlg_lay->addWidget(butBox);

    resize(400, 150+(35*(with_nm?1:0))+(35*(with_id?1:0)));
}

QString InputDlg::id( )		{ return mId ? mId->text() : ""; }

QString InputDlg::name( )	{ return mName ? mName->text() : ""; }

QString InputDlg::mess( )	{ return inpLab->text(); }

void InputDlg::setId( const QString &val )	{ if(mId) mId->setText(val); }

void InputDlg::setName( const QString &val )	{ if(mName) mName->setText(val); }

void InputDlg::setMess( const QString &val )	{ inpLab->setText(val); }

void InputDlg::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));

#if defined(__ANDROID__)
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
#endif
}

//*****************************************************
//* ReqIdNameDlg: Request node identifier and/or name *
//*****************************************************
ReqIdNameDlg::ReqIdNameDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg ) :
    InputDlg(parent, icon, mess, ndlg, 1000, 10000)
{
    itTpLab = new QLabel(_("Item type:"), this);
    edLay->addWidget(itTpLab, 0, 0);
    itTp = new QComboBox(this);
    itTp->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    edLay->addWidget(itTp, 0, 1);
    connect(itTp, SIGNAL(currentIndexChanged(int)), this, SLOT(selectItTp(int)));
}

string ReqIdNameDlg::target( )
{
    if(itTp->count() <= 0) return "";

    return itTp->itemData(itTp->currentIndex()).toString().toStdString();
}

void ReqIdNameDlg::setTargets( const vector<string> &tgs )
{
    itTp->clear();
    int defPos = 0;
    for(unsigned iT = 0; iT < tgs.size(); iT++) {
	itTp->addItem(TSYS::strSepParse(tgs[iT],3,'\n').c_str(), tgs[iT].c_str());
	if(s2i(TSYS::strSepParse(tgs[iT],4,'\n'))) defPos = itTp->count()-1;
    }
    if(tgs.size()) itTp->setCurrentIndex(defPos);
    bool tpView = !(itTp->count()==1 && itTp->itemText(0).isEmpty());
    itTpLab->setVisible(tpView); itTp->setVisible(tpView);
    itTp->setEnabled(itTp->count()>1);
}

void ReqIdNameDlg::setPassive( )
{
    itTp->setEnabled(false);
    if(mId)	mId->setEnabled(false);
    if(mName)	mName->setEnabled(false);
    if(mName && name().size())	mName->setVisible(false);
}

void ReqIdNameDlg::selectItTp( int it )
{
    if(it < 0) return;
    string its = itTp->itemData(it).toString().toStdString();
    int idSz = s2i(TSYS::strSepParse(its,0,'\n'));
    if(idSz > 0) mId->setMaxLength(idSz);
    mIdLab->setVisible(idSz>=0); mId->setVisible(idSz>=0);
    int idm = s2i(TSYS::strSepParse(its,1,'\n'));	//Default idm is boolean for id-mode, enable name
    if(idm > 1)	mName->setMaxLength(idm);
    mNameLab->setVisible(idm); mName->setVisible(idm);
}

//*************************************************
//* DlgUser: User select dialog                   *
//*************************************************
DlgUser::DlgUser( QWidget *parent ) : QDialog(parent)
{
    setWindowTitle(_("Selecting an user"));

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *edLay = new QGridLayout;
    edLay->setSpacing(6);
    edLay->addWidget(new QLabel(_("User:"),this), 0, 0);
    users = new QComboBox(this);
    edLay->addWidget(users, 0, 1);
    edLay->addWidget(new QLabel(_("Password:"),this), 1, 0);
    passwd = new QLineEdit(this);
    passwd->setEchoMode(QLineEdit::Password);
    edLay->addWidget(passwd, 1, 1);
    dlg_lay->addItem(edLay);

    dlg_lay->addItem(new QSpacerItem(20,0,QSizePolicy::Minimum,QSizePolicy::Expanding));

    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Raised);
    dlg_lay->addWidget(sep);

    QDialogButtonBox *butBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QImage ico_t;
    butBox->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    butBox->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(accepted()), this, SLOT(accept()));
    butBox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    butBox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(butBox, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget(butBox);

    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));

    //Fill users list
    vector<string> u_list;
    SYS->security().at().usrList(u_list);
    for(unsigned i_l = 0; i_l < u_list.size(); i_l++) {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(u_list[i_l]).at().picture(), TSYS::base64);
	QImage img;
	if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
	    users->addItem(QPixmap::fromImage(img),u_list[i_l].c_str());
	else users->addItem(u_list[i_l].c_str());
    }
}

QString DlgUser::user( )	{ return users->currentText(); }

QString DlgUser::password( )	{ return passwd->text(); }

void DlgUser::finish( int result )
{
    if(result) {
	//Check user
	if(SYS->security().at().usrPresent(user().toStdString()) &&
		SYS->security().at().usrAt(user().toStdString()).at().auth(password().toStdString()))
	    setResult(SelOK);
	else setResult(SelErr);
    }
    else setResult(SelCancel);
}

void DlgUser::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));

#if defined(__ANDROID__)
    move((QApplication::desktop()->width()-width())/2, (QApplication::desktop()->height()-height())/2);
#endif
}

//*********************************************
//* UserStBar: Status bar user widget.        *
//*********************************************
UserStBar::UserStBar( const QString &iuser, QWidget * parent ) : QLabel(parent)
{
    setUser(iuser);
}

QString UserStBar::user( )	{ return userTxt; }

void UserStBar::setUser( const QString &val )
{
    setText(QString("<font color='%1'>%2</font>").arg(colorAdjToBack((val=="root")?"red":"green",qApp->palette().color(QPalette::Window)).name()).arg(val));
    userTxt = val;
}

bool UserStBar::event( QEvent *event )
{
    if(event->type() == QEvent::MouseButtonDblClick) userSel();
    else if(event->type() == QEvent::PaletteChange) setUser(user());

    return QLabel::event(event);
}

bool UserStBar::userSel( )
{
    DlgUser d_usr(parentWidget());
    int rez = d_usr.exec();
    if(rez == DlgUser::SelOK && d_usr.user() != user()) {
	setUser(d_usr.user());
	emit userChanged();
	return true;
    }
    else if(rez == DlgUser::SelErr)
	mod->postMess(mod->nodePath().c_str(),_("Error authentication!!!"),TUIMod::Warning,this);

    return false;
}

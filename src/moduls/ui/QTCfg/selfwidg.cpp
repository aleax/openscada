
//OpenSCADA system module UI.QTCfg file: selfwidg.cpp
/***************************************************************************
 *   Copyright (C) 2004-2014 by Roman Savochenko, <rom_as@oscada.org>      *
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

#include <tsys.h>

#include "tuimod.h"
#include "selfwidg.h"

using namespace QTCFG;

//*************************************************
//* ImgView: Image view widget.                   *
//*************************************************
ImgView::ImgView( QWidget * parent, Qt::WindowFlags f, int ih_sz, int iv_sz ) :
    QWidget(parent,f), h_sz(ih_sz), v_sz(iv_sz)
{

}

ImgView::~ImgView( )	{ }

bool ImgView::setImage( const string &imgdata )
{
    bool rez = m_img.loadFromData((const uchar*)imgdata.c_str(),imgdata.size());

    if(rez)
    {
	m_img = m_img.scaled(QSize(h_sz?vmin(h_sz,m_img.width()):m_img.width(),
				   v_sz?vmin(v_sz,m_img.height()):m_img.height()),Qt::KeepAspectRatio);
	setMinimumSize(m_img.width(),m_img.height());
    }
    else
    {
	m_img = QImage();
	setMinimumSize(200,40);
    }

    update();

    return rez;
}

void ImgView::paintEvent( QPaintEvent * )
{
    QPainter pnt( this );
    if(m_img.isNull())
    {
	pnt.setWindow(0,0,rect().width(),rect().height());
	pnt.setPen(QColor(255,0,0));
	pnt.setBackground(QBrush(QColor(210,237,234)));
	pnt.drawRect(0,0,199,39);
	pnt.drawText(3,3,194,34,Qt::AlignCenter,_("Picture is not set!"));
    }
    else
    {
	pnt.setWindow( 0, 0, rect().width(), rect().height() );
	pnt.drawImage(QPoint(0,0),m_img);
	pnt.setPen(QColor(0,0,255));
	pnt.drawRect( 0, 0, m_img.width()-1, m_img.height()-1 );
    }
}

//*********************************************************************************************
//* Universal edit line widget. Contain support of: QLineEdit, QSpinBox, QDoubleSpinBox,      *
//* QTimeEdit, QDateEdit and QDateTimeEdit.                                                   *
//*********************************************************************************************
LineEdit::LineEdit( QWidget *parent, LType tp, bool prev_dis ) :
    QWidget(parent), m_tp((LineEdit::LType)-1), mPrev(!prev_dis), ed_fld(NULL), bt_fld(NULL)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    setType(tp);
}

void LineEdit::viewApplyBt( bool view )
{
    if(view == (bool)bt_fld) return;

    if(view && !bt_fld)
    {
	bt_fld = new QPushButton(this);
	bt_fld->setIcon(QIcon(":/images/ok.png"));
	bt_fld->setIconSize(QSize(12,12));
	bt_fld->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
	//bt_fld->setMaximumWidth( 15 );
	connect(bt_fld, SIGNAL(clicked()), this, SLOT(applySlot()));
	layout()->addWidget(bt_fld);
    }
    if(!view && bt_fld) { bt_fld->deleteLater(); bt_fld = NULL; }
}

bool LineEdit::isEdited( )	{ return bt_fld; }

void LineEdit::setType( LType tp )
{
    if(tp == m_tp) return;

    //Delete previous
    if(tp >= 0 && ed_fld) delete ed_fld;

    //Create new widget
    switch(tp)
    {
	case Text:
	    ed_fld = new QLineEdit(this);
	    connect((QLineEdit*)ed_fld, SIGNAL(textEdited(const QString&)), SLOT(changed()));
	    break;
	case Integer:
	    ed_fld = new QSpinBox(this);
	    connect((QSpinBox*)ed_fld, SIGNAL(valueChanged(int)), SLOT(changed()));
	    break;
	case Real:
	    ed_fld = new QDoubleSpinBox(this);
	    connect((QDoubleSpinBox*)ed_fld, SIGNAL(valueChanged(double)), SLOT(changed()));
	    break;
	case Time:
	    ed_fld = new QTimeEdit(this);
	    connect((QTimeEdit*)ed_fld, SIGNAL(timeChanged(const QTime&)), SLOT(changed()));
	    break;
	case Date:
	    ed_fld = new QDateEdit(this);
	    ((QDateEdit*)ed_fld)->setCalendarPopup(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setGridVisible(true);
	    ((QDateEdit*)ed_fld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateEdit*)ed_fld, SIGNAL(dateChanged(const QDate&)), SLOT(changed()));
	    break;
	case DateTime:
	    ed_fld = new QDateTimeEdit(this);
	    ((QDateTimeEdit*)ed_fld)->setCalendarPopup(true);
	    ((QDateTimeEdit*)ed_fld)->calendarWidget()->setGridVisible(true);
	    ((QDateTimeEdit*)ed_fld)->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	    connect((QDateTimeEdit*)ed_fld, SIGNAL(dateTimeChanged(const QDateTime&)), SLOT(changed()));
	    break;
	case Combo:
	    ed_fld = new QComboBox(this);
	    ((QComboBox*)ed_fld)->setEditable(true);
	    connect((QComboBox*)ed_fld, SIGNAL(editTextChanged(const QString&)), SLOT(changed()));
	    connect((QComboBox*)ed_fld, SIGNAL(activated(int)), this, SLOT(applySlot()));
	    break;
    }
    ((QBoxLayout*)layout())->insertWidget(0, ed_fld);
    setFocusProxy(ed_fld);

    m_tp = tp;
}

void LineEdit::changed( )
{
    //> Enable apply
    if(mPrev && !bt_fld) viewApplyBt(true);

    emit valChanged(value());
}

void LineEdit::setValue(const QString &txt)
{
    if(ed_fld) ed_fld->blockSignals(true);
    switch(type())
    {
	case Text:
	    if(txt == ((QLineEdit*)ed_fld)->text()) break;
	    ((QLineEdit*)ed_fld)->setText(txt);
	    ((QLineEdit*)ed_fld)->setCursorPosition(0);
	    break;
	case Integer:
	    if(txt.toInt() == ((QSpinBox*)ed_fld)->value()) break;
	    ((QSpinBox*)ed_fld)->setValue(txt.toInt());
	    break;
	case Real:
	    if(txt.toDouble() == ((QDoubleSpinBox*)ed_fld)->value()) break;
	    ((QDoubleSpinBox*)ed_fld)->setValue(txt.toDouble());
	    break;
	case Time:
	    if(QTime().addSecs(txt.toInt()) == ((QTimeEdit*)ed_fld)->time()) break;
	    ((QTimeEdit*)ed_fld)->setTime(QTime().addSecs(txt.toInt()));
	    break;
	case Date: case DateTime:
	    if(QDateTime::fromTime_t(txt.toInt()) == ((QDateTimeEdit*)ed_fld)->dateTime()) break;
	    ((QDateTimeEdit*)ed_fld)->setDateTime(QDateTime::fromTime_t(txt.toInt()));
	    break;
	case Combo:
	    if(txt == ((QComboBox*)ed_fld)->currentText()) break;
	    if(((QComboBox*)ed_fld)->findText(txt) < 0) ((QComboBox*)ed_fld)->addItem(txt);
	    ((QComboBox*)ed_fld)->setEditText(txt);
	    break;
    }
    if(ed_fld) ed_fld->blockSignals(false);

    m_val = txt;

    if(bt_fld) viewApplyBt(false);
}

void LineEdit::setCfg( const QString &cfg )
{
    if(ed_fld) ed_fld->blockSignals(true);
    switch(type())
    {
	case Text:	((QLineEdit*)ed_fld)->setInputMask(cfg);	break;
	case Integer:
	{
	    int		minv = 0, maxv = 100, sstep = 1;
	    string	pref, suff;
	    if(!cfg.isEmpty())
	    {
		minv  = atoi(TSYS::strSepParse(cfg.toStdString(),0,':').c_str());
		maxv  = atoi(TSYS::strSepParse(cfg.toStdString(),1,':').c_str());
		sstep = atoi(TSYS::strSepParse(cfg.toStdString(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
	    }
	    ((QSpinBox*)ed_fld)->setRange(minv,maxv);
	    ((QSpinBox*)ed_fld)->setSingleStep(sstep);
	    ((QSpinBox*)ed_fld)->setPrefix(pref.c_str());
	    ((QSpinBox*)ed_fld)->setSuffix(suff.c_str());
	    break;
	}
	case Real:
	{
	    double minv = 0, maxv = 100, sstep = 1;
	    string pref, suff;
	    int    dec = 2;
	    if( !cfg.isEmpty() )
	    {
		minv  = atof(TSYS::strSepParse(cfg.toStdString(),0,':').c_str());
		maxv  = atof(TSYS::strSepParse(cfg.toStdString(),1,':').c_str());
		sstep = atof(TSYS::strSepParse(cfg.toStdString(),2,':').c_str());
		pref  = TSYS::strSepParse(cfg.toStdString(),3,':');
		suff  = TSYS::strSepParse(cfg.toStdString(),4,':');
		dec   = atoi(TSYS::strSepParse(cfg.toStdString(),5,':').c_str());
	    }
	    ((QDoubleSpinBox*)ed_fld)->setRange(minv,maxv);
	    ((QDoubleSpinBox*)ed_fld)->setSingleStep(sstep);
	    ((QDoubleSpinBox*)ed_fld)->setPrefix(pref.c_str());
	    ((QDoubleSpinBox*)ed_fld)->setSuffix(suff.c_str());
	    ((QDoubleSpinBox*)ed_fld)->setDecimals(dec);
	    break;
	}
	case Time: case Date: case DateTime:
	    ((QDateTimeEdit*)ed_fld)->setDisplayFormat(cfg);
	    break;
	case Combo:
	{
	    QString ctext = ((QComboBox*)ed_fld)->currentText();
	    ((QComboBox*)ed_fld)->clear();
	    ((QComboBox*)ed_fld)->addItems(cfg.split("\n"));
	    if( ((QComboBox*)ed_fld)->findText(ctext) < 0 ) ((QComboBox*)ed_fld)->addItem(ctext);
	    ((QComboBox*)ed_fld)->setEditText(ctext);
	    break;
	}
    }
    if(bt_fld) viewApplyBt(false);
    if(ed_fld) ed_fld->blockSignals(false);
}

QString LineEdit::value()
{
    switch(type())
    {
	case Text:	return ((QLineEdit*)ed_fld)->text();
	case Integer:	return QString::number(((QSpinBox*)ed_fld)->value());
	case Real:	return QString::number(((QDoubleSpinBox*)ed_fld)->value());
	case Time:	return QString::number(QTime().secsTo(((QTimeEdit*)ed_fld)->time()));
	case Date: case DateTime:
	    return QString::number(((QDateTimeEdit*)ed_fld)->dateTime().toTime_t());
	case Combo:	return ((QComboBox*)ed_fld)->currentText();
    }
    return "";
}

void LineEdit::applySlot( )
{
    viewApplyBt(false);

    if(m_val == value()) return;
    m_val = value();

    emit valChanged(value());
    emit apply();
}

bool LineEdit::event( QEvent * e )
{
    if(e->type() == QEvent::KeyRelease && bt_fld)
    {
	QKeyEvent *keyEvent = (QKeyEvent*)e;
	if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
	{
	    bt_fld->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape)
	{
	    emit cancel();
	    setValue(m_val);
	    return true;
	}
    }
    return QWidget::event(e);
}

//*************************************************
//* SyntxHighl: Syntax highlighter                *
//*************************************************
SyntxHighl::SyntxHighl(QTextDocument *parent) : QSyntaxHighlighter(parent)
{

}

void SyntxHighl::setSnthHgl(XMLNode nd)
{
    rules = nd;

    //> Set current font settings
    QFont rez;

    char family[101]; strcpy(family,"Arial");
    int size = 10, bold = 0, italic = 0, underline = 0, strike = 0;
    sscanf(nd.attr("font").c_str(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
    rez.setFamily(QString(family).replace(QRegExp("_")," "));
    rez.setPointSize(size);
    rez.setBold(bold);
    rez.setItalic(italic);
    rez.setUnderline(underline);
    rez.setStrikeOut(strike);
    document()->setDefaultFont(rez);

    rehighlight();
}

void SyntxHighl::rule(XMLNode *irl, const QString &text, int off, char lev)
{
    XMLNode *rl;
    vector<int> rul_pos(irl->childSize(),-1);
    int minPos = -1, minRule, endIndex, startBlk, sizeBlk;
    QTextCharFormat kForm;
    QRegExp expr;

    if(lev > 3) return;

    //> Init previous block continue
    int curBlk = (currentBlockState()>>(lev*8))&0xFF;

    //> Stream process by rules
    for(int i_t = 0; i_t < text.length(); )
    {
	if(curBlk && !i_t) { minRule = curBlk-1; minPos = 0; }
	else minRule = -1;

	for(int i_ch = 0; i_t != minPos && i_ch < (int)irl->childSize(); i_ch++)
	{
	    if(!(minPos < i_t || rul_pos[i_ch] < i_t || rul_pos[i_ch] < minPos)) continue;
	    if(rul_pos[i_ch] >= i_t && rul_pos[i_ch] < minPos)	{ minPos = rul_pos[i_ch]; minRule = i_ch; continue; }
	    if(rul_pos[i_ch] == i_t && rul_pos[i_ch] == minPos)	{ minRule = i_ch; break; }

	    //> Call rule
	    rl = irl->childGet(i_ch);
	    if(rl->name() == "rule")	expr.setPattern(rl->attr("expr").c_str());
	    else if(rl->name() == "blk")expr.setPattern(rl->attr("beg").c_str());
	    else continue;
	    expr.setMinimal(atoi(rl->attr("min").c_str()));
	    rul_pos[i_ch] = expr.indexIn(text,i_t);
	    if(expr.matchedLength() <= 0) continue;
	    if(rul_pos[i_ch] < 0) rul_pos[i_ch] = text.length();
	    if(minPos < i_t || rul_pos[i_ch] < minPos) { minPos = rul_pos[i_ch]; minRule = i_ch; }
	}
	if(minRule < 0)	break;

	//> Process minimal rule
	rl = irl->childGet(minRule);
	kForm.setForeground(QColor(rl->attr("color").c_str()));
	kForm.setFontWeight(atoi(rl->attr("font_weight").c_str()) ? QFont::Bold : QFont::Normal);
	kForm.setFontItalic(atoi(rl->attr("font_italic").c_str()));

	if(rl->name() == "rule")
	{
	    expr.setPattern(rl->attr("expr").c_str());
	    expr.setMinimal(atoi(rl->attr("min").c_str()));
	    if(expr.indexIn(text,i_t) != rul_pos[minRule]) break;
	    setFormat(rul_pos[minRule]+off, expr.matchedLength(), kForm);
	    //> Call include rules
	    if(rl->childSize()) rule(rl, text.mid(rul_pos[minRule],expr.matchedLength()), rul_pos[minRule]+off, lev+1);
	    i_t = rul_pos[minRule]+expr.matchedLength();
	}
	else if(rl->name() == "blk")
	{
	    if(curBlk) rul_pos[minRule] = curBlk = startBlk = 0;
	    else
	    {
		expr.setPattern(rl->attr("beg").c_str());
		expr.setMinimal(atoi(rl->attr("min").c_str()));
		if(expr.indexIn(text,i_t) != rul_pos[minRule]) break;
		startBlk = rul_pos[minRule]+expr.matchedLength();
	    }
	    QRegExp eExpr(rl->attr("end").c_str());
	    eExpr.setMinimal(atoi(rl->attr("min").c_str()));
	    endIndex = eExpr.indexIn(text, startBlk);
	    if(endIndex == -1 || eExpr.matchedLength() <= 0)
	    {
		setFormat(rul_pos[minRule]+off, (text.length()-rul_pos[minRule]), kForm);
		sizeBlk = text.length()-startBlk;
		i_t = text.length();
	    }
	    else
	    {
		setFormat(rul_pos[minRule]+off, (endIndex-rul_pos[minRule]+eExpr.matchedLength()), kForm);
		sizeBlk = endIndex-startBlk;
		i_t = endIndex + eExpr.matchedLength();
	    }
	    //> Call include rules
	    if(rl->childSize()) rule(rl, text.mid(startBlk,sizeBlk), startBlk+off, lev+1);
	    if(endIndex == -1 || eExpr.matchedLength() <= 0)
		setCurrentBlockState(((minRule+1)<<(lev*8))|currentBlockState());
	    else setCurrentBlockState(currentBlockState()& ~(0xFFFFFFFF<<(lev*8)));
	}
    }
}

void SyntxHighl::highlightBlock(const QString &text)
{
    setCurrentBlockState((previousBlockState()<0)?0:previousBlockState());
    rule(&rules,text);
}

//*************************************************
//* TextEdit: Text edit widget.                   *
//*************************************************
TextEdit::TextEdit( QWidget *parent, const char *name, bool prev_dis ) :
    QWidget(parent), isInit(false), snt_hgl(NULL), but_box(NULL)
{
    setObjectName(name);
    QVBoxLayout *box = new QVBoxLayout(this);
    box->setContentsMargins(0,0,0,2);
    box->setSpacing(0);

    ed_fld = new QTextEdit(this);
#if QT_VERSION < 0x050000
    ed_fld->setStyle(new QPlastiqueStyle());	//> Force style set for resize allow everywhere
#else
    ed_fld->setStyle(new QCommonStyle());	//> Force style set for resize allow everywhere
#endif
    ed_fld->setContextMenuPolicy(Qt::CustomContextMenu);
    ed_fld->setTabStopWidth(20);
    ed_fld->setAcceptRichText(false);
    connect(ed_fld, SIGNAL(textChanged()), this, SLOT(changed()));
    connect(ed_fld, SIGNAL(cursorPositionChanged()), this, SLOT(curPosChange()));
    connect(ed_fld, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ctrTreePopup()));
    box->addWidget(ed_fld);

    QImage ico_t;
    if(!ico_t.load(TUIS::icoGet("find",NULL,true).c_str())) ico_t.load(":/images/find.png");
    actFind = new QAction(QPixmap::fromImage(ico_t), _("Find"), ed_fld);
    actFind->setShortcut(Qt::CTRL+Qt::Key_F);
    actFind->setShortcutContext(Qt::WidgetShortcut);
    connect(actFind, SIGNAL(triggered()), this, SLOT(find()));
    ed_fld->addAction(actFind);
    actFindNext = new QAction(_("Find next"), ed_fld);
    actFindNext->setShortcut(Qt::Key_F3);
    actFindNext->setShortcutContext(Qt::WidgetShortcut);
    connect(actFindNext, SIGNAL(triggered()), this, SLOT(find()));
    ed_fld->addAction(actFindNext);

    if(!prev_dis)
    {
	but_box = new QDialogButtonBox(QDialogButtonBox::Apply|QDialogButtonBox::Cancel, Qt::Horizontal, this);
	QImage ico_t;
	but_box->button(QDialogButtonBox::Apply)->setText(_("Apply"));
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	but_box->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
	connect(but_box->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(btApply()));
	but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	connect(but_box, SIGNAL(rejected()), this, SLOT(btCancel()));
	but_box->setVisible(false);
	box->addWidget(but_box);
    }
}

bool TextEdit::isChanged( )	{ return (but_box && but_box->isVisible()); }

QString TextEdit::text( )	{ return ed_fld->toPlainText(); }

bool TextEdit::hasFocus( ) const{ return ed_fld->hasFocus(); }

void TextEdit::setText( const QString &text )
{
    isInit = true;
    if(text != ed_fld->toPlainText()) ed_fld->setPlainText(text);
    ed_fld->document()->setModified(false);
    isInit = false;
    changed();
}

void TextEdit::setSnthHgl( XMLNode nd )
{
    int scrollPos = ed_fld->verticalScrollBar()->value();
    if(!snt_hgl) snt_hgl = new SyntxHighl(ed_fld->document());
    snt_hgl->setSnthHgl(nd);
    ed_fld->verticalScrollBar()->setValue(scrollPos);
}

void TextEdit::changed( )
{
    if(isInit) return;
    if(but_box) but_box->setVisible(ed_fld->document()->isModified());
    if(ed_fld->document()->isModified()) emit textChanged(text());
}

void TextEdit::btApply( )
{
    emit textChanged(text());
    but_box->setVisible(false);
    emit apply();
}

void TextEdit::btCancel( )
{
    but_box->setVisible(false);
    emit cancel();
}

void TextEdit::curPosChange( )
{
    ((QMainWindow*)window())->statusBar()->showMessage(QString(_("Cursor = (%1:%2)")).arg(ed_fld->textCursor().blockNumber()+1).arg(ed_fld->textCursor().columnNumber()+1),10000);
}

bool TextEdit::event( QEvent *e )
{
    if(but_box && e->type() == QEvent::KeyRelease)
    {
	QKeyEvent *keyEvent = (QKeyEvent*)e;
	if((keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) && QApplication::keyboardModifiers()&Qt::ControlModifier)
	{
	    but_box->button(QDialogButtonBox::Apply)->animateClick();
	    return true;
	}
	else if(keyEvent->key() == Qt::Key_Escape)
	{
	    but_box->button(QDialogButtonBox::Cancel)->animateClick();
	    return true;
	}
    }
    else if(e->type() == QEvent::MouseButtonPress) holdPnt = mapFromGlobal(cursor().pos());
    else if(e->type() == QEvent::MouseMove)
    {
	QPoint curp = mapFromGlobal(cursor().pos());
	int hg = vmax(50,edit()->size().height()+(curp-holdPnt).y());
	edit()->setMinimumHeight(hg); edit()->setMaximumHeight(hg);
	holdPnt = curp;
    }

    return QWidget::event(e);
}

void TextEdit::ctrTreePopup( )
{
    QMenu *menu = ed_fld->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(actFind);
    menu->addAction(actFindNext);
    menu->exec(QCursor::pos());
    delete menu;
}

void TextEdit::find( )
{
    bool isFind = false;
    int fopt = (QTextDocument::FindFlag)actFind->objectName().section(':',0,0).toInt();
    QString fstr = actFind->objectName().section(':',1);
    if(sender() == actFind)
    {
	InputDlg dlg(this,actFind->icon(),QString(_("Enter text string for search:")),_("String search"),0,0);
	QLineEdit *le = new QLineEdit(fstr,&dlg);
	dlg.ed_lay->addWidget(le, 0, 0);
	QCheckBox *bw = new QCheckBox(_("Backward"),&dlg);
	if(fopt & QTextDocument::FindBackward) bw->setCheckState(Qt::Checked);
	dlg.ed_lay->addWidget(bw, 1, 0);
	QCheckBox *cs = new QCheckBox(_("Case sensitively"),&dlg);
	if(fopt & QTextDocument::FindCaseSensitively) cs->setCheckState(Qt::Checked);
	dlg.ed_lay->addWidget(cs, 2, 0);
	QCheckBox *ww = new QCheckBox(_("Whole words"),&dlg);
	if(fopt & QTextDocument::FindWholeWords) ww->setCheckState(Qt::Checked);
	dlg.ed_lay->addWidget(ww, 3, 0);
	le->setFocus(Qt::OtherFocusReason);
	dlg.resize(400,210);
	if(dlg.exec() == QDialog::Accepted && !le->text().isEmpty())
	{
	    fopt = (QTextDocument::FindFlag)0;
	    if(bw->checkState() == Qt::Checked) fopt |= QTextDocument::FindBackward;
	    if(cs->checkState() == Qt::Checked) fopt |= QTextDocument::FindCaseSensitively;
	    if(ww->checkState() == Qt::Checked) fopt |= QTextDocument::FindWholeWords;
	    fstr = le->text();
	    isFind = true;
	}
    }
    else if(sender() == actFindNext && !fstr.isEmpty()) isFind = true;

    if(isFind)
    {
	ed_fld->find(fstr,(QTextDocument::FindFlag)fopt);
	actFind->setObjectName(QString::number(fopt)+":"+fstr);
    }
}

//************************************************
//* CfgTable: Table view widget                  *
//************************************************
CfgTable::CfgTable( QWidget *parent ) : QTableWidget(parent)
{
#if QT_VERSION < 0x050000
    setStyle(new QPlastiqueStyle());	//> Force style set for resize allow everywhere
#else
    setStyle(new QCommonStyle());	//> Force style set for resize allow everywhere
#endif
}

bool CfgTable::event( QEvent *e )
{
    if(e->type() == QEvent::MouseButtonPress)
	holdPnt = mapFromGlobal(cursor().pos());
    else if(e->type() == QEvent::MouseMove)
    {
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
    setMaximumSize(800,600);
    setWindowTitle(ndlg);
    setWindowIcon(icon);
    setSizeGripEnabled(true);

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    //- Icon label and text message -
    QHBoxLayout *intr_lay = new QHBoxLayout;
    intr_lay->setSpacing(6);

    QLabel *icon_lab = new QLabel(this);
    icon_lab->setSizePolicy( QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum) );
    icon_lab->setPixmap(icon.pixmap(48));
    intr_lay->addWidget(icon_lab);

    inpLab = new QLabel(mess,this);
    inpLab->setWordWrap(true);
    intr_lay->addWidget(inpLab);
    dlg_lay->addItem(intr_lay);

    //- Id and name fields -
    ed_lay = new QGridLayout;
    if( with_nm || with_id )
    {
	ed_lay->setSpacing(6);
	if( with_id )
	{
	    mIdLab = new QLabel(_("ID:"),this);
	    ed_lay->addWidget( mIdLab, 3, 0 );
	    mId = new QLineEdit(this);
	    mId->setMaxLength(with_id);
	    ed_lay->addWidget( mId, 3, 1 );
	}
	if( with_nm )
	{
	    mNameLab = new QLabel(_("Name:"),this);
	    ed_lay->addWidget( mNameLab, 4, 0 );
	    mName = new QLineEdit(this);
	    mName->setMaxLength(with_nm);
	    ed_lay->addWidget( mName, 4, 1 );
	}
    }

    ed_lay->addItem(new QSpacerItem(10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding),100,0,1,-1);

    dlg_lay->addItem(ed_lay);

    //- Qk and Cancel buttons -
    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox( buttons, Qt::Horizontal, this );
    QImage ico_t;
    if( buttons & QDialogButtonBox::Ok )
    {
	but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
	if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
	but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
	connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    }
    if( buttons & QDialogButtonBox::Cancel )
    {
	but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
	if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
	but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
	connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    }
    dlg_lay->addWidget( but_box );

    resize(400,150+(35*(with_nm?1:0))+(35*(with_id?1:0)));
}

QString InputDlg::id()
{
    if( mId )  return mId->text();
    return "";
}

QString InputDlg::name()
{
    if( mName )return mName->text();
    return "";
}

QString InputDlg::mess( )
{
    return inpLab->text();
}

void InputDlg::setId(const QString &val)
{
    if( mId )  mId->setText(val);
}

void InputDlg::setName(const QString &val)
{
    if( mName ) mName->setText(val);
}

void InputDlg::setMess( const QString &val )
{
    inpLab->setText( val );
}

void InputDlg::showEvent( QShowEvent * event )
{
    QSize src = size();
    adjustSize();
    resize(size().expandedTo(src));
}

//*****************************************************
//* ReqIdNameDlg: Request node identifier and/or name *
//*****************************************************
ReqIdNameDlg::ReqIdNameDlg( QWidget *parent, const QIcon &icon, const QString &mess, const QString &ndlg ) :
    InputDlg(parent, icon, mess, ndlg , 20, 500)
{
    itTpLab = new QLabel(_("Item type:"),this);
    ed_lay->addWidget(itTpLab, 0, 0);
    itTp = new QComboBox(this);
    itTp->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    ed_lay->addWidget(itTp, 0, 1);
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
    for(unsigned i_t = 0; i_t < tgs.size(); i_t++)
    {
	itTp->addItem(TSYS::strSepParse(tgs[i_t],3,'\n').c_str(), tgs[i_t].c_str());
	if(atoi(TSYS::strSepParse(tgs[i_t],4,'\n').c_str())) defPos = itTp->count()-1;
    }
    if(tgs.size()) itTp->setCurrentIndex(defPos);
    bool tpView = !(itTp->count()==1 && itTp->itemText(0).isEmpty());
    itTpLab->setVisible(tpView); itTp->setVisible(tpView);
    itTp->setEnabled(itTp->count()>1);
}

void ReqIdNameDlg::selectItTp( int it )
{
    if(it < 0) return;
    string its = itTp->itemData(it).toString().toStdString();
    int idSz = atoi(TSYS::strSepParse(its,0,'\n').c_str());
    if(idSz > 0) mId->setMaxLength(idSz);
    mIdLab->setVisible(idSz>=0); mId->setVisible(idSz>=0);
    int idm = atoi(TSYS::strSepParse(its,1,'\n').c_str());	//Default idm is boolean for id-mode, enable name
    if(idm > 1)	mName->setMaxLength(idm);
    mNameLab->setVisible(idm); mName->setVisible(idm);
}

//*************************************************
//* DlgUser: User select dialog                   *
//*************************************************
DlgUser::DlgUser( QWidget *parent ) : QDialog(parent)
{
    setWindowTitle(_("Select user"));

    QVBoxLayout *dlg_lay = new QVBoxLayout(this);
    dlg_lay->setMargin(10);
    dlg_lay->setSpacing(6);

    QGridLayout *ed_lay = new QGridLayout;
    ed_lay->setSpacing(6);
    ed_lay->addWidget( new QLabel(_("User:"),this), 0, 0 );
    users = new QComboBox(this);
    ed_lay->addWidget( users, 0, 1 );
    ed_lay->addWidget( new QLabel(_("Password:"),this), 1, 0 );
    passwd = new QLineEdit(this);
    passwd->setEchoMode( QLineEdit::Password );
    ed_lay->addWidget( passwd, 1, 1 );
    dlg_lay->addItem(ed_lay);

    dlg_lay->addItem( new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ) );

    QFrame *sep = new QFrame(this);
    sep->setFrameShape( QFrame::HLine );
    sep->setFrameShadow( QFrame::Raised );
    dlg_lay->addWidget( sep );

    QDialogButtonBox *but_box = new QDialogButtonBox( QDialogButtonBox::Ok|
						      QDialogButtonBox::Cancel, Qt::Horizontal, this );
    QImage ico_t;
    but_box->button(QDialogButtonBox::Ok)->setText(_("Ok"));
    if(!ico_t.load(TUIS::icoGet("button_ok",NULL,true).c_str())) ico_t.load(":/images/button_ok.png");
    but_box->button(QDialogButtonBox::Ok)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(accepted()), this, SLOT(accept()));
    but_box->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoGet("button_cancel",NULL,true).c_str())) ico_t.load(":/images/button_cancel.png");
    but_box->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    connect(but_box, SIGNAL(rejected()), this, SLOT(reject()));
    dlg_lay->addWidget( but_box );

    connect(this, SIGNAL(finished(int)), this, SLOT(finish(int)));

    //- Fill users list -
    vector<string> u_list;
    SYS->security().at().usrList(u_list);
    for(unsigned i_l = 0; i_l < u_list.size(); i_l++)
    {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(u_list[i_l]).at().picture(),TSYS::base64);
	QImage img;
	if(img.loadFromData((const uchar*)simg.c_str(),simg.size()))
	    users->addItem(QPixmap::fromImage(img),u_list[i_l].c_str());
	else users->addItem(u_list[i_l].c_str());
    }
}

QString DlgUser::user()
{
    return users->currentText();
}

QString DlgUser::password()
{
    return passwd->text();
}

void DlgUser::finish( int result )
{
    if( result )
    {
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
}

//*********************************************
//* UserStBar: Status bar user widget.        *
//*********************************************
UserStBar::UserStBar( const QString &iuser, QWidget * parent ) : QLabel(parent)
{
    setUser(iuser);
}

QString UserStBar::user( )
{
    return user_txt;
}

void UserStBar::setUser( const QString &val )
{
    setText(QString("<font color='%1'>%2</font>").arg((val=="root")?"red":"green").arg(val));
    user_txt = val;
}

bool UserStBar::event( QEvent *event )
{
    if( event->type() == QEvent::MouseButtonDblClick )  userSel();
    return QLabel::event( event );
}

bool UserStBar::userSel( )
{
    DlgUser d_usr(parentWidget());
    int rez = d_usr.exec();
    if( rez == DlgUser::SelOK && d_usr.user() != user() )
    {
	setUser( d_usr.user() );
	emit userChanged();
	return true;
    }
    else if( rez == DlgUser::SelErr )
        mod->postMess(mod->nodePath().c_str(),_("Auth is wrong!!!"),TUIMod::Warning,this);

    return false;
}

//*************************************************
//* TableDelegate: Combobox table delegate.       *
//*************************************************
TableDelegate::TableDelegate(QObject *parent) : QItemDelegate(parent)
{

}

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    drawFocus(painter,option,option.rect.adjusted(+1,+1,-1,-1));

    QVariant value = index.data(Qt::DisplayRole);
    switch(value.type())
    {
	case QVariant::Bool:
	    //painter->save();
	    if(value.toBool())
	    {
		QImage img(":/images/ok.png");
		painter->drawImage(option.rect.center().x()-img.width()/2,option.rect.center().y()-img.height()/2,img);
	    }
	    //painter->restore();
	    break;
	default:
	    drawDisplay(painter,option,option.rect,value.toString());
	    break;
    }
}

QWidget *TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::DisplayRole);
    QVariant val_user = index.data(Qt::UserRole);

    if(val_user.isValid()) w_del = new QComboBox(parent);
    else if(value.type() == QVariant::String)
    {
        w_del = new QTextEdit(parent);
        ((QTextEdit*)w_del)->setTabStopWidth(40);
        ((QTextEdit*)w_del)->setLineWrapMode(QTextEdit::NoWrap);
        ((QTextEdit*)w_del)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ((QTextEdit*)w_del)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //((QTextEdit*)w_del)->resize(50,50);
    }
    else
    {
	QItemEditorFactory factory;
	w_del = factory.createEditor(value.type(), parent);
    }
    w_del->installEventFilter(const_cast<TableDelegate*>(this));
    return w_del;
}

void TableDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
    QVariant value = index.data(Qt::DisplayRole);
    QVariant val_user = index.data(Qt::UserRole);

    if(dynamic_cast<QComboBox*>(editor))
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	if(value.type() == QVariant::Bool) comb->setCurrentIndex(value.toBool());
	else if(val_user.isValid())
	{
	    comb->clear();
	    comb->addItems(val_user.toStringList());
	    comb->setCurrentIndex(comb->findText(value.toString()));
	}
    }
    else if(dynamic_cast<QTextEdit*>(editor))	((QTextEdit*)editor)->setPlainText(value.toString());
    else if(dynamic_cast<QLineEdit*>(editor))	((QLineEdit*)editor)->setText(value.toString());
    else QItemDelegate::setEditorData(editor, index);
}

void TableDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    if(dynamic_cast<QComboBox*>(editor))
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	QVariant val_user = index.data(Qt::UserRole);
	if(!val_user.isValid())
	    model->setData(index,(bool)comb->currentIndex(),Qt::EditRole);
	else model->setData(index,comb->currentText(),Qt::EditRole);
    }
    else if(dynamic_cast<QTextEdit*>(editor))	model->setData(index, ((QTextEdit*)editor)->toPlainText(),Qt::EditRole);
    else if(dynamic_cast<QLineEdit*>(editor))	model->setData(index, ((QLineEdit*)editor)->text(),Qt::EditRole);
    else QItemDelegate::setModelData(editor, model, index);
}

void TableDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & ) const
{
    editor->setGeometry(option.rect);
}

bool TableDelegate::eventFilter( QObject *object, QEvent *event )
{
    if( dynamic_cast<QComboBox*>(object) )
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(object);
	if( event->type() == QEvent::KeyRelease )
	    switch( static_cast<QKeyEvent *>(event)->key() )
	    {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		    emit commitData(comb);
		    emit closeEditor(comb, QAbstractItemDelegate::SubmitModelCache);
		    return true;
		case Qt::Key_Escape:
		    emit closeEditor(comb, QAbstractItemDelegate::RevertModelCache);
		    return true;
		default:
		    return false;
	    }
    }
    else if(dynamic_cast<QTextEdit*>(object))
    {
        QTextEdit *ted = dynamic_cast<QTextEdit*>(object);
        if(event->type() == QEvent::KeyPress)
            switch(static_cast<QKeyEvent *>(event)->key())
            {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                    if( QApplication::keyboardModifiers()&Qt::ControlModifier )
                    {
                        emit commitData(ted);
                        emit closeEditor(ted, QAbstractItemDelegate::SubmitModelCache);
                        return true;
                    }
                    else return false;
                case Qt::Key_Escape:
                    emit closeEditor(ted, QAbstractItemDelegate::RevertModelCache);
                    return true;
            }
    }

    return QItemDelegate::eventFilter(object,event);
}

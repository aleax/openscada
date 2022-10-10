
//OpenSCADA module UI.QTStarter file: lib_qtgen.cpp
/***************************************************************************
 *   Copyright (C) 2021-2022 by Roman Savochenko, <roman@oscada.org>       *
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
#include <QKeyEvent>
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QItemEditorFactory>

#include <tsys.h>
#include "lib_qtgen.h"

int OSCADA_QT::icoSize( float mult )	{ return (int)(mult * QFontMetrics(qApp->font()).height()); }

QColor OSCADA_QT::colorAdjToBack( const QColor &clr, const QColor &backClr )
{
    int wV = vmax(60,(256-abs(clr.saturation()-backClr.saturation()))/2);

    int wS = abs(clr.hue()-backClr.hue());
    if(wS > 360/2) wS = 360 - wS;
    wS = vmax(0, (180-wS)/2);

    if(wV > abs(clr.value()-backClr.value()))
	wV = backClr.value() + ((backClr.value() < 175) ?
				    wV*(((backClr.value()+wV)<256)?1:-1) :
				    wV*(((backClr.value()-wV)>0)?-1:1));
    else wV = clr.value();

    if(wS > abs(clr.saturation()-backClr.saturation()))
	wS = backClr.saturation() + wS*(((backClr.saturation()+wV)<256)?1:-1);
    else wS = clr.saturation();

    return QColor::fromHsv(clr.hue(), wS, wV, clr.alpha());
}

QFont OSCADA_QT::getFont( const string &val, float fsc, bool pixSize, const QFont &defFnt )
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

QColor OSCADA_QT::getColor( const string &val )
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

using namespace OSCADA;
using namespace OSCADA_QT;

//*************************************************
//* TableDelegate: Combobox table delegate.       *
//*************************************************
TableDelegate::TableDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

QSize TableDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QRect rect = option.rect;
    const bool wrapText = index.data(Qt::TextAlignmentRole).isValid() ?
		    (index.data(Qt::TextAlignmentRole).toInt()&Qt::TextWordWrap) : true;
	    //option.features & QStyleOptionViewItem::WrapText;
    switch(option.decorationPosition) {
	case QStyleOptionViewItem::Left: case QStyleOptionViewItem::Right:
	    rect.setWidth(wrapText && rect.isValid() ? rect.width() : 1000);
	    break;
	case QStyleOptionViewItem::Top: case QStyleOptionViewItem::Bottom:
	    rect.setWidth(wrapText ? option.decorationSize.width() : 1000);
	    break;
    }

    return textRectangle(0, rect, qvariant_cast<QFont>(index.data(Qt::FontRole)).resolve(option.font),
	TSYS::strEncode(index.data(Qt::DisplayRole).toString().toStdString(),TSYS::Limit,
	    i2s(index.data(TextLimRole).isValid()?index.data(TextLimRole).toInt():LIM_TEXT_DEF)).c_str()).size();
}

void TableDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItem opt = setOptions(index, option);

    drawBackground(painter, opt, index);
    //See the Qt source file "qitemdelegate.cpp" for more details
    //drawCheck(painter, opt, checkRect, checkState);
    //drawDecoration(painter, opt, decorationRect, pixmap);
    painter->setFont(opt.font);
    painter->setPen(opt.palette.brush(QPalette::Text).color());

    QVariant value = index.data(Qt::DisplayRole);
    switch(value.type()) {
	case QVariant::Bool:
	    if(value.toBool()) {
		QImage img = QImage(":/images/button_ok.png").scaled(icoSize(), icoSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		painter->drawImage(opt.rect.center().x()-img.width()/2, opt.rect.center().y()-img.height()/2, img);
	    }
	    break;
	case QVariant::Int: case QVariant::UInt: case QVariant::LongLong: case QVariant::ULongLong:
	case QVariant::Double:
	    painter->drawText(opt.rect, Qt::AlignCenter, value.toString());
	    break;
	default:
	    int drawOpts = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap;
	    if(index.data(Qt::TextAlignmentRole).isValid())
		drawOpts = index.data(Qt::TextAlignmentRole).toInt();

	    QString lim = TSYS::strEncode(value.toString().toStdString(), TSYS::Limit,
		i2s(index.data(TextLimRole).isValid()?index.data(TextLimRole).toInt():LIM_TEXT_DEF)).c_str();
	    painter->drawText(opt.rect, drawOpts, (lim.size() < value.toString().size()) ? lim+"..." : value.toString());
	    //drawDisplay(painter, opt, opt.rect, value.toString());
	    break;
    }

    drawFocus(painter, opt, opt.rect.adjusted(+1,+1,-1,-1));
}

QWidget *TableDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QWidget *wDel;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::DisplayRole);
    QVariant val_user = index.data(SelectRole);

    if(val_user.isValid()) wDel = new QComboBox(parent);
    else if(value.type() == QVariant::String && !index.data(OneLineString).toBool()) {
	wDel = new QTextEdit(parent);
#if QT_VERSION >= 0x050A00
	((QTextEdit*)wDel)->setTabStopDistance(40);
#else
	((QTextEdit*)wDel)->setTabStopWidth(40);
#endif
	((QTextEdit*)wDel)->setLineWrapMode(QTextEdit::NoWrap);
	((QTextEdit*)wDel)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)wDel)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//((QTextEdit*)wDel)->resize(parent->width(), ((QTextEdit*)wDel)->height());
    }
    else if(value.type() == QVariant::Double) wDel = new QLineEdit(parent);
    else {
	QItemEditorFactory factory;
	wDel = factory.createEditor(value.type(), parent);
    }
    wDel->installEventFilter(const_cast<TableDelegate*>(this));

    return wDel;
}

void TableDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
    QVariant value = index.data(Qt::DisplayRole);
    QVariant val_user = index.data(SelectRole);

    if(dynamic_cast<QComboBox*>(editor)) {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	if(value.type() == QVariant::Bool) comb->setCurrentIndex(value.toBool());
	else if(val_user.isValid()) {
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
    if(dynamic_cast<QComboBox*>(editor)) {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	QVariant val_user = index.data(SelectRole);
	if(!val_user.isValid())
	    model->setData(index, (bool)comb->currentIndex(), Qt::EditRole);
	else model->setData(index, comb->currentText(), Qt::EditRole);
    }
    else if(dynamic_cast<QTextEdit*>(editor))	model->setData(index, ((QTextEdit*)editor)->toPlainText(), Qt::EditRole);
    else if(dynamic_cast<QLineEdit*>(editor))
	switch(index.data(Qt::DisplayRole).type()) {
	    case QVariant::Int: case QVariant::UInt: case QVariant::LongLong: case QVariant::ULongLong:
		model->setData(index, ((QLineEdit*)editor)->text().toLongLong(), Qt::EditRole);
		break;
	    case QVariant::Double:
		model->setData(index, s2r(((QLineEdit*)editor)->text().toStdString()) /*((QLineEdit*)editor)->text().toDouble()*/, Qt::EditRole);
		break;
	    default: model->setData(index, ((QLineEdit*)editor)->text(), Qt::EditRole);	break;
	}
    else QItemDelegate::setModelData(editor, model, index);
}

void TableDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & ) const
{
    editor->setGeometry(option.rect);
}

bool TableDelegate::eventFilter( QObject *object, QEvent *event )
{
    if(dynamic_cast<QComboBox*>(object)) {
	QComboBox *comb = dynamic_cast<QComboBox*>(object);
	if(event->type() == QEvent::KeyRelease)
	    switch(static_cast<QKeyEvent *>(event)->key()) {
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
    else if(dynamic_cast<QTextEdit*>(object)) {
	QTextEdit *ted = dynamic_cast<QTextEdit*>(object);
	if(event->type() == QEvent::KeyPress)
	    switch(static_cast<QKeyEvent *>(event)->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		    if(static_cast<QKeyEvent *>(event)->text() == "<REFORWARD>") return false;
		    if(!(QApplication::keyboardModifiers()&Qt::ControlModifier)) {
			emit commitData(ted);
			emit closeEditor(ted, QAbstractItemDelegate::SubmitModelCache);
			return true;
		    }
		    QCoreApplication::postEvent(object,
				    new QKeyEvent(QEvent::KeyPress,static_cast<QKeyEvent *>(event)->key(),Qt::NoModifier,"<REFORWARD>"));
		    return true;
		case Qt::Key_Escape:
		    emit closeEditor(ted, QAbstractItemDelegate::RevertModelCache);
		    return true;
	    }
    }

    return QItemDelegate::eventFilter(object,event);
}

//*************************************************
//* SnthHgl: Syntax highlighter                   *
//*************************************************
SnthHgl::SnthHgl( QTextDocument *parent ) : QSyntaxHighlighter(parent), isBuiltInSH(false)
{

}

void SnthHgl::setSnthHgl( const XMLNode &nd )
{
    rules = nd;

    //Set current font settings
    document()->setDefaultFont(getFont(rules.attr("font"),1,false,document()->defaultFont()));

    rehighlight();
}

bool SnthHgl::checkInSnthHgl( const QString &text, XMLNode &nd )
{
    bool isInSH = false;

    TArrayObj *rezSH = NULL;
    if((rezSH=TRegExp("<SnthHgl\\b.*>.*<\\/ *SnthHgl>","gm").match(text.toStdString()))) {
	if(rezSH->arSize())
	    try {
		nd.load(rezSH->arGet(0).getS());
		isInSH = true;
	    } catch(TError&) { }
	delete rezSH;

    }

    return isInSH;
}

void SnthHgl::rule( XMLNode *irl, const QString &text, int off, char lev )
{
    XMLNode *rl;
    vector<int> rul_pos(irl->childSize(),-1);
    vector<int> matchedLength(irl->childSize(), 0);
    int minPos = -1, minRule, endIndex, startBlk, sizeBlk;
    QTextCharFormat kForm;
    TRegExp expr("", "", TRegExp::MD_WCHAR);	//Richer and faster in 5 times !!!!
    bool useQT = (expr.mode() == TRegExp::MD_8);
    std::wstring textW = text.toStdWString(), patW;
    QRegExp exprQ;
    if(useQT) exprQ.setPatternSyntax(QRegExp::RegExp2);

    if(lev > 3) return;

    //Init previous block continue
    int curBlk = (currentBlockState()>>(lev*8))&0xFF;

    //Stream process by rules
    for(int iT = 0; iT < text.length(); ) {
	if(curBlk && !iT) { minRule = curBlk-1; minPos = 0; }
	else minRule = -1;

	for(int iCh = 0; iT != minPos && iCh < (int)irl->childSize(); iCh++) {
	    if(!(minPos < iT || rul_pos[iCh] < iT || rul_pos[iCh] < minPos)) continue;
	    if(rul_pos[iCh] >= iT && rul_pos[iCh] < minPos)	{ minPos = rul_pos[iCh]; minRule = iCh; continue; }
	    if(rul_pos[iCh] == iT && rul_pos[iCh] == minPos)	{ minRule = iCh; break; }

	    //Call rule
	    rl = irl->childGet(iCh);
	    if(rl->name() == "rule") {
		if(!useQT) {
		    patW = QString(rl->attr("expr").c_str()).toStdWString();
		    expr.setPattern(string((const char*)patW.data(),patW.size()*sizeof(wchar_t)), s2i(rl->attr("min"))?"U":"");
		} else exprQ.setPattern(rl->attr("expr").c_str());
	    }
	    else if(rl->name() == "blk") {
		if(!useQT) {
		    patW = QString(rl->attr("beg").c_str()).toStdWString();
		    expr.setPattern(string((const char*)patW.data(),patW.size()*sizeof(wchar_t)), s2i(rl->attr("min"))?"U":"");
		} else exprQ.setPattern(rl->attr("beg").c_str());
	    }
	    else continue;

	    if(!useQT)
		rul_pos[iCh] = expr.search(string((const char*)textW.data(),textW.size()*sizeof(wchar_t)), iT, &matchedLength[iCh]);
	    else {
		exprQ.setMinimal(s2i(rl->attr("min")));
		rul_pos[iCh] = exprQ.indexIn(text, iT); matchedLength[iCh] = exprQ.matchedLength();
	    }

	    if(matchedLength[iCh] <= 0) continue;

	    if(rul_pos[iCh] < 0) rul_pos[iCh] = text.length();
	    if(minPos < iT || rul_pos[iCh] < minPos) { minPos = rul_pos[iCh]; minRule = iCh; }
	}
	if(minRule < 0) break;

	//Process minimal rule
	rl = irl->childGet(minRule);
	kForm.setForeground(colorAdjToBack(rl->attr("color").c_str(),qApp->palette().color(QPalette::Base)));
	kForm.setFontWeight(s2i(rl->attr("font_weight")) ? QFont::Bold : QFont::Normal);
	kForm.setFontItalic(s2i(rl->attr("font_italic")));

	if(rl->name() == "rule") {
	    setFormat(rul_pos[minRule]+off, matchedLength[minRule], kForm);

	    //Call include rules
	    if(rl->childSize()) rule(rl, text.mid(rul_pos[minRule],matchedLength[minRule]), rul_pos[minRule]+off, lev+1);
	    iT = rul_pos[minRule] + matchedLength[minRule];
	}
	else if(rl->name() == "blk") {
	    if(curBlk) rul_pos[minRule] = curBlk = startBlk = 0;
	    else startBlk = rul_pos[minRule] + matchedLength[minRule];

	    if(!useQT) {
		patW = QString(rl->attr("end").c_str()).toStdWString();
		expr.setPattern(string((const char*)patW.data(),patW.size()*sizeof(wchar_t)), s2i(rl->attr("min"))?"U":"");
		endIndex = expr.search(string((const char*)textW.data(),textW.size()*sizeof(wchar_t)), startBlk, &matchedLength[minRule]);
	    }
	    else {
		exprQ.setPattern(rl->attr("end").c_str());
		exprQ.setMinimal(s2i(rl->attr("min")));
		endIndex = exprQ.indexIn(text, startBlk); matchedLength[minRule] = exprQ.matchedLength();
	    }

	    if(endIndex == -1 || matchedLength[minRule] <= 0) {
		setFormat(rul_pos[minRule]+off, (text.length()-rul_pos[minRule]), kForm);
		sizeBlk = text.length()-startBlk;
		iT = text.length();
	    }
	    else {
		setFormat(rul_pos[minRule]+off, (endIndex-rul_pos[minRule]+matchedLength[minRule]), kForm);
		sizeBlk = endIndex-startBlk;
		iT = endIndex + matchedLength[minRule];
	    }
	    //Call include rules
	    if(rl->childSize()) rule(rl, text.mid(startBlk,sizeBlk), startBlk+off, lev+1);
	    if(endIndex == -1 || matchedLength[minRule] <= 0)
		setCurrentBlockState(((minRule+1)<<(lev*8))|currentBlockState());
	    else setCurrentBlockState(currentBlockState()& ~(0xFFFFFFFF<<(lev*8)));
	}
    }
}

void SnthHgl::highlightBlock( const QString &text )
{
    setCurrentBlockState((previousBlockState()<0)?0:previousBlockState());
    rule(&rules, text);
}

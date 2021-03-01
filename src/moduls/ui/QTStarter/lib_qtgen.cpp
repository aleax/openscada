
//OpenSCADA module UI.QTStarter file: lib_qtgen.cpp
/***************************************************************************
 *   Copyright (C) 2021 by Roman Savochenko, <roman@oscada.org>            *
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
    //printf("TEST 00: %s on %s\n", clr.name().toStdString().c_str(), backClr.name().toStdString().c_str());

    int wV = vmax(60,(256-abs(clr.saturation()-backClr.saturation()))/2);

    int wS = abs(clr.hue()-backClr.hue());
    if(wS > 360/2) wS = 360 - wS;
    wS = vmax(0, (180-wS)/2);

    //printf("TEST 01: wV=%d; wS=%d\n", wV, wS);

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
    QWidget *w_del;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::DisplayRole);
    QVariant val_user = index.data(SelectRole);

    if(val_user.isValid()) w_del = new QComboBox(parent);
    else if(value.type() == QVariant::String) {
	w_del = new QTextEdit(parent);
	((QTextEdit*)w_del)->setTabStopWidth(40);
	((QTextEdit*)w_del)->setLineWrapMode(QTextEdit::NoWrap);
	((QTextEdit*)w_del)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//((QTextEdit*)w_del)->resize(parent->width(), ((QTextEdit*)w_del)->height());
    }
    else {
	QItemEditorFactory factory;
	w_del = factory.createEditor(value.type(), parent);
    }
    w_del->installEventFilter(const_cast<TableDelegate*>(this));

    return w_del;
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
    else if(dynamic_cast<QLineEdit*>(editor))	model->setData(index, ((QLineEdit*)editor)->text(), Qt::EditRole);
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
		    if(QApplication::keyboardModifiers()&Qt::ControlModifier) {
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


//OpenSCADA system module UI.Vision file: vis_devel_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2006-2008 by Roman Savochenko                           *
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

#include <math.h>

#include <QApplication>
#include <QHeaderView>
#include <QTreeWidget>
#include <QItemEditorFactory>
#include <QMenu>
#include <QEvent>
#include <QKeyEvent>
#include <QTextEdit>
#include <QComboBox>
#include <QWorkspace>
#include <QToolBar>
#include <QPainter>
#include <QStatusBar>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QBuffer>
#include <QDateTimeEdit>
#include <QTimer>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QClipboard>

#include <tsys.h>

#include "vis_devel.h"
#include "tvision.h"
#include "vis_shapes.h"
#include "vis_widgs.h"
#include "vis_devel_widgs.h"

using namespace VISION;


//****************************************
//* Inspector of attributes model        *
//****************************************
ModInspAttr::ModInspAttr( const string &iwdg, VisDevelop *mainWind ) : main_win(mainWind)
{
    rootItem = new Item("wgrp",Item::WdgGrp);
    setWdg(iwdg);
}

ModInspAttr::~ModInspAttr( )
{
    //> Delete root item
    beginRemoveRows(QModelIndex(),0,rootItem->childCount());
    delete rootItem;
    endRemoveRows();
}

string ModInspAttr::user( )
{
    return mainWin( )->user();
}

void ModInspAttr::setWdg( const string &iwdg )
{
    bool full_reset = false;
    string sval;
    vector<string> wdg_ls;

    bool isChange = (cur_wdg != iwdg);
    cur_wdg = iwdg;

    //> Get widgets list
    for( int v_off = 0; (sval=TSYS::strSepParse(iwdg,0,';',&v_off)).size(); ) wdg_ls.push_back(sval);

    if( wdg_ls.size() == 0 )
    {
	delete rootItem;
	rootItem = new Item("",Item::Wdg);
	full_reset = true;
    }
    else if( wdg_ls.size() == 1 )
    {
	//> Set one widget. Check for change root item
	if( rootItem->type() != Item::Wdg || rootItem->id() != wdg_ls[0] )
	{
	    delete rootItem;
	    rootItem = new Item(wdg_ls[0],Item::Wdg);
	    full_reset = true;
	}
	//>> Update attributes
	wdgAttrUpdate( QModelIndex() );
    }
    else if( wdg_ls.size() > 1 )
    {
	//> Set group widget
	if( rootItem->type() != Item::WdgGrp )
	{
	    beginRemoveRows(QModelIndex(),0,rootItem->childCount());
	    delete rootItem;
	    rootItem = new Item("wgrp",Item::WdgGrp);
	    endRemoveRows();
	}

	//> Check for delete widgets from group
	bool masterWdg = !isChange && (rootItem->childCount() && rootItem->child(0)->id() == "<*>");
	for( int i_it = (masterWdg?1:0); i_it < rootItem->childCount(); i_it++ )
	{
	    int i_w;
	    for( i_w = 0; i_w < wdg_ls.size(); i_w++ )
		if( rootItem->child(i_it)->id() == wdg_ls[i_w] )
		    break;
	    if( i_w >= wdg_ls.size() )
	    {
		beginRemoveRows(QModelIndex(),i_it,i_it);
		rootItem->childDel(i_it);
		endRemoveRows();
		i_it--;
	    }
	}

	//> Check for add master widget
	if( !masterWdg )
	{
	    beginInsertRows(QModelIndex(),0,0);
	    rootItem->childInsert("<*>",0,Item::Wdg);
	    rootItem->child(0)->setName(_("<Group>"));
	    endInsertRows();
	}

	//> Add new items and update attributes
	for( int i_w = 0; i_w < wdg_ls.size(); i_w++ )
	{
	    int row = rootItem->childGet(wdg_ls[i_w]);
	    if( row < 0 )
	    {
		beginInsertRows(QModelIndex(),i_w+1,i_w+1);
		row = rootItem->childInsert(wdg_ls[i_w],i_w+1,Item::Wdg);
		endInsertRows();
	    }
	    wdgAttrUpdate( index(i_w+1,0,QModelIndex()), index(0,0,QModelIndex()) );
	}
    }

    if( full_reset )	reset();
    else emit layoutChanged();
}

void ModInspAttr::wdgAttrUpdate( const QModelIndex &mod_it, const QModelIndex &grp_it )
{
    vector<int> idst;
    bool grpW = false;
    Item *it = mod_it.isValid() ? static_cast<Item*>(mod_it.internalPointer()) : rootItem;
    if( it->type() != Item::Wdg )   return;
    QModelIndex curmod = mod_it;
    string itId = it->id();

    try
    {
	XMLNode info_req("info");
	XMLNode req("get");

	//> Set/update widget name
	req.setAttr("path",itId+"/"+TSYS::strEncode("/wdg/cfg/name",TSYS::PathEl));
	if( !mainWin()->cntrIfCmd(req) )	it->setName(req.text().c_str());

	info_req.setAttr("path",itId+"/%2fattr" );
	mainWin()->cntrIfCmd(info_req);
	XMLNode *root = info_req.childGet(0);

	repIt:
	//> Delete items of a no present attributes
	idst.clear();
	idst.push_back(0);
	int it_lev = 0;
	Item *curit = it;

	//>> Get next item
	while( idst[it_lev] < curit->childCount() )
	{
	    //>> Process next attribute
	    if( curit->child(idst[it_lev])->type( ) == Item::Attr )
	    {
		string it_id = curit->child(idst[it_lev])->id();
		//>>> Find into present attributes list
		int i_a;
		for( i_a = 0; i_a < root->childSize(); i_a++ )
		    if( root->childGet(i_a)->attr("id") == it_id )
			break;
		//>>> Remove no present item
		if( i_a >= root->childSize() && (!grpW || !curit->child(idst[it_lev])->setWdgs(itId,true)) )
		{
		    beginRemoveRows(curmod,idst[it_lev],idst[it_lev]);
		    curit->childDel(idst[it_lev]);
		    endRemoveRows();
		    idst[it_lev]--;
		}
	    }
	    //>> Enter to group
	    else if( curit->child(idst[it_lev])->type( ) == Item::AttrGrp )
	    {
		curmod = index(idst[it_lev],0,curmod);
		curit = static_cast<Item*>(curmod.internalPointer());

		idst.push_back(0);
		it_lev++;
		continue;
	    }

	    //>> Up to level
	    idst[it_lev]++;
	    while( idst[it_lev] >= curit->childCount() )
	    {
		if( it_lev == 0 ) break;

		int prev_tp    = curit->type();
		int prev_chlds = curit->childCount();

		curmod = curmod.parent();
		curit = curmod.isValid() ? static_cast<Item*>(curmod.internalPointer()) : rootItem;
		idst.pop_back();
		it_lev--;

		if( prev_tp == Item::AttrGrp && !prev_chlds )
		{
		    beginRemoveRows(curmod,idst[it_lev],idst[it_lev]);
		    curit->childDel(idst[it_lev]);
		    endRemoveRows();
		}else idst[it_lev]++;
	    }
	    if( idst[it_lev] >= curit->childCount() && it_lev == 0 )	break;
	}

	//> Add items for present attributes
	for( int i_a = 0; i_a < root->childSize(); i_a++ )
	{
	    XMLNode *gnd = root->childGet(i_a);
	    if( grpW && !(atoi(gnd->attr("acs").c_str())&SEC_WR) ) continue;

	    string a_id = gnd->attr("id");
	    string a_nm = gnd->attr("dscr");
	    Item *cur_it = it;
	    //>> Parse attributes group
	    if( TSYS::strSepParse(a_nm,1,':').size() )
		for( int i_l = 0; true; i_l++ )
		{
		    string c_sel = TSYS::strSepParse(a_nm,i_l,':');
		    if( TSYS::strSepParse(a_nm,i_l+1,':').size() )
		    {
			int ga_id = cur_it->childGet(c_sel);
			if( ga_id < 0 ) ga_id = cur_it->childInsert(c_sel,-1,Item::AttrGrp);
			cur_it = cur_it->child(ga_id);
			continue;
		    }
		    else
		    {
			a_nm = c_sel;
			break;
		    }
	    }
	    //>> Check attribute item
	    int ga_id = cur_it->childGet(a_id);
	    if( grpW && ga_id >= 0 ) { cur_it->child(ga_id)->setWdgs(itId); continue; }
	    if( ga_id < 0 ) ga_id = cur_it->childInsert( a_id, -1, Item::Attr );
	    cur_it->child(ga_id)->setName( a_nm );
	    cur_it->child(ga_id)->setEdited( atoi(gnd->attr("acs").c_str())&SEC_WR );
	    cur_it->child(ga_id)->setFlag( atoi(gnd->attr("wdgFlg").c_str()) );
	    cur_it->child(ga_id)->setModify( grpW ? false : atoi(gnd->attr("modif").c_str()) );
	    if( grpW ) cur_it->child(ga_id)->setWdgs(itId);
	    //>> Get Value
	    string sval;
	    req.clear()->setAttr( "path", itId+"/%2fattr%2f"+a_id );
	    if( !mainWin()->cntrIfCmd(req) )	sval = req.text();
	    string stp = gnd->attr("tp");
	    if( stp == "bool" )		cur_it->child(ga_id)->setData((bool)atoi(sval.c_str()));
	    else if( stp == "dec" || stp == "hex" || stp == "oct" )
					cur_it->child(ga_id)->setData(atoi(sval.c_str()));
	    else if( stp == "real" )	cur_it->child(ga_id)->setData(atof(sval.c_str()));
	    else if( stp == "str"  )	cur_it->child(ga_id)->setData(sval.c_str());
	    //>>> Get selected list
	    if( gnd->attr("dest") == "select" )
		cur_it->child(ga_id)->setDataEdit( QString(gnd->attr("sel_list").c_str()).split(";") );
	}

	if( grp_it.isValid() && !grpW )
	{
	    curmod = grp_it;
	    it = static_cast<Item*>(grp_it.internalPointer());
	    grpW = true;
	    goto repIt;
	}
    } catch(...){ }
}

Qt::ItemFlags ModInspAttr::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flg = Qt::ItemIsEnabled;

    if( index.isValid() )
    {
	flg |= Qt::ItemIsSelectable;
	if( index.column() == 1 && static_cast<Item*>(index.internalPointer())->edited() )
	    flg |= Qt::ItemIsEditable;
    }

    return flg;
}

QVariant ModInspAttr::headerData( int section, Qt::Orientation orientation, int role ) const
{
    QVariant val;

    if( role == Qt::DisplayRole )
    {
	if( orientation == Qt::Horizontal )
	    val = (section==0)?_("Attribute"):_("Value");
	else val = section;
    }
    return val;
}

QModelIndex ModInspAttr::index( int row, int column, const QModelIndex &parent ) const
{
    QModelIndex idx;
    Item *it_index;

    if( !parent.isValid() ) it_index = rootItem->child(row);
    else it_index = static_cast<Item*>(parent.internalPointer())->child(row);

    if( it_index )  idx = createIndex(row, column, it_index);

    return idx;
}

QModelIndex ModInspAttr::parent( const QModelIndex &index ) const
{
    QModelIndex idx;
    if( index.isValid() )
    {
	Item *parentItem = static_cast<Item*>(index.internalPointer())->parent();
	if( parentItem && parentItem != rootItem )
	    idx = createIndex( parentItem->parent()->childGet(parentItem->id()), 0, parentItem );
    }

    return idx;
}

int ModInspAttr::rowCount( const QModelIndex &parent ) const
{
    if( !parent.isValid() ) return rootItem->childCount();
    return static_cast<Item*>(parent.internalPointer())->childCount();
}

int ModInspAttr::columnCount( const QModelIndex &parent ) const
{
    return 2;
}

QVariant ModInspAttr::data( const QModelIndex &index, int role ) const
{
    QVariant val;
    if( index.isValid() )
    {
	Item *it = static_cast<Item*>(index.internalPointer());

	if( index.column() == 0 )
	    switch(role)
	    {
		case Qt::DisplayRole:	val = it->name().c_str();	break;
		case Qt::ForegroundRole:
		    if( it->modify() )	val = QBrush(Qt::blue);
		    break;
	    }
	if( index.column() == 1 )
	    switch(role)
	    {
		case Qt::DisplayRole:
		    val = it->data();
		    if( val.type() == QVariant::Int && it->flag()&ModInspAttr::Item::DateTime )
			val = QDateTime::fromTime_t(val.toInt()?val.toInt():time(NULL)).toString("dd.MM.yyyy hh:mm:ss");
		    break;
		case Qt::EditRole:		val = it->dataEdit();	break;
		case Qt::UserRole:		val = it->flag();	break;
		case Qt::DecorationRole:
		    if( it->flag()&ModInspAttr::Item::Color )
		    {
			QPixmap pct(16,16);
			pct.fill("white");
			QPainter painter(&pct);
			QColor clr;
			size_t found = it->data().toString().toStdString().find("-");
			if (found != string::npos)
			{
			    clr = QColor( it->data().toString().toStdString().substr(0,found).c_str() );
			    clr.setAlpha( atoi(it->data().toString().toStdString().substr(found+1).c_str()) );
			}
			else clr = QColor(it->data().toString());

			if( clr.isValid() )
			{
			    painter.fillRect(pct.rect(),QBrush(clr));
			    painter.drawRect(pct.rect().adjusted(0,0,-1,-1));
			    painter.end();
			    val = pct;
			}
		    }
		    else if( it->flag()&ModInspAttr::Item::Font )
		    {
			QPixmap pct(24,24);
			QPainter painter(&pct);
			painter.fillRect(pct.rect(),QBrush(QColor("white")));
			painter.drawRect(pct.rect().adjusted(0,0,-1,-1));
			QFont fnt;
			char family[101]; strcpy(family,"Arial");
			int size = 10, bold = 0, italic = 0, underline = 0, strike = 0;
			sscanf(it->data().toString().toAscii().data(),"%100s %d %d %d %d %d",family,&size,&bold,&italic,&underline,&strike);
			fnt.setStrikeOut(strike);
			fnt.setUnderline(underline);
			fnt.setItalic(italic);
			fnt.setBold(bold);
			fnt.setPixelSize(size);
			fnt.setFamily(string(family,100).c_str());
			painter.setFont(fnt);
			painter.drawText(pct.rect(),Qt::AlignCenter,"Aa");
			painter.end();
			val = pct;
		    }
		    else if( it->flag()&ModInspAttr::Item::Image )
		    {
			Item *tit = it;
			while( tit && tit->type() != ModInspAttr::Item::Wdg )	tit = tit->parent();
			if( tit && tit->type() == ModInspAttr::Item::Wdg )
			{
			    XMLNode req("get");
			    req.setAttr("path",tit->id()+"/%2fwdg%2fres")->setAttr("id",it->data().toString().toAscii().data());
			    req.setText(TSYS::strDecode(req.text(),TSYS::base64));
			    QImage img;
			    if( !const_cast<ModInspAttr*>(this)->mainWin()->cntrIfCmd(req) )
			    {
				req.setText(TSYS::strDecode(req.text(),TSYS::base64));
				if( !req.text().empty() && img.loadFromData((const uchar*)req.text().c_str(),req.text().size()) )
				    val = QPixmap::fromImage(img).scaled(32,32,Qt::KeepAspectRatio,Qt::SmoothTransformation);
			    }
			}
		    }
		    break;
	    }
    }
    return val;
}

bool ModInspAttr::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if( !index.isValid() )  return false;

    //> Attribute
    Item *it = static_cast<Item*>(index.internalPointer());
    if( it->data() == value ) return true;
    string nattr = it->id();

    //> Attribute widget(s)
    string nwdg = it->wdgs( ), swdg;
    if( nwdg.empty() )
    {    
	Item *cit = it;
	while(cit)
	    if(cit->type() == Item::Wdg)
	    {
		nwdg = cit->id();
		break;
	    }
	    else cit = cit->parent();
    }

    try
    {
	string val = (value.type()==QVariant::Bool) ? (value.toBool()?"1":"0") : value.toString().toAscii().data();
	XMLNode req("set");
	for( int off = 0; (swdg=TSYS::strSepParse(nwdg,0,';',&off)).size(); )
	{
	    req.setAttr("path",swdg+"/%2fattr%2f"+nattr)->setText(val);
	    if( !mainWin()->cntrIfCmd(req) && req.text() == val )
	    {
		it->setData( (it->data().type()==QVariant::Bool) ? value.toBool() : value );
		it->setModify(true);
		emit modified(swdg);
		emit dataChanged(index,index);
		if( it->flag()&Item::Active ) setWdg(cur_wdg);
	    }
	}
    }catch(...){ return false; }

    return true;
}

//* Item of the inspector of attributes model  *
ModInspAttr::Item::Item( const string &iid, Type tp, Item *parent ) :
        idItem(iid), parentItem(parent), typeItem(tp), edit_access(false), flag_item(0), mModify(false)
{

}

ModInspAttr::Item::~Item()
{
    clean();
}

string ModInspAttr::Item::name( )
{
    return nameItem.size()?nameItem:id();
}

void ModInspAttr::Item::clean( )
{
    qDeleteAll(childItems);
    childItems.clear();
}

ModInspAttr::Item *ModInspAttr::Item::child(int row) const
{
    return childItems.value(row,NULL);
}

int ModInspAttr::Item::childGet( const string &iid ) const
{
    for( int i_c = 0; i_c < childCount(); i_c++ )
        if( child(i_c)->id() == iid )
            return i_c;

    return -1;
}

int ModInspAttr::Item::childCount() const
{
    return childItems.count();
}

int ModInspAttr::Item::childInsert( const string &iid, int row, Type itp )
{
    if( row<0 || row>childItems.size() )
	row=childItems.size();
    childItems.insert(row, new Item(iid,itp,this));

    return row;
}

void ModInspAttr::Item::childDel( int row )
{
    if( row<0 || row>=childItems.size() )    return;
    delete childItems.value(row);
    childItems.removeAt(row);
}

QVariant ModInspAttr::Item::data( )
{
    if( type() == AttrGrp )
    {
	QString dtv;
	for( int i_c = 0; i_c < childCount(); i_c++ )
	    if( i_c == 0 )  dtv=child(i_c)->data().toString();
	    else dtv=dtv+", "+child(i_c)->data().toString();
	return QString("[%1]").arg(dtv);
    }
    else return dataItem;
}

QVariant ModInspAttr::Item::dataEdit( )
{
    return dataEditItem.isValid()?dataEditItem:dataItem;
}

bool ModInspAttr::Item::setWdgs( const string &w, bool del )
{
    int pos = 0;
    if( !del && wdgsItem.find(w+";") == string::npos ) wdgsItem += w+";";
    if( del && (pos=wdgsItem.find(w+";")) != string::npos ) wdgsItem.replace(pos,w.size()+1,"");

    return !wdgsItem.empty();
}

//****************************************
//* Inspector of attributes widget       *
//****************************************
InspAttr::InspAttr( QWidget * parent, VisDevelop *mainWind ) : QTreeView(parent), modelData("",mainWind)
{
    //setEditTriggers(QAbstractItemView::AllEditTriggers);
    setAlternatingRowColors(true);
    setModel(&modelData);
    setItemDelegate(new ItemDelegate);
    connect(&modelData, SIGNAL(modified(const string &)), this, SIGNAL(modified(const string &)));
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

InspAttr::~InspAttr( )
{

}

bool InspAttr::hasFocus( )
{
    return (QApplication::focusWidget() == this || state() == QAbstractItemView::EditingState);
}

void InspAttr::setWdg( const string &iwdg )
{
    modelData.setWdg(iwdg);
}

bool InspAttr::event( QEvent *event )
{
    if(event->type() == QEvent::KeyPress &&
	    static_cast<QKeyEvent *>(event)->key() == Qt::Key_Space && currentIndex().isValid() )
    {
	QModelIndex ed_id = modelData.index(currentIndex().row(),1,currentIndex().parent());
	if( modelData.flags(ed_id)&Qt::ItemIsEditable )
	{
	    emit setCurrentIndex(ed_id);
	    emit edit(ed_id);
	}
	return true;
    }
    return QTreeView::event( event );
}

void InspAttr::contextMenuEvent( QContextMenuEvent *event )
{
    string nattr, nwdg;
    QAction *actClr, *actCopy;
    actClr = actCopy = NULL;
    ModInspAttr::Item *it = NULL;

    //Attribute
    if( selectedIndexes().size() && selectedIndexes()[0].isValid() )
    {
	it = static_cast<ModInspAttr::Item*>(selectedIndexes()[0].internalPointer());
	nattr = it->id();
	//Attribute widget
	ModInspAttr::Item *cit = it;
	while(cit)
	    if( cit->type() == ModInspAttr::Item::Wdg )
	    {
		nwdg = cit->id();
		break;
	    }
	    else cit = cit->parent();
    }

    QMenu popup;

    //Add actions
    if( it )
    {
	//> Copy action
	QImage ico_t;
	if(!ico_t.load(TUIS::icoPath("editcopy").c_str())) ico_t.load(":/images/editcopy.png");
	actCopy = new QAction(QPixmap::fromImage(ico_t),_("Copy"),this);
	popup.addAction(actCopy);

	//> Changes clear action
	if( it->modify() )
	{
	    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
	    actClr = new QAction(QPixmap::fromImage(ico_t),_("Clear changes"),this);
	    actClr->setStatusTip(_("Press to clear attribute's changes."));
	    popup.addAction(actClr);
	}
    }

    if( !popup.isEmpty() )
    {
	QAction *rez = popup.exec(QCursor::pos());
	if( actCopy && rez == actCopy )
	    QApplication::clipboard()->setText(it->data().toString());
	if( actClr && rez == actClr )
	{
	    modelData.mainWin()->visualItClear(nwdg+"/a_"+nattr);
	    modelData.setWdg(modelData.curWdg());
	}

	popup.clear();
    }
}

//* Attributes item delegate    *
//*******************************
InspAttr::ItemDelegate::ItemDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

QWidget *InspAttr::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();

    if( flag&ModInspAttr::Item::Select )	w_del = new QComboBox(parent);
    else if( value.type() == QVariant::String && flag&ModInspAttr::Item::FullText )
    {
	w_del = new QTextEdit(parent);
	((QTextEdit*)w_del)->setTabStopWidth(40);
	((QTextEdit*)w_del)->setLineWrapMode(QTextEdit::NoWrap);
	((QTextEdit*)w_del)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->resize(50,50);
    }
    else if( value.type() == QVariant::String && flag&ModInspAttr::Item::Font )
	w_del = new LineEditProp(parent,LineEditProp::Font);
    else if( value.type() == QVariant::String && flag&ModInspAttr::Item::Color )
	w_del = new LineEditProp(parent,LineEditProp::Color);
    else if( value.type() == QVariant::Int && flag&ModInspAttr::Item::DateTime )
    {
	w_del = new QDateTimeEdit(parent);
	((QDateTimeEdit*)w_del)->setCalendarPopup(true);
	((QDateTimeEdit*)w_del)->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    }
    else if( value.type() == QVariant::Int )
    {
	w_del = new QSpinBox(parent);
	((QSpinBox*)w_del)->setMinimum(-2147483647);
	((QSpinBox*)w_del)->setMaximum(2147483647);
    }
    else if( value.type() == QVariant::Double )
    {
	w_del = new QDoubleSpinBox(parent);
	((QDoubleSpinBox*)w_del)->setMinimum(-1e100);
	((QDoubleSpinBox*)w_del)->setMaximum(1e100);
    }
    else
    {
	QItemEditorFactory factory;
	w_del = factory.createEditor(value.type(), parent);
    }

    w_del->installEventFilter(const_cast<InspAttr::ItemDelegate*>(this));

    return w_del;
}

void InspAttr::ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();

    if( flag&ModInspAttr::Item::Select && dynamic_cast<QComboBox*>(editor) )
    {
	QComboBox *comb = (QComboBox*)editor;
	comb->addItems(value.toStringList());
	comb->setCurrentIndex(comb->findText(index.data(Qt::DisplayRole).toString()));
    }
    else if( value.type()==QVariant::String && flag&ModInspAttr::Item::FullText && dynamic_cast<QTextEdit*>(editor) )
	((QTextEdit*)editor)->setPlainText(value.toString());
    else if( value.type() == QVariant::String && (flag&ModInspAttr::Item::Font || flag&ModInspAttr::Item::Color) && dynamic_cast<LineEditProp*>(editor) )
	((LineEditProp*)editor)->setValue(value.toString());
    else if( value.type() == QVariant::Int && flag&ModInspAttr::Item::DateTime && dynamic_cast<QDateTimeEdit*>(editor) )
	((QDateTimeEdit*)editor)->setDateTime(QDateTime::fromTime_t(value.toInt()?value.toInt():time(NULL)));
    else QItemDelegate::setEditorData(editor, index);
}

void InspAttr::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();

    if( flag&ModInspAttr::Item::Select && dynamic_cast<QComboBox*>(editor) )
	model->setData(index,((QComboBox*)editor)->currentText(),Qt::EditRole);
    else if( value.type()==QVariant::String && flag&ModInspAttr::Item::FullText && dynamic_cast<QTextEdit*>(editor) )
	model->setData(index,((QTextEdit*)editor)->toPlainText(),Qt::EditRole);
    else if( value.type() == QVariant::String && (flag&ModInspAttr::Item::Font || flag&ModInspAttr::Item::Color) && dynamic_cast<LineEditProp*>(editor) )
	model->setData(index,((LineEditProp*)editor)->value());
    else if( value.type() == QVariant::Int && flag&ModInspAttr::Item::DateTime && dynamic_cast<QDateTimeEdit*>(editor) )
    {
	int tm = ((QDateTimeEdit*)editor)->dateTime().toTime_t();
	model->setData(index,(tm>(time(NULL)+3600))?0:tm,Qt::EditRole);
    }
    else QItemDelegate::setModelData(editor, model, index);
}

QSize InspAttr::ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize w_sz = QItemDelegate::sizeHint(option,index);

    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();
    if( value.type()==QVariant::String && flag&ModInspAttr::Item::FullText )
	return QSize(w_sz.width(),vmin(150,vmax(50,w_sz.height())));
    return QSize(w_sz.width(),vmin(150,w_sz.height()));
}

bool InspAttr::ItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    if(dynamic_cast<QComboBox*>(object))
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(object);
	if(event->type() == QEvent::KeyPress)
	    switch (static_cast<QKeyEvent *>(event)->key())
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

//****************************************
//* Inspector of attributes dock widget  *
//****************************************
InspAttrDock::InspAttrDock( VisDevelop *parent ) : QDockWidget(_("Attributes"),(QWidget*)parent)
{
    setObjectName("InspAttrDock");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    ainsp_w = new InspAttr(this,owner());
    setWidget(ainsp_w);
    connect(ainsp_w, SIGNAL(modified(const string &)), this, SIGNAL(modified(const string &)));
}

InspAttrDock::~InspAttrDock( )
{

}

VisDevelop *InspAttrDock::owner()
{
    return (VisDevelop*)parentWidget();
}

bool InspAttrDock::hasFocus( )
{
    return ainsp_w->hasFocus();
}

void InspAttrDock::setWdg( const string &iwdg )
{
    if( !hasFocus( ) )	ainsp_w->setWdg(iwdg);
}

//****************************************
//* Inspector of links widget            *
//****************************************
InspLnk::InspLnk( QWidget * parent, VisDevelop *mainWind ) : main_win(mainWind), show_init(false)
{
    //setEditTriggers(QAbstractItemView::AllEditTriggers);
    setAlternatingRowColors(true);
    setItemDelegate(new LinkItemDelegate(this));

    QStringList headLabels;
    headLabels << _("Name") << _("Value");
    setHeaderLabels(headLabels);
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeLnk(QTreeWidgetItem*,int)));
}

InspLnk::~InspLnk( )
{

}

string InspLnk::user( )
{
    return mainWin()->user();
}

bool InspLnk::event( QEvent *event )
{
    if( event->type() == QEvent::KeyPress &&
	    static_cast<QKeyEvent *>(event)->key() == Qt::Key_Space &&
	    currentItem() && currentItem()->flags()&Qt::ItemIsEditable )
    {
	editItem(currentItem(),1);
	return true;
    }
    return QTreeWidget::event( event );
}

void InspLnk::contextMenuEvent( QContextMenuEvent *event )
{
    QAction *actCopy = NULL;
    if( !currentItem() ) return;

    QMenu popup;

    //Add actions
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("editcopy").c_str())) ico_t.load(":/images/editcopy.png");
    actCopy = new QAction(QPixmap::fromImage(ico_t),_("Copy"),this);
    popup.addAction(actCopy);

    if( !popup.isEmpty() )
    {
	QAction *rez = popup.exec(QCursor::pos());
	if( actCopy && rez == actCopy )
	    QApplication::clipboard()->setText(currentItem()->data(1,Qt::DisplayRole).toString());

	popup.clear();
    }
}

void InspLnk::setWdg( const string &iwdg )
{
    string lnid, lngrp, lnwdg, lnatr, grpcd;
    map<string,bool> ugrps;

    if( it_wdg != TSYS::strSepParse(iwdg,0,';') )
    {
	clear();
	it_wdg = TSYS::strSepParse(iwdg,0,';');
    }

    show_init = true;
    //Update tree
    XMLNode req("get");

    //> Get links info
    XMLNode info_req("info");
    info_req.setAttr("path",it_wdg+"/%2flinks%2flnk")->setAttr("showAttr","1")->setAttr("inclValue","1");
    if( mainWin()->cntrIfCmd(info_req) ) return;
    XMLNode *rootel = info_req.childGet(0);
    //> Create widget's root items
    for( int i_l = 0; i_l < rootel->childSize(); i_l++ )
    {
	lnid  = rootel->childGet(i_l)->attr("id");
	lngrp = rootel->childGet(i_l)->attr("elGrp");
	lnwdg = TSYS::strSepParse(lnid.substr(3),0,'.');
	lnatr = TSYS::strSepParse(lnid.substr(3),1,'.');
	if( lnatr.empty() )	{ lnatr = lnwdg; lnwdg = "."; }

	//> Search widget item
	QTreeWidgetItem *wdg_it;
	int i_it;
	for( i_it = 0; i_it < topLevelItemCount(); i_it++ )
	    if( lnwdg == topLevelItem(i_it)->text(0).toAscii().data() )
		break;
	if( i_it < topLevelItemCount() ) wdg_it = topLevelItem(i_it);
	else
	{
	    wdg_it = new QTreeWidgetItem(this);
	    wdg_it->setText(0,lnwdg.c_str());
	}

	if( !lngrp.empty() )
	{
	    //>> Search group
	    for( i_it = 0; i_it < wdg_it->childCount(); i_it++ )
		if( lngrp == wdg_it->child(i_it)->text(0).toAscii().data() )
		    break;
	    if( i_it < wdg_it->childCount() ) wdg_it = wdg_it->child(i_it);
	    else
	    {
		wdg_it = new QTreeWidgetItem(wdg_it);
		wdg_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
		wdg_it->setText(0,lngrp.c_str());
		wdg_it->setData(0,Qt::UserRole,QString(lnid.substr(3).c_str()));
	    }
	    //>>> Get group value
	    grpcd = TSYS::addr2str(wdg_it)+lngrp;
	    if( ugrps.find(grpcd) == ugrps.end() )
	    {
		req.clear()->setAttr("path",it_wdg+"/%2flinks%2flnk%2fpr_"+lnid.substr(3));
		if( !mainWin()->cntrIfCmd(req) ) wdg_it->setText(1,req.text().c_str());
		ugrps.insert(std::pair<string,bool>(grpcd,true));
	    }
	}
	//>> Search parameter
	QTreeWidgetItem *prm_it;
	for( i_it = 0; i_it < wdg_it->childCount(); i_it++ )
	    if( lnatr == wdg_it->child(i_it)->text(0).toAscii().data() )
		break;
	if( i_it < wdg_it->childCount() ) prm_it = wdg_it->child(i_it);
	else
	{
	    prm_it = new QTreeWidgetItem(wdg_it);
	    prm_it->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
	    prm_it->setText(0,lnatr.c_str());
	    prm_it->setData(0,Qt::UserRole,QString(lnid.substr(3).c_str()));
	}
	//>>> Get parameter's value
	prm_it->setText(1,rootel->childGet(i_l)->text().c_str());
	//req.clear()->setAttr("path",it_wdg+"/%2flinks%2flnk%2f"+lnid);
	//if( !mainWin()->cntrIfCmd(req) )
	//    prm_it->setText(1,req.text().c_str());
    }

    //> Check for deleted links
    for( int i_it = 0; i_it < topLevelItemCount(); i_it++ )
	for( int i_g = 0, i_a = 0; i_g < topLevelItem(i_it)->childCount(); )
	{
	    QTreeWidgetItem *wdg_g  = topLevelItem(i_it)->child(i_g);
	    QTreeWidgetItem *wdg_it = (wdg_g->childCount())?wdg_g->child(i_a):wdg_g;
	
	    int i_l;
	    for( i_l = 0; i_l < rootel->childSize(); i_l++ )
		if( rootel->childGet(i_l)->attr("id") == ("el_"+wdg_it->data(0,Qt::UserRole).toString()).toAscii().data() &&
			((wdg_g==wdg_it && rootel->childGet(i_l)->attr("elGrp").empty()) || (wdg_g!=wdg_it && rootel->childGet(i_l)->attr("elGrp") == wdg_g->text(0).toAscii().data())) )
			//((bool)rootel->childGet(i_l)->attr("elGrp").size()^(wdg_g==wdg_it)) )
		    break;
	    if( i_l >= rootel->childSize() )
	    {
		delete wdg_it;
		if( wdg_g != wdg_it && !wdg_g->childCount() )
		{
		    delete wdg_g; i_a = 0;
		    if( topLevelItem(i_it)->childCount() )	continue;
		    delete topLevelItem(i_it); i_it--;
		    break;
		}
		if( wdg_g == wdg_it && !topLevelItem(i_it)->childCount() )
		{
		    delete topLevelItem(i_it); i_it--;
		    break;
		}
	    }
	    else
	    {
		if( wdg_g == wdg_it ) i_g++;
		else i_a++;
	    }
	    if( wdg_g != wdg_it && i_a >= wdg_g->childCount() )	{ i_a = 0; i_g++; }
	}

    //> Set widget's path
    if( topLevelItemCount() )	topLevelItem(0)->setData(0,Qt::UserRole,QString(it_wdg.c_str()));

    show_init = false;
}

void InspLnk::changeLnk( QTreeWidgetItem *index, int col )
{
    if( col != 1 || show_init ) return;

    string attr_id = index->data(0,Qt::UserRole).toString().toAscii().data();

    XMLNode req("set");
    req.setAttr("path",it_wdg+"/%2flinks%2flnk%2f"+(index->childCount()?"pr_":"el_")+attr_id)->
	setText(index->text(1).toAscii().data());
    if( mainWin()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,mainWin());
    setWdg(it_wdg);
}

//*******************************
//* Links item delegate         *
//*******************************
LinkItemDelegate::LinkItemDelegate( InspLnk *parent ) : QItemDelegate(parent)
{

}

InspLnk *LinkItemDelegate::owner() const
{
    return (InspLnk*)parent();
}

QWidget *LinkItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid() || !index.parent().isValid() || index.column() == 0) return 0;

    string wdg_it  = index.model()->index(0,0).data(Qt::UserRole).toString().toAscii().data();
    QModelIndex id_it = index.model()->index(index.row(),0,index.parent());
    string attr_id = id_it.data(Qt::UserRole).toString().toAscii().data();

    //> Get combobox values
    XMLNode req("get");
    req.setAttr("path",wdg_it+"/%2flinks%2flnk%2f"+(id_it.child(0,0).isValid()?"pl_":"ls_")+attr_id);
    if( !owner()->mainWin()->cntrIfCmd(req) )
    {
	w_del = new QComboBox(parent);

	((QComboBox*)w_del)->setEditable(true);
	for( int i_l = 0; i_l < req.childSize(); i_l++ )
	    ((QComboBox*)w_del)->addItem(req.childGet(i_l)->text().c_str());
	connect( w_del, SIGNAL( currentIndexChanged(int) ), this, SLOT( selItem(int) ) );
    }
    else
    {
	QItemEditorFactory factory;
	w_del = factory.createEditor(index.data().type(), parent);
    }

    return w_del;
}

void LinkItemDelegate::selItem( int pos )
{
    QCoreApplication::postEvent((QWidget*)sender(),new QKeyEvent(QEvent::KeyPress,Qt::Key_Return,Qt::NoModifier));
//    emit commitData((QWidget*)sender());
//    emit closeEditor((QWidget*)sender());
}

void LinkItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if( dynamic_cast<QComboBox*>(editor) )
    {
	QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	comb->blockSignals(true);
	QString val = index.data(Qt::DisplayRole).toString();
	if( comb->findText(val) < 0 )	comb->addItem(val);
	comb->setCurrentIndex(comb->findText(val));
	comb->blockSignals(false);
    }
    else QItemDelegate::setEditorData(editor, index);
}

void LinkItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if( dynamic_cast<QComboBox*>(editor) )
	model->setData(index,dynamic_cast<QComboBox*>(editor)->currentText(),Qt::EditRole);
    else QItemDelegate::setModelData(editor, model, index);
}


//****************************************
//* Inspector of links dock widget       *
//****************************************
InspLnkDock::InspLnkDock( VisDevelop * parent ) : QDockWidget(_("Links"),(QWidget*)parent)
{
    setObjectName("InspLnkDock");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    ainsp_w = new InspLnk(this,owner());
    setWidget(ainsp_w);
}

InspLnkDock::~InspLnkDock( )
{

}

VisDevelop *InspLnkDock::owner()
{
    return (VisDevelop*)parentWidget();
}

void InspLnkDock::setWdg( const string &iwdg )
{
    if( QApplication::focusWidget() != ainsp_w )
	ainsp_w->setWdg(iwdg);
}


//****************************************
//* Widget's libraries tree              *
//****************************************
WdgTree::WdgTree( VisDevelop * parent ) : QDockWidget(_("Widgets"),(QWidget*)parent)
{
    setObjectName("WdgTree");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    //Create Tree Widget
    treeW = new QTreeWidget(this);
    treeW->setContextMenuPolicy(Qt::CustomContextMenu);
    //- Set collumn headers -
    QStringList headerLabels;
    headerLabels << _("Name") << _("Type") << _("Id");
    treeW->setHeaderLabels(headerLabels);
    treeW->header()->setStretchLastSection(false);
    treeW->setColumnWidth(0,180);
    treeW->setColumnWidth(1,60);
    treeW->setColumnWidth(2,0);
    //treeW->header()->resizeSection(0,120);
    //- Connect to signals -
    connect( treeW, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( ctrTreePopup() ) );
    connect( treeW, SIGNAL( itemSelectionChanged() ), this, SLOT( selectItem() ) );
    connect( treeW, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( dblClick() ) );

    setWidget(treeW);

    treeW->installEventFilter(this);
    treeW->viewport()->installEventFilter(this);
}

WdgTree::~WdgTree()
{

}

VisDevelop *WdgTree::owner()
{
    return (VisDevelop*)parentWidget();
}

bool WdgTree::hasFocus( )
{
    return (QApplication::focusWidget() == treeW);
}

bool WdgTree::eventFilter( QObject *target, QEvent *event )
{
    if( event->type() == QEvent::FocusIn )			selectItem( );
    if( event->type() == QEvent::FocusOut && !hasFocus() )	owner()->selectItem("");
    if( event->type() == QEvent::MouseButtonPress && ((QMouseEvent*)event)->button() == Qt::LeftButton )
	dragStartPos = ((QMouseEvent*)event)->pos();
    if( event->type() == QEvent::MouseMove &&
	    ((QMouseEvent*)event)->buttons()&Qt::LeftButton &&
	    (((QMouseEvent*)event)->pos()-dragStartPos).manhattanLength() >= QApplication::startDragDistance() )
    {
	QTreeWidgetItem *item = treeW->currentItem( );
	if( item )
	{
	    //- Get current widget -
	    int w_lev = 0;
	    string work_wdg;
	    QTreeWidgetItem *cur_el = item;
	    while(cur_el)
	    {
	        work_wdg.insert(0,string(cur_el->parent()?"/wdg_":"/wlb_")+cur_el->text(2).toAscii().data());
	        cur_el=cur_el->parent();
	        w_lev++;
	    }
	    //Prepare for drag and drop operation
	    if( owner()->work_space->activeWindow() && w_lev == 2 )
	    {
	        //- Prepare put data stream -
	        QByteArray itemData;
	        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	        dataStream << QString(work_wdg.c_str());

	        //- Prepare mime data -
	        QMimeData *mimeData = new QMimeData;
	        mimeData->setData("application/OpenSCADA-libwdg",itemData);

	        //- Create drag object -
	        QDrag *drag = new QDrag(this);
	        drag->setMimeData(mimeData);
	        //drag->setDragCursor(item->icon(0).pixmap(64,64),Qt::MoveAction);
	        drag->setPixmap(item->icon(0).pixmap(64,64));
	        drag->setHotSpot(QPoint(5,5));

	        drag->start(Qt::CopyAction);
	    }
	}
    }
    return QDockWidget::eventFilter( target, event );
}

void WdgTree::dblClick( )
{
    selectItem(true);
    owner()->actVisItProp->trigger();
}

void WdgTree::selectItem( bool force )
{
    //Get select list
    QList<QTreeWidgetItem *> sel_ls = treeW->selectedItems();
    if(sel_ls.size() != 1)	return;

    //Get current widget
    string work_wdg;
    QTreeWidgetItem *cur_el = sel_ls.at(0);
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->parent()?"/wdg_":"/wlb_")+cur_el->text(2).toAscii().data());
	cur_el=cur_el->parent();
    }

    emit selectItem(work_wdg,force);
}

void WdgTree::updateTree( const string &vca_it )
{
#if OSC_DEBUG >= 3
    long long t_cnt = TSYS::curTime();
#endif

    bool is_create, root_allow;
    int i_l, i_w, i_cw, i_top, i_topwl, i_topcwl;
    int i_t, i_m, i_a;
    QTreeWidgetItem *nit, *nit_w, *nit_cw;
    vector<string> list_wl;
    string t_el, simg;
    QImage img;

    //> Get elements number into VCA item
    int vca_lev = 0;
    for( int off = 0; !(t_el=TSYS::pathLev(vca_it,0,true,&off)).empty(); ) vca_lev++;

    if( (vca_lev && TSYS::pathLev(vca_it,0).substr(0,4) != "wlb_") )	return;
    string upd_lb   = (vca_lev>=1) ? TSYS::pathLev(vca_it,0).substr(4) : "";
    string upd_wdg  = (vca_lev>=2) ? TSYS::pathLev(vca_it,1).substr(4) : "";
    string upd_wdgi = (vca_lev>=3) ? TSYS::pathLev(vca_it,2).substr(4) : "";

    XMLNode req("get");
    req.setAttr("path","/%2fserv%2fwlbBr")->setAttr("item",vca_it);
    owner()->cntrIfCmd(req);

#if OSC_DEBUG >= 3
    mess_debug("VCA DEBUG",_("Widgets' development tree '%s' request time %f ms."),vca_it.c_str(),1e-3*(TSYS::curTime()-t_cnt));
#endif

    //> Remove no present libraries
    for( i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
    {
	nit = treeW->topLevelItem(i_top);
	if( !upd_lb.empty() && upd_lb != nit->text(2).toAscii().data() ) continue;
	for( i_l = 0; i_l < req.childSize(); i_l++ )
	    if( req.childGet(i_l)->name() == "wlb" && req.childGet(i_l)->attr("id") == nit->text(2).toAscii().data() )
		break;
	if( i_l < req.childSize() )	continue;
	delete treeW->takeTopLevelItem(i_top);
	i_top--;
    }

    //> Delete library tool bars and menus
    if( vca_lev != 3 )
    {
	//>> Delete toolbars
	for( i_t = 0; i_t < owner()->lb_toolbar.size(); i_t++ )
	{
	    if( !upd_lb.empty() && upd_lb != owner()->lb_toolbar[i_t]->objectName().toAscii().data() ) continue;
	    for( i_l = 0; i_l < req.childSize(); i_l++ )
		if( req.childGet(i_l)->name() == "wlb" && req.childGet(i_l)->attr("id") == owner()->lb_toolbar[i_t]->objectName().toAscii().data() )
		    break;
	    if( i_l >= req.childSize() )
	    {
		delete owner()->lb_toolbar[i_t];
		owner()->lb_toolbar.erase(owner()->lb_toolbar.begin()+i_t);
		i_t--;
	    }
	}

	//>> Delete menus
	for( i_m = 0; i_m < owner()->lb_menu.size(); i_m++)
	{
	    if( !upd_lb.empty() && upd_lb != owner()->lb_menu[i_m]->objectName().toAscii().data() ) continue;
	    for( i_l = 0; i_l < req.childSize(); i_l++ )
		if( req.childGet(i_l)->name() == "wlb" && req.childGet(i_l)->attr("id") == owner()->lb_menu[i_m]->objectName().toAscii().data() )
		    break;
	    if( i_l >= req.childSize() )
	    {
		delete owner()->lb_menu[i_m];
		owner()->lb_menu.erase(owner()->lb_menu.begin()+i_m);
		i_m--;
	    }
	}
    }

    //> Add new libraries
    for( i_l = 0; i_l < req.childSize(); i_l++ )
    {
	XMLNode *wlbN =  req.childGet(i_l);
	string wlbId = wlbN->attr("id");
	if( wlbN->name() != "wlb" || (!upd_lb.empty() && upd_lb != wlbId) )	continue;

	for( i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
	    if( wlbId == treeW->topLevelItem(i_top)->text(2).toAscii().data() )
		break;
	if( i_top >= treeW->topLevelItemCount() ) nit = new QTreeWidgetItem(treeW);
	else nit = treeW->topLevelItem(i_top);

	//>> Update libraries data
	img = QImage();
	simg = TSYS::strDecode(wlbN->childGet("ico")->text(),TSYS::base64);
	img.loadFromData((const uchar*)simg.c_str(),simg.size());
	if( !img.isNull() ) nit->setIcon(0,QPixmap::fromImage(img));
	nit->setText(0,wlbN->text().c_str());
	nit->setText(1,_("Library"));
	nit->setText(2,wlbId.c_str());

	//>> Add toolbars and menus
	if( vca_lev != 3 )
	{
	    is_create = root_allow = false;
	    for( i_t = 0; i_t < owner()->lb_toolbar.size(); i_t++)
		if( owner()->lb_toolbar[i_t]->objectName() == wlbId.c_str() )
		    break;
	    if( i_t >= owner()->lb_toolbar.size() )
	    {
		owner()->lb_toolbar.push_back( new QToolBar(QString(_("Library: %1")).arg(wlbId.c_str()),this) );
		owner()->lb_toolbar[i_t]->setObjectName(wlbId.c_str());
		owner()->addToolBar(owner()->lb_toolbar[i_t]);
		owner()->mn_view->addAction(owner()->lb_toolbar[i_t]->toggleViewAction());
		is_create = true;
	    }
	    for( i_m = 0; i_m < owner()->lb_menu.size(); i_m++ )
		if( owner()->lb_menu[i_m]->objectName() == wlbId.c_str() )
		    break;
	    if( i_m >= owner()->lb_menu.size() )
	    {
		owner()->lb_menu.push_back( new QMenu(QString(_("Library: %1")).arg(wlbId.c_str())) );
		owner()->lb_menu[i_m]->setObjectName(wlbId.c_str());
		owner()->mn_widg->addMenu(owner()->lb_menu[i_m]);
	    }
	    //>>> Update menu icon
	    if( !img.isNull() ) owner()->lb_menu[i_m]->setIcon(QPixmap::fromImage(img));
	}

	//>>> Remove no present widgets
	for( i_topwl = 0; i_topwl < nit->childCount(); i_topwl++ )
	{
	    nit_w = nit->child(i_topwl);
	    if( upd_wdg.empty() ) i_w = wlbN->childSize();
	    else
	    {
		if( upd_wdg != nit_w->text(2).toAscii().data() ) continue;
		for( i_w = 0; i_w < wlbN->childSize(); i_w++ )
		    if( wlbN->childGet(i_w)->name() == "w" && wlbN->childGet(i_w)->attr("id") == nit_w->text(2).toAscii().data() )
			break;
	    }
	    if( i_w < wlbN->childSize() )	continue;
	    delete nit->takeChild(i_topwl);
	    i_topwl--;
	}

	//>>> Delete widget's actions from toolbar and menu
	if( vca_lev != 3 )
	{
	    QList<QAction*> use_act = owner()->lb_toolbar[i_t]->actions();
	    for( i_a = 0; i_a < use_act.size(); i_a++ )
	    {
		if( upd_wdg.empty() ) i_w = wlbN->childSize();
		else
		{
		    if( upd_wdg != use_act[i_a]->objectName().toAscii().data() ) continue;
		    for( i_w = 0; i_w < wlbN->childSize(); i_w++ )
			if( wlbN->childGet(i_w)->name() == "w" && ("/wlb_"+wlbId+"/wdg_"+wlbN->childGet(i_w)->attr("id")) == use_act[i_a]->objectName().toAscii().data() )
			    break;
		}
		if( i_w < wlbN->childSize() ) continue;
		delete use_act[i_a];
	    }
	}

	//>>> Add new widgets
	for( i_w = 0; i_w < wlbN->childSize(); i_w++ )
	{
	    XMLNode *wdgN =  wlbN->childGet(i_w);
	    if( wdgN->name() != "w" ) continue;
	    string wdgId = wdgN->attr("id");
	    if( upd_wdg.empty() ) i_topwl = nit->childCount();
	    else
	    {
		if( upd_wdg != wdgId )	continue;
		for( i_topwl = 0; i_topwl < nit->childCount(); i_topwl++ )
		    if( wdgId == nit->child(i_topwl)->text(2).toAscii().data() )
			break;
	    }
	    if( i_topwl >= nit->childCount() ) nit_w = new QTreeWidgetItem(nit);
	    else nit_w = nit->child(i_topwl);

	    //>>> Update widget's data
	    img = QImage();
	    simg = TSYS::strDecode(wdgN->childGet("ico")->text(),TSYS::base64);
	    img.loadFromData((const uchar*)simg.c_str(),simg.size());
	    if( !img.isNull() ) nit_w->setIcon(0,QPixmap::fromImage(img));
	    nit_w->setText(0,wdgN->text().c_str());
	    nit_w->setText(1,_("Widget"));
	    nit_w->setText(2,wdgId.c_str());

	    //>>> Add widget's actions to toolbar and menu
	    if( vca_lev != 3 )
	    {
		QList<QAction*> use_act = owner()->lb_toolbar[i_t]->actions();
		QAction *cur_act;
		string wipath = "/wlb_"+wlbId+"/wdg_"+wdgId;
		//>>>> Get parent name
		if( !root_allow && wdgN->attr("parent") == "root" ) root_allow = true;
		//>>>> Add action
		if( upd_wdg.empty() ) i_a = use_act.size();
		else
		    for( i_a = 0; i_a < use_act.size(); i_a++ )
			if( use_act[i_a]->objectName() == wipath.c_str() )
			    break;
		if( i_a < use_act.size() ) cur_act = use_act[i_a];
		else
		{
		    //>>>>> Create new action
		    cur_act = new QAction(wdgN->text().c_str(),owner());
		    cur_act->setObjectName(wipath.c_str());
		    cur_act->setToolTip(QString(_("Add widget based at '%1'")).arg(wipath.c_str()));
		    cur_act->setWhatsThis(QString(_("The button for addition widget based at '%1'")).arg(wipath.c_str()));
		    cur_act->setStatusTip(QString(_("Press to add widget based at '%1'.")).arg(wipath.c_str()));
		    cur_act->setEnabled(false);
		    cur_act->setCheckable(true);
		    //>>>>> Add action to toolbar and menu
		    owner()->actGrpWdgAdd->addAction(cur_act);
		    owner()->lb_toolbar[i_t]->addAction(cur_act);
		    owner()->lb_menu[i_m]->addAction(cur_act);
		}
		//>>>> Update action
		if( !img.isNull() )	cur_act->setIcon(QPixmap::fromImage(img));
	    }

	    //>>>> Remove no present widgets
	    for( i_topcwl = 0; i_topcwl < nit_w->childCount(); i_topcwl++ )
	    {
		nit_cw = nit_w->child(i_topcwl);
		if( upd_wdgi.empty() ) i_cw = wdgN->childSize();
		else
		{
		    if( upd_wdgi != nit_cw->text(2).toAscii().data() ) continue;
		    for( i_cw = 0; i_cw < wdgN->childSize(); i_cw++ )
			if( wdgN->childGet(i_cw)->name() == "cw" && wdgN->childGet(i_cw)->attr("id") == nit_cw->text(2).toAscii().data() )
			    break;
		}
		if( i_cw < wdgN->childSize() )	continue;
		delete nit_w->takeChild(i_topcwl);
		i_topcwl--;
	    }
	    //>>>> Add new widgets
	    for( i_cw = 0; i_cw < wdgN->childSize(); i_cw++ )
	    {
		XMLNode *cwdgN = wdgN->childGet(i_cw);
		if( cwdgN->name() != "cw" ) continue;
		string cwdgId = cwdgN->attr("id");
		if( upd_wdgi.empty() ) i_topcwl = nit_w->childCount();
		else
		{
		    if( upd_wdgi != cwdgId ) continue;
		    for( i_topcwl = 0; i_topcwl < nit_w->childCount(); i_topcwl++ )
			if( cwdgId == nit_w->child(i_topcwl)->text(2).toAscii().data())
			    break;
		}
		if( i_topcwl >= nit_w->childCount() ) nit_cw = new QTreeWidgetItem(nit_w);
		else nit_cw = nit_w->child(i_topcwl);
		//>>> Update widget's data
		img = QImage();
		simg = TSYS::strDecode(cwdgN->childGet("ico")->text(),TSYS::base64);
		img.loadFromData((const uchar*)simg.c_str(),simg.size());
		if( !img.isNull() ) nit_cw->setIcon(0,QPixmap::fromImage(img));
		nit_cw->setText(0,cwdgN->text().c_str());
		nit_cw->setText(1,_("Container's widget"));
		nit_cw->setText(2,cwdgId.c_str());
	    }
	}
	if( vca_lev != 3 && is_create )
	{
	    owner()->lb_toolbar[i_t]->setVisible(root_allow);
	    owner()->lb_menu[i_m]->setProperty("rootLib",root_allow);
	}
    }

#if OSC_DEBUG >= 3
    mess_debug("VCA DEBUG",_("Widgets' development tree '%s' load time %f ms."),vca_it.c_str(),1e-3*(TSYS::curTime()-t_cnt));
#endif
}

void WdgTree::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    //Add actions
    popup.addAction(owner()->actLibNew);
    popup.addAction(owner()->actVisItAdd);
    popup.addAction(owner()->actVisItDel);
    popup.addAction(owner()->actVisItClear);
    popup.addAction(owner()->actVisItProp);
    popup.addAction(owner()->actVisItEdit);
    popup.addSeparator();
    for( int i_lm = 0; i_lm < owner()->lb_menu.size(); i_lm++ )
	if( owner()->lb_menu.size() <= 10 || owner()->lb_menu[i_lm]->property("rootLib").toBool() )
	    popup.addMenu(owner()->lb_menu[i_lm]);
    popup.addSeparator();
    popup.addAction(owner()->actVisItCut);
    popup.addAction(owner()->actVisItCopy);
    popup.addAction(owner()->actVisItPaste);
    popup.addSeparator();
    popup.addAction(owner()->actDBLoad);
    popup.addAction(owner()->actDBSave);
    popup.addSeparator();
    //> Reload action
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
    QAction *actRefresh = new QAction(QPixmap::fromImage(ico_t),_("Refresh libraries"),this);
    actRefresh->setStatusTip(_("Press to refresh present libraries."));
    connect(actRefresh, SIGNAL(activated()), this, SLOT(updateTree()));
    popup.addAction(actRefresh);

    QAction *rez = popup.exec(QCursor::pos());

    popup.clear();
}

//****************************************
//* Project's tree                       *
//****************************************
ProjTree::ProjTree( VisDevelop * parent ) : QDockWidget(_("Projects"),(QWidget*)parent)
{
    setObjectName("ProjTree");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

    treeW = new QTreeWidget(this);
    treeW->setContextMenuPolicy(Qt::CustomContextMenu);
    //- Set collumn headers -
    QStringList headerLabels;
    headerLabels << _("Name") << _("Type") << _("Id");
    treeW->setHeaderLabels(headerLabels);
    treeW->header()->setStretchLastSection(false);
    treeW->setColumnWidth(0,180);
    treeW->setColumnWidth(1,60);
    treeW->setColumnWidth(2,0);
    //- Connect to signals -
    connect( treeW, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( ctrTreePopup() ) );
    connect( treeW, SIGNAL( itemSelectionChanged() ), this, SLOT( selectItem() ) );
    connect( treeW, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT(dblClick()) );

    setWidget(treeW);

    treeW->installEventFilter(this);
}

ProjTree::~ProjTree()
{

}

VisDevelop *ProjTree::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

bool ProjTree::hasFocus( )
{
    return (QApplication::focusWidget() == treeW);
}

bool ProjTree::eventFilter( QObject *target, QEvent *event )
{
    if( target == treeW )
    {
	if( event->type() == QEvent::FocusIn )	selectItem( );
	if( event->type() == QEvent::FocusOut && !hasFocus( ) )
	    owner()->selectItem("");
    }
    return QDockWidget::eventFilter( target, event );
}

void ProjTree::dblClick( )
{
    selectItem(true);
    owner()->actVisItProp->trigger();
}

void ProjTree::selectItem( bool force )
{
    //Get select list
    QList<QTreeWidgetItem *> sel_ls = treeW->selectedItems();
    if(sel_ls.size() != 1)	return;

    //Get current widget
    string work_wdg;
    QTreeWidgetItem *cur_el = sel_ls.at(0);
    while(cur_el)
    {
	bool cur_it = (cur_el == sel_ls.at(0));
	work_wdg.insert(0,string(cur_el->parent()?"/pg_":"/prj_")+cur_el->text(2).toAscii().data());
	cur_el=cur_el->parent();
    }

    emit selectItem(work_wdg,force);
}

void ProjTree::updateTree( const string &vca_it, QTreeWidgetItem *it )
{
    vector<string> list_pr, list_pg;
    QTreeWidgetItem *nit, *nit_pg;
    string t_el, simg;
    QImage img;

    XMLNode req("get");

    if( !it )
    {
#if OSC_DEBUG >= 3
	long long t_cnt = TSYS::curTime();
#endif

	//- Get elements number into VCA item -
	int vca_lev = 0;
	for( int off = 0; !(t_el=TSYS::pathLev(vca_it,0,true,&off)).empty(); )	vca_lev++;

	if( (vca_lev && TSYS::pathLev(vca_it,0).substr(0,4) != "prj_") ||
	    (vca_lev > 2 && TSYS::pathLev(vca_it,vca_lev-1).substr(0,4) == "wdg_") )	return;
	string upd_prj = (vca_lev>=1) ? TSYS::pathLev(vca_it,0).substr(4) : "";
	//- Process top level items and project's list -
	//-- Get widget's libraries list --
	XMLNode prj_req("get");
	prj_req.setAttr("path","/%2fprm%2fcfg%2fprj");
	if( owner()->cntrIfCmd(prj_req) )
	{
	    mod->postMess(prj_req.attr("mcat").c_str(),prj_req.text().c_str(),TVision::Error,this);
	    return;
	}
	for( int i_ch = 0; i_ch < prj_req.childSize(); i_ch++ )
	    list_pr.push_back(prj_req.childGet(i_ch)->attr("id"));
	//-- Remove no present project --
	for( int i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
	{
	    int i_l;
	    for( i_l = 0; i_l < list_pr.size(); i_l++ )
		if( list_pr[i_l] == treeW->topLevelItem(i_top)->text(2).toAscii().data() )
		    break;
	    if( i_l < list_pr.size() )	continue;
	    delete treeW->takeTopLevelItem(i_top);
	    i_top--;
	}
	//-- Add new projects --
	for( int i_l = 0; i_l < list_pr.size(); i_l++ )
	{
	    if( !upd_prj.empty() && upd_prj != list_pr[i_l] ) continue;
	    int i_top;
	    for( i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
		if( list_pr[i_l] == treeW->topLevelItem(i_top)->text(2).toAscii().data() )
		    break;
	    if( i_top >= treeW->topLevelItemCount() )
		nit = new QTreeWidgetItem(treeW);
	    else nit = treeW->topLevelItem(i_top);

	    //-- Update projects data --
	    req.clear()->setAttr("path","/prj_"+list_pr[i_l]+"/%2fico");
	    if( !owner()->cntrIfCmd(req) )
	    {
		simg = TSYS::strDecode(req.text(),TSYS::base64);
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    nit->setIcon(0,QPixmap::fromImage(img));
	    }
	    nit->setText(0,prj_req.childGet(i_l)->text().c_str());
	    nit->setText(1,_("Project"));
	    nit->setText(2,list_pr[i_l].c_str());

	    updateTree(vca_it,nit);
	}
#if OSC_DEBUG >= 3
	mess_debug("VCA DEBUG",_("Project's development tree '%s' load time %f ms."),vca_it.c_str(),1e-3*(TSYS::curTime()-t_cnt));
#endif
	return;
    }
    //- Process project's pages -
    //-- Get page path
    nit = it;
    string work_wdg;
    QTreeWidgetItem *cur_el = nit;
    int vca_lev = 0;
    while( cur_el )
    {
	work_wdg.insert(0,string(cur_el->parent()?"/pg_":"/prj_")+cur_el->text(2).toAscii().data());
	cur_el = cur_el->parent();
	vca_lev++;
    }
    string upd_pg = TSYS::pathLev(vca_it,vca_lev-1);
    upd_pg = (upd_pg.size()>3) ? upd_pg.substr(3) : "";
    //-- Update include pages --
    //--- Get page's list ---
    XMLNode pg_req("get");
    pg_req.setAttr("path",work_wdg+"/%2fpage%2fpage");
    if( owner()->cntrIfCmd(pg_req) )
    {
	mod->postMess(pg_req.attr("mcat").c_str(),pg_req.text().c_str(),TVision::Error,this);
	return;
    }
    for( int i_ch = 0; i_ch < pg_req.childSize(); i_ch++ )
	list_pg.push_back(pg_req.childGet(i_ch)->attr("id"));
    //--- Remove no present pages ---
    for( int i_pit = 0; i_pit < nit->childCount(); i_pit++ )
    {
	int i_p;
	for( i_p = 0; i_p < list_pg.size(); i_p++ )
	    if( list_pg[i_p] == nit->child(i_pit)->text(2).toAscii().data() )
		break;
	if( i_p < list_pg.size() ) continue;
	delete nit->takeChild(i_pit);
	i_pit--;
    }
    //--- Add new pages ---
    for( int i_p = 0; i_p < list_pg.size(); i_p++ )
    {
	//if( !upd_pg.empty() && upd_pg != list_pg[i_p] ) continue;
	int i_pit;
	for( i_pit = 0; i_pit < nit->childCount(); i_pit++ )
	    if( list_pg[i_p] == nit->child(i_pit)->text(2).toAscii().data() )
		break;
	if( i_pit >= it->childCount() )
	    nit_pg = new QTreeWidgetItem(it);
	else nit_pg = it->child(i_pit);

	//--- Update page's data ---
	req.clear()->setAttr("path",work_wdg+"/pg_"+list_pg[i_p]+"/%2fico");
	if( !owner()->cntrIfCmd(req) )
	{
	    simg = TSYS::strDecode(req.text(),TSYS::base64);
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		nit_pg->setIcon(0,QPixmap::fromImage(img));
	}
	nit_pg->setText(0,pg_req.childGet(i_p)->text().c_str());
	nit_pg->setText(1,_("Page"));
	nit_pg->setText(2,list_pg[i_p].c_str());

	updateTree(vca_it,nit_pg);
    }
}

void ProjTree::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    //Add actions
    popup.addAction(owner()->actPrjRun);
    popup.addSeparator();
    popup.addAction(owner()->actPrjNew);
    popup.addAction(owner()->actVisItAdd);
    popup.addAction(owner()->actVisItDel);
    popup.addAction(owner()->actVisItClear);
    popup.addAction(owner()->actVisItProp);
    popup.addAction(owner()->actVisItEdit);
    popup.addSeparator();
    for( int i_lm = 0; i_lm < owner()->lb_menu.size(); i_lm++ )
	if( owner()->lb_menu.size() <= 10 || owner()->lb_menu[i_lm]->property("rootLib").toBool() )
	    popup.addMenu(owner()->lb_menu[i_lm]);
    popup.addSeparator();
    popup.addAction(owner()->actVisItCut);
    popup.addAction(owner()->actVisItCopy);
    popup.addAction(owner()->actVisItPaste);
    popup.addSeparator();
    popup.addAction(owner()->actDBLoad);
    popup.addAction(owner()->actDBSave);
    popup.addSeparator();

    //- Reload action -
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
    QAction *actRefresh = new QAction(QPixmap::fromImage(ico_t),_("Refresh projects"),this);
    actRefresh->setStatusTip(_("Press to refresh present projects."));
    connect(actRefresh, SIGNAL(activated()), this, SLOT(updateTree()));
    popup.addAction(actRefresh);

    QAction *rez = popup.exec(QCursor::pos());

    popup.clear();
}

//**********************************************************************************************
//* Text edit line widget with detail dialog edit button. Support: Font and Color edit dialogs.*
//**********************************************************************************************
LineEditProp::LineEditProp( QWidget *parent, DType tp ) : QWidget( parent ), m_tp(tp)
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->setMargin(0);
    box->setSpacing(0);

    ed_fld = new QLineEdit(this);
    box->addWidget(ed_fld);

    QPushButton *bt_fld = new QPushButton(this);
    bt_fld->setIcon(QIcon(":/images/edit.png"));
    bt_fld->setIconSize(QSize(12,12));
    bt_fld->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
    bt_fld->setMaximumWidth(15);
    box->addWidget(bt_fld);
    connect( bt_fld, SIGNAL( pressed() ), this, SLOT( callDlg() ) );

    setFocusProxy(ed_fld);
}

QString LineEditProp::value( )
{
    return ed_fld->text();
}

void LineEditProp::setValue( const QString &val )
{
    ed_fld->setText(val);
}

void LineEditProp::callDlg( )
{
    if( type() == LineEditProp::Font )
    {
	FontDlg fnt_dlg(this,value().toAscii().data());
	if( fnt_dlg.exec() )	setValue(fnt_dlg.font());
	setFocus();
    }
    else if( type() == LineEditProp::Color )
    {
        QColor clr;
        size_t found = value().toStdString().find("-");
        if (found != string::npos)
        {
            clr = QColor( value().toStdString().substr(0,found).c_str() );
            clr.setAlpha( atoi(value().toStdString().substr(found+1).c_str()) );
        }
        else clr = QColor(value());
	clr = QColorDialog::getColor(clr,this);
	if( clr.isValid() ) setValue(clr.name());
	setFocus();
    }
    QApplication::postEvent(this,new QKeyEvent(QEvent::KeyPress,Qt::Key_Return,Qt::NoModifier));
}

//*********************************************
//* Status bar scale indicator                *
//*********************************************
WScaleStBar::WScaleStBar( QWidget *parent ) : QLabel(parent)
{
    setScale(false);
}

void WScaleStBar::setScale( bool val )
{
    isScale = val;
    setText( (isScale)?_("Scale"):_("Resize") );
}

void WScaleStBar::mousePressEvent( QMouseEvent * event )
{
    setScale(!scale());
}

//*********************************************
//* Status bar modify indicator               *
//*********************************************
WMdfStBar::WMdfStBar( QWidget *parent ) : QLabel(parent)
{

}

void WMdfStBar::mousePressEvent( QMouseEvent * event )
{
    emit press();
}

//****************************************
//* Shape widget view development mode   *
//****************************************
DevelWdgView::DevelWdgView( const string &iwid, int ilevel, VisDevelop *mainWind, QWidget* parent ) :
    WdgView(iwid,ilevel,mainWind,parent), pntView(NULL), editWdg(NULL),
    fMakeScale(false), fWdgEdit(false), fWdgSelect(false), fMoveHold(false), fHoldChild(false), fLeftTop(false),
    fHoldSelRect(false), fMoveHoldMove(false), fHideChilds(false), fSelChange(false), mVisScale(1)
{
    setMouseTracking(true);
    if( wLevel() == 0 )
    {
	pntView = new SizePntWdg(this);
	pntView->setSelArea(QRectF());
	pntView->hide();
	setFocusPolicy(Qt::StrongFocus);
	setCursor(Qt::ArrowCursor);
	setAcceptDrops(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	mainWin( )->setWdgVisScale( mVisScale );
	connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( wdgPopup() ) );
    }
    //> Select only created widgets by user
    else if( wLevel() == 1 && ((WdgView*)parentWidget())->isReload )
    { setSelect(true,PrcChilds); z_coord = 100000; }
}

DevelWdgView::~DevelWdgView( )
{
    if( select() && !mod->endRun( ) )
    {
	setSelect(false);
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
		((DevelWdgView*)children().at(i_c))->setSelect(false,PrcChilds);
    }
}

string DevelWdgView::user( )
{
    return mainWin( )->user();
}

VisDevelop *DevelWdgView::mainWin( )
{
    return (VisDevelop *)WdgView::mainWin();
}

WdgView *DevelWdgView::newWdgItem( const string &iwid )
{
    DevelWdgView *wdg = new DevelWdgView(iwid,wLevel()+1,mainWin(),this);
    connect(wdg, SIGNAL(selected(const string&)), this, SIGNAL(selected(const string& )));
    if( wLevel() == 0 )  pntView->raise();
    return wdg;
}

int DevelWdgView::cntrIfCmd( XMLNode &node, bool glob )
{
    return mainWin()->cntrIfCmd(node,glob);
}

void DevelWdgView::saveGeom( const string& item )
{
    if( item.empty() || item == id() )
    {
	attrSet("geomX", TSYS::real2str(TSYS::realRound((wLevel()>0) ? posF().x()/((WdgView*)parentWidget())->xScale(true) : posF().x(),2)), 7);
	attrSet("geomY", TSYS::real2str(TSYS::realRound((wLevel()>0) ? posF().y()/((WdgView*)parentWidget())->yScale(true) : posF().y(),2)), 8);
	attrSet("geomW", TSYS::real2str(TSYS::realRound(sizeF().width()/xScale(true),2)), 9);
	attrSet("geomH", TSYS::real2str(TSYS::realRound(sizeF().height()/yScale(true),2)), 10);
	attrSet("geomXsc", TSYS::real2str(TSYS::realRound(x_scale,2)), 13);
	attrSet("geomYsc", TSYS::real2str(TSYS::realRound(y_scale,2)), 14);
	attrSet("geomZ", TSYS::int2str(parent()->children().indexOf(this)),11);
    }

    if( item != id() && wLevel() == 0 )
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
		((DevelWdgView*)children().at(i_c))->saveGeom(item);

    //> For top items (like inspector) data update
    if( wLevel() == 0  )  setSelect(true,PrcChilds);
}

void DevelWdgView::setSelect( bool vl, char flgs )// bool childs, bool onlyFlag, bool noUpdate )
{
    int chld_cnt = 0;

    fWdgSelect = vl;
    if( !vl && edit() && !(flgs&OnlyFlag) ) setEdit(false);

    //> Level 0 process
    if( wLevel() != 0 && !edit() ) return;

    if( vl && !(flgs&OnlyFlag) )
    {
	string sel_chlds = selectChilds(&chld_cnt);
	if( sel_chlds.size() )	emit selected(sel_chlds);
	else			emit selected(id());
    }
    if( !vl )
    {
	if( flgs&PrcChilds )
	    for( int i_c = 0; i_c < children().size(); i_c++ )
		if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
		    qobject_cast<DevelWdgView*>(children().at(i_c))->setSelect(false,flgs|OnlyFlag);
	if( !(flgs&OnlyFlag) )	emit selected("");
    }

    //> Update actions access
    //>> Enable view toolbar
    if( !(flgs&OnlyFlag) )
    {
	if( !edit() )
	{
	    mainWin()->wdgToolView->setVisible(vl);
	    disconnect( mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(wdgViewTool(QAction*)) );
	    if( vl ) connect( mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(wdgViewTool(QAction*)) );

	    //>> Update widget view tools
	    for( int i_a = 0; i_a < mainWin()->wdgToolView->actions().size(); i_a++ )
		mainWin()->wdgToolView->actions().at(i_a)->setEnabled(chld_cnt>0);
	}

	if( !(flgs&NoUpdate) ) update();
    }
}

void DevelWdgView::setEdit( bool vl )
{
    fWdgEdit = vl;

    if( vl )
    {
	editWdg = this;
	if( shape->isEditable( ) ) shape->editEnter( this );
	//- Raise top included editable widget -
	if( wLevel( ) == 0 )
	    for( int i_c = 0; i_c < children().size(); i_c++ )
		if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
			((DevelWdgView*)children().at(i_c))->edit() )
		{
		    editWdg = (DevelWdgView*)children().at(i_c);
		    editWdg->raise();
		    pntView->raise();
		    break;
		}
	//- Disable widget view tools -
	for( int i_a = 0; i_a < mainWin()->wdgToolView->actions().size(); i_a++ )
	    mainWin()->wdgToolView->actions().at(i_a)->setEnabled(false);
    }
    else
    {
	if( shape->isEditable( ) ) shape->editExit( this );
	editWdg = NULL;
	//- Update widgets order -
	if( wLevel( ) == 0 )	orderUpdate( );
    }
}

string DevelWdgView::selectChilds( int *cnt, vector<DevelWdgView*> *wdgs )
{
    string sel_chlds;

    if( cnt ) *cnt = 0;
    for( int i_c = 0; i_c < children().size(); i_c++ )
    {
	DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
	if( !curw ) continue;
	if( curw->select() )
	{
	    sel_chlds=sel_chlds+curw->id()+";";
	    if( wdgs )	wdgs->push_back(curw);
	    if( cnt )	(*cnt)++;
	}
    }
    return sel_chlds;
}

bool DevelWdgView::grepAnchor( const QPointF &apnt, const QPoint &cpnt )
{
    if( (cpnt.x() > apnt.x()-4) && (cpnt.x() < apnt.x()+4) &&
	    (cpnt.y() > apnt.y()-4) && (cpnt.y() < apnt.y()+4) )
	return true;
    else return false;
}

void DevelWdgView::upMouseCursors( const QPoint &curp )
{
    if( fMoveHold ) return;

    Qt::CursorShape new_shp = Qt::ArrowCursor;
    //> Widget geometry
    if( grepAnchor(rect().bottomRight(),curp) )
	new_shp = Qt::SizeFDiagCursor;
    else if( curp.x()>(rect().width()-4) && curp.x()<(rect().width()+4) )
	new_shp = Qt::SizeHorCursor;
    else if( curp.y()>(rect().height()-4) && curp.y()<(rect().height()+4) )
	new_shp = Qt::SizeVerCursor;
    if( new_shp != Qt::ArrowCursor )
    {
	fHoldChild = false;
	if( new_shp != cursor().shape() ) setCursor(new_shp);
	return;
    }

    //> Childs' selection process
    fLeftTop = false;

    //>> Check child's anchor selection and widget's geometry
    QRectF selRect;
    bool firs_nosel = true, noSelUp = false;
    for( int i_c = children().size()-1; i_c >= 0; i_c-- )
	if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
	{
	    if( ((DevelWdgView*)children().at(i_c))->select( ) )
	    { selRect = selRect.united(((DevelWdgView*)children().at(i_c))->geometryF()); firs_nosel = false;  }
	    else if( firs_nosel && ((DevelWdgView*)children().at(i_c))->geometryF().contains(curp) )	noSelUp = true;
	}

    //>> Select childs anchors
    if( !selRect.isNull() )
    {
	if( grepAnchor(selRect.topLeft(),curp) )
	{ new_shp = Qt::SizeFDiagCursor; fLeftTop = true; }
	else if( grepAnchor(selRect.bottomRight(),curp) )					new_shp = Qt::SizeFDiagCursor;
	else if( grepAnchor(selRect.bottomLeft(),curp) )					{ new_shp = Qt::SizeBDiagCursor; fLeftTop = true; }
	else if( grepAnchor(selRect.topRight(),curp) )						new_shp = Qt::SizeBDiagCursor;
	else if( grepAnchor(QPointF(selRect.center().x(),selRect.y()),curp) )			{ new_shp = Qt::SizeVerCursor; fLeftTop = true; }
	else if( grepAnchor(QPointF(selRect.center().x(),selRect.bottomRight().y()),curp) )	new_shp = Qt::SizeVerCursor;
	else if( grepAnchor(QPointF(selRect.x(),selRect.center().y()),curp) )			{ new_shp = Qt::SizeHorCursor; fLeftTop = true; }
	else if( grepAnchor(QPointF(selRect.bottomRight().x(),selRect.center().y()),curp) )	new_shp = Qt::SizeHorCursor;
	else if( !noSelUp && selRect.contains(curp) )						new_shp = Qt::PointingHandCursor;
	if( new_shp != Qt::ArrowCursor )	fHoldChild = true;
    }
    if( new_shp != cursor().shape() ) setCursor(new_shp);
}

void DevelWdgView::wdgViewTool( QAction *act )
{
    if( edit() )    return;
    QStringList sact = act->objectName().split('_');
    if( sact.at(0) == "align" )
    {
	//- Get selected rect -
	QRectF selRect;
	int sel_cnt = 0;
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    DevelWdgView *cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
	    if( cwdg && cwdg->select( ) )
	    {
		selRect = selRect.united(cwdg->geometryF());
		sel_cnt++;
	    }
	}
	if( sel_cnt == 0 ) return;
	if( sel_cnt == 1 ) selRect = selRect.united(QRectF(QPointF(0,0),sizeF()));

	//- Update selected widgets position -
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    DevelWdgView *cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
	    if( cwdg && cwdg->select( ) )
	    {
		if( sact.at(1) == "left" )
		    cwdg->moveF(QPointF(selRect.x(),cwdg->posF().y()));
		else if( sact.at(1) == "right" )
		    cwdg->moveF(QPointF(selRect.x()+selRect.width()-cwdg->sizeF().width(),cwdg->posF().y()));
		else if( sact.at(1) == "vcenter" )
		    cwdg->moveF(QPointF(selRect.x()+(selRect.width()-cwdg->sizeF().width())/2,cwdg->posF().y()));
		else if( sact.at(1) == "top" )
		    cwdg->moveF(QPointF(cwdg->posF().x(),selRect.y()));
		else if( sact.at(1) == "bottom" )
		    cwdg->moveF(QPointF(cwdg->posF().x(),selRect.y()+selRect.height()-cwdg->sizeF().height()));
		else if( sact.at(1) == "hcenter" )
		    cwdg->moveF(QPointF(cwdg->posF().x(),selRect.y()+(selRect.height()-cwdg->sizeF().height())/2));
	    }
	}
	saveGeom("");
    }
    else if( sact.at(0) == "level" )
    {
	bool is_rise = (sact.at(1) == "rise");
	bool is_up   = (sact.at(1) == "up");
	bool is_lower= (sact.at(1) == "lower");
	bool is_down = (sact.at(1) == "down");
	string sel_ws = selectChilds();
	string sel_w;

	if( is_rise || is_up )
	    for( int w_off=0; (sel_w=TSYS::strSepParse(sel_ws,0,';',&w_off)).size(); )
	    {
		bool is_move = false;
		DevelWdgView *cwdg = NULL;
		DevelWdgView *ewdg = NULL;
		for( int i_c = 0; i_c < children().size(); i_c++ )
		{
		    if( !qobject_cast<DevelWdgView*>(children().at(i_c)) )   continue;
		    ewdg = qobject_cast<DevelWdgView*>(children().at(i_c));
		    if( ewdg->id() == sel_w.c_str() )   cwdg = ewdg;
		    else if( is_up && !is_move && cwdg && !ewdg->select() &&
			    ewdg->geometryF().intersects(cwdg->geometryF()) )
		    {
			cwdg->stackUnder(ewdg);
			ewdg->stackUnder(cwdg);
			is_move = true;
		    }
		}
		if(is_rise && cwdg && ewdg && cwdg!=ewdg )
		{
		    cwdg->stackUnder(ewdg);
		    ewdg->stackUnder(cwdg);
		}
	    }
	if( is_lower || is_down )
	    for( int w_off=0; (sel_w=TSYS::strSepParse(sel_ws,0,';',&w_off)).size(); )
	    {
		bool is_move = false;
		DevelWdgView *cwdg = NULL;
		DevelWdgView *ewdg = NULL;
		for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		{
		    if( !qobject_cast<DevelWdgView*>(children().at(i_c)) )   continue;
		    ewdg = qobject_cast<DevelWdgView*>(children().at(i_c));
		    if( ewdg->id() == sel_w.c_str() )   cwdg = ewdg;
		    else if( is_down && !is_move && cwdg && !ewdg->select() &&
			    ewdg->geometryF().intersects(cwdg->geometryF()) )
		    {
			cwdg->stackUnder(ewdg);
			is_move = true;
		    }
		}
		if(is_lower && cwdg && ewdg && cwdg!=ewdg )	cwdg->stackUnder(ewdg);
	    }
	saveGeom("");
    }
}

void DevelWdgView::wdgPopup( )
{
    int sel_cnt;
    vector<DevelWdgView*> sel_wdgs;
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    //> Cancel new widget inserting
    QAction *act = mainWin()->actGrpWdgAdd->checkedAction();
    if( act && act->isChecked() )
    {
	act->setChecked(false);
	setCursor(Qt::ArrowCursor);
	return;
    }

    //> Add actions
    if( edit() )
    {
	//>> Individual primitive menus
	if( editWdg && editWdg->shape )	editWdg->shape->wdgPopup( editWdg, popup );
	//>> Exit from widget edition
	QAction *actExitEdit = new QAction(_("Exit from widget editing"),this);
	actExitEdit->setStatusTip(_("Press to exit from widget editing."));
	connect(actExitEdit, SIGNAL(activated()), this, SLOT(editExit()));
	popup.addAction(actExitEdit);
    }
    else
    {
	//>> Insert item actions
	if( !selectChilds(&sel_cnt,&sel_wdgs).empty() )
	{
	    popup.addAction(mainWin()->actVisItDel);
	    popup.addAction(mainWin()->actVisItClear);
	    if( sel_cnt == 1 )
	    {
		popup.addAction(mainWin()->actVisItProp);
		popup.addAction(mainWin()->actVisItEdit);
	    }
	    popup.addSeparator();
	    popup.addAction(mainWin()->actDBLoad);
	    popup.addAction(mainWin()->actDBSave);
	    popup.addSeparator();
	    //>> Insert item actions
	    popup.addMenu(mainWin()->mn_widg_fnc);
	}
	//>> Make edit enter action
	popup.addSeparator();
	if( (sel_wdgs.size() == 1 && sel_wdgs[0]->shape && sel_wdgs[0]->shape->isEditable()) || (shape && shape->isEditable()) )
	{
	    QAction *actEnterEdit = new QAction(_("Enter for widget editing"),this);
	    actEnterEdit->setStatusTip(_("Press to enter for widget editing."));
	    connect(actEnterEdit, SIGNAL(activated()), this, SLOT(editEnter()));
	    popup.addAction(actEnterEdit);
	}
	//>> Make widget icon
	QAction *actMakeIco = new QAction(parentWidget()->windowIcon(),_("Make icon from widget"),this);
	actMakeIco->setStatusTip(_("Press to make icon from widget."));
	connect(actMakeIco, SIGNAL(activated()), this, SLOT(makeIcon()));
	popup.addAction(actMakeIco);
	//>> Make widget image
	QAction *actMakeImg = new QAction(_("Make image from widget"),this);
	actMakeImg->setStatusTip(_("Press to make image from widget."));
	connect(actMakeImg, SIGNAL(activated()), this, SLOT(makeImage()));
	popup.addAction(actMakeImg);
	popup.addSeparator();
	//>> Unset, increase, decrease the visual scale of thge widget
	QAction *actIncVisScale = new QAction(_("Zoom in (+10%)"),this);
	actIncVisScale->setObjectName("inc");
	actIncVisScale->setStatusTip(_("Press to increase the visual scale of the widget by 10%."));
	connect(actIncVisScale, SIGNAL(activated()), this, SLOT(incDecVisScale()));
	popup.addAction(actIncVisScale);
	QAction *actDecVisScale = new QAction(_("Zoom out (-10%)"),this);
	actDecVisScale->setObjectName("dec");
	actDecVisScale->setStatusTip(_("Press to decrease the visual scale of the widget by 10%."));
	connect(actDecVisScale, SIGNAL(activated()), this, SLOT(incDecVisScale()));
	popup.addAction(actDecVisScale);
	QAction *actUnsetVisScale = new QAction(_("Reset zoom (100%)"),this);
	actUnsetVisScale->setObjectName("unset");
	actUnsetVisScale->setStatusTip(_("Press to set the visual scale of the widget to 100%."));
	connect(actUnsetVisScale, SIGNAL(activated()), this, SLOT(incDecVisScale()));
	popup.addAction(actUnsetVisScale);
	popup.addSeparator();

	popup.addAction(mainWin()->actVisItCut);
	popup.addAction(mainWin()->actVisItCopy);
	popup.addAction(mainWin()->actVisItPaste);
	popup.addSeparator();
	popup.addAction(mainWin()->actDBLoad);
	popup.addAction(mainWin()->actDBSave);
    }

    //- Execute of menu -
    QAction *rez = popup.exec(QCursor::pos());

    popup.clear();
}

void DevelWdgView::makeIcon( )
{
    QPixmap ico_new = QPixmap::grabWidget(this);
    ico_new = ico_new.scaled(64,64,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    parentWidget()->setWindowIcon(ico_new);
    //> Send to VCA engine
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_new.save(&buffer,"PNG");
//    ico_new.save(&buffer,"JPG",80);

    XMLNode req("set");
    req.setAttr("path",id()+"/%2fwdg%2fcfg%2fico")->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if( mainWin()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else emit apply(id());
}

void DevelWdgView::makeImage( )
{
    QPixmap img = QPixmap::grabWidget(this);

    //> Call save file dialog
    QString fileName = QFileDialog::getSaveFileName(this,_("Save widget's image"),
	(TSYS::path2sepstr(id())+".png").c_str(), _("Images (*.png *.xpm *.jpg)"));
    if( !fileName.isEmpty() && !img.save(fileName) )
	mod->postMess(mod->nodePath().c_str(),QString(_("Save to file '%1' is error.")).arg(fileName),TVision::Error,this);
}

void DevelWdgView::editEnter( )
{
    if( edit() )	return;

    vector<DevelWdgView*> sel_wdgs;
    selectChilds(NULL,&sel_wdgs);
    if( sel_wdgs.size() == 1 && sel_wdgs[0]->shape && sel_wdgs[0]->shape->isEditable( ) )
    {
	sel_wdgs[0]->setEdit(true);
	setEdit(true);
	setCursor(Qt::ArrowCursor);
	update();
    }
    else if( sel_wdgs.size() == 0 && shape && shape->isEditable( ) )
    {
	setEdit(true);
	setCursor(Qt::ArrowCursor);
	update();
    }
}

void DevelWdgView::editExit( )
{
    for( int i_c = 0; i_c < children().size(); i_c++ )
	if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
	    ((DevelWdgView*)children().at(i_c))->setSelect(false,PrcChilds);
    setEdit(false);
    update();
}

void DevelWdgView::wdgsMoveResize( const QPointF &dP )
{
    if( QApplication::keyboardModifiers()&Qt::ControlModifier )	mainWin()->setWdgScale(true);
    bool isScale = mainWin()->wdgScale( );
    if( isScale )	fMakeScale = true;

    if( fHoldChild )
    {
	QRectF bsRct = pntView->geometryF();
	float xSc = (float)dP.x()/(float)bsRct.width();
	float ySc = (float)dP.y()/(float)bsRct.height();
	//-- Update selected widgets geometry --
	for( int i_c = 0; i_c < children().size(); i_c++ )
	{
	    DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
	    if( !curw || !curw->select() ) continue;
	    QRectF  geom = curw->geometryF();
	    switch(cursor().shape())
	    {
		case Qt::SizeFDiagCursor:
		    if( isScale )
		    {
			curw->x_scale *= 1+((fLeftTop)?-1:1)*xSc;
			curw->y_scale *= 1+((fLeftTop)?-1:1)*ySc;
		    }
		    if( fLeftTop )
		    {
			curw->moveF(QPointF(geom.x()+xSc*(bsRct.x()+bsRct.width()-geom.x()),geom.y()+ySc*(bsRct.y()+bsRct.height()-geom.y())));
			curw->resizeF(QSizeF((float)geom.width()*(1.0-xSc), (float)geom.height()*(1.0-ySc)));
		    }
		    else
		    {
		        curw->moveF(QPointF(geom.x()+xSc*(geom.x()-bsRct.x()),geom.y()+ySc*(geom.y()-bsRct.y())));
			curw->resizeF(QSizeF((float)geom.width()*(1.0+xSc), (float)geom.height()*(1.0+ySc)));
		    }
		    break;
		case Qt::SizeBDiagCursor:
		    if( isScale )
		    {
			curw->x_scale *= 1+((fLeftTop)?-1:1)*xSc;
			curw->y_scale *= 1-((fLeftTop)?-1:1)*ySc;
		    }
		    if( fLeftTop )
		    {
			curw->moveF(QPointF(geom.x()+xSc*(bsRct.x()+bsRct.width()-geom.x()),geom.y()+ySc*(geom.y()-bsRct.y())));
			curw->resizeF(QSizeF((float)geom.width()*(1.0-xSc), (float)geom.height()*(1.0+ySc)));
		    }
		    else
		    {
			curw->moveF(QPointF(geom.x()+xSc*(geom.x()-bsRct.x()),geom.y()+ySc*(bsRct.y()+bsRct.height()-geom.y())));
			curw->resizeF(QSizeF((float)geom.width()*(1.0+xSc), (float)geom.height()*(1.0-ySc)));
		    }
		    break;
		case Qt::SizeVerCursor:
		    if( isScale )
			curw->y_scale *= 1+((fLeftTop)?-1:1)*ySc;
		    if( fLeftTop )
		    {
			curw->moveF( QPointF(geom.x(), geom.y()+ySc*(bsRct.y()+bsRct.height()-geom.y())) );
			curw->resizeF( QSizeF(geom.width(), (float)geom.height()*(1.0-ySc)) );
		    }
		    else
		    {
			curw->moveF( QPointF(geom.x(), geom.y()+ySc*(geom.y()-bsRct.y())) );
			curw->resizeF( QSizeF(geom.width(), (float)geom.height()*(1.0+ySc)) );
		    }
		    break;
		case Qt::SizeHorCursor:
		    if( isScale )
			curw->x_scale *= 1+((fLeftTop)?-1:1)*xSc;
		    if( fLeftTop )
		    {
			curw->moveF(QPointF(geom.x()+xSc*(bsRct.x()+bsRct.width()-geom.x()),geom.y()));
			curw->resizeF(QSizeF((float)geom.width()*(1.0-xSc), geom.height()));
		    }
		    else
		    {
			curw->moveF(QPointF(geom.x()+xSc*(geom.x()-bsRct.x()),geom.y()));
			curw->resizeF(QSizeF((float)geom.width()*(1.0+xSc),geom.height()));
		    }
		    break;
		case Qt::PointingHandCursor:
		    curw->moveF(curw->posF()+dP);
		    break;
		default:
		    if( fMoveHold )	break;
		    curw->moveF(curw->posF()+dP);
		    break;
	    }
	}
	//-- Set status bar --
	QRectF srect;
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
		    ((DevelWdgView*)children().at(i_c))->select( ) )
		srect = srect.united(((DevelWdgView*)children().at(i_c))->geometryF());
	mainWin()->statusBar()->showMessage(
	    QString(_("Selected elements --- xy(%2:%3) wh[%4:%5]"))
		.arg(srect.x()/xScale(true)).arg(srect.y()/yScale(true))
		.arg(srect.width()/xScale(true)).arg(srect.height()/yScale(true)),10000);
    }
    else
    {
	//- Change widget geometry -
	switch(cursor().shape())
	{
	    case Qt::SizeHorCursor:
		if( isScale )	x_scale *= 1+dP.x()/sizeF().width();
		resizeF(QSizeF(sizeF().width()+dP.x(),sizeF().height()));
		break;
	    case Qt::SizeVerCursor:
		if( isScale )	y_scale *= 1+dP.y()/sizeF().height();
		resizeF(QSizeF(sizeF().width(),sizeF().height()+dP.y()));
	        break;
	    case Qt::SizeFDiagCursor:
		if( isScale )
		{
		    x_scale *= 1+dP.x()/sizeF().width();
		    y_scale *= 1+dP.y()/sizeF().height();
		}
		resizeF(QSizeF(sizeF().width()+dP.x(),sizeF().height()+dP.y()));
		break;
	}
	//-- Set status bar --
	mainWin()->statusBar()->showMessage(
	    QString(_("Page: '%1' --- xy(%2:%3) wh[%4:%5]"))
		.arg(id().c_str()).arg(posF().x()/xScale(true)).arg(posF().y()/yScale(true))
		.arg(sizeF().width()/xScale(true)).arg(sizeF().height()/yScale(true)), 10000 );
    }
}

DevelWdgView *DevelWdgView::levelWidget( int lev )
{
    if( qobject_cast<DevelWdgView*>(parentWidget()) && wLevel() > lev )
	return ((DevelWdgView*)parentWidget())->levelWidget( lev );
    return this;
}

string DevelWdgView::resGet( const string &res )
{
    if( res.empty() )	return "";
    string ret = levelWidget(0)->cacheResGet(res);
    if( ret.empty() && !(ret=WdgView::resGet(res)).empty() )
	levelWidget(0)->cacheResSet(res,ret);

    return ret;
}

string DevelWdgView::cacheResGet( const string &res )
{
    map<string,string>::iterator ires = mCacheRes.find(res);
    if( ires == mCacheRes.end() ) return "";
    return ires->second;
}

void DevelWdgView::cacheResSet( const string &res, const string &val )
{
    if( val.size() > 1024*1024 ) return;
    mCacheRes[res] = val;
}

float DevelWdgView::xScale( bool full )
{
    if( wLevel() == 0 ) return WdgView::xScale( full ) * visScale( );
    return WdgView::xScale( full );

}
float DevelWdgView::yScale( bool full )
{
    if( wLevel() == 0 ) return WdgView::yScale( full ) * visScale( );
    return WdgView::yScale( full );
}

void DevelWdgView::setVisScale( float val )
{
    mVisScale = vmin(10,vmax(0.1,val));
    load("");
    mainWin( )->setWdgVisScale( mVisScale );
}

void DevelWdgView::incDecVisScale( )
{
    if( sender()->objectName() == "unset" )	setVisScale(1);
    else if( sender()->objectName() == "inc" )	setVisScale(visScale()+0.1);
    else if( sender()->objectName() == "dec" )	setVisScale(visScale()-0.1);
}

bool DevelWdgView::event( QEvent *event )
{
    //> Paint event process
    if( event->type() == QEvent::Paint )
    {
	QPainter pnt( this );
	pnt.setWindow( rect() );

	//> Draw background for root widget
	if( wLevel() == 0 )
	{
	    pnt.setPen("black");
	    pnt.setBrush(QBrush(QColor("white")));
	    pnt.drawRect(rect().adjusted(0,0,-1,-1));
	}
	//> Draw widget border geometry
	else if( levelWidget(1)->select() && levelWidget(0)->fMoveHoldMove )
	{
	    if( !fHideChilds )
	    {
		fHideChilds = true;
		for( int i_c = 0; i_c < children().size(); i_c++ )
		    if( qobject_cast<QWidget*>(children().at(i_c)) )
			((QWidget*)children().at(i_c))->setEnabled(false);//hide();
	    }
	    pnt.setPen("white");
	    pnt.drawRect(rect().adjusted(0,0,-1,-1));
	    QPen pen(QColor("black"));
	    pen.setStyle(Qt::DashLine);
	    pnt.setPen(pen);
	    pnt.drawRect(rect().adjusted(0,0,-1,-1));
	    return true;
	}
	else if( fHideChilds )
	{
	    fHideChilds = false;
	    for( int i_c = 0; i_c < children().size(); i_c++ )
		if( qobject_cast<QWidget*>(children().at(i_c)) )
		    ((QWidget*)children().at(i_c))->setEnabled(true);//show();
	}
	//> Check widget
	if( !shape )
	{
	    pnt.drawImage(rect(),QImage(":/images/attention.png"));
	    setToolTip(QString(_("Widget is not enabled or shape is not supported!")));
	}

	//> Update select widget data
	if( wLevel() == 0 && !fHoldSelRect )
	{
	    QRectF rsel;
	    //if( !(flags()&DevelWdgView::moveHoldMove) )
		for( int i_c = 0; i_c < children().size(); i_c++ )
		    if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
			    ((DevelWdgView*)children().at(i_c))->select( ) )
			rsel = rsel.united(((DevelWdgView*)children().at(i_c))->geometryF());
	    pntView->setSelArea( rsel, fMoveHoldMove ?  SizePntWdg::Hide : (edit() ? SizePntWdg::EditBorder : SizePntWdg::SizeDots) );
	}
	pnt.end();
	if( WdgView::event(event) )	return true;
	return QWidget::event(event);
    }

    //> Other events process
    if( wLevel() == 0 )
	switch( event->type() )
	{
	    case QEvent::DragEnter:
	    {
		QDragEnterEvent *ev = static_cast<QDragEnterEvent*>(event);
		if( ev->mimeData()->hasFormat("application/OpenSCADA-libwdg") )
		{
		    ev->acceptProposedAction();
		    emit selected(id());
		    return true;
		}
		break;
	    }
	    case QEvent::Drop:
	    {
		QDropEvent *ev = static_cast<QDropEvent*>(event);
		if( ev->mimeData()->hasFormat("application/OpenSCADA-libwdg") )
		{
		    QByteArray itemData = ev->mimeData()->data("application/OpenSCADA-libwdg");
		    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		    QString lwdg;
		    dataStream >> lwdg;
		    //-- Search need action --
		    QPoint curp = mapFromGlobal(cursor().pos());
		    for( int i_a = 0; i_a < mainWin()->actGrpWdgAdd->actions().size(); i_a++ )
			if( mainWin()->actGrpWdgAdd->actions().at(i_a)->objectName() == lwdg )
			    mainWin()->visualItAdd(mainWin()->actGrpWdgAdd->actions().at(i_a),QPointF((float)curp.x()/xScale(true),(float)curp.y()/yScale(true)));

		    ev->accept();
		    return true;
		}
		break;
	    }
	    case QEvent::MouseButtonPress:
	    {
		if( edit() )    break;

		QPoint curp = mapFromGlobal(cursor().pos());

		//> New widget inserting
		QAction *act = mainWin()->actGrpWdgAdd->checkedAction();
		if( act && act->isChecked() && (static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton )
		{
		    mainWin()->visualItAdd(act,QPointF((float)curp.x()/xScale(true),(float)curp.y()/yScale(true)));
		    setCursor(Qt::ArrowCursor);
		    event->accept();
		    return true;
		}

		//> Select widget
		if( (static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton )
		{
		    dragStartPos = ((QMouseEvent*)event)->pos();
		    bool sh_hold = QApplication::keyboardModifiers()&Qt::ShiftModifier;
		    if( cursor().shape() == Qt::ArrowCursor || sh_hold )
		    {
			//>> Scan childs
			bool sel_modif = false;
			bool chld_sel = false;
			DevelWdgView *cwdg = NULL;
			for( int i_c = children().size()-1; i_c >= 0; i_c-- )
			{
			    cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
			    if( !cwdg ) continue;
			    if( cwdg->geometryF().contains(curp) )
			    {
				if( !cwdg->select() )	{ cwdg->setSelect(true,PrcChilds|OnlyFlag);  sel_modif = true; }
				else if( sh_hold )	{ cwdg->setSelect(false,PrcChilds|OnlyFlag); sel_modif = true; }
				if( cwdg->select() )	chld_sel = true;
				break;
			    }
			}
			//>> Select clean for childs
			if( !sh_hold )
			    for( int i_c = 0; i_c < children().size(); i_c++ )
			    {
				DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
				if( !curw || (chld_sel && (curw == cwdg)) )	continue;
				if( curw->select() )	{ curw->setSelect(false,PrcChilds|OnlyFlag); sel_modif = true; }
			    }
			if( sel_modif || !select() )	{ setSelect(true,PrcChilds|OnlyFlag); fSelChange = true; }
			event->accept();

			upMouseCursors(mapFromGlobal(cursor().pos()));

			//>> Update status bar
			mainWin()->statusBar()->showMessage(QString(_("Select elements: '%1'")).
							arg(selectChilds().c_str()), 10000 );

			//>> Hold select rect paint
			if( !chld_sel )
			{
			    fHoldSelRect = true;
			    holdPnt = curp;
			}
		    }
		    if( cursor().shape() != Qt::ArrowCursor )
		    {
			fMoveHold = true;
			holdPnt = curp;
		    }
		    update();	// ???? For QT's included widget's update bug hack (Document,Protocol and other)
		    return true;
		}
		break;
	    }
	    case QEvent::MouseButtonRelease:
	    {
		if( edit() ) break;

		QPoint curp = mapFromGlobal(cursor().pos());

		if( fHoldSelRect )
		{
		    for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		    {
			DevelWdgView *cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
			if( !cwdg || !QRect(holdPnt,curp).contains(cwdg->geometryF().toRect()) ) continue;
			cwdg->setSelect(true,PrcChilds|OnlyFlag);
		    }
		    setSelect(true,PrcChilds);
		    fHoldSelRect = false;
		    //pntView->setSelArea(QRectF());
		}

		//> Check for select next underly widget
		if( fMoveHold && cursor().shape() != Qt::ArrowCursor && !fSelChange && !fMoveHoldMove )
		{
		    DevelWdgView *fsel = NULL, *nsel = NULL;
		    int i_c;
		    for( i_c = children().size()-1; i_c >= 0; i_c-- )
		    {
			DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
			if( !curw ) continue;
			if( !fsel && curw->select() ) fsel = curw;
			else if( fsel && curw->geometryF().contains(curp) ) { nsel = curw; break; }
		    }
		    if( fsel ) fsel->setSelect(false,PrcChilds|OnlyFlag);
		    if( nsel ) nsel->setSelect(true,PrcChilds|OnlyFlag);
		    else setCursor(Qt::ArrowCursor);
		    setSelect(true,PrcChilds);
		}

		if( fSelChange )
		{
		    setSelect(true,PrcChilds|NoUpdate);	// ???? For QT's included widget's update bug hack (Document,Protocol and other)
		    fSelChange = false;
		}

		if( fMoveHold )
		{
		    if( cursor().shape() != Qt::ArrowCursor )
		    {
			vector<DevelWdgView*> lswdgs;
			selectChilds(NULL,&lswdgs);
			if( fMoveHoldMove )
			{
			    if( !lswdgs.size() )
			    {
				saveGeom(id().c_str());
				if( fMakeScale ) load("");
			    }
			    else for( int i_w = 0; i_w < lswdgs.size(); i_w++ )
			    {
				saveGeom(lswdgs[i_w]->id());
				if( fMakeScale ) lswdgs[i_w]->load("");
			    }
			}
			fMakeScale = false;
		    }
		    fMoveHold = fMoveHoldMove = false;
		    return true;
		}

		break;
	    }
	    case QEvent::MouseButtonDblClick:
	    {
		if( edit() )	break;

		//- Enter to Edit mode -
		QPoint curp = mapFromGlobal(cursor().pos());
		DevelWdgView *edwdg = NULL;
		for( int i_c = children().size()-1; i_c >= 0; i_c-- )
		{
		    DevelWdgView *cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
		    if( !cwdg ) continue;
		    if( cwdg->geometryF().contains(curp) && !edwdg )
		    {
		        if( !cwdg->shape || !cwdg->shape->isEditable( ) )	continue;
		        edwdg = cwdg;
		    }
		    else if( cwdg->select() ) cwdg->setSelect(false,PrcChilds);
		}
		if( edwdg && !edwdg->select() )	edwdg->setSelect(true,PrcChilds);
		editEnter( );
		return true;
	    }
	    case QEvent::Wheel:
		if( !(QApplication::keyboardModifiers()&Qt::ControlModifier) ) break;
		setVisScale( visScale()+(float)((QWheelEvent*)event)->delta()/12000 );
		return true;
	    case QEvent::FocusIn:
		if( edit() )    break;
		//-- Unselect and store child widgets --
		//if(select())
		setSelect(true,PrcChilds);
		mainWin()->setWdgScale(false);
		return true;
	    case QEvent::FocusOut:
		if( cursor().shape() != Qt::ArrowCursor )	setCursor(Qt::ArrowCursor);
		if( QApplication::focusWidget() != this && !mainWin()->attrInsp->hasFocus() && !mainWin()->lnkInsp->hasFocus() )
		{
		    if( editWdg )	editWdg->setSelect(false,PrcChilds);
		    //emit selected("");
		    setSelect(false);
		    //-- Unselect and store child widgets --
		    /*for( int i_c = 0; i_c < children().size(); i_c++ )
			if( qobject_cast<DevelWdgView*>(children().at(i_c)) && ((DevelWdgView*)children().at(i_c))->select() )
			    ((DevelWdgView*)children().at(i_c))->setSelect(false,PrcChilds);*/
		}
		return true;
	    case QEvent::MouseMove:
	    {
		if( edit() )	break;

		QPoint curp = mapFromGlobal(cursor().pos());

		//- Select board draw -
		if( fHoldSelRect )
		{
		    pntView->setSelArea(QRect(holdPnt,curp).normalized(),SizePntWdg::SelectBorder);
		    return true;
		}

		//- New widget add cursor view -
		if( mainWin()->actGrpWdgAdd->checkedAction() &&
		    mainWin()->actGrpWdgAdd->checkedAction()->isChecked() )
		{
		    setCursor(QCursor(((VisDevelop *)main_win)->actGrpWdgAdd->
				checkedAction()->icon().pixmap(64,64),0,0));
		    return true;
		}

		//- Update move cursors
		upMouseCursors(curp);

		//- Move widgets control -
		if( fMoveHold && cursor().shape() != Qt::ArrowCursor &&
		    ((QMouseEvent*)event)->buttons()&Qt::LeftButton &&
		    (((QMouseEvent*)event)->pos()-dragStartPos).manhattanLength() >= QApplication::startDragDistance() )
		{
		    dragStartPos = QPoint(-100,-100);
		    wdgsMoveResize(curp-holdPnt);
		    holdPnt = curp;
		    //if( fHoldChild )	update();
		    fMoveHoldMove = true;
		    return true;
		}
		if( fMoveHold && !(((QMouseEvent*)event)->buttons()&Qt::LeftButton) )	fMoveHold = false;

		break;
	    }
	    case QEvent::KeyPress:
	    {
		QKeyEvent *key = static_cast<QKeyEvent*>(event);
		if( edit() && key->key() == Qt::Key_Escape )	{ editExit(); return true; }
		if( !edit() )
		{
		    switch( key->key() )
		    {
			case Qt::Key_A:
			    if( !(QApplication::keyboardModifiers()&Qt::ControlModifier) )	break;
			    for( int i_c = children().size()-1; i_c >= 0; i_c-- )
				if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
				    ((DevelWdgView*)children().at(i_c))->setSelect(true,PrcChilds);
			    setSelect(true,PrcChilds);
			    break;
			case Qt::Key_Left: case Qt::Key_Right: case Qt::Key_Up: case Qt::Key_Down:
			{
			    QPointF dP(0,0);
			    switch( key->key() )
			    {
				case Qt::Key_Left:	dP.setX(-1/xScale(true));	break;
				case Qt::Key_Right:	dP.setX(1/xScale(true));	break;
				case Qt::Key_Up:	dP.setY(-1/yScale(true));	break;
				case Qt::Key_Down:	dP.setY(1/yScale(true));	break;
			    }
			    if( dP.isNull() )	break;
			    dP *= ((QApplication::keyboardModifiers()&Qt::ShiftModifier) ? 1 : 5);
			    wdgsMoveResize(dP);
			    if( cursor().shape() != Qt::ArrowCursor )
				QCursor::setPos((QCursor::pos()+dP).toPoint());
			    //- Save geometry -
			    bool isSel = false;
			    for( int i_c = 0; i_c < children().size(); i_c++ )
				if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
					((DevelWdgView*)children().at(i_c))->select( ) )
				{
				    saveGeom(((DevelWdgView*)children().at(i_c))->id());
				    if( fMakeScale ) ((DevelWdgView*)children().at(i_c))->load("");
				    isSel = true;
				}
			    if( !isSel )
			    {
				if( cursor().shape() == Qt::ArrowCursor )	return false;
				saveGeom(id());
				if( fMakeScale ) load("");
			    }
			    fMakeScale = false;
			    return true;
			}
		    }
		}
	    }
	}

    //- Self widget view -
    if( edit() && editWdg && wLevel() <= 1 && editWdg->shape->event(editWdg,event) )
	return true;

    if( WdgView::event(event) )	return true;
    return QWidget::event(event);
}

//************************************************
//* Size points view widget                      *
//************************************************
SizePntWdg::SizePntWdg( QWidget* parent ) : QWidget(parent), view(SizeDots)
{
    //setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
}

void SizePntWdg::setSelArea( const QRectF &geom, WView iview )
{
    if( view == iview && mWPos == geom.topLeft() && mWSize == geom.size() ) return;

    view   = iview;
    mWPos  = geom.topLeft();
    mWSize = geom.size();
    apply();
}

void SizePntWdg::apply( )
{
    if( mWSize.width() > 2 && mWSize.height() > 2 )
    {
	QRegion reg;
	QRect   wrect, irect;
	switch( view )
	{
	    case SizeDots:
		wrect = QRectF(mWPos,mWSize).adjusted(-3,-3,3,3).toRect();
		irect = QRect(0,0,wrect.width(),wrect.height());
		//- Make widget's mask -
		for(int i_p = 0; i_p < 9; i_p++)
		    if( i_p != 4 )
			reg+=QRegion(QRect(irect.x()+(i_p%3)*((irect.width()-7)/2),
				   irect.y()+(i_p/3)*((irect.height()-7)/2),7,7));
		break;
	    case EditBorder:
		wrect = QRectF(mWPos,mWSize).adjusted(-7,-7,7,7).toRect();
		irect = QRect(0,0,wrect.width(),wrect.height());
		reg = QRegion(irect).subtracted(QRegion(irect.adjusted(7,7,-7,-7)));
		break;
	    case SelectBorder:
		wrect = QRectF(mWPos,mWSize).adjusted(-1,-1,1,1).toRect();
		irect = QRect(0,0,wrect.width(),wrect.height());
		reg = QRegion(irect).subtracted(QRegion(irect.adjusted(1,1,-1,-1)));
		break;
	}
	if( geometry() != wrect )
	{
	    setGeometry(wrect);
	    setMask(reg);
	}
	if( !isVisible() ) show();
    }
    else hide();
}

bool SizePntWdg::event( QEvent *ev )
{
    switch( ev->type() )
    {
	case QEvent::Paint:
	    if( rect().isValid() )
	    {
		QPainter pnt( this );
		pnt.setWindow( rect() );

		switch( view )
		{
		    case SizeDots:
			pnt.setPen(QColor("black"));
			pnt.setBrush(QBrush(QColor("lightgreen")));
			for(int i_p = 0; i_p < 9; i_p++)
			{
			    if( i_p == 4 ) continue;
			    QRect anch(rect().x()+(i_p%3)*((rect().width()-7)/2),
				rect().y()+(i_p/3)*((rect().height()-7)/2),6,6);
			    pnt.drawRect(anch);
			}
			break;
		    case EditBorder:
			pnt.fillRect(rect(),QBrush(Qt::black,Qt::Dense4Pattern));
			pnt.setPen(QColor("black"));
			pnt.drawRect(rect().adjusted(6,6,-7,-7));
			break;
		    case SelectBorder:
			pnt.setPen("white");
			pnt.drawRect(rect().adjusted(0,0,-1,-1));
			QPen pen(QColor("black"));
			pen.setStyle(Qt::DashDotLine);
			pnt.setPen(pen);
			pnt.drawRect(rect().adjusted(0,0,-1,-1));
		    break;
		}
		return true;
	    }
	    break;
	//case QEvent::MouseButtonPress:
	//case QEvent::MouseButtonRelease:
	//    return QApplication::sendEvent(parent(),ev);
    }

    return QWidget::event(ev);
}

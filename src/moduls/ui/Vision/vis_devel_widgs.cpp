
//OpenSCADA system module UI.Vision file: vis_devel_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2005-2007 by Roman Savochenko                           *
 *   rom_as@fromru.com                                                     *
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

#include <tsys.h>

#include "vis_devel.h"
#include "tvision.h"
#include "vis_widgs.h"
#include "vis_devel_widgs.h"

using namespace VISION;


//****************************************
//* Inspector of attributes model        *
//**************************************** 
ModInspAttr::ModInspAttr( const string &iwdg )
{
    rootItem = new Item("wgrp",Item::WdgGrp);
    setWdg(iwdg);
}

ModInspAttr::~ModInspAttr( )
{
    //Delete root item
    delete rootItem;
}

void ModInspAttr::setWdg( const string &iwdg )
{
    bool full_reset = false;
    string sval;
    vector<string> wdg_ls;

    //- Get widgets list -
    int v_cnt = 0;
    while((sval=TSYS::strSepParse(iwdg,v_cnt++,';')).size())
        wdg_ls.push_back(sval);
    if( wdg_ls.size() == 0 )
    {
	delete rootItem;
	rootItem = new Item("",Item::Wdg);
	full_reset = true;
    }
    else if( wdg_ls.size() == 1 )
    {
        //- Set one widget. Check for change root item -
        if( rootItem->type() != Item::Wdg || rootItem->id() != wdg_ls[0] )
        {
            delete rootItem;
            rootItem = new Item(wdg_ls[0],Item::Wdg);
	    full_reset = true;
        }
	//-- Update attributes --
    	wdgAttrUpdate(rootItem);	    
    }    
    else if( wdg_ls.size() > 1 )
    {
        //- Set group widget -
        if( rootItem->type() != Item::WdgGrp )
        {
            delete rootItem;
            rootItem = new Item("wgrp",Item::WdgGrp);
	    full_reset = true;
        }
        //- Check for delete widgets from group -
        for( int i_it = 0; i_it < rootItem->childCount(); i_it++ )
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
        //- Add new items and update attributes -
        for( int i_w = 0; i_w < wdg_ls.size(); i_w++ )
        {
            int row = rootItem->childGet(wdg_ls[i_w]);
            if( row < 0 )
	    {
		beginInsertRows(QModelIndex(),i_w,i_w);
                row = rootItem->childInsert(wdg_ls[i_w],i_w,Item::Wdg);
		endInsertRows();
	    }
	    wdgAttrUpdate(rootItem->child(i_w));
        }
    }

    if( full_reset )	reset();
    else emit layoutChanged();
}

void ModInspAttr::wdgAttrUpdate(Item *it)
{
    if( it->type() != Item::Wdg )   return;

    try
    {
        string wlib_id = TSYS::strSepParse(it->id(),0,'.');
        string wdg_id = TSYS::strSepParse(it->id(),1,'.');
        string wdgc_id = TSYS::strSepParse(it->id(),2,'.');

        //- Connect to widget -
        AutoHD<VCA::Widget> wdg;
        if( wdgc_id.size() )
            wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
        else if( wdg_id.size() )
            wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
        if( wdg.freeStat() )    return;

        //- Set/update widget name -
        it->setName( wdg.at().name() );
        
        //- Get attributes list -
        vector<string> als;
        wdg.at().attrList(als);

        //- Delete items of a no present attributes -
        //???

        //- Add items for present attributes -
        for( int i_a = 0; i_a < als.size(); i_a++ )
        {
            string a_nm = wdg.at().attrAt(als[i_a]).at().name();
            Item *cur_it = it;
            //-- Parse attributes group --
            if(TSYS::strSepParse(a_nm,1,':').size())
                for(int i_l = 0; true; i_l++)
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
            //-- Check attribute item --
            int ga_id = cur_it->childGet(als[i_a]);
            if( ga_id < 0 )
	    {
		ga_id = cur_it->childInsert(als[i_a],-1,Item::Attr);
		cur_it->child(ga_id)->setEdited(!(wdg.at().attrAt(als[i_a]).at().flgGlob()&TFld::NoWrite));
		cur_it->child(ga_id)->setFlag(wdg.at().attrAt(als[i_a]).at().flgGlob());
	    }
            cur_it->child(ga_id)->setName(a_nm);
            if( cur_it->child(ga_id)->flag()&TFld::Selected )
            {
                cur_it->child(ga_id)->setData(wdg.at().attrAt(als[i_a]).at().getSEL().c_str());
                cur_it->child(ga_id)->setDataEdit(QString(wdg.at().attrAt(als[i_a]).at().fld().selNames().c_str()).split(";"));
            }
            else switch(wdg.at().attrAt(als[i_a]).at().type())
            {
                case TFld::Boolean:
                    cur_it->child(ga_id)->setData(wdg.at().attrAt(als[i_a]).at().getB());
                    break;
                case TFld::Integer:
                    cur_it->child(ga_id)->setData(wdg.at().attrAt(als[i_a]).at().getI());
                    break;
                case TFld::Real:
                    cur_it->child(ga_id)->setData(wdg.at().attrAt(als[i_a]).at().getR());
                    break;
                case TFld::String:
                    cur_it->child(ga_id)->setData(wdg.at().attrAt(als[i_a]).at().getS().c_str());
                    break;
            }
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

QModelIndex ModInspAttr::parent(const QModelIndex &index) const
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

        if( index.column() == 0 && role == Qt::DisplayRole )
            val = it->name().c_str();
        if( index.column() == 1 )
            switch(role)
            {
                case Qt::DisplayRole:   val = it->data();   	break;
                case Qt::EditRole:      val = it->dataEdit();   break;
		case Qt::UserRole:	val = it->flag();	break;
            }
    }
    return val;
}

bool ModInspAttr::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if( !index.isValid() )  return false;

    //Attribute
    Item *it = static_cast<Item*>(index.internalPointer());
    string nattr = it->id();
    //Attribute widget
    string nwdg;
    Item *cit = it;
    while(cit)
        if(cit->type() == Item::Wdg)
        {
            nwdg = cit->id();
            break;
        }
        else cit = cit->parent();
    string wlib_id = TSYS::strSepParse(nwdg,0,'.');
    string wdg_id = TSYS::strSepParse(nwdg,1,'.');
    string wdgc_id = TSYS::strSepParse(nwdg,2,'.');

    try
    {
        //- Connect to widget -
        AutoHD<VCA::Widget> wdg;
        if( wdgc_id.size() )
            wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
        else if( wdg_id.size() )
            wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
        if( wdg.freeStat() )    return false;

        if( wdg.at().attrAt(nattr).at().flgGlob()&TFld::Selected )
	{
            wdg.at().attrAt(nattr).at().setSEL(value.toString().toAscii().data());
	    it->setData(value.toString());
	}
        else switch(wdg.at().attrAt(nattr).at().type())
        {
            case TFld::Boolean:
                wdg.at().attrAt(nattr).at().setB(value.toBool());
		it->setData(value.toBool());
                break;
            case TFld::Integer:
                wdg.at().attrAt(nattr).at().setI(value.toInt());
		it->setData(value.toInt());
                break;
            case TFld::Real:
                wdg.at().attrAt(nattr).at().setR(value.toDouble());
		it->setData(value.toDouble());
                break;
            case TFld::String:
                wdg.at().attrAt(nattr).at().setS(value.toString().toAscii().data());
		it->setData(value.toString());
                break;
        }	
        //it->setData(value);

	emit modified(nwdg);
    }catch(...){ return false; }        

    emit dataChanged(index,index);
    return true;
}

//* Item of the inspector of attributes model  *
ModInspAttr::Item::Item( const string &iid, Type tp, Item *parent ) :
        idItem(iid), parentItem(parent), typeItem(tp), edit_access(false), flag_item(0)
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

//****************************************
//* Inspector of attributes widget       *
//****************************************
InspAttr::InspAttr( QWidget * parent ) : QTreeView(parent)
{
    //setEditTriggers(QAbstractItemView::AllEditTriggers);
    setAlternatingRowColors(true);
    setModel(&modelData);
    setItemDelegate(new ItemDelegate);
    connect(&modelData, SIGNAL(modified(const string &)), this, SIGNAL(modified(const string &)));
}

InspAttr::~InspAttr( )
{

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

//* Attributes item delegate    *
InspAttr::ItemDelegate::ItemDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

QWidget *InspAttr::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid()) return 0;

    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();

    if( flag&TFld::Selected )	w_del = new QComboBox(parent);
    else if( value.type()==QVariant::String && flag&TFld::FullText )
    {
	w_del = new QTextEdit(parent);
	((QTextEdit*)w_del)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->resize(50,50);
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

    if( flag&TFld::Selected )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        comb->addItems(value.toStringList());
        comb->setCurrentIndex(comb->findText(index.data(Qt::DisplayRole).toString()));
    }
    else if( value.type()==QVariant::String && flag&TFld::FullText )
    {
	QTextEdit *ted = dynamic_cast<QTextEdit*>(editor);
	ted->setPlainText(value.toString());
    }
    else QItemDelegate::setEditorData(editor, index);
}

void InspAttr::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    int flag = index.data(Qt::UserRole).toInt();
    
    if( flag&TFld::Selected )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        model->setData(index,comb->currentText(),Qt::EditRole);
    }
    else if( value.type()==QVariant::String && flag&TFld::FullText )
    {
	QTextEdit *ted = dynamic_cast<QTextEdit*>(editor);
	model->setData(index,ted->toPlainText(),Qt::EditRole);
    }
    else QItemDelegate::setModelData(editor, model, index);
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
InspAttrDock::InspAttrDock( QWidget * parent ) : QDockWidget(_("Attributes"),parent)
{
    setObjectName("InspAttrDock");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    
    ainsp_w = new InspAttr(this);
    setWidget(ainsp_w);
    connect(ainsp_w, SIGNAL(modified(const string &)), this, SIGNAL(modified(const string &)));
}

InspAttrDock::~InspAttrDock( )
{

}

void InspAttrDock::setWdg( const string &iwdg )
{
    if( QApplication::focusWidget() != ainsp_w )
        ainsp_w->setWdg(iwdg);
}

//****************************************
//* Inspector of links dock widget       *
//****************************************
InspLnk::InspLnk( QWidget * parent ) : QDockWidget(_("Links"),parent)
{
    setObjectName("InspLnk");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
}

InspLnk::~InspLnk( )
{

}
 
//****************************************
//* Widget's libraries tree              *
//****************************************
WdgTree::WdgTree( VisDevelop * parent ) : 
    QDockWidget(_("Widgets"),(QWidget*)parent)
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
    treeW->header()->resizeSection(0,120);
    //- Connect to signals -
    connect( treeW, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( ctrTreePopup() ) );
    connect( treeW, SIGNAL( itemSelectionChanged() ), this, SLOT( selectItem() ) );
    connect( treeW, SIGNAL( itemPressed(QTreeWidgetItem*,int) ), this, SLOT( pressItem(QTreeWidgetItem*) ) );
    
    setWidget(treeW);

    treeW->installEventFilter(this);
    
    updateLibs();
}

WdgTree::~WdgTree()
{

}

VisDevelop *WdgTree::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

bool WdgTree::eventFilter( QObject *target, QEvent *event )
{
    if( target == treeW )
    {
	if( event->type() == QEvent::FocusIn )	selectItem( );
	if( event->type() == QEvent::FocusOut && QApplication::focusWidget() != treeW )
	    owner()->selectItem("");
    }
    return QDockWidget::eventFilter( target, event );
}

void WdgTree::selectItem( )
{
    //Get select list
    QList<QTreeWidgetItem *> sel_ls = treeW->selectedItems();
    if(sel_ls.size() != 1)	return;
    
    //Get current widget
    string work_wdg = sel_ls.at(0)->text(2).toAscii().data();
    QTreeWidgetItem *cur_el = sel_ls.at(0)->parent();
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->text(2).toAscii().data())+".");
	cur_el=cur_el->parent();
    }
    
    owner()->selectItem(work_wdg);
}

void WdgTree::pressItem(QTreeWidgetItem *item)
{
    int w_lev = 0;
    //Get select list
    if( !item )	return;
    
    //Get current widget
    string work_wdg = item->text(2).toAscii().data();
    QTreeWidgetItem *cur_el = item->parent();
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->text(2).toAscii().data())+".");
	cur_el=cur_el->parent();
	w_lev++;
    }
    
    //Prepare for drag and drop operation
    if( owner()->work_space->activeWindow() && w_lev == 1 )
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

void WdgTree::updateLibs()
{    
    int i_l, i_w, i_cw, i_top, i_topwl, i_topcwl;
    QTreeWidgetItem *nit, *nit_w, *nit_cw;
    vector<string> list_wl, list_w, list_wc;

    //- Get widget's libraries list -
    mod->engine().at().wlbList(list_wl);
    //- Remove no present libraries -
    for(i_top = 0; i_top < treeW->topLevelItemCount(); i_top++)
    {
	nit = treeW->topLevelItem(i_top);
	for( i_l = 0; i_l < list_wl.size(); i_l++ )
	    if( list_wl[i_l] == nit->text(2).toAscii().data() )
		break;
	if( i_l < list_wl.size() )	continue;
	delete treeW->takeTopLevelItem(i_top);
	i_top--;
    }

    //- Add new libraries -
    for( i_l = 0; i_l < list_wl.size(); i_l++ )
    {
	for( i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
    	    if( list_wl[i_l] == treeW->topLevelItem(i_top)->text(2).toAscii().data() )
		break;
	if( i_top >= treeW->topLevelItemCount() )
	    nit = new QTreeWidgetItem(treeW);
	else nit = treeW->topLevelItem(i_top);

	//-- Update libraries data --
	AutoHD<VCA::WidgetLib> wlb = mod->engine().at().wlbAt(list_wl[i_l]);
	string simg = TSYS::strDecode(wlb.at().ico(),TSYS::base64);
	QImage img;
	if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
            nit->setIcon(0,QPixmap::fromImage(img));
	nit->setText(0,wlb.at().name().c_str());
	nit->setText(1,_("Library"));
	nit->setText(2,list_wl[i_l].c_str());

	//-- Update librarie's widgets --
	//--- Remove no present widgets ---
	wlb.at().list(list_w);
	for( i_topwl = 0; i_topwl < nit->childCount(); i_topwl++ )
	{
	    nit_w = nit->child(i_topwl);
	    for( i_w = 0; i_w < list_w.size(); i_w++ )
		if( list_w[i_w] == nit_w->text(2).toAscii().data() )
		    break;
	    if( i_w < list_w.size() )	continue;
	    delete nit->takeChild(i_topwl);
	    i_topwl--;
	}
	//--- Add new widgets ---
	for( i_w = 0; i_w < list_w.size(); i_w++ )
	{
	    for( i_topwl = 0; i_topwl < nit->childCount(); i_topwl++ )
    		if(list_w[i_w] == nit->child(i_topwl)->text(2).toAscii().data())
		    break;
	    if( i_topwl >= nit->childCount() )
		nit_w = new QTreeWidgetItem(nit);
	    else nit_w = nit->child(i_topwl);

	    //--- Update widget's data ---
	    AutoHD<VCA::Widget> wdg = wlb.at().at(list_w[i_w]);
	    string simg = TSYS::strDecode(wdg.at().ico(),TSYS::base64);
	    QImage img;
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
        	nit_w->setIcon(0,QPixmap::fromImage(img));
	    nit_w->setText(0,wdg.at().name().c_str());
	    nit_w->setText(1,_("Widget"));	    
	    nit_w->setText(2,list_w[i_w].c_str());

	    //--- Update container's widgets ---
	    if( !wdg.at().isContainer() )	continue;
	    //---- Remove no present widgets ----
	    wdg.at().wdgList(list_wc);
	    for( i_topcwl = 0; i_topcwl < nit_w->childCount(); i_topcwl++ )
	    {
		nit_cw = nit_w->child(i_topcwl);
		for( i_cw = 0; i_cw < list_wc.size(); i_cw++ )
		    if( list_wc[i_cw] == nit_cw->text(2).toAscii().data() )
			break;
		if( i_cw < list_wc.size() )	continue;
		delete nit_w->takeChild(i_topcwl);
		i_topcwl--;
	    }
	    //---- Add new widgets ----
	    for( i_cw = 0; i_cw < list_wc.size(); i_cw++ )
	    {
		for( i_topcwl = 0; i_topcwl < nit_w->childCount(); i_topcwl++ )
    		    if(list_wc[i_cw] == nit_w->child(i_topcwl)->text(2).toAscii().data())
			break;
		if( i_topcwl >= nit_w->childCount() )
		    nit_cw = new QTreeWidgetItem(nit_w);
		else nit_cw = nit_w->child(i_topcwl);

		//--- Update widget's data ---
		AutoHD<VCA::Widget> cwdg = wdg.at().wdgAt(list_wc[i_cw]);
		string simg = TSYS::strDecode(cwdg.at().ico(),TSYS::base64);
		QImage img;
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
        	    nit_cw->setIcon(0,QPixmap::fromImage(img));
		nit_cw->setText(0,cwdg.at().name().c_str());
		nit_cw->setText(1,_("Container widget"));
		nit_cw->setText(2,list_wc[i_cw].c_str());
	    }
	}
    }
    
    treeW->resizeColumnToContents(0);
}

void WdgTree::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    //Add actions
    popup.addAction(owner()->actWdgLibAdd);
    popup.addAction(owner()->actWdgLibDel);
    popup.addAction(owner()->actWdgLibProp);
    popup.addSeparator();    
    popup.addAction(owner()->actWdgAdd);
    popup.addAction(owner()->actWdgDel);
    popup.addAction(owner()->actWdgProp);
    popup.addAction(owner()->actWdgEdit);

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

    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(1);
    treeWidget->setItemsExpandable(true);
    for (int i = 0; i < 10; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget,QStringList(QString("item: %1").arg(i)));
	if(i==3) item->setFlags(Qt::ItemIsEnabled);
    }
    setWidget(treeWidget);
}

ProjTree::~ProjTree()
{

}


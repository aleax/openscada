
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
#include <QTreeView>
#include <QItemEditorFactory>
#include <QVBoxLayout>
#include <QMenu>
#include <QEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QBuffer>

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
    string sval;
    vector<string> wdg_ls;

    //- Get widgets list -
    int v_cnt = 0;
    while((sval=TSYS::strSepParse(iwdg,v_cnt++,';')).size())
        wdg_ls.push_back(sval);
    if( wdg_ls.size() == 0 )    rootItem->clean();
    else if( wdg_ls.size() == 1 )
    {
        //- Set one widget. Check for change root item -
        if( rootItem->type() != Item::Wdg || rootItem->id() != wdg_ls[0] )
        {
            delete rootItem;
            rootItem = new Item(wdg_ls[0],Item::Wdg);
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
                rootItem->childDel(i_it);
                i_it--;
            }
        }
        //- Add new items and update attributes -
        for( int i_w = 0; i_w < wdg_ls.size(); i_w++ )
        {
            int row = rootItem->childGet(wdg_ls[i_w]);
            if( row < 0 )
                row = rootItem->childInsert(wdg_ls[i_w],i_w,Item::Wdg);
            
            wdgAttrUpdate(rootItem->child(i_w));
        }
    }

    reset();
    layoutChanged();
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
            if( ga_id < 0 ) ga_id = cur_it->childInsert(als[i_a],-1,Item::Attr);
            cur_it->child(ga_id)->setName(a_nm);
            if( wdg.at().attrAt(als[i_a]).at().flgGlob()&TFld::Selected )
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
    if( !index.isValid() )  return Qt::ItemIsEnabled;
    if( index.column() == 1 &&
            static_cast<Item*>(index.internalPointer())->type() == Item::Attr )
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
                case Qt::DisplayRole:   val = it->data();   break;
                case Qt::EditRole:      val = it->dataEdit();   break;
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
        idItem(iid), parentItem(parent), typeItem(tp)
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

    if(value.type() == QVariant::StringList)
        w_del = new QComboBox(parent);
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

    if(dynamic_cast<QComboBox*>(editor) && value.type() == QVariant::StringList)
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        comb->addItems(value.toStringList());
        comb->setCurrentIndex(comb->findText(index.data(Qt::DisplayRole).toString()));
    }
    else QItemDelegate::setEditorData(editor, index);
}

void InspAttr::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    
    if(dynamic_cast<QComboBox*>(editor) && value.type() == QVariant::StringList)
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        model->setData(index,comb->currentText(),Qt::EditRole);
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

//****************************************
//* Library properties dialog            *
//****************************************
WdgLibProp::WdgLibProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), is_modif(false), ico_modif(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *glay;
    QImage ico_t;
    setWindowTitle(_("Widget's library properties"));
    setWindowIcon(owner()->actWdgLibProp->icon());
    
    QGridLayout *dlg_lay = new QGridLayout(this);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    //- State parameters -
    grp = new QGroupBox(_("State"),this);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);
    wlb_ico = new QPushButton(this);
    wlb_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    wlb_ico->setIconSize(QSize(60,60));
    wlb_ico->setAutoDefault(false);
    connect(wlb_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(wlb_ico,0,0,3,1);
    lab = new QLabel(_("Enabled:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,1);
    wlb_enable = new QCheckBox(this);
    connect(wlb_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(wlb_enable,0,2,1,2);    
    lab = new QLabel(_("Library DB:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);
    wlb_db = new QLineEdit(this);
    connect(wlb_db, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wlb_db,1,2,1,2);
    lab = new QLabel(_("User and group:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);    
    wlb_user = new QComboBox(this);
    connect(wlb_user, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectUser(const QString&)));
    glay->addWidget(wlb_user,2,2);
    wlb_grp = new QComboBox(this);
    connect(wlb_grp, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectPermission()));        
    glay->addWidget(wlb_grp,2,3);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,0);
    
    //- Access parameters -
    grp = new QGroupBox(_("Access"),this);
    QStringList perm_ls;
    perm_ls << _("No access") << _("Use(open)") << _("Modify") << _("Full");        
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);    
    lab = new QLabel(_("User:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    wlb_accuser = new QComboBox(this);
    wlb_accuser->addItems(perm_ls);    
    connect(wlb_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accuser,0,1);    
    lab = new QLabel(_("Group:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);    
    wlb_accgrp  = new QComboBox(this);
    wlb_accgrp->addItems(perm_ls);    
    connect(wlb_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accgrp,1,1);    
    lab = new QLabel(_("Other:"),this);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    wlb_accother= new QComboBox(this);
    wlb_accother->addItems(perm_ls);    
    connect(wlb_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wlb_accother,2,1);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,1);    
    
    //- Config parameters -
    grp = new QGroupBox(_("Configuration"),this);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    glay->addWidget(new QLabel(_("Id:"),this),0,0);
    wlb_id = new QLabel(this);
    wlb_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(wlb_id,0,1);    
    glay->addWidget(new QLabel(_("Name:"),this),1,0);
    wlb_name = new QLineEdit(this);
    connect(wlb_name, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wlb_name,1,1);  
    glay->addWidget(new QLabel(_("Description:"),this),2,0);
    wlb_descr = new QTextEdit(this);
    connect(wlb_descr, SIGNAL(textChanged()), this, SLOT(isModify()));
    glay->addWidget(wlb_descr,3,0,1,2);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);    

    //- Button box -
    butbox = new QDialogButtonBox(QDialogButtonBox::Apply|
				  QDialogButtonBox::Cancel|
				  QDialogButtonBox::Close, Qt::Horizontal,this);
    //-- Init Apply button --
    butbox->button(QDialogButtonBox::Apply)->setText(_("Apply"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    dlg_lay->addWidget(butbox,2,0,1,2);
    
    resize(500,400);    
}

WdgLibProp::~WdgLibProp( )
{

}

VisDevelop *WdgLibProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

string WdgLibProp::user()
{
    return wlb_user->currentText().toAscii().data();
}

string WdgLibProp::grp()
{
    if(wlb_grp->currentText().isEmpty())
	return "UI";
    return wlb_grp->currentText().toAscii().data();
}		

short WdgLibProp::permit()
{
    string accs;
    short permit = 0, wperm;

    for(int i_p = 0; i_p < 3; i_p++)
    {
	switch(i_p)
	{
	    case 0: accs = wlb_accother->currentText().toAscii().data();break;
	    case 1: accs = wlb_accgrp->currentText().toAscii().data();	break;		
	    case 2: accs = wlb_accuser->currentText().toAscii().data();	break;
	}
	wperm = 0;
	if( accs == _("Use(open)") )	wperm = 4;
	else if( accs == _("Modify") )	wperm = 2;
	else if( accs == _("Full") )	wperm = 6;

	permit|=(wperm<<(i_p*3));
    }
    
    return permit;
}

void WdgLibProp::setPermit( short vl )
{
    short wperm = vl;
    string accs;
    
    for(int i_p = 0; i_p < 3; i_p++)
    {
        switch(wperm&0x7)
        {
    	    case 0: accs = _("No access");	break;
	    case 4: accs = _("Use(open)");	break;
	    case 2: accs = _("Modify");	break;
	    case 6: accs = _("Full");	break;
	}
	switch(i_p)
	{
	    case 0: wlb_accother->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;
	    case 1: wlb_accgrp->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;		
	    case 2: wlb_accuser->setCurrentIndex(wlb_accother->findText(accs.c_str()));	break;
	}
	wperm>>=3;
    }
}
 
void WdgLibProp::showDlg( const string &ilb )
{
    vector<string> ls;
    QImage ico_t;
    AutoHD<VCA::WidgetLib> wlb;
    ed_lib = ilb;    

    //- Load library data -
    try{ wlb = mod->engine().at().wlbAt(ilb); }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Library '%1' no present.")).arg(ilb.c_str()),TVision::Warning, this );
	return;
    }

    //-- Load library icon --
    string simg = TSYS::strDecode(wlb.at().ico(),TSYS::base64);
    if(ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()))
	wlb_ico->setIcon(QPixmap::fromImage(ico_t));
    else wlb_ico->setIcon(QIcon());
    //setWindowIcon(QPixmap::fromImage(ico_t));
    //-- Load library state --
    wlb_enable->setChecked(wlb.at().enable());
    //-- Load library DB --
    wlb_db->setText(wlb.at().fullDB().c_str());
    //-- Load users and groups --
    SYS->security().at().usrList(ls);
    //--- Delete users ---
    wlb_user->clear();
    for(int i_l = 0; i_l < ls.size(); i_l++)
    {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(ls[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
            wlb_user->addItem(QPixmap::fromImage(img),ls[i_l].c_str());
        else wlb_user->addItem(ls[i_l].c_str());
	if( wlb.at().user() == ls[i_l] ) wlb_user->setCurrentIndex(i_l);
    }
    wlb_grp->setCurrentIndex(wlb_grp->findText(wlb.at().grp().c_str()));
    setPermit(wlb.at().permit());
    
    //selectGroup(wlb.at().grp().c_str());
    //-- Load library identifier, name and description --    
    wlb_id->setText(wlb.at().id().c_str());
    wlb_name->setText(wlb.at().name().c_str());
    wlb_descr->setPlainText(wlb.at().descr().c_str());

    //- Disable OK and Cancel buttons -
    is_modif = false;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    //- Show dialog -
    show();
    raise();
    activateWindow();
}

void WdgLibProp::selectIco( )
{
    QImage ico_t;
    
    if( !ico_modif )	return;
    
    QString fileName = QFileDialog::getOpenFileName(this,_("Load icon picture"),"",_("Images (*.png *.jpg)"));
    
    if( fileName.isEmpty() )	return;
    if(!ico_t.load(fileName))
    {
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Loaded icon image '%1' error.")).arg(fileName),TVision::Warning, this );
	return;    
    }
    
    wlb_ico->setIcon(QPixmap::fromImage(ico_t));
    
    isModify();
}

void WdgLibProp::isModify( )
{
    is_modif = true;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(true);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(true);    
}

void WdgLibProp::pressApply( )
{    
    AutoHD<VCA::WidgetLib> wlb;
    //- Download modified data -

    try
    { 
	//-- Open library --    
	wlb = mod->engine().at().wlbAt(ed_lib); 

	//-- Save library icon --
	if(!wlb_ico->icon().isNull())
	{
	    QByteArray ba;
    	    QBuffer buffer(&ba);
	    buffer.open(QIODevice::WriteOnly);
	    wlb_ico->icon().pixmap(64,64).save(&buffer,"PNG");
	    wlb.at().setIco(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
	}
	//-- Save library DB --
	wlb.at().setFullDB(wlb_db->text().toAscii().data());
	//-- Save users and groups --
	wlb.at().setUser(user());
	wlb.at().setGrp(grp());
	//-- Set permition --
	wlb.at().setPermit(permit());
	//-- Save library name and description --
	wlb.at().setName(wlb_name->text().toAscii().data());
	wlb.at().setDescr(wlb_descr->toPlainText().toAscii().data());    
	//-- Save library state --
	wlb.at().setEnable(wlb_enable->isChecked());	
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Library '%1' error: %2")).arg(ed_lib.c_str()).arg(err.mess.c_str()), 
		TVision::Warning, this );
	return;
    }    
    
    //- Update widgets tree and toolbars -
    owner()->wdgTree->updateLibs();
    owner()->updateLibToolbar();
    
    //- Reload date -
    showDlg(ed_lib);
}

void WdgLibProp::pressCancel( )
{    
    showDlg(ed_lib);
}

void WdgLibProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,wlb_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_lib = "";
    hide();
}

void WdgLibProp::selectUser(const QString &val)
{
    //Get default users group
    vector<string> gls;
    SYS->security().at().usrGrpList(val.toAscii().data(),gls);    
    string vgrp = gls.size()?gls[0].c_str():"UI";
    wlb_grp->clear();
    for(int i_l = 0; i_l < gls.size(); i_l++)
    {
	wlb_grp->addItem(gls[i_l].c_str());
        if( vgrp == gls[i_l] ) wlb_grp->setCurrentIndex(i_l);
    }
    selectPermission();    
}

void WdgLibProp::selectPermission( )
{
    wlb_enable->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWRWR_));
    wlb_db->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_user->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,"root","root",RWRWR_));
    wlb_grp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accuser->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accgrp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_accother->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wlb_name->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wlb_descr->setEnabled( SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    ico_modif = SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit());
    
    isModify( );    
}

//****************************************
//* Widget properties dialog             *
//****************************************
WdgProp::WdgProp( VisDevelop *parent ) : 
    QDialog((QWidget*)parent), is_modif(false), ico_modif(false)
{
    QLabel *lab;
    QGroupBox *grp;
    QGridLayout *dlg_lay, *glay;
    QImage ico_t;
    setWindowTitle(_("Widget properties"));
    setWindowIcon(owner()->actWdgProp->icon());

    //- Create tabulator -
    QVBoxLayout *tab_lay = new QVBoxLayout(this);
    tab_lay->setMargin(5);
    wdg_tabs = new QTabWidget(this);
    tab_lay->addWidget(wdg_tabs);
    
    //- Add tab 'Widget' -
    //--------------------
    wdg_tabs->addTab(new QWidget,_("Widget"));    
    QWidget *tab_w = wdg_tabs->widget(0);
    
    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);
    
    //-- State parameters --
    grp = new QGroupBox(_("State"),tab_w);
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);    
    wdg_ico = new QPushButton(tab_w);
    wdg_ico->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum) );
    wdg_ico->setIconSize(QSize(60,60));
    wdg_ico->setAutoDefault(false);
    connect(wdg_ico, SIGNAL(released()), this, SLOT(selectIco()));
    glay->addWidget(wdg_ico,0,0,3,1);    
    lab = new QLabel(_("Enabled:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,0,1);
    wdg_enable = new QCheckBox(tab_w);
    connect(wdg_enable, SIGNAL(stateChanged(int)), this, SLOT(isModify()));
    glay->addWidget(wdg_enable,0,2,1,2);    
    lab = new QLabel(_("Parent widget:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,1,1);    
    wdg_parent = new QComboBox(tab_w);
    connect(wdg_parent, SIGNAL(activated(const QString&)), this, SLOT(selectParent(const QString&)));
    glay->addWidget(wdg_parent,1,2,1,2);    
    lab = new QLabel(_("User and group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );    
    glay->addWidget(lab,2,1);
    wdg_user = new QComboBox(tab_w);
    connect(wdg_user, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectUser(const QString&)));
    glay->addWidget(wdg_user,2,2);    
    wdg_grp = new QComboBox(tab_w);
    connect(wdg_grp, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_grp,2,3);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,0,0);
    
    //-- Access parameters --
    grp = new QGroupBox(_("Access"),tab_w);
    QStringList perm_ls;
    perm_ls << _("No access") << _("Use(open)") << _("Modify") << _("Full");        
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    lab = new QLabel(_("User:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,0,0);
    wdg_accuser = new QComboBox(tab_w);
    wdg_accuser->addItems(perm_ls);    
    connect(wdg_accuser, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accuser,0,1);    
    lab = new QLabel(_("Group:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,1,0);
    wdg_accgrp  = new QComboBox(tab_w);
    wdg_accgrp->addItems(perm_ls);    
    connect(wdg_accgrp, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accgrp,1,1);    
    lab = new QLabel(_("Other:"),tab_w);
    lab->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred) );
    glay->addWidget(lab,2,0);
    wdg_accother= new QComboBox(tab_w);
    wdg_accother->addItems(perm_ls);    
    connect(wdg_accother, SIGNAL(currentIndexChanged(int)), this, SLOT(selectPermission()));
    glay->addWidget(wdg_accother,2,1);
    grp->setLayout(glay);    
    dlg_lay->addWidget(grp,0,1);
    
    //-- Config parameters --
    grp = new QGroupBox(_("Configuration"),tab_w);    
    glay = new QGridLayout;
    glay->setMargin(4);
    glay->setSpacing(6);        
    glay->addWidget(new QLabel(_("Id:"),tab_w),0,0);
    wdg_id = new QLabel(tab_w);
    wdg_id->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    glay->addWidget(wdg_id,0,1);    
    glay->addWidget(new QLabel(_("Name:"),tab_w),1,0);
    wdg_name = new QLineEdit(tab_w);
    connect(wdg_name, SIGNAL(textChanged(const QString&)), this, SLOT(isModify()));
    glay->addWidget(wdg_name,1,1);    
    glay->addWidget(new QLabel(_("Description:"),tab_w),2,0);
    wdg_descr = new QTextEdit(tab_w);
    connect(wdg_descr, SIGNAL(textChanged()), this, SLOT(isModify()));
    glay->addWidget(wdg_descr,3,0,1,2);
    grp->setLayout(glay);
    dlg_lay->addWidget(grp,1,0,1,2);

    //- Add tab 'Attributes' -
    //------------------------
    wdg_tabs->addTab(new QWidget,_("Attributes and links"));
    tab_w = wdg_tabs->widget(1);

    dlg_lay = new QGridLayout(tab_w);
    dlg_lay->setMargin(9);
    dlg_lay->setSpacing(6);

    //-- Add model and viewer for attributes --
    wdg_attr = new InspAttr(tab_w);
    dlg_lay->addWidget(wdg_attr,0,0);

    //- Add button box -
    //------------------
    butbox = new QDialogButtonBox(QDialogButtonBox::Apply|
				  QDialogButtonBox::Cancel|
				  QDialogButtonBox::Close, Qt::Horizontal,this);
    //-- Init Apply button --
    butbox->button(QDialogButtonBox::Apply)->setText(_("Apply"));
    if(!ico_t.load(TUIS::icoPath("button_ok").c_str())) ico_t.load(":/images/button_ok.png");
    butbox->button(QDialogButtonBox::Apply)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(pressApply()));    
    //-- Init Cancel button --
    butbox->button(QDialogButtonBox::Cancel)->setText(_("Cancel"));
    if(!ico_t.load(TUIS::icoPath("button_cancel").c_str())) ico_t.load(":/images/button_cancel.png");
    butbox->button(QDialogButtonBox::Cancel)->setIcon(QPixmap::fromImage(ico_t));
    butbox->button(QDialogButtonBox::Cancel)->setShortcut(QKeySequence("Esc"));
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);
    connect(butbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(pressCancel()));
    //-- Init close button --
    connect(butbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(pressClose()));

    tab_lay->addWidget(butbox);

    //- End resize -
    //--------------
    resize(500,400);    
}

WdgProp::~WdgProp( )
{

}

VisDevelop *WdgProp::owner()
{
    return (VISION::VisDevelop*)parentWidget();
}

string WdgProp::user()
{
    return wdg_user->currentText().toAscii().data();
}

string WdgProp::grp()
{
    if(wdg_grp->currentText().isEmpty())
	return "UI";
    return wdg_grp->currentText().toAscii().data();
}		

short WdgProp::permit()
{
    string accs;
    short permit = 0, wperm;

    for(int i_p = 0; i_p < 3; i_p++)
    {
	switch(i_p)
	{
	    case 0: accs = wdg_accother->currentText().toAscii().data();break;
	    case 1: accs = wdg_accgrp->currentText().toAscii().data();	break;		
	    case 2: accs = wdg_accuser->currentText().toAscii().data();	break;
	}
	wperm = 0;
	if( accs == _("Use(open)") )	wperm = 4;
	else if( accs == _("Modify") )	wperm = 2;
	else if( accs == _("Full") )	wperm = 6;
	    
	permit|=(wperm<<(i_p*3));
    }
    
    return permit;
}

void WdgProp::setPermit( short vl )
{
    short wperm = vl;
    string accs;
    
    for(int i_p = 0; i_p < 3; i_p++)
    {
        switch(wperm&0x7)
        {
    	    case 0: accs = _("No access");	break;
	    case 4: accs = _("Use(open)");	break;
	    case 2: accs = _("Modify");	break;
	    case 6: accs = _("Full");	break;
	}
	switch(i_p)
	{
	    case 0: wdg_accother->setCurrentIndex(wdg_accother->findText(accs.c_str()));	break;
	    case 1: wdg_accgrp->setCurrentIndex(wdg_accother->findText(accs.c_str()));	break;		
	    case 2: wdg_accuser->setCurrentIndex(wdg_accother->findText(accs.c_str()));	break;
	}
	wperm>>=3;
    }
}
 
void WdgProp::showDlg( const string &ilb )
{
    vector<string> ls;
    QImage ico_t;
    AutoHD<VCA::Widget> wdg;
    ed_lib = ilb;		

    //- Load widget data -
    try
    {
	string wlib_id = TSYS::strSepParse(ed_lib,0,'.');
	string wdg_id = TSYS::strSepParse(ed_lib,1,'.');
	string wdgc_id = TSYS::strSepParse(ed_lib,2,'.');
	
	if(wdgc_id.size())
	    wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
	else wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Widget '%1' no present.")).arg(ed_lib.c_str()), TVision::Warning, this );
	return;
    }

    //-- Load widget icon --
    string simg = TSYS::strDecode(wdg.at().ico(),TSYS::base64);
    if(ico_t.loadFromData((const uchar*)simg.c_str(),simg.size()))
	wdg_ico->setIcon(QPixmap::fromImage(ico_t));
    else wdg_ico->setIcon(QIcon());
    //-- Load widget state --
    wdg_enable->setChecked(wdg.at().enable());
    //-- Load parent widget --
    selectParent(wdg.at().parentNm().c_str());
    //-- Load users and groups --
    SYS->security().at().usrList(ls);
    //--- Delete users ---
    wdg_user->clear();
    for(int i_l = 0; i_l < ls.size(); i_l++)
    {
	string simg = TSYS::strDecode(SYS->security().at().usrAt(ls[i_l]).at().picture(),TSYS::base64);
        QImage img;
        if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
            wdg_user->addItem(QPixmap::fromImage(img),ls[i_l].c_str());
        else wdg_user->addItem(ls[i_l].c_str());
	if( wdg.at().user() == ls[i_l] ) wdg_user->setCurrentIndex(i_l);
    }
    wdg_grp->setCurrentIndex(wdg_grp->findText(wdg.at().grp().c_str()));
    setPermit(wdg.at().permit());
    
    //-- Load widget identifier, name and description --    
    wdg_id->setText(wdg.at().id().c_str());
    wdg_name->setText(wdg.at().name().c_str());
    wdg_descr->setPlainText(wdg.at().descr().c_str());

    //- Disable OK and Cancel buttons -
    is_modif = false;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(false);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    //- Set attributes -
    wdg_attr->setWdg(ilb);

    //- Show dialog -
    show();
    raise();
    activateWindow();
    wdg_tabs->setCurrentIndex(0);
}

void WdgProp::selectIco( )
{
    QImage ico_t;
    
    if( !ico_modif )	return;
    
    QString fileName = QFileDialog::getOpenFileName(this,_("Load icon picture"),"",_("Images (*.png *.jpg)"));
    
    if( fileName.isEmpty() )	return;
    if(!ico_t.load(fileName))
    {
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Loaded icon image '%1' error.")).arg(fileName),TVision::Warning, this );
	return;    
    }
    
    wdg_ico->setIcon(QPixmap::fromImage(ico_t));
    
    isModify();
}

void WdgProp::selectParent( const QString &val )
{
    wdg_parent->clear();
    
    int c_lv = 0;
    string c_path = "";
    string lnk = val.toAscii().data();

    wdg_parent->addItem(c_path.c_str());
    while(TSYS::strSepParse(lnk,c_lv,'.').size())
    {
        if( c_lv ) c_path+=".";
        c_path = c_path+TSYS::strSepParse(lnk,c_lv,'.');
        wdg_parent->addItem(c_path.c_str());
        c_lv++;
    }
    if(c_lv) c_path+=".";
    vector<string>  ls;
    switch(c_lv)
    {
        case 0: mod->engine().at().wlbList(ls);   break;
        case 1:
            if( mod->engine().at().wlbPresent(TSYS::strSepParse(lnk,0,'.')) )
                mod->engine().at().wlbAt(TSYS::strSepParse(lnk,0,'.')).at().list(ls);
            break;
    }
    for(int i_l = 0; i_l < ls.size(); i_l++)
        wdg_parent->addItem((c_path+ls[i_l]).c_str());

    int cur_id = wdg_parent->findText(val);
    if( cur_id >= 0 )	wdg_parent->setCurrentIndex( cur_id );
	
    isModify( );
}

void WdgProp::isModify( )
{
    is_modif = true;
    butbox->button(QDialogButtonBox::Apply)->setEnabled(true);
    butbox->button(QDialogButtonBox::Cancel)->setEnabled(true);    
}

void WdgProp::pressApply( )
{    
    AutoHD<VCA::Widget> wdg;
    //- Download modified data -
    try
    { 
	//-- Open widget --		    
       	string wlib_id = TSYS::strSepParse(ed_lib,0,'.');
	string wdg_id = TSYS::strSepParse(ed_lib,1,'.');
	string wdgc_id = TSYS::strSepParse(ed_lib,2,'.');

	if(wdgc_id.size())
	    wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id).at().wdgAt(wdgc_id);
	else wdg = mod->engine().at().wlbAt(wlib_id).at().at(wdg_id);

	//-- Save widget icon --
	if(!wdg_ico->icon().isNull())
	{
	    QByteArray ba;
    	    QBuffer buffer(&ba);
	    buffer.open(QIODevice::WriteOnly);
	    wdg_ico->icon().pixmap(64,64).save(&buffer,"PNG");
	    wdg.at().setIco(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
        }
	//-- Save parent widget --
	wdg.at().setParentNm(wdg_parent->currentText().toAscii().data());
	//-- Save users and groups --
	wdg.at().setUser(user());
	wdg.at().setGrp(grp());
	//-- Set permition --
	wdg.at().setPermit(permit());
	//-- Save widget name and description --
	wdg.at().setName(wdg_name->text().toAscii().data());
	wdg.at().setDescr(wdg_descr->toPlainText().toAscii().data());
	//-- Save widget state --
	wdg.at().setEnable(wdg_enable->isChecked());	
    }
    catch(TError err)
    { 
	mod->postMess( mod->nodePath().c_str(), 
		QString(_("Widget '%1' error: %2")).arg(ed_lib.c_str()).arg(err.mess.c_str()),
		TVision::Warning,this);
	return;
    }   
     
    //- Update widgets tree and toolbars -
    owner()->wdgTree->updateLibs();
    owner()->updateLibToolbar();
    
    //- Reload date -
    showDlg(ed_lib);
}

void WdgProp::pressCancel( )
{    
    showDlg(ed_lib);
}

void WdgProp::pressClose( )
{
    if( is_modif )
    {
	InputDlg dlg(this,wdg_ico->icon(),
             _("Some attributes is changed. You sure to close window?"),_("Close window"),false,false);
	if( dlg.exec() != QDialog::Accepted )	return;
    }			    

    is_modif = false;
    ed_lib = "";
    hide();
}

void WdgProp::selectUser(const QString &val)
{
    //Get default users group
    vector<string> gls;
    SYS->security().at().usrGrpList(val.toAscii().data(),gls);    
    string vgrp = gls.size()?gls[0].c_str():"UI";
    wdg_grp->clear();
    for(int i_l = 0; i_l < gls.size(); i_l++)
    {
	wdg_grp->addItem(gls[i_l].c_str());
        if( vgrp == gls[i_l] ) wdg_grp->setCurrentIndex(i_l);
    }
    selectPermission();    
}

void WdgProp::selectPermission( )
{
    wdg_enable->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWRWR_));
    wdg_parent->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wdg_user->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,"root","root",RWRWR_));
    wdg_grp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accuser->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accgrp->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_accother->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),RWR_R_));
    wdg_name->setEnabled(SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    wdg_descr->setEnabled( SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit()));
    ico_modif = SYS->security().at().access(owner()->user(),SEQ_WR,user(),grp(),permit());
    
    isModify( );
}

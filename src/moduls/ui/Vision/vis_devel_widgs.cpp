
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
        //- Connect to widget -    
	if( TSYS::pathLev(it->id(),1).empty() )	return;
	
        AutoHD<VCA::Widget> wdg(mod->engine().at().nodeAt(it->id()),true);	//!?!? Dangerous
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

    try
    {    
        //- Connect to widget -
	if( TSYS::pathLev(nwdg,1).empty() )	return false;
        AutoHD<VCA::Widget> wdg(mod->engine().at().nodeAt(nwdg),true);	//!?!? Dangerous
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
//* Inspector of links widget            *
//****************************************
InspLnk::InspLnk( QWidget * parent ) : QTreeWidget(parent), show_init(false)
{
    //setEditTriggers(QAbstractItemView::AllEditTriggers);
    setAlternatingRowColors(true);
    setItemDelegate(new ItemDelegate);
    
    QStringList headLabels;
    headLabels << _("Name") << _("Value");
    setHeaderLabels(headLabels);
    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeLnk(QTreeWidgetItem*,int)));
}

InspLnk::~InspLnk( )
{

}
    
void InspLnk::setWdg( const string &iwdg )
{
    if( it_wdg != TSYS::strSepParse(iwdg,0,';') )
    { 
	clear();
	it_wdg = TSYS::strSepParse(iwdg,0,';');
    }

    show_init = true;
    //Update tree
    XMLNode get_req("get");
    get_req.setAttr("user","user");    
    
    //- Get links info -
    XMLNode info_req("info");
    info_req.setAttr("user","user")->setAttr("path",it_wdg+"/%2flinks%2flnk")->setAttr("showAttr","1");
    if( mod->cntrIfCmd(info_req) ) return;
    XMLNode *rootel = info_req.childGet(0);
    //- Create widget's root items -
    for( int i_l = 0; i_l < rootel->childSize(); i_l++ )
    {
	string lnid  = rootel->childGet(i_l)->attr("id");
	string lngrp = rootel->childGet(i_l)->attr("elGrp");
	string lnwdg = TSYS::strSepParse(lnid.substr(3),0,'.');
	string lnatr = TSYS::strSepParse(lnid.substr(3),1,'.');
	if( lnatr.empty() )
	{
	    lnatr = lnwdg;
	    lnwdg = ".";
	}
	
	//- Search widget item -
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
	    //-- Search group --
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
	    //--- Get group value ---
	    get_req.setAttr("path",it_wdg+"/%2flinks%2flnk%2fpr_"+lnid.substr(3));
	    if( !mod->cntrIfCmd(get_req) )
	        wdg_it->setText(1,get_req.text().c_str());
	}
	//-- Search parameter --
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
	//--- Get parameter's value ---
	get_req.setAttr("path",it_wdg+"/%2flinks%2flnk%2f"+lnid);
	if( !mod->cntrIfCmd(get_req) )
	    prm_it->setText(1,get_req.text().c_str());
    }
    
    //- Set widget's path -
    if( topLevelItemCount() )	topLevelItem(0)->setData(0,Qt::UserRole,QString(it_wdg.c_str()));
    
    show_init = false;
}

void InspLnk::changeLnk( QTreeWidgetItem *index, int col )
{
    if( col != 1 || show_init ) return;
    
    string wdg_it  = topLevelItem(0)->data(0,Qt::UserRole).toString().toAscii().data();
    string attr_id = index->data(0,Qt::UserRole).toString().toAscii().data();    
    
    XMLNode set_req("set");
    set_req.setAttr("user","user")->
	    setAttr("path",wdg_it+"/%2flinks%2flnk%2f"+(index->childCount()?"pr_":"el_")+attr_id)->
	    setText(index->text(1).toAscii().data());
    if( mod->cntrIfCmd(set_req) )
	mod->postMess(set_req.attr("mcat").c_str(),set_req.text().c_str(),TVision::Error,this);
    else setWdg(it_wdg);
}

//* Links item delegate         *
//*******************************
InspLnk::ItemDelegate::ItemDelegate( QObject *parent ) : QItemDelegate(parent)
{

}

QWidget *InspLnk::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid() || !index.parent().isValid() || index.column() == 0) return 0;

    string wdg_it  = index.model()->index(0,0).data(Qt::UserRole).toString().toAscii().data();
    QModelIndex id_it = index.model()->index(index.row(),0,index.parent());
    string attr_id = id_it.data(Qt::UserRole).toString().toAscii().data();

    //- Get combobox values -
    XMLNode get_req("get");
    get_req.setAttr("user","user")->
	    setAttr("path",wdg_it+"/%2flinks%2flnk%2f"+(id_it.child(0,0).isValid()?"pl_":"ls_")+attr_id);
    if( !mod->cntrIfCmd(get_req) )
    {
	w_del = new QComboBox(parent);
	((QComboBox*)w_del)->setEditable(true);
	for( int i_l = 0; i_l < get_req.childSize(); i_l++ )
	    ((QComboBox*)w_del)->addItem(get_req.childGet(i_l)->text().c_str());
    }
    else
    {
        QItemEditorFactory factory;
        w_del = factory.createEditor(index.data().type(), parent);
    }
    
    return w_del;
}

void InspLnk::ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if( dynamic_cast<QComboBox*>(editor) )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
	QString val = index.data(Qt::DisplayRole).toString();	
	if( comb->findText(val) < 0 )	comb->addItem(val);
        comb->setCurrentIndex(comb->findText(val));
    }
    else QItemDelegate::setEditorData(editor, index);
}

void InspLnk::ItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if( dynamic_cast<QComboBox*>(editor) )
        model->setData(index,dynamic_cast<QComboBox*>(editor)->currentText(),Qt::EditRole);
    else QItemDelegate::setModelData(editor, model, index);
}


//****************************************
//* Inspector of links dock widget       *
//****************************************
InspLnkDock::InspLnkDock( QWidget * parent ) : QDockWidget(_("Links"),parent)
{
    setObjectName("InspLnkDock");
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    
    ainsp_w = new InspLnk(this);
    setWidget(ainsp_w);
}

InspLnkDock::~InspLnkDock( )
{

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
    connect( treeW, SIGNAL( itemPressed(QTreeWidgetItem*,int) ), this, SLOT( pressItem(QTreeWidgetItem*) ) );
    
    setWidget(treeW);

    treeW->installEventFilter(this);
}

WdgTree::~WdgTree()
{

}

VisDevelop *WdgTree::owner()
{
    return (VisDevelop*)parentWidget();
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
    string work_wdg;
    QTreeWidgetItem *cur_el = sel_ls.at(0);
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->parent()?"/wdg_":"/wlb_")+cur_el->text(2).toAscii().data());
	cur_el=cur_el->parent();
    }
    
    emit selectItem(work_wdg);
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

void WdgTree::updateTree( const string &vca_it )
{    
    int i_l, i_w, i_cw, i_top, i_topwl, i_topcwl;
    QTreeWidgetItem *nit, *nit_w, *nit_cw;
    vector<string> list_wl;
    string simg;
    QImage img;    

    if( !vca_it.empty() && TSYS::pathLev(vca_it,0).substr(0,4) != "wlb_" )return;

    XMLNode prm_req("get");
    prm_req.setAttr("user",owner()->user());

    //- Get widget's libraries list -
    XMLNode lb_req("get");    
    lb_req.setAttr("user",owner()->user())->setAttr("path","/%2fprm%2fcfg%2fwlb");
    if( mod->cntrIfCmd(lb_req) )
    {
	mod->postMess(lb_req.attr("mcat").c_str(),lb_req.text().c_str(),TVision::Error,this);
	return;
    }
    for( int i_ch = 0; i_ch < lb_req.childSize(); i_ch++ )
	list_wl.push_back(lb_req.childGet(i_ch)->attr("id"));
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
	prm_req.setAttr("path","/wlb_"+list_wl[i_l]+"/%2fico");
	if( !mod->cntrIfCmd(prm_req) )
	{
	    simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		nit->setIcon(0,QPixmap::fromImage(img));
	}
	nit->setText(0,lb_req.childGet(i_l)->text().c_str());
	nit->setText(1,_("Library"));
	nit->setText(2,list_wl[i_l].c_str());

	
	//-- Update librarie's widgets --
	//--- Get librarie's widgets list ---
	XMLNode lbw_req("get");
	lbw_req.setAttr("user",owner()->user())->setAttr("path","/wlb_"+list_wl[i_l]+"/%2fwdg%2fwdg");
	if( mod->cntrIfCmd(lbw_req) )
	{
	    mod->postMess(lbw_req.attr("mcat").c_str(),lbw_req.text().c_str(),TVision::Error,this);
	    return;
	}
	vector<string> list_w;
	for( int i_ch = 0; i_ch < lbw_req.childSize(); i_ch++ )
	    list_w.push_back(lbw_req.childGet(i_ch)->attr("id"));
	//--- Remove no present widgets ---	
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
	    prm_req.setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/%2fico");
	    if( !mod->cntrIfCmd(prm_req) )
	    {
		simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    nit_w->setIcon(0,QPixmap::fromImage(img));
	    }
	    nit_w->setText(0,lbw_req.childGet(i_w)->text().c_str());
	    nit_w->setText(1,_("Widget"));	    
	    nit_w->setText(2,list_w[i_w].c_str());
	    
	    //--- Update container's widgets ---
	    //---- Get container's widgets ---
	    XMLNode w_req("get");
	    w_req.setAttr("user",owner()->user())->setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/%2finclwdg%2fwdg");
	    if( mod->cntrIfCmd(w_req) )
	    {
		mod->postMess(w_req.attr("mcat").c_str(),w_req.text().c_str(),TVision::Error,this);
		return;
	    }
	    vector<string> list_wc;
	    for( int i_ch = 0; i_ch < w_req.childSize(); i_ch++ )
		list_wc.push_back(w_req.childGet(i_ch)->attr("id"));		    
	    //---- Remove no present widgets ----	    
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
		prm_req.setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/wdg_"+list_wc[i_cw]+"/%2fico");
		if( !mod->cntrIfCmd(prm_req) )
		{
		    simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
		    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
			nit_cw->setIcon(0,QPixmap::fromImage(img));
		}
		nit_cw->setText(0,w_req.childGet(i_cw)->text().c_str());
		nit_cw->setText(1,_("Container widget"));
		nit_cw->setText(2,list_wc[i_cw].c_str());
	    }
	}
    }
}

void WdgTree::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();

    //Add actions
    popup.addAction(owner()->actLibNew);
    popup.addAction(owner()->actVisItAdd);
    popup.addAction(owner()->actVisItDel);
    popup.addAction(owner()->actVisItProp);
    popup.addAction(owner()->actVisItEdit);
    popup.addSeparator();
    popup.addAction(owner()->actDBLoad);
    popup.addAction(owner()->actDBSave);
    popup.addSeparator();
    //- Reload action -
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
    QAction *actRefresh = new QAction(QPixmap::fromImage(ico_t),_("Refresh libraries"),this);
    actRefresh->setStatusTip(_("Press for refresh present libraries."));
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

bool ProjTree::eventFilter( QObject *target, QEvent *event )
{
    if( target == treeW )
    {
	if( event->type() == QEvent::FocusIn )	selectItem( );
	if( event->type() == QEvent::FocusOut && QApplication::focusWidget() != treeW )
	    owner()->selectItem("");
    }
    return QDockWidget::eventFilter( target, event );
}

void ProjTree::selectItem( )
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
    
    emit selectItem(work_wdg);
}

void ProjTree::updateTree( const string &vca_it, QTreeWidgetItem *it )
{
    vector<string> list_pr, list_pg;
    QTreeWidgetItem *nit, *nit_pg;    
    string simg;
    QImage img;    

    XMLNode prm_req("get");
    prm_req.setAttr("user",owner()->user());

    if( !it )
    {
	if( !vca_it.empty() && TSYS::pathLev(vca_it,0).substr(0,4) != "prj_" )	return;
	//- Process top level items and project's list -
	//-- Get widget's libraries list --
	XMLNode prj_req("get");    
	prj_req.setAttr("user",owner()->user())->setAttr("path","/%2fprm%2fcfg%2fprj");
	if( mod->cntrIfCmd(prj_req) )
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
	//-- Add new libraries --
	for( int i_l = 0; i_l < list_pr.size(); i_l++ )
	{
	    int i_top;
	    for( i_top = 0; i_top < treeW->topLevelItemCount(); i_top++ )
    		if( list_pr[i_l] == treeW->topLevelItem(i_top)->text(2).toAscii().data() )
		    break;
	    if( i_top >= treeW->topLevelItemCount() )
		nit = new QTreeWidgetItem(treeW);
	    else nit = treeW->topLevelItem(i_top);

	    //-- Update libraries data --
	    prm_req.setAttr("path","/prj_"+list_pr[i_l]+"/%2fico");
	    if( !mod->cntrIfCmd(prm_req) )
	    {
		simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    nit->setIcon(0,QPixmap::fromImage(img));
	    }	    
	    nit->setText(0,prj_req.childGet(i_l)->text().c_str());
	    nit->setText(1,_("Project"));
	    nit->setText(2,list_pr[i_l].c_str());
	    
    	    updateTree(vca_it,nit);
	}	
	return;
    }
    //- Process project's pages -
    //-- Get page path
    nit = it;
    string work_wdg;
    QTreeWidgetItem *cur_el = nit;
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->parent()?"/pg_":"prj_")+cur_el->text(2).toAscii().data());
	cur_el=cur_el->parent();
    }
    bool is_prj = TSYS::pathLev(work_wdg,1).empty();
    //-- Update include pages --
    //--- Get page's list ---
    XMLNode pg_req("get");
    pg_req.setAttr("user",owner()->user())->setAttr("path",work_wdg+"/%2fpage%2fpage");
    if( mod->cntrIfCmd(pg_req) )
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
	int i_pit;
	for( i_pit = 0; i_pit < nit->childCount(); i_pit++ )
    	    if( list_pg[i_p] == nit->child(i_pit)->text(2).toAscii().data() )
		break;
	if( i_pit >= it->childCount() )
	    nit_pg = new QTreeWidgetItem(it);
	else nit_pg = it->child(i_pit);
		
	//--- Update page's data ---	
	prm_req.setAttr("path",work_wdg+"/pg_"+list_pg[i_p]+"/%2fico");
	if( !mod->cntrIfCmd(prm_req) )
	{
	    simg = TSYS::strDecode(prm_req.text(),TSYS::base64);
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
    popup.addAction(owner()->actPrjNew);
    popup.addAction(owner()->actVisItAdd);
    popup.addAction(owner()->actVisItDel);
    popup.addAction(owner()->actVisItProp);
    popup.addAction(owner()->actVisItEdit);    
    popup.addSeparator();
    popup.addAction(owner()->actDBLoad);
    popup.addAction(owner()->actDBSave);
    popup.addSeparator();    
    
    //- Reload action -
    QImage ico_t;
    if(!ico_t.load(TUIS::icoPath("reload").c_str())) ico_t.load(":/images/reload.png");
    QAction *actRefresh = new QAction(QPixmap::fromImage(ico_t),_("Refresh projects"),this);
    actRefresh->setStatusTip(_("Press for refresh present projects."));
    connect(actRefresh, SIGNAL(activated()), this, SLOT(updateTree()));
    popup.addAction(actRefresh);

    QAction *rez = popup.exec(QCursor::pos());
    
    popup.clear();
}			  


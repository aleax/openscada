
//OpenSCADA system module UI.Vision file: vis_devel_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2006-2007 by Roman Savochenko                           *
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
    //Delete root item
    delete rootItem;
}

string ModInspAttr::user( )
{
    return mainWin( )->user();
}

void ModInspAttr::setWdg( const string &iwdg )
{
    cur_wdg = iwdg;
    bool full_reset = false;
    string sval;
    vector<string> wdg_ls;

    //- Get widgets list -
    for( int v_off = 0; (sval=TSYS::strSepParse(iwdg,0,';',&v_off)).size(); )
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
    	wdgAttrUpdate( QModelIndex() );// rootItem );	    
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
	    wdgAttrUpdate( index(i_w,0,QModelIndex()) );// rootItem->child(i_w));
        }
    }

    if( full_reset )	reset();
    else emit layoutChanged();
}

void ModInspAttr::wdgAttrUpdate( const QModelIndex &mod_it ) // Item *it)
{    
    Item *it = mod_it.isValid() ? static_cast<Item*>(mod_it.internalPointer()) : rootItem;
    if( it->type() != Item::Wdg )   return;

    try
    {
     	XMLNode info_req("info");
     	XMLNode req("get");

        //- Set/update widget name -
    	req.setAttr("path",it->id()+"/"+TSYS::strEncode("/wdg/cfg/name",TSYS::PathEl));
	if( !mainWin()->cntrIfCmd(req) )	it->setName(req.text().c_str());        

	info_req.setAttr("path",it->id()+"/%2fattr" );
	mainWin()->cntrIfCmd(info_req);
    	XMLNode *root = info_req.childGet(0); 
	
        //- Delete items of a no present attributes -
	vector<int> idst;
	idst.push_back(0);
	int it_lev = 0;
	QModelIndex curmod = mod_it;
	Item *curit = it;
	//-- Get next item --
	while( idst[it_lev] < curit->childCount() )
	{
	    //-- Process next attribute --
	    if( curit->child(idst[it_lev])->type( ) == Item::Attr )
	    {
		string it_id = curit->child(idst[it_lev])->id();
		//--- Find into present attributes list ---
		int i_a;
    		for( i_a = 0; i_a < root->childSize(); i_a++ )
		    if( root->childGet(i_a)->attr("id") == it_id )
			break;
		//--- Remove no present item ---
		if( i_a >= root->childSize() )
    		{
		    beginRemoveRows(curmod,idst[it_lev],idst[it_lev]);
            	    curit->childDel(idst[it_lev]);
		    endRemoveRows();
		    idst[it_lev]--;
		}
	    }
	    //-- Enter to group --
	    else if( curit->child(idst[it_lev])->type( ) == Item::AttrGrp )
	    {
		curmod = index(idst[it_lev],0,curmod);
		curit = static_cast<Item*>(curmod.internalPointer());
		
		idst.push_back(0);
		it_lev++;
		continue;
	    }
	    
	    //-- Up to level --
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
	
        //- Add items for present attributes -
        for( int i_a = 0; i_a < root->childSize(); i_a++ )
        {
	    XMLNode *gnd = root->childGet(i_a);
	    string a_id = gnd->attr("id");
            string a_nm = gnd->attr("dscr");
            Item *cur_it = it;
            //-- Parse attributes group --
            if( TSYS::strSepParse(a_nm,1,':').size() )
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
            int ga_id = cur_it->childGet(a_id);
            if( ga_id < 0 ) ga_id = cur_it->childInsert(a_id,-1,Item::Attr);
            cur_it->child(ga_id)->setName(a_nm);	    
	    cur_it->child(ga_id)->setEdited(atoi(gnd->attr("acs").c_str())&SEQ_WR);
	    cur_it->child(ga_id)->setFlag( atoi(gnd->attr("wdgFlg").c_str()) );	    
            //-- Get Value --
	    string sval;
	    req.clear()->setAttr("path",it->id()+"/%2fattr%2f"+a_id);
	    if( !mainWin()->cntrIfCmd(req) )	sval = req.text();
	    string stp = gnd->attr("tp");
	    if( stp == "bool" )		cur_it->child(ga_id)->setData((bool)atoi(sval.c_str()));
	    else if( stp == "dec" || stp == "hex" || stp == "oct" )
			   		cur_it->child(ga_id)->setData(atoi(sval.c_str()));
	    else if( stp == "real" )	cur_it->child(ga_id)->setData(atof(sval.c_str()));
	    else if( stp == "str"  )	cur_it->child(ga_id)->setData(sval.c_str());
	    //--- Get selected list ---
            if( gnd->attr("dest") == "select" )
            {
		req.clear()->setAttr("path",it->id()+"/"+TSYS::strEncode(gnd->attr("select"),TSYS::PathEl));
		if( !mainWin()->cntrIfCmd(req) )
		{
		    QStringList	el_ls;
		    for( int i_el = 0; i_el < req.childSize(); i_el++ )
			el_ls.push_back(req.childGet(i_el)->text().c_str());
            	    cur_it->child(ga_id)->setDataEdit(el_ls);
		}
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
      	XMLNode req("set");
    	req.setAttr("path",nwdg+"/%2fattr%2f"+nattr)->
	    setText((value.type()==QVariant::Bool) ? 
			(value.toBool()?"1":"0") : value.toString().toAscii().data());
	if( !mainWin()->cntrIfCmd(req) )
	{
	    it->setData( (it->data().type()==QVariant::Bool) ? value.toBool() : value );
	    emit modified(nwdg);
	    emit dataChanged(index,index);
	    if( it->flag()&Item::Active ) setWdg(cur_wdg);
	}
    }catch(...){ return false; }        


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
InspAttr::InspAttr( QWidget * parent, VisDevelop *mainWind ) : QTreeView(parent), modelData("",mainWind)
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
	((QTextEdit*)w_del)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	((QTextEdit*)w_del)->resize(50,50);
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

    if( flag&ModInspAttr::Item::Select )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        comb->addItems(value.toStringList());
        comb->setCurrentIndex(comb->findText(index.data(Qt::DisplayRole).toString()));
    }
    else if( value.type()==QVariant::String && flag&ModInspAttr::Item::FullText )
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
    
    if( flag&ModInspAttr::Item::Select )
    {
        QComboBox *comb = dynamic_cast<QComboBox*>(editor);
        model->setData(index,comb->currentText(),Qt::EditRole);
    }
    else if( value.type()==QVariant::String && flag&ModInspAttr::Item::FullText )
    {
	QTextEdit *ted = dynamic_cast<QTextEdit*>(editor);
	model->setData(index,ted->toPlainText(),Qt::EditRole);
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
    setItemDelegate(new ItemDelegate(this));
    
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
    
void InspLnk::setWdg( const string &iwdg )
{
    string lnid, lngrp, lnwdg, lnatr;
    
    if( it_wdg != TSYS::strSepParse(iwdg,0,';') )
    { 
	clear();
	it_wdg = TSYS::strSepParse(iwdg,0,';');
    }

    show_init = true;
    //Update tree
    XMLNode req("get");
    
    //- Get links info -
    XMLNode info_req("info");
    info_req.setAttr("path",it_wdg+"/%2flinks%2flnk")->setAttr("showAttr","1");
    if( mainWin()->cntrIfCmd(info_req) ) return;
    XMLNode *rootel = info_req.childGet(0);
    //- Create widget's root items -
    for( int i_l = 0; i_l < rootel->childSize(); i_l++ )
    {
	lnid  = rootel->childGet(i_l)->attr("id");
	lngrp = rootel->childGet(i_l)->attr("elGrp");
	lnwdg = TSYS::strSepParse(lnid.substr(3),0,'.');
	lnatr = TSYS::strSepParse(lnid.substr(3),1,'.');
	if( lnatr.empty() )	{ lnatr = lnwdg; lnwdg = "."; }
	
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
	    req.clear()->setAttr("path",it_wdg+"/%2flinks%2flnk%2fpr_"+lnid.substr(3));
	    if( !mainWin()->cntrIfCmd(req) )
	        wdg_it->setText(1,req.text().c_str());
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
	req.clear()->setAttr("path",it_wdg+"/%2flinks%2flnk%2f"+lnid);
	if( !mainWin()->cntrIfCmd(req) )
	    prm_it->setText(1,req.text().c_str());
    }

    //- Check for deleted links -
    for( int i_it = 0; i_it < topLevelItemCount(); i_it++ )
	for( int i_g = 0, i_a = 0; i_g < topLevelItem(i_it)->childCount(); )
	{
	    QTreeWidgetItem *wdg_g  = topLevelItem(i_it)->child(i_g);
	    QTreeWidgetItem *wdg_it = (wdg_g->childCount())?wdg_g->child(i_a):wdg_g;
	
	    int i_l;
	    for( i_l = 0; i_l < rootel->childSize(); i_l++ )
		if( rootel->childGet(i_l)->attr("id") == ("el_"+wdg_it->data(0,Qt::UserRole).toString()).toAscii().data() &&
			((bool)rootel->childGet(i_l)->attr("elGrp").size()^(wdg_g==wdg_it)) )
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
    
    //- Set widget's path -
    if( topLevelItemCount() )	topLevelItem(0)->setData(0,Qt::UserRole,QString(it_wdg.c_str()));
    
    show_init = false;
}

void InspLnk::changeLnk( QTreeWidgetItem *index, int col )
{
    if( col != 1 || show_init ) return;
    
    string wdg_it  = it_wdg;// topLevelItem(0)->data(0,Qt::UserRole).toString().toAscii().data();
    string attr_id = index->data(0,Qt::UserRole).toString().toAscii().data();    
    
    XMLNode req("set");
    req.setAttr("path",wdg_it+"/%2flinks%2flnk%2f"+(index->childCount()?"pr_":"el_")+attr_id)->
	setText(index->text(1).toAscii().data());
    if( mainWin()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else setWdg(it_wdg);
}

//* Links item delegate         *
//*******************************
InspLnk::ItemDelegate::ItemDelegate( InspLnk *parent ) : QItemDelegate(parent)
{

}

InspLnk *InspLnk::ItemDelegate::owner() const
{
    return (InspLnk*)parent();
}

QWidget *InspLnk::ItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *w_del;
    if(!index.isValid() || !index.parent().isValid() || index.column() == 0) return 0;

    string wdg_it  = index.model()->index(0,0).data(Qt::UserRole).toString().toAscii().data();
    QModelIndex id_it = index.model()->index(index.row(),0,index.parent());
    string attr_id = id_it.data(Qt::UserRole).toString().toAscii().data();

    //- Get combobox values -
    XMLNode req("get");
    req.setAttr("path",wdg_it+"/%2flinks%2flnk%2f"+(id_it.child(0,0).isValid()?"pl_":"ls_")+attr_id);
    if( !owner()->mainWin()->cntrIfCmd(req) )
    {
	w_del = new QComboBox(parent);
	((QComboBox*)w_del)->setEditable(true);
	for( int i_l = 0; i_l < req.childSize(); i_l++ )
	    ((QComboBox*)w_del)->addItem(req.childGet(i_l)->text().c_str());
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
    connect( treeW, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), owner()->actVisItProp, SLOT(trigger()) );
    
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
    if( event->type() == QEvent::MouseButtonPress && 
	    ((QMouseEvent*)event)->button() == Qt::LeftButton )
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

void WdgTree::updateTree( const string &vca_it )
{    
    int i_l, i_w, i_cw, i_top, i_topwl, i_topcwl;
    QTreeWidgetItem *nit, *nit_w, *nit_cw;
    vector<string> list_wl;
    string simg;
    QImage img;    

    if( !vca_it.empty() && TSYS::pathLev(vca_it,0).substr(0,4) != "wlb_" )return;

    XMLNode req("get");

    //- Get widget's libraries list -
    XMLNode lb_req("get");    
    lb_req.setAttr("path","/%2fprm%2fcfg%2fwlb");
    if( owner()->cntrIfCmd(lb_req) )
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
	req.clear()->setAttr("path","/wlb_"+list_wl[i_l]+"/%2fico");
	if( !owner()->cntrIfCmd(req) )
	{
	    simg = TSYS::strDecode(req.text(),TSYS::base64);
	    if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		nit->setIcon(0,QPixmap::fromImage(img));
	}
	nit->setText(0,lb_req.childGet(i_l)->text().c_str());
	nit->setText(1,_("Library"));
	nit->setText(2,list_wl[i_l].c_str());

	
	//-- Update librarie's widgets --
	//--- Get librarie's widgets list ---
	XMLNode lbw_req("get");
	lbw_req.setAttr("path","/wlb_"+list_wl[i_l]+"/%2fwdg%2fwdg");
	if( owner()->cntrIfCmd(lbw_req) )
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
	    req.clear()->setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/%2fico");
	    if( !owner()->cntrIfCmd(req) )
	    {
		simg = TSYS::strDecode(req.text(),TSYS::base64);
		if( img.loadFromData((const uchar*)simg.c_str(),simg.size()) )
		    nit_w->setIcon(0,QPixmap::fromImage(img));
	    }
	    nit_w->setText(0,lbw_req.childGet(i_w)->text().c_str());
	    nit_w->setText(1,_("Widget"));	    
	    nit_w->setText(2,list_w[i_w].c_str());
	    
	    //--- Update container's widgets ---
	    //---- Get container's widgets ---
	    XMLNode w_req("get");
	    w_req.setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/%2finclwdg%2fwdg");
	    if( owner()->cntrIfCmd(w_req) )
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
		req.clear()->setAttr("path","/wlb_"+list_wl[i_l]+"/wdg_"+list_w[i_w]+"/wdg_"+list_wc[i_cw]+"/%2fico");
		if( !owner()->cntrIfCmd(req) )
		{
		    simg = TSYS::strDecode(req.text(),TSYS::base64);
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
    connect( treeW, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), owner()->actVisItProp, SLOT(trigger()) );
     
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

    XMLNode req("get");

    if( !it )
    {
	if( !vca_it.empty() && TSYS::pathLev(vca_it,0).substr(0,4) != "prj_" )	return;
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
	return;
    }
    //- Process project's pages -
    //-- Get page path
    nit = it;
    string work_wdg;
    QTreeWidgetItem *cur_el = nit;
    while(cur_el)
    {
	work_wdg.insert(0,string(cur_el->parent()?"/pg_":"/prj_")+cur_el->text(2).toAscii().data());
	cur_el=cur_el->parent();
    }
    bool is_prj = TSYS::pathLev(work_wdg,1).empty();
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


//****************************************
//* Shape widget view development mode   *
//****************************************
DevelWdgView::DevelWdgView( const string &iwid, int ilevel, VisDevelop *mainWind, QWidget* parent ) :
    WdgView(iwid,ilevel,mainWind,parent), m_select(false), m_edit(false), pntView(NULL), 
    moveHold(false), holdChild(false), leftTop(false), editWdg(NULL)
{
    setMouseTracking(true);
    if( wLevel() == 0 )	
    {
	pntView = new SizePntWdg(this);
	pntView->setSelArea(QRect());
	setFocusPolicy(Qt::StrongFocus);
	setCursor(Qt::ArrowCursor);
	setAcceptDrops(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ), this, SLOT( wdgPopup() ) );	
    } 
}

DevelWdgView::~DevelWdgView( )
{
    if( select() && !mod->endRun( ) )
    {
	setSelect(false,false);
	for( int i_c = 0; i_c < children().size(); i_c++ )
	    if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
    		((DevelWdgView*)children().at(i_c))->setSelect(false);
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
	attrSet("geomX", TSYS::real2str(0.01*floor(100.*((wLevel()>0) ? posF().x()/((WdgView*)parentWidget())->xScale(true) : posF().x())+0.5) ), 7);
	attrSet("geomY", TSYS::real2str(0.01*floor(100.*((wLevel()>0) ? posF().y()/((WdgView*)parentWidget())->yScale(true) : posF().y())+0.5) ), 8);
	attrSet("geomW", TSYS::real2str(0.01*floor(100.*sizeF().width()/xScale(true)+0.5)), 9);
	attrSet("geomH", TSYS::real2str(0.01*floor(100.*sizeF().height()/yScale(true)+0.5)), 10);
	attrSet("geomXsc", TSYS::real2str(x_scale),13);
	attrSet("geomYsc", TSYS::real2str(y_scale),14);
	attrSet("geomZ", TSYS::int2str(z_coord),11);
    }
    
    if( item != id() && wLevel() == 0 )
        for( int i_c = 0; i_c < children().size(); i_c++ )
            if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
                ((DevelWdgView*)children().at(i_c))->saveGeom(item);

    //- For top items (like inspector) data update -
    if( wLevel() == 0  )  setSelect(true);
}

void DevelWdgView::setSelect( bool vl, bool childs )
{
    int chld_cnt = 0;
	
    m_select = vl;
    if( !vl && edit() ) setEdit(false);
		    
    //- Level 0 process -
    if( wLevel() != 0 && !edit() ) return;
			    
    if( vl )
    {
        string sel_chlds = selectChilds(&chld_cnt);
        if( sel_chlds.size() )  emit selected(sel_chlds);
        else                    emit selected(id());
    }
    else
    {
	if( childs )
            for( int i_c = 0; i_c < children().size(); i_c++ )
                if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
            	    qobject_cast<DevelWdgView*>(children().at(i_c))->setSelect(false);
	emit selected("");
    }

    //- Update actions access -
    //-- Enable view toolbar --
    mainWin()->wdgToolView->setVisible(vl);
    disconnect( mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(wdgViewTool(QAction*)) );
    if( vl ) connect( mainWin()->wdgToolView, SIGNAL(actionTriggered(QAction*)), this, SLOT(wdgViewTool(QAction*)) );
							      
    //-- Update widget view tools --
    for( int i_a = 0; i_a < mainWin()->wdgToolView->actions().size(); i_a++ )
	mainWin()->wdgToolView->actions().at(i_a)->setEnabled(chld_cnt>0);
    
    update();
}
										  
void DevelWdgView::setEdit( bool vl )
{
    m_edit = vl;

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
							    
string DevelWdgView::selectChilds( int *cnt )
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
            if( cnt ) (*cnt)++;
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
    if( moveHold ) return;

    Qt::CursorShape new_shp = Qt::ArrowCursor;
    //- Widget geometry -
    if( grepAnchor(rect().bottomRight(),curp) )	
    	new_shp = Qt::SizeFDiagCursor;
    else if( curp.x()>(rect().width()-4) && curp.x()<(rect().width()+4) )
    	new_shp = Qt::SizeHorCursor;
    else if( curp.y()>(rect().height()-4) && curp.y()<(rect().height()+4) )
	new_shp = Qt::SizeVerCursor;
    if( new_shp != Qt::ArrowCursor ) 
    { 
	holdChild = false;
    	if( new_shp != cursor().shape() ) setCursor(new_shp);
	return;
    }

    //- Childs' selection process -
    //-- Check child's anchor selection and widget's geometry --
    leftTop = false;
    QRectF selRect;
    for( int i_c = 0; i_c < children().size(); i_c++ )
        if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
                ((DevelWdgView*)children().at(i_c))->select( ) )
            selRect = selRect.united(((DevelWdgView*)children().at(i_c))->geometryF());
    //-- Select childs anchors --
    if( !selRect.isNull() )
    {
        if( grepAnchor(selRect.topLeft(),curp) )		
	{ new_shp = Qt::SizeFDiagCursor; leftTop = true; }
        else if( grepAnchor(selRect.bottomRight(),curp) )	
	    new_shp = Qt::SizeFDiagCursor;
        else if( grepAnchor(selRect.bottomLeft(),curp) )	
	{ new_shp = Qt::SizeBDiagCursor; leftTop = true; }
        else if( grepAnchor(selRect.topRight(),curp) )	
	    new_shp = Qt::SizeBDiagCursor;
        else if( grepAnchor(QPointF(selRect.center().x(),selRect.y()),curp) )
        { new_shp = Qt::SizeVerCursor; leftTop = true; }
    	else if( grepAnchor(QPointF(selRect.center().x(),selRect.bottomRight().y()),curp) )
	    new_shp = Qt::SizeVerCursor;
	else if( grepAnchor(QPointF(selRect.x(),selRect.center().y()),curp) )
	{ new_shp = Qt::SizeHorCursor; leftTop = true; }
	else if( grepAnchor(QPointF(selRect.bottomRight().x(),selRect.center().y()),curp) )
            new_shp = Qt::SizeHorCursor;
        else if( selRect.contains(curp) )
            new_shp = Qt::PointingHandCursor;
        if( new_shp != Qt::ArrowCursor ) holdChild = true;
    }
    if( new_shp != cursor().shape() ) setCursor(new_shp);
}
																	   
void DevelWdgView::wdgViewTool( QAction *act )
{
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
                    cwdg->setZ(ewdg->z()+1);
                    is_move = true;
                }
                else if( is_move )  ewdg->setZ(ewdg->z()+1);
            }
            if(is_rise && cwdg && ewdg && cwdg!=ewdg )
            {
        	cwdg->stackUnder(ewdg);
        	ewdg->stackUnder(cwdg);
        	cwdg->setZ(ewdg->z()+1);
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
                    cwdg->setZ(ewdg->z()-1);
                    is_move = true;
                }
                else if( is_move )  ewdg->setZ(ewdg->z()-1);
            }
            if(is_lower && cwdg && ewdg && cwdg!=ewdg )
            {
                cwdg->stackUnder(ewdg);
                cwdg->setZ(ewdg->z()-1);
            }
	}
        saveGeom("");
    }
}

void DevelWdgView::wdgPopup( )
{
    int sel_cnt;
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();
	
    //- Add actions -
    if( edit() )
    {
	//-- Individual primitive menus --
	if( editWdg && editWdg->shape )	editWdg->shape->wdgPopup( editWdg, popup );
	//-- Exit from widget edition --
	QAction *actExitEdit = new QAction(_("Exit from widget editing"),this);
	actExitEdit->setStatusTip(_("Press for exit from widget editing."));
	connect(actExitEdit, SIGNAL(activated()), this, SLOT(editExit()));
	popup.addAction(actExitEdit);
    }
    else
    {
	//-- Insert item actions --
	if( !selectChilds(&sel_cnt).empty() )
	{	    
	    popup.addAction(mainWin()->actVisItDel);
	    if( sel_cnt == 1 )
	    {
		popup.addAction(mainWin()->actVisItProp);
		popup.addAction(mainWin()->actVisItEdit);
	    }
	    popup.addSeparator();
	    popup.addAction(mainWin()->actDBLoad);
	    popup.addAction(mainWin()->actDBSave);
	    popup.addSeparator();
	    //-- Insert item actions --
	    popup.addMenu(mainWin()->mn_widg_fnc);
	}
	//-- Make widget icon --
	popup.addSeparator();
	QAction *actMakeIco = new QAction(parentWidget()->windowIcon(),_("Make icon from widget"),this);
	actMakeIco->setStatusTip(_("Press for make icon from widget."));
	connect(actMakeIco, SIGNAL(activated()), this, SLOT(makeIcon()));
	popup.addAction(actMakeIco);
    }

    //- Execute of menu -
    QAction *rez = popup.exec(QCursor::pos());

    popup.clear();
}

void DevelWdgView::makeIcon( )
{
    QPixmap ico_new = QPixmap::grabWidget(this);
    ico_new = ico_new.scaled(64,64,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    parentWidget()->setWindowIcon(ico_new);
    //- Send to VCA engine -
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    ico_new.save(&buffer,"PNG");
		    
    XMLNode req("set");
    req.setAttr("path",id()+"/%2fwdg%2fcfg%2fico")->
	setText(TSYS::strEncode(string(ba.data(),ba.size()),TSYS::base64));
    if( mainWin()->cntrIfCmd(req) )
	mod->postMess(req.attr("mcat").c_str(),req.text().c_str(),TVision::Error,this);
    else emit apply(id());
}

bool DevelWdgView::event( QEvent *event )
{
    //- Paint event process -
    if( event->type() == QEvent::Paint )
    {
    	QPainter pnt( this );
        pnt.setWindow( rect() );
    	
	//- Draw background for root widget -
        if( wLevel() == 0 )
        {
            pnt.setPen("black");
            pnt.setBrush(QBrush(QColor("white")));
            pnt.drawRect(rect().adjusted(0,0,-1,-1));
        }
    	//- Check widget -
    	if( !shape )
    	{
            pnt.drawImage(rect(),QImage(":/images/attention.png"));
            setToolTip(QString(_("Widget shape no support!")));
        }
	    
	//- Update select widget data -
        if( wLevel() == 0 )
        {
            QRectF rsel;
            for( int i_c = 0; i_c < children().size(); i_c++ )
        	if( qobject_cast<DevelWdgView*>(children().at(i_c)) &&
                    	    ((DevelWdgView*)children().at(i_c))->select( ) )
                    rsel = rsel.united(((DevelWdgView*)children().at(i_c))->geometryF());
            pntView->setSelArea(rsel.toRect(),edit());
        }
	pnt.end();
	
	if( WdgView::event(event) )	return true;
	return QWidget::event(event);
    }
    
    //- Other events process -
    if( wLevel() == 0 )
    {
	switch(event->type())
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
                    	    mainWin()->visualItAdd(mainWin()->actGrpWdgAdd->actions().at(i_a),QPointF((float)curp.x()/x_scale,(float)curp.y()/y_scale));

		    ev->accept();
                    return true;
                }
                break;
            }
            case QEvent::MouseButtonPress:
            {
                if( edit() )    break;
				
	        QPoint curp = mapFromGlobal(cursor().pos());
		
	        //- Cancel new widget inserting -
	        QAction *act = mainWin()->actGrpWdgAdd->checkedAction();
	        if( act && act->isChecked() )
                {
                    if( ((static_cast<QMouseEvent*>(event))->buttons()&Qt::RightButton) )
                        act->setChecked(false);
                    else if( ((static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton) )
                        mainWin()->visualItAdd(act,QPointF((float)curp.x()/x_scale,(float)curp.y()/y_scale));

                    setCursor(Qt::ArrowCursor);
                    event->accept();
            	    return true;
        	}
		
 	    	//- Select widget -
		if( (static_cast<QMouseEvent*>(event))->buttons()&Qt::LeftButton )
	     	{
		    dragStartPos = ((QMouseEvent*)event)->pos();
	    	    bool sh_hold = QApplication::keyboardModifiers()&Qt::ShiftModifier;	    
	    	    if( cursor().shape() == Qt::ArrowCursor || sh_hold )
	    	    {
	    		//-- Scan childs --
	    		bool sel_modif = false;
	    		bool chld_sel = false;
	    		DevelWdgView *cwdg = NULL;
	    		for( int i_c = children().size()-1; i_c >= 0; i_c-- )
	    		{
	    		    cwdg = qobject_cast<DevelWdgView*>(children().at(i_c));
	    		    if( !cwdg ) continue;
	    		    if( cwdg->geometryF().contains(curp) ) 
	    		    {
	    			if( !cwdg->select() ) 	{ cwdg->setSelect(true);  sel_modif = true; }
	    			else if( sh_hold )	{ cwdg->setSelect(false); sel_modif = true; }
	    			if( cwdg->select() )	chld_sel = true;
	    			break;
	    		    }
	    		}
	    		//-- Select clean for childs --
	    		if( !sh_hold )
	    		    for( int i_c = 0; i_c < children().size(); i_c++ )
	    		    {
	    			DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
	    			if( !curw || (chld_sel && (curw == cwdg)) )	continue;
	    			if( curw->select() )	{ curw->setSelect(false); sel_modif = true; }
			    }
		    	if( sel_modif || !select() ) setSelect(true);
			event->accept();
		    
		     	upMouseCursors(mapFromGlobal(cursor().pos()));
		    
    			//-- Update status bar --
    			mainWin()->statusBar()->showMessage(QString(_("Select elements: '%1'")).
							arg(selectChilds().c_str()), 10000 );
    		    }
    		    if( cursor().shape() != Qt::ArrowCursor )
    		    {
    			moveHold = true;
    			holdPnt = curp;
    		    }
    		    return true;
		}
    		break;        	
	    }        	
     	    case QEvent::MouseButtonRelease:
		if( moveHold && !edit() )
		{
	    	    if( cursor().shape() != Qt::ArrowCursor )
		    {
			saveGeom(holdChild?"":id().c_str());
			load("");
		    }
	    	    moveHold = false;
	    	    return true;
		}
     		break;
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
		        if( !cwdg->shape || !cwdg->shape->isEditable( ) )	break;
		        edwdg = cwdg;
		    }
		    else if( cwdg->select() ) cwdg->setSelect(false);
		}
		if( edwdg )
		{
		    edwdg->setEdit(true);
		    setEdit(true);
		    setCursor(Qt::ArrowCursor);
		    update();
		    return true;
		}
		else if( shape && shape->isEditable( ) )
		{
		    setEdit(true);
		    setCursor(Qt::ArrowCursor);
		    update();
		    return true;
		}		
		break;        	
	    }
	    case QEvent::FocusIn:
		if(select()) setSelect(true);
		return true;
	    case QEvent::FocusOut:	
		if( cursor().shape() != Qt::ArrowCursor )
		    setCursor(Qt::ArrowCursor);
		if( QApplication::focusWidget() != this )
		{
		    if( !((VisDevelop *)main_win)->attrInsp->hasFocus() )
		    {
			setSelect(false,false);
			//-- Unselect child widgets --
		    //if( !((VisDevelop *)main_win)->attrInsp->hasFocus() )
			for( int i_c = 0; i_c < children().size(); i_c++ )
			    if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
				((DevelWdgView*)children().at(i_c))->setSelect(false);
		    }
		}
		    //emit selected("");
		return true;
    	    case QEvent::MouseMove:
	    {
	    	if( edit() )	break;
		
		QPoint curp = mapFromGlobal(cursor().pos());
		
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
		if( moveHold && cursor().shape() != Qt::ArrowCursor && ((QMouseEvent*)event)->buttons()&Qt::LeftButton &&
		    (((QMouseEvent*)event)->pos()-dragStartPos).manhattanLength() >= QApplication::startDragDistance() )
		{
		    dragStartPos = QPoint(-100,-100);
		    if( holdChild )
		    {
			QPoint dP = curp-holdPnt;
			//-- Update selected widgets geometry --			
			for( int i_c = 0; i_c < children().size(); i_c++ )
			{
			    DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
			    if( !curw || !curw->select() ) continue;
			    bool isScale = QApplication::keyboardModifiers()&Qt::ControlModifier;
			    QRectF  geom = curw->geometryF();
			    switch(cursor().shape())
			    {
				case Qt::SizeFDiagCursor:
				    if( isScale ) 
				    { 
				        curw->x_scale *= (geom.width()+((leftTop)?-1:1)*dP.x())/geom.width();
				        curw->y_scale *= (geom.height()+((leftTop)?-1:1)*dP.y())/geom.height();
				    }
				    if( leftTop )
				    {
					curw->moveF(QPointF(geom.x()+dP.x(), geom.y()+dP.y()));
					curw->resizeF(QSizeF(geom.width()-dP.x(), geom.height()-dP.y()));
				    }
				    else curw->resizeF(QSizeF(geom.width()+dP.x(), geom.height()+dP.y()));
				    break;
				case Qt::SizeBDiagCursor:
				    if( isScale ) 
				    { 
				        curw->x_scale *= (geom.width()+((leftTop)?-1:1)*dP.x())/geom.width();
				        curw->y_scale *= (geom.height()-((leftTop)?-1:1)*dP.y())/geom.height();
				    }				
				    if( leftTop )
				    { 
					curw->moveF(QPointF(geom.x()+dP.x(),geom.y())); 
					curw->resizeF(QSizeF(geom.width()-dP.x(), geom.height()+dP.y())); 
				    }
				    else
				    {
					curw->moveF(QPointF(geom.x(),geom.y()+dP.y()));
					curw->resizeF(QSizeF(geom.width()+dP.x(), geom.height()-dP.y()));
				    }
				    break;
				case Qt::SizeVerCursor:
				    if( isScale ) 
					curw->y_scale *= (geom.height()+((leftTop)?-1:1)*dP.y())/geom.height();
				    if( leftTop )
				    {
					curw->moveF(QPointF(geom.x(),geom.y()+dP.y()));
					curw->resizeF(QSizeF(geom.width(), geom.height()-dP.y()));
				    }
				    else curw->resizeF(QSizeF(geom.width(), geom.height()+dP.y()));
				    break;				    
				case Qt::SizeHorCursor:
				    if( isScale ) 
					curw->x_scale *= (geom.width()+((leftTop)?-1:1)*dP.x())/geom.width();
				    if( leftTop )
				    {
					curw->moveF(QPointF(geom.x()+dP.x(),geom.y()));
					curw->resizeF(QSizeF(geom.width()-dP.x(), geom.height()));
				    }
				    else curw->resizeF(QSizeF(geom.width()+dP.x(), geom.height()));
				    break;
				case Qt::PointingHandCursor:
				    curw->moveF(curw->posF()+dP);
				    break;
			    }
			}			
			holdPnt = curp;
			update();
			//-- Set status bar --
			QRectF srect;
			for( int i_c = 0; i_c < children().size(); i_c++ )
			    if( qobject_cast<DevelWdgView*>(children().at(i_c)) && 
				    ((DevelWdgView*)children().at(i_c))->select( ) )
				srect = srect.united(((DevelWdgView*)children().at(i_c))->geometryF());
			mainWin()->statusBar()->showMessage(
			    QString(_("Elements: '%1' --- xy(%2:%3) wh[%4:%5]"))
				.arg(selectChilds().c_str())
				.arg(srect.x()/x_scale).arg(srect.y()/y_scale)
				.arg(srect.width()/x_scale).arg(srect.height()/y_scale),10000);
		    }
		    else
		    {
			bool isScale = QApplication::keyboardModifiers()&Qt::ControlModifier;
			//- Change widget geometry -
			switch(cursor().shape())
			{
		    	    case Qt::SizeHorCursor:
				if( isScale )	x_scale *= (float)curp.x()/sizeF().width();
				resizeF(QSizeF(curp.x(),sizeF().height()));
				break;
			    case Qt::SizeVerCursor:
				if( isScale )	y_scale *= (float)curp.y()/sizeF().height();
				resizeF(QSizeF(sizeF().width(),curp.y()));
			        break;
			    case Qt::SizeFDiagCursor:
				if( isScale )
				{
				    x_scale *= (float)curp.x()/sizeF().width();
				    y_scale *= (float)curp.y()/sizeF().height();
				}
				resizeF(QSizeF(curp.x(),curp.y()));
				break;	    
			}
			//-- Set status bar --
			mainWin()->statusBar()->showMessage(
			    QString(_("Page: '%1' --- xy(%2:%3) wh[%4:%5]"))
				.arg(id().c_str()).arg(posF().x()/x_scale).arg(posF().y()/y_scale)
				.arg(sizeF().width()/x_scale).arg(sizeF().height()/y_scale), 10000 );
		    }
		    return true;
		}
        	break;
	    }
	    case QEvent::KeyPress:
	    {
		QKeyEvent *key = static_cast<QKeyEvent*>(event);	    
		if( edit() )
		{
		    if( key->key() == Qt::Key_Escape )
		    {	
			//-- Unselect child widgets --
			for( int i_c = 0; i_c < children().size(); i_c++ )
			    if( qobject_cast<DevelWdgView*>(children().at(i_c)) )
			        ((DevelWdgView*)children().at(i_c))->setSelect(false);
			setEdit(false);
			update();			
			return true;
		    }
		}
		else
		{
		    QPointF dP(0,0);
		    switch( key->key() )
		    {
			case Qt::Key_Left: 	dP.setX(-1/x_scale);	break;
			case Qt::Key_Right: 	dP.setX(1/x_scale);	break;
			case Qt::Key_Up:	dP.setY(-1/y_scale);	break;
			case Qt::Key_Down:	dP.setY(1/y_scale);	break;
		    }
		    if( dP.isNull() )	break;
		    dP *= ((QApplication::keyboardModifiers()&Qt::ShiftModifier) ? 1 : 5);
		    for( int i_c = 0; i_c < children().size(); i_c++ )
		    {
		        DevelWdgView *curw = qobject_cast<DevelWdgView*>(children().at(i_c));
		        if( !curw || !curw->select() ) continue;
		        curw->moveF(curw->posF()+dP);
		    }
		    //-- Set status bar --
		    QRectF srect;
		    for( int i_c = 0; i_c < children().size(); i_c++ )
		        if( qobject_cast<DevelWdgView*>(children().at(i_c)) && 
		    	    ((DevelWdgView*)children().at(i_c))->select( ) )
			{
			    srect = srect.united(((DevelWdgView*)children().at(i_c))->geometryF());
			    saveGeom(((DevelWdgView*)children().at(i_c))->id());
			}
		    mainWin()->statusBar()->showMessage(
		        QString(_("Elements: '%1' --- xy(%2:%3) wh[%4:%5]"))
		    	    .arg(selectChilds().c_str())
			    .arg(srect.x()/x_scale).arg(srect.y()/y_scale)
			    .arg(srect.width()/x_scale).arg(srect.height()/y_scale),10000);			    
		    return true;
		}
		break;
	    }
    	}
    }
    
    //- Self widget view -
    if( edit() && editWdg && wLevel() <= 1 && editWdg->shape->event(editWdg,event) )	return true;    

    if( WdgView::event(event) )	return true;
    return QWidget::event(event);
}

//* Size points view widget              *
//****************************************
DevelWdgView::SizePntWdg::SizePntWdg( QWidget* parent ) : QWidget(parent), m_edit(false)
{
    //setAttribute(Qt::WA_NoSystemBackground);
    setMouseTracking(true);
}
			
void DevelWdgView::SizePntWdg::setSelArea( const QRect &geom, bool edit )
{
    m_edit = edit;
    
    if( geom.isValid() )
    {	
	QRegion reg;
    	if( m_edit )
	{
	    setGeometry(geom.adjusted(-7,-7,7,7));	    
	    reg = QRegion(rect()).subtracted(QRegion(rect().adjusted(7,7,-7,-7)));
	}
	else
	{
	    setGeometry(geom.adjusted(-3,-3,3,3));
	    //- Make widget's mask -
	    for(int i_p = 0; i_p < 9; i_p++)
		if( i_p != 4 )
		    reg+=QRegion(QRect(rect().x()+(i_p%3)*((rect().width()-7)/2),
		    		   rect().y()+(i_p/3)*((rect().height()-7)/2),7,7));
	}
	setMask(reg);
    }
    else setGeometry(geom);	
}

void DevelWdgView::SizePntWdg::paintEvent ( QPaintEvent *event )
{
    if( rect().isValid() )
    {
	QPainter pnt( this );
	pnt.setWindow( rect() );

	if( m_edit )
	{
	    pnt.fillRect(rect(),QBrush(Qt::black,Qt::Dense4Pattern));
	    pnt.setPen("black");
	    pnt.drawRect(rect().adjusted(6,6,-7,-7));
	}
	else
	{
	    pnt.setPen("black");
	    pnt.setBrush(QBrush(QColor("lightgreen")));
	    for(int i_p = 0; i_p < 9; i_p++)
	    {
		if( i_p == 4 ) continue;
		QRect anch(rect().x()+(i_p%3)*((rect().width()-7)/2),
		       rect().y()+(i_p/3)*((rect().height()-7)/2),6,6);
		pnt.drawRect(anch);
	    }
	}
    }
} 


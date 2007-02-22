
//OpenSCADA system module UI.Vision file: vis_devel_widgs.cpp
/***************************************************************************
 *   Copyright (C) 2004-2006 by Roman Savochenko                           *
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

#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QEvent>

#include "tsys.h"

#include "vis_devel_widgs.h"
#include "vis_devel.h"
#include "tvision.h"

using namespace VISION;

//****************************************
//* Inspector of attributes              *
//****************************************
InspAttr::InspAttr( QWidget * parent ) : QDockWidget(_("Attributes"),parent)
{
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
}

InspAttr::~InspAttr( )
{

}

//****************************************
//* Inspector of links                   *
//****************************************
InspLnk::InspLnk( QWidget * parent ) : QDockWidget(_("Links"),parent)
{
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
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    //setMinimumSize( QSize( 100, 100 ) );
    setMaximumSize( QSize( 200, 32767 ) );

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
	/*if( event->type() == QEvent::FocusOut && !popup_open )
	{
	    owner()->actWdgLibDel->setEnabled(false);
	    owner()->actWdgLibProp->setEnabled(false);
	    owner()->actWdgDel->setEnabled(false);
	    owner()->actWdgProp->setEnabled(false);
	    owner()->actWdgEdit->setEnabled(false);
	}*/
    }
    return QDockWidget::eventFilter( target, event );
}

void WdgTree::selectItem( )
{
    //Disable all actions
    owner()->actWdgLibDel->setEnabled(false);
    owner()->actWdgLibProp->setEnabled(false);
    owner()->actWdgDel->setEnabled(false);
    owner()->actWdgProp->setEnabled(false);
    owner()->actWdgEdit->setEnabled(false);

    //Get select list
    QList<QTreeWidgetItem *> sel_ls = treeW->selectedItems();
    if(sel_ls.size() != 1)	return;
    
    if(sel_ls.at(0)->parent())
    {
	//Enable widgets actions
	owner()->actWdgDel->setEnabled(true);
	owner()->actWdgProp->setEnabled(true);
	owner()->actWdgEdit->setEnabled(true);	
    }
    else
    {
	//Enable widget libraryes actions
	owner()->actWdgLibDel->setEnabled(true);
	owner()->actWdgLibProp->setEnabled(true);
    }
    //Get current widget
    owner()->work_wdg = sel_ls.at(0)->text(2).toAscii().data();
    QTreeWidgetItem *cur_el = sel_ls.at(0)->parent();
    while(cur_el)
    {
	owner()->work_wdg.insert(0,string(cur_el->text(2).toAscii().data())+".");
	cur_el=cur_el->parent();
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
	AutoHD<VCA::LibWdg> wlb = mod->engine().at().wlbAt(list_wl[i_l]);
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
	    if( !wdg.at().container() )	continue;
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
}
	
void WdgTree::ctrTreePopup( )
{
    QMenu popup;
    QTreeWidget *lview = (QTreeWidget *)sender();
	
    //Add actions
    popup.addAction(owner()->actWdgProp);
    popup.addAction(owner()->actWdgEdit);
    popup.addAction(owner()->actWdgDel);
    popup.addSeparator();
    popup.addAction(owner()->actWdgLibProp);
    popup.addAction(owner()->actWdgLibAdd);
    popup.addAction(owner()->actWdgLibDel);

    QAction *rez = popup.exec(QCursor::pos());
    
    popup.clear();
}			    

//****************************************
//* Project's tree                       *
//****************************************
ProjTree::ProjTree( VisDevelop * parent ) : QDockWidget(_("Projects"),(QWidget*)parent)
{
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    //setMinimumSize( QSize( 100, 100 ) );
    setMaximumSize( QSize( 200, 32767 ) );        

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

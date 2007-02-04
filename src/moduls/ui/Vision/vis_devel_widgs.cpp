
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

#include <QTreeWidget>
#include <QVBoxLayout>

#include "vis_devel_widgs.h"
#include "tvision.h"

using namespace VISION;

ProjTree::ProjTree( QWidget * parent ) : QDockWidget(_("Projects"),parent)
{
    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setMinimumSize(120, 200);
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

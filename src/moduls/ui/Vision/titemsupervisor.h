// file "titemsupervisor.h"

/***************************************************************************
 *   Copyright (C) 2005 by Evgen Zaichuk                               
 *   evgen@diyaorg.dp.ua                                                     
 *                                                                         
 *   This program is free software; you can redistribute it and/or modify  
 *   it under the terms of the GNU General Public License as published by  
 *   the Free Software Foundation; either version 2 of the License, or     
 *   (at your option) any later version.                                   
 *                                                                         
 *   This program is distributed in the hope that it will be useful,       
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *   GNU General Public License for more details.                          
 *                                                                         
 *   You should have received a copy of the GNU General Public License     
 *   along with this program; if not, write to the                         
 *   Free Software Foundation, Inc.,                                       
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             
 ***************************************************************************/

#ifndef TITEMSV
#define TITEMSV 

#include <qscrollview.h> 
#include <qstringlist.h> 
#include <qvbuttongroup.h>

#include "tframedev.h"
#include "titem.h"

class QAction; 
class QLabel; 
class QWorkspace; 
class QListView;
class QSplitter; 
class QTable;
class QTabBar;
class QToolBox;
class QTableItem;

namespace VISION
{

//Класс супервизора объектов
class TItemSupervisor : public QScrollView
{
   Q_OBJECT
   public:
      TItemSupervisor(QWidget *parent = 0, const char *name = 0, int wflags = 0);
      //обновить полностью:
      void updateAll(const QString *category = NULL, const QString *propName = NULL, const bool clear = true); 
      void setFrame(TFrameDev*);
   signals:
      
   protected:
      
   private slots:
      //void whenSelectionChanged();
      void valueChanged (int row, int col);
      void showColorDialog();
      void showDynamicDialog();
      
   private:
      void clearSupervisor();
      
      TFrameDev *frame;
      QTable *table;
      
};

}

#endif

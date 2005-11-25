// file "tvisionrun.h"

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

#ifndef TVISIONRUN
#define TVISIONRUN

#include <qwidget.h>
#include <qmainwindow.h> 
#include <qstringlist.h> 
#include <qlistview.h>
#include <qvbuttongroup.h>

#include "tconfiguration.h"


class QAction; 
class QLabel; 
class QTabBar;
class QToolBox;

namespace VISION
{

//Класс "Главное окно режима исполнения"
class TVisionRun : public QMainWindow 
{ 
  Q_OBJECT 
public: 
  TVisionRun(void *v, TConfiguration *cfg, QWidget *parent = 0, const char *name = 0, int wflags = 0); 
   
protected:
   void closeEvent(QCloseEvent* ce);
   
private:
   void *v;
   TConfiguration *cfg;
   
   QWidget *centralWidget;
   
   QAction *exitAct;
   
   QPopupMenu *systemMenu;
   
   void createActions();
   void createMenus();
   void createStatusBar();
   void createWorkspace();
  
private slots:
   void exitVR();
};

}

#endif

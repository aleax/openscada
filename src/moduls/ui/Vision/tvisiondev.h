// file "tvision.h"

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

#ifndef TVISIONDEV 
#define TVISIONDEV 

#include <qmainwindow.h> 
#include <qstringlist.h> 
#include <qlistview.h>
#include <qvbuttongroup.h>

#include "tframedev.h"
#include "titemsupervisor.h"
#include "tconfiguration.h"
#include "tvisionrun.h"


class QAction; 
class QLabel; 
class QWorkspace; 
class QSplitter; 
class QTabBar;
class QToolBox;

namespace VISION
{
typedef list<QVButtonGroup *> listQVButtonGroup;
#define reservedChar "#"
#define unnamedDocName "unnamed document.frm"

//Класс "Лист дерева проекта":
class TPrjListViewItem : public QListViewItem
{
public:
   TPrjListViewItem(QListView * parent);
   TPrjListViewItem(TPrjListViewItem * parent);

   virtual void startRename(int col); 
   QString getTextBeforeRenaming() const;
   
protected:
    virtual void cancelRename(int col);
private:
   QString textBeforeRenaming;
};

//Класс "Главное окно режима разработки"
class TVisionDev : public QMainWindow 
{ 
  Q_OBJECT 
public: 
  TVisionDev(void *v, TConfiguration *cfg, QWidget *parent = 0, const char *name = 0, int wflags = 0); 

protected: 
  void closeEvent(QCloseEvent *event); 
  void contextMenuEvent(QContextMenuEvent *event);
  
private slots: 
  TFrameDev* newFrame();
  bool saveFrame();
  bool deleteFrame();
  void selectAllItems(); //выбрать все элементы
  void framesMenuAboutToShow(); //обновление пунков меню MDI
  void framesMenuActivated(int id); //Переключение окна MDI
  
  void addItem(int x, int y); //добавление элемента в кадр (сигнал формируется в TFrameViewDev)
  void selectingItem(); //выделение/снятие выделения другого элемента в кадре (сигнал формируется в TFrameViewDev)
  void movingItem(); //перемещение выделенных элементов в кадре (сигнал формируется в TFrameViewDev)
  void resizingItem(); //изменение размеров выделенного элемента в кадре (сигнал формируется в TFrameViewDev)
  void deletingItem(); //удаление выделенных элементов в кадре (сигнал формируется в TFrameViewDev)
  
  void mouseMove(int x, int y); //перемещение мыши в в кадре (сигнал формируется в TFrameViewDev)
  void mousePress(int x, int y); //щелчок мышью в в кадре (сигнал формируется в TFrameViewDev)
  
  void addItemButtonClicked(bool inserting);
  
  void windowActivated(QWidget * w);
  void saveCfg(); 
  
  void runtimeMode();
  void runtimeIsClosed();
  //bool saveFrameAs(); 
  //void find(); 
  //void about();
  
  void lvDoubleClicked(QListViewItem *, const QPoint &, int);
  void lvItemRenamed (QListViewItem *, int, const QString &);
  void lvCurrentChanged(QListViewItem *);
private:
  void openFrame(const QString &fileName); 
  
  QPopupMenu *frameMenu; 
  QPopupMenu *projectMenu; 
  //QPopupMenu *editMenu; 
  //QPopupMenu *selectSubMenu; 
  //QPopupMenu *toolsMenu; 
  //QPopupMenu *optionsMenu; 
  QPopupMenu *framesMenu; 
  //QPopupMenu *helpMenu; 
  QPopupMenu *runtimeMenu;
  
  QToolBar *frameToolBar; 
  QToolBar *editToolBar;
  QToolBar *projectToolBar;
  QToolBar *widgetsToolBar;
  QToolBar *itemSVToolBar;
  QToolBar *framesTreeToolBar;
  QToolBox *widgetsToolBox;
  
  listQVButtonGroup listBtGroup;
  
  QAction *newFrameAct;
  QAction *saveCfgAct;
  QAction *saveFrameAct; 
  QAction *deleteFrameAct;
  QAction *selAllItemsFrameAct;
  QAction *runtimeAct;
  
  QLabel *lbSBMessage1;
  QLabel *lbSBMessage2;
  QLabel *lbSBMessage3;
  QLabel *lbSBPosition;
  QLabel *lbSBSize;
  QLabel *lbSBCAPS;
  QLabel *lbSBNUM;
  QLabel *lbSBSCRL;
  
  QListView *lvFrames;
  TPrjListViewItem *lviProject;
  TPrjListViewItem *lviFrames;
  
  
  QSplitter *splitter1;
  //QSplitter *splitter2;
  QWorkspace *workspace;
  QTabBar *tbWidgets;
  TItemSupervisor *itemSupervisor;
  void *v;
  TConfiguration *cfg;
  TVisionRun *visionRun;
  
  void createActions(); 
  void createMenus(); 
  void createToolBars(); 
  void createStatusBar();
  void createWorkspace();
  
  bool newFrameOpen; //открытие существующего кадра
  bool addingAfterRename; //перед переименованием кадра добавить его в проект 
                          // (кадр только что был добавлен и переименован)
};

}
#endif

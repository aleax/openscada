// file "tvision.cpp"

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

#include <vector>
#include <string>
 
#include <qaction.h> 
#include <qapplication.h> 
#include <qcombobox.h> 
#include <qfiledialog.h> 
#include <qlabel.h> 
#include <qlineedit.h> 
#include <qmenubar.h> 
#include <qmessagebox.h> 
#include <qpopupmenu.h> 
#include <qsettings.h> 
#include <qstatusbar.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qtabbar.h>
#include <qpushbutton.h>
#include <qworkspace.h>
#include <qtoolbox.h>
#include <qvbox.h>
#include <qimage.h>

#include <qmessagebox.h>


#include "tvisiondev.h"
#include "tvision.h"

#include "tdbgw.h"

#include "xpm/newFrame_xpm.xpm"
#include "xpm/saveFrame_xpm.xpm"
#include "xpm/deleteFrame_xpm.xpm"
#include "xpm/saveCfg_xpm.xpm"
#include "xpm/deleteItems_xpm.xpm"

using std::string;
using std::vector;

using namespace VISION;

//------------------------------------------------TVisionDev---------------------------------------
TVisionDev::TVisionDev(void *v, TConfiguration *cfg, QWidget *parent, const char *name, int wflags) 
    : QMainWindow(0, name, WDestructiveClose),
    v(v)
{
    mod->regWin( this );
     
  if (cfg != NULL )
     this->cfg = cfg;
     else cfg = new TConfiguration();
  this->visionRun = visionRun;
     
  createActions(); 
  createMenus(); 
  createToolBars(); 
  createStatusBar(); 
  //readSettings(); 
  setCaption(tr("OpenSCADA VISION Development")); 
  //setIcon(QPixmap::fromMimeSource("icon.png")); 
  createWorkspace();
  
  newFrameOpen = false;
  addingAfterRename = false;
} 

TVisionDev::~TVisionDev()
{
    mod->unregWin( this );
}

void TVisionDev::windowActivated(QWidget * w)
{//QMessageBox::information( NULL, "workspace", "activated");
   if (w == 0)
      lvFrames->setSelected(lvFrames->selectedItem(), false);
      else
         {//проход по веткам узла Frames и выделение соотв-й ветки кадра:
         QListViewItem *item = lviFrames->firstChild();
         if (item != 0)
            do
               {
	        if (item->text(0).lower() == w->caption().lower())
	           {
	            lvFrames->setSelected(item, true);
	            item = 0;
	           }
	           else item = lviFrames->nextSibling();
	       }
            while (item != 0);
	 }
   
   itemSupervisor->setFrame((TFrameDev *)workspace->activeWindow());
   itemSupervisor->updateAll();
}

void TVisionDev::createActions() 
{ 
  newFrameAct = new QAction(QIconSet(QImage(newFrame_xpm)),tr("&New"), tr("Ctrl+N"), this); 
  newFrameAct->setStatusTip(tr("Create a new frame"));
  connect(newFrameAct, SIGNAL(activated()), this, SLOT(newFrame()));
  
  saveFrameAct = new QAction(QIconSet(QImage(saveFrame_xpm)),tr("&Save"), tr("Ctrl+S"), this); 
  saveFrameAct->setStatusTip(tr("Save current frame")); 
  connect(saveFrameAct, SIGNAL(activated()), this, SLOT(saveFrame()));
  
  selAllItemsFrameAct = new QAction(tr("Select all"), tr("Ctrl+A"), this); 
  selAllItemsFrameAct->setStatusTip(tr("Select all")); 
  connect(selAllItemsFrameAct, SIGNAL(activated()), this, SLOT(selectAllItems()));
  
  saveCfgAct = new QAction(QIconSet(QImage(saveCfg_xpm)),tr("Save confi&guration"), tr("Ctrl+Shift+S"), this); 
  saveCfgAct->setStatusTip(tr("Save configuration")); 
  connect(saveCfgAct, SIGNAL(activated()), this, SLOT(saveCfg()));
  
  delSelItemsAct = new QAction(QIconSet(QImage(deleteItems_xpm)),tr("&Delete Items"), tr(""), this); 
  delSelItemsAct->setStatusTip(tr("Delete frame")); 
  connect(delSelItemsAct, SIGNAL(activated()), this, SLOT(deleteItems()));
  
  deleteFrameAct = new QAction(QIconSet(QImage(deleteFrame_xpm)),tr("&Delete Frame"), tr("Ctrl+D"), this); 
  deleteFrameAct->setStatusTip(tr("Delete frame")); 
  connect(deleteFrameAct, SIGNAL(activated()), this, SLOT(deleteFrame()));
  
  runtimeAct = new QAction(/*QIconSet(QImage(newframe)),*/tr("&Runtime"), tr("Alt+R"), this); 
  runtimeAct->setStatusTip(tr("Call Runtime")); 
  connect(runtimeAct, SIGNAL(activated()), this, SLOT(runtimeMode()));
  
  aboutAct = new QAction(tr("About..."), tr(""), this); 
  aboutAct->setStatusTip(tr("About VISION")); 
  connect(aboutAct, SIGNAL(activated()), this, SLOT(helpAbout()));
  
}

void TVisionDev::createMenus() 
{ 
  frameMenu = new QPopupMenu(this); 
  newFrameAct->addTo(frameMenu);
  saveFrameAct->addTo(frameMenu);
  deleteFrameAct->addTo(frameMenu);
  
  projectMenu = new QPopupMenu(this); 
  saveCfgAct->addTo(projectMenu);
  
  editMenu = new QPopupMenu(this); 
  selAllItemsFrameAct->addTo(editMenu);
  delSelItemsAct->addTo(editMenu);
  
  runtimeMenu = new QPopupMenu(this); 
  runtimeAct->addTo(runtimeMenu);
  
  helpMenu = new QPopupMenu(this); 
  aboutAct->addTo(helpMenu);
  
  framesMenu = new QPopupMenu(this);
  framesMenu->setCheckable(TRUE);
  //обновление списка пунктов меню при выборе пункта меню Frames:
  connect(framesMenu, SIGNAL(aboutToShow()), this, SLOT(framesMenuAboutToShow()));
  
  //------------------------------
  menuBar()->insertItem(tr("&Frame"), frameMenu); 
  menuBar()->insertItem(tr("&Project"), projectMenu); 
  menuBar()->insertItem(tr("&Edit"), editMenu); 
  /*menuBar()->insertItem(tr("&Tools"), toolsMenu); 
  menuBar()->insertItem(tr("&Options"), optionsMenu); */
  menuBar()->insertItem( "Frame&s", framesMenu );
  menuBar()->insertSeparator();
  menuBar()->insertItem( "&Runtime", runtimeMenu );
  menuBar()->insertItem( "&Help", helpMenu );
  /*menuBar()->insertSeparator(); 
  menuBar()->insertItem(tr("&Help"), helpMenu); 
  */
  
}

void TVisionDev::createToolBars() 
{ 
  frameToolBar = new QToolBar(tr("Frame"), this); 
  newFrameAct->addTo(frameToolBar);
  saveFrameAct->addTo(frameToolBar);
  deleteFrameAct->addTo(frameToolBar);
  //openAct->addTo(fileToolBar); 
  //saveAct->addTo(fileToolBar); 
  
  projectToolBar = new QToolBar(tr("Project"), this); 
  saveCfgAct->addTo(projectToolBar);
  
  editToolBar = new QToolBar(tr("Edit"), this); 
  //cutAct->addTo(editToolBar); 
  //copyAct->addTo(editToolBar); 
  //pasteAct->addTo(editToolBar); 
  //editToolBar->addSeparator(); 
  //findAct->addTo(editToolBar); 
  //selAllItemsFrameAct->addTo(editToolBar); 
  delSelItemsAct->addTo(editToolBar); 
  
  // //////////////////////////////////////////////////////////
  //Дерево проекта:
  framesTreeToolBar = new QToolBar(tr("Project"), this);
  moveDockWindow(framesTreeToolBar, Qt::DockLeft);
  lvFrames = new QListView(framesTreeToolBar);
  framesTreeToolBar->setResizeEnabled(true);
  framesTreeToolBar->setCloseMode(QDockWindow::Always);
  setDockEnabled(framesTreeToolBar, Qt::DockTop, false);
  setDockEnabled(framesTreeToolBar, Qt::DockBottom, false);
  lvFrames->setDefaultRenameAction(QListView::Reject);
  connect(lvFrames, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)), this,
                    SLOT(lvDoubleClicked(QListViewItem *, const QPoint &, int)));
  //connect(lvFrames, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(lvCurrentChanged(QListViewItem *)));
  connect(lvFrames, SIGNAL(itemRenamed(QListViewItem *, int, const QString &)), this,
                    SLOT(lvItemRenamed(QListViewItem *, int, const QString &)));
  //
  lvFrames->addColumn("1");
  lvFrames->setColumnText(0, "Project");
  lvFrames->setSorting(-1);
  QStringList frames = cfg->getProjectFrames();
  lviProject = new TPrjListViewItem(lvFrames);
  lviProject->setText(0, tr("Project"));
  lviProject->setRenameEnabled(0, false);
  lviFrames = new TPrjListViewItem(lviProject);
  lviFrames->setText(0, tr("Frames"));
  lviFrames->setRenameEnabled(0, false);
  for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
     {
      TPrjListViewItem *item = new TPrjListViewItem(lviFrames);
      item->setText(0, (*i));
     }
  // //////////////////////////////////////////////////////////////
  
  widgetsToolBar = new QToolBar(tr("Widgets"), this);
  moveDockWindow(widgetsToolBar, Qt::DockRight);
  widgetsToolBox = new QToolBox(widgetsToolBar, tr("Widgets"));
  widgetsToolBar->setResizeEnabled(true);
  widgetsToolBar->setCloseMode(QDockWindow::Always);
  setDockEnabled(widgetsToolBar, Qt::DockTop, false);
  setDockEnabled(widgetsToolBar, Qt::DockBottom, false);
  
  
  // /////////////////////////////////////////////////////////
  //Супервизор объектов:
  itemSVToolBar = new QToolBar(tr("Item SV"), this);
  moveDockWindow(itemSVToolBar, Qt::DockRight);
  itemSupervisor = new TItemSupervisor(itemSVToolBar, "item SV");
  itemSVToolBar->setResizeEnabled(true);
  itemSVToolBar->setCloseMode(QDockWindow::Always);
  setDockEnabled(itemSVToolBar, Qt::DockTop, false);
  setDockEnabled(itemSVToolBar, Qt::DockBottom, false);
  // /////////////////////////////////////////////////////////
  
  // /////////////////////////////////////////////////////////
  //Кнопки добавления элементов отображения
  // --------------------------------------------------------------------------------------
  // Группа Standart:
  QVBox *vb = new QVBox(); 
  QVButtonGroup *btGroup = new QVButtonGroup();
  listBtGroup.push_back(	btGroup);
  widgetsToolBox->addItem(btGroup, tr("Standart"));
  // Элемент "Rectangle":
  QPushButton *btRectangle = new QPushButton(btGroup, tr("Rectangle"));
  connect(btRectangle, SIGNAL(toggled(bool)), this, SLOT(addItemButtonClicked(bool)));
  btRectangle->setIconSet(QIconSet(QPixmap(TCRectangle::itemImage())));
  btRectangle->setToggleButton(true);
  btRectangle->setText(tr(TCRectangle::itemName()));
  btGroup->insert(btRectangle, TCRectangle::RTTI);
  btRectangle->show();
  // Элемент "Line":
  QPushButton *btLine = new QPushButton(btGroup, tr("Line"));
  connect(btLine, SIGNAL(toggled(bool)), this, SLOT(addItemButtonClicked(bool)));
  btLine->setIconSet(QIconSet(QPixmap(TCLine::itemImage())));
  btLine->setToggleButton(true);
  btLine->setText(tr(TCLine::itemName()));
  btGroup->insert(btLine, TCLine::RTTI);
  btLine->show();
  // Элемент "Text":
  QPushButton *btText = new QPushButton(btGroup, tr("Text"));
  connect(btText, SIGNAL(toggled(bool)), this, SLOT(addItemButtonClicked(bool)));
  btText->setIconSet(QIconSet(QPixmap(TCText::itemImage())));
  btText->setToggleButton(true);
  btText->setText(tr(TCText::itemName()));
  btGroup->insert(btText, TCText::RTTI);
  btText->show();
  // Элемент "Image":
  QPushButton *btImage = new QPushButton(btGroup, tr("Image"));
  connect(btImage, SIGNAL(toggled(bool)), this, SLOT(addItemButtonClicked(bool)));
  btImage->setIconSet(QIconSet(QPixmap(TCImage::itemImage())));
  btImage->setToggleButton(true);
  btImage->setText(tr(TCImage::itemName()));
  btGroup->insert(btImage, TCImage::RTTI);
  btImage->show();
  // --------------------------------------------------------------------------------------------------
  // Группа #2
  /*QVBox *vb2 = new QVBox(); 
  widgetsToolBox->addItem(vb2, tr("Standart2"));
  QPushButton *btRectangle2 = new QPushButton(vb2, tr("Rectangle2"));
  //btRectangle->setIconSet();
  btRectangle2->setToggleButton(true);
  btRectangle2->setText("22222");
  btRectangle2->show();
  
  QPushButton *btRectangle3 = new QPushButton(vb2, tr("Rectangle3"));
  //btRectangle->setIconSet();
  btRectangle3->setToggleButton(true);
  btRectangle3->setText("3333");
  btRectangle3->show();
  */
  // tbWidgets->insertTab(standartTab, -1);
  // /////////////////////////////////////////////////////////
}    

 
void TVisionDev::createStatusBar() 
{ 
  lbSBMessage1 = new QLabel(" W999 ", this); 
  lbSBMessage1->setAlignment(AlignHCenter); 
  lbSBMessage1->setMinimumSize(lbSBMessage1->sizeHint()); 
  lbSBMessage1->clear();
  
  lbSBMessage2 = new QLabel("1234567890", this); 
  lbSBMessage2->setMinimumSize(lbSBMessage2->sizeHint());
  lbSBMessage2->clear();
  lbSBMessage3 = new QLabel("1234567890", this); 
  lbSBMessage3->setMinimumSize(lbSBMessage3->sizeHint());
  lbSBMessage3->clear();
  
  lbSBPosition = new QLabel(tr(" X:1024; Y:1024 "), this); 
  lbSBPosition->setAlignment(AlignHCenter); 
  lbSBPosition->setMinimumSize(lbSBPosition->sizeHint());      
  lbSBPosition->clear(); 
  
  lbSBSize = new QLabel(tr(" W:1024; H:1024 "), this); 
  lbSBSize->setAlignment(AlignHCenter); 
  lbSBSize->setMinimumSize(lbSBSize->sizeHint());      
  lbSBSize->clear(); 
  
  lbSBCAPS = new QLabel(tr("CAPS"), this); 
  lbSBCAPS->setAlignment(AlignHCenter); 
  lbSBCAPS->setMinimumSize(lbSBCAPS->sizeHint());      
  lbSBCAPS->clear(); 
  
  lbSBNUM = new QLabel(tr("NUM"), this); 
  lbSBNUM->setAlignment(AlignHCenter); 
  lbSBNUM->setMinimumSize(lbSBNUM->sizeHint());      
  lbSBNUM->clear(); 
  
  lbSBSCRL = new QLabel(tr("SCRL"), this); 
  lbSBSCRL->setAlignment(AlignHCenter); 
  lbSBSCRL->setMinimumSize(lbSBSCRL->sizeHint());      
  lbSBSCRL->clear(); 
   
  statusBar()->addWidget(lbSBMessage1, 1); 
  statusBar()->addWidget(lbSBMessage2); 
  statusBar()->addWidget(lbSBMessage3); 
  statusBar()->addWidget(lbSBPosition);
  statusBar()->addWidget(lbSBSize);
  statusBar()->addWidget(lbSBCAPS);
  statusBar()->addWidget(lbSBNUM);
  statusBar()->addWidget(lbSBSCRL);
  
  //connect(spreadsheet, SIGNAL(currentChanged(int, int)), 
  //        this, SLOT(updateCellIndicators())); 
  // updateCellIndicators(); 
}

void TVisionDev::createWorkspace()
{
  workspace = new QWorkspace(this);
  connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(windowActivated(QWidget *)));
  setCentralWidget(workspace);
}

void TVisionDev::contextMenuEvent(QContextMenuEvent *event) 
{ 
  QPopupMenu contextMenu(this); 
  //cutAct->addTo(&contextMenu); 
  //copyAct->addTo(&contextMenu); 
  //pasteAct->addTo(&contextMenu); 
  contextMenu.exec(event->globalPos()); 
}      

void TVisionDev::closeEvent(QCloseEvent *event) 
{ 
    workspace->closeAllWindows();
    event->accept();
  /*if (activeEditor()) 
    event->ignore(); 
  else 
    event->accept(); 
  */
}

TFrameDev* TVisionDev::newFrame()
{
  TFrameDev* w = new TFrameDev(workspace, VISION::Development, "frame1");
  connect (w, SIGNAL(addItem(int, int)), this, SLOT(addItem(int, int)));
  connect (w, SIGNAL(selectingItem()), this, SLOT(selectingItem()));
  connect (w, SIGNAL(movingItem()), this, SLOT(movingItem()));
  connect (w, SIGNAL(resizingItem()), this, SLOT(resizingItem()));
  connect (w, SIGNAL(deletingItem()), this, SLOT(deletingItem()));
  connect (w, SIGNAL(mouseMove(int, int)), this, SLOT(mouseMove(int, int)));
  connect (w, SIGNAL(mousePress(int, int)), this, SLOT(mousePress(int, int)));
  //connect( w, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );
  w->setCaption(unnamedDocName);
  //w->setIcon( QPixmap("document.xpm") );
  // show the very first window in maximized mode
  if ( workspace->windowList().isEmpty() )
      w->showMaximized();
  else
      w->show();
  
  if (!newFrameOpen)
     {
      //добавление нового кадра в проект:
      TPrjListViewItem *item = new TPrjListViewItem(lviFrames);
      //item->setText(0, unnamedDocName);
      item->setRenameEnabled(0, true);
      addingAfterRename = true;
      
      QStringList frames = cfg->getProjectFrames();
      QStringList framesUnnamedDocName; //список имен кадров, начинающихся с unnamedDocName
      QString frameName; //имя добавляемого кадра
      //Заполнение списка framesUnnamedDocName. В этом списке ищем строки, начинающиеся с unnamedDocName;
      // имя добавляемого кадра будет состоять из unnamedDocName + следующее целое из framesUnnamedDocName:
      for (QStringList::iterator i = frames.begin(); i != frames.end(); i++)
         if ((*i).left(QString(unnamedDocName).length()) == unnamedDocName)
	    framesUnnamedDocName.push_back(*i);
      if (framesUnnamedDocName.size() > 0)
         {
	  unsigned int max = 0; //максимальное число после строки unnamedDocName
          for (QStringList::iterator i = framesUnnamedDocName.begin(); i != framesUnnamedDocName.end(); i++)
	    {
	     //поиск максимального числа после строки unnamedDocName:
	     QString s = (*i).right((*i).length()-QString(unnamedDocName).length());
	     bool ok;
	     unsigned int n = s.toUInt(&ok);
	     if (ok)
		 if (max < n)
		    max = n;
	    }
	    frameName = unnamedDocName + QString::number(max + 1);
	 }
	 else frameName = QString(unnamedDocName);// + QString("1");
      bool result = cfg->addFrameToProject(frameName);
      if (!result)
         {
	  QMessageBox::critical( NULL, "Adding error", "Error occured when adding frame '" + frameName + "'!");
	  workspace->closeActiveWindow();
	 }
	 else 
	    {
	     item->setText(0, frameName);
	     item->startRename(0);
	    }
      
      
      selectingItem();
     }
  //framesMenuAboutToShow();//!!!
  return w;
}

bool TVisionDev::saveFrame()
{
   if ((TFrameDev *)workspace->activeWindow() != NULL)
      ((TFrameDev *)workspace->activeWindow())->saveFrame(subdir + workspace->activeWindow()->caption());
}

void TVisionDev::saveCfg()
{
   cfg->saveCfgToFile();
}

void TVisionDev::openFrame(const QString &fileName)
{
   bool result = false; //кадр  с именем fileName уже открыт
   QWidgetList openFrames = workspace->windowList();
   for (QWidgetList::iterator i = openFrames.begin(); i != openFrames.end(); i++)
      if ( (/*(TFrameDev*)*/(*i))->caption() == fileName)
	         {
                   (*i)->showNormal();
                   (*i)->setFocus();
		   result = true;
		   break;
		  }
    if (!result)
       {
        QFile file(subdir+fileName);
        QXmlSimpleReader reader; 
  
        TFrameDev *frm = newFrame();
        TSaxHandler handler(frm->frameView()); 
        reader.setContentHandler(&handler); 
        reader.setErrorHandler(&handler); 
        bool result = reader.parse(&file); 
        if (!result)
           {
            QMessageBox::critical( NULL, "Opening error", 
	                "Error occured when opening frame '" + fileName + "'!");
	     delete workspace->activeWindow();
           }
           else
              { //установка имени окна MDI:
	       frm->setCaption(fileName);//frm->getName());
	       //QListViewItem *item = new QListViewItem(lviFrames, fileName);
               //item->setRenameEnabled(0, true);
	      }
        }//if !result
   selectingItem();
}

void TVisionDev::deleteItems()
{
   if ((TFrameDev*)workspace->activeWindow() != NULL)
      ((TFrameDev*)workspace->activeWindow())->frameView()->delSelectedItems();
}

bool TVisionDev::deleteFrame()
{
   bool result = false;
   QListViewItem *item = lvFrames->selectedItem();
   if (item != 0)
      if (item->parent() == lviFrames)
         {
	  result = true;
	  if (QMessageBox::question(NULL, "Delete Frame", "Do you want to delete frame '" + item->text(0) + "'?", 
	                                                                QMessageBox::Yes, QMessageBox::No ,
									QMessageBox::Cancel) == QMessageBox::Yes)
	     {//Удаление кадра из проекта:
	     if (cfg->deleteFrameFromProject(item->text(0)))
	        {
	         delete item;
	         selectingItem();
	         result = true;
	        }
	        else
	           {
		    QMessageBox::warning( NULL, "Delete Frame", "Can't delete Frame '" + item->text(0) + "'!");
		    return false;
		   }
	     }
         }
   if (!result)
      QMessageBox::warning( NULL, "Delete Frame", "No Frame is selected!");
   
   return result;
}

void TVisionDev::runtimeMode()
{
/*

TListParamDev listParamDev;
TDBGW db;
string s;
dbgw->getParamsDev(listParamDev, intType);
for (TListParamDev::iterator i = listParamDev.begin(); i != listParamDev.end(); i++)
   {
    s += (*i).name + ": ";
    for (list <TAttrDev>::iterator j = (*i).properties.begin(); j != (*i).properties.end(); j++)
       {
        s += "\tname = " + (*j).name + "\tdescr = " + (*j).descr + "\ttype = ";
	switch ((*j).type)
	   {
	    case intType : s += "intType"; break;
	    case floatType : s += "floatType"; break;
	    case stringType : s += "stringType"; break;
	    case enumType : s += "enumType"; break;
	    case colorType : s += "colorType"; break;
	    case unknownType : s += "unknownType"; break;
	    default : s += "type = ???";
	   }
	s += "\tvalue = " + (*j).value + ";\n";
       }
   }
QMessageBox::warning( NULL, "dbgw", s);
QString paramName = "TimeSYS";
QString attrName = "min";
QString value;
/*if (dbgw->getValue(paramName, attrName, value))
   QMessageBox::warning( NULL, "dbgw", value + " Ok");
   else QMessageBox::warning( NULL, "dbgw", value + " Not Ok");
*/
/////

   ((TVision *)v)->callRuntime();
   if (visionRun != NULL)
      {//QMessageBox::warning( NULL, "runtime", "show TVisionRun");
       ;//visionRun->show();
      }
}

void TVisionDev::runtimeIsClosed()
{
   visionRun = NULL;
   QMessageBox::warning( NULL, "info", "runtime is closed");
}
//---------------------------------------------------------------------------------------------------
//Обновление пунктов меню "Windows":
void TVisionDev::framesMenuAboutToShow()
{
    framesMenu->clear();
    int cascadeId = framesMenu->insertItem(tr("&Cascade"), workspace, SLOT(cascade()));
    int tileId = framesMenu->insertItem(tr("&Tile"), workspace, SLOT(tile()));
    //int horTileId = framesMenu->insertItem(tr("Tile &Horizontally"), this, SLOT(tileHorizontal()));
    if (workspace->windowList().isEmpty()) 
       {
        framesMenu->setItemEnabled(cascadeId, FALSE);
        framesMenu->setItemEnabled(tileId, FALSE);
        //framesMenu->setItemEnabled( horTileId, FALSE );
       }
    framesMenu->insertSeparator();
    QWidgetList windows = workspace->windowList();
    for (int i = 0; i < int(windows.count()); ++i) 
       {
        int id = framesMenu->insertItem(windows.at(i)->caption(),
                                         this, SLOT(framesMenuActivated(int)));
        framesMenu->setItemParameter(id, i);
        framesMenu->setItemChecked(id, workspace->activeWindow() == windows.at(i));
       }
    //QMessageBox::information( NULL, "info", "press" );
}
//---------------------------------------------------------------------------------------------------
//Переключение окна MDI:
void TVisionDev::framesMenuActivated(int id)
{
    QWidget* w = workspace->windowList().at(id);
    if (w)
        w->showNormal();
    w->setFocus();
    selectingItem();//itemSupervisor->updateAll();
    //QMessageBox::information( NULL, "info", "pressActiv" );
}
//--------------------------------------------------------------------------------------------------
//добавление элемента в кадр:
void TVisionDev::addItem(int x, int y)
{
//QMessageBox::information( NULL, "info", "addItem from TVisionDev" ); //     
   //проход по всем группам кнопок QToolBox в поиске вдавленной; 
   // если такая находится, то посылка сообщения о добавлении элемента, сброс кнопки и выход из цикла
   QButton *bt;
   for (listQVButtonGroup::iterator i = listBtGroup.begin(); i != listBtGroup.end(); i++)
      if ((*i)->selectedId() != -1)
         {
          ((TFrameDev *)workspace->activeWindow())->frameView()->addItem((*i)->selectedId(), x, y);
	  ((QPushButton *)((*i)->selected()))->setOn(false);//setState(QButton::Off);//toggled(false);//
	  break;
	 }
}

//выбрать все элементы:
void TVisionDev::selectAllItems()
{
   if ((TFrameDev *)workspace->activeWindow() != NULL)
      ((TFrameDev *)workspace->activeWindow())->selectAll();
}

//выделение/снятие выделения другого элемента в кадре:
void TVisionDev::selectingItem()
{
//QMessageBox::information( NULL, "info", "selectingItem from TVisionDev" ); //     
   //передача сообщения инспектору элементов о необходимости полного обновления:
   itemSupervisor->setFrame((TFrameDev *)workspace->activeWindow());
   itemSupervisor->updateAll();
}

//перемещение выделенных элементов в кадре (сигнал формируется в TFrameViewDev)
void TVisionDev::movingItem()
{
   //передача сообщения инспектору элементов о необходимости полного обновления:
   itemSupervisor->updateAll();
   mousePress(0, 0);
}

//изменение размеров выделенного элемента в кадре (сигнал формируется в TFrameViewDev)
void TVisionDev::resizingItem()
{
   //передача сообщения инспектору элементов о необходимости полного обновления:
   itemSupervisor->updateAll();
   mousePress(0, 0);
}

//удаление выделенных элементов в кадре (сигнал формируется в TFrameViewDev)
void TVisionDev::deletingItem()
{
   //передача сообщения инспектору элементов о необходимости полного обновления:
   itemSupervisor->updateAll();
}

//вызов окна "О программе...":
void TVisionDev::helpAbout()
{
   QMessageBox::information(NULL, "About VISION", "MOD_ID = " + QString(MOD_ID) + 
                                              "\nMOD_NAME = " + QString(MOD_NAME) +
					      "\nMOD_TYPE = " + QString(MOD_TYPE) +
					      "\nSUB_TYPE = " + QString(SUB_TYPE) + 
					      "\nVERSION = " + QString(VERSION) +
					      "\nAUTHORS = " + QString(AUTORS) + 
					      "\nDESCRIPTION = " + QString(DESCRIPTION) +
					      "\nLICENSE = " + QString(LICENSE));
}

void TVisionDev::mouseMove(int x, int y)
{
   lbSBMessage2->setText(QString::number(x));
   lbSBMessage3->setText(QString::number(y));
}

void TVisionDev::mousePress(int x, int y)
{
   itemList items;
   QString X, Y, W, H;
   if (((TFrameDev *)workspace->activeWindow())->frameView()->getCountSelected(&items) == 1)
      {
       X = (*items.begin())->getPropValue("x");
       Y = (*items.begin())->getPropValue("y");
       W = (*items.begin())->getPropValue("width");
       H = (*items.begin())->getPropValue("height");
      }
   lbSBPosition->setText("X: " + X + "; Y: " + Y);
   lbSBSize->setText("W: " + W + "; H: " + H);
}

void TVisionDev::addItemButtonClicked(bool inserting)
{
   if (workspace->activeWindow() == NULL)
       //если нет открытых кадров:
      {
       //проход по всем группам кнопок QToolBox в поиске вдавленной; 
      // если такая находится, то сброс кнопки и выход из цикла и функции:
      QButton *bt;
      for (listQVButtonGroup::iterator i = listBtGroup.begin(); i != listBtGroup.end(); i++)
         if ((*i)->selectedId() != -1)
            {
	     ((QPushButton *)((*i)->selected()))->setOn(false);
	     break;
	    }
       return;
      }
      
   if (inserting)
      ((TFrameDev *)workspace->activeWindow())->frameView()->setInsertingNewItem(true);
}

//QListView:
void TVisionDev::lvDoubleClicked(QListViewItem *item, const QPoint &p, int column)
{
   bool result = false;
   if (item != NULL)
      {
       if (item->parent() == lviFrames)
          {//QMessageBox::information( NULL, "QListView", item->text(column));
	   /*QWidgetList openFrames = workspace->windowList();
	   for (QWidgetList::iterator i = openFrames.begin(); i != openFrames.end(); i++)
	     ??? if ( ((TFrameDev*)(*i))->getName() == item->text(column))
	         {
                   (*i)->showNormal();
                   (*i)->setFocus();
		   result = true;
		  }
           if (!result)*/
	      newFrameOpen = true;
	      openFrame(item->text(column));
	      newFrameOpen = false;
	  }
      }
}

void TVisionDev::lvCurrentChanged(QListViewItem *item)
{
   lvDoubleClicked(item, QPoint(), 0);
}

void TVisionDev::lvItemRenamed (QListViewItem *item, int col, const QString &text)
{
    //QMessageBox::information( NULL, "QListView", ((TPrjListViewItem *)item)->getTextBeforeRenaming() + "->" + text);
   //if (addingAfterRename)
      //cfg->addFrameToProject(((TPrjListViewItem *)item)->getTextBeforeRenaming());
   bool result = cfg->renameFrameInProject(((TPrjListViewItem *)item)->getTextBeforeRenaming(), text);
   if (result && (text.left(1) != reservedChar))
      {
       workspace->activeWindow()->setCaption(text);//QMessageBox::information( NULL, "QListView", "Ok");
       addingAfterRename = false;
      }
      else 
         {
	  QMessageBox::critical( NULL, "Renaming error", "Can't rename frame '" + 
	                         ((TPrjListViewItem *)item)->getTextBeforeRenaming() + "' to '" +  text + "'!");
	  item->setText(col, ((TPrjListViewItem *)item)->getTextBeforeRenaming());
	  item->startRename(0);
	 }
}



//-----------------------------------------TPrjListViewItem------------------------------------
TPrjListViewItem::TPrjListViewItem(QListView * parent)
   : QListViewItem(parent)
{
}

TPrjListViewItem::TPrjListViewItem(TPrjListViewItem * parent)
   : QListViewItem(parent)
{
}

 void TPrjListViewItem::startRename(int col)
 {
    textBeforeRenaming = this->text(col);
    QListViewItem::startRename(col);
 }
 
void TPrjListViewItem::cancelRename(int col)
{
   //QMessageBox::information( NULL, "ListViewItem", "cancelRename");
   //startRename(col);
   QListViewItem::cancelRename(col);
}
 
QString TPrjListViewItem::getTextBeforeRenaming() const
{
   return QString(textBeforeRenaming);
}


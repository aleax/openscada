// file "tframeview.h"

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
 
#ifndef TFRAMEVIEW
#define TFRAMEVIEW

#include <qcanvas.h> 
#include <list.h> 
#include <vector.h> 
#include <qxml.h> 
#include <qtimer.h>

#include "titem.h"


namespace VISION
{

typedef list<TItem *> itemList;
typedef vector<TItem *> itemVector;

//Класс "Представление кадра"
class TFrameView : public QCanvasView
{ 
  Q_OBJECT 
  
public: 
  TFrameView(QCanvas*, TVISIONMode, QWidget* parent=0, const char* name=0, WFlags f=0);
  
  TItem* addItem(int itemRtti, int x, int y, bool toSelect = true);
  void setInsertingNewItem(bool inserting); //режим добавления нового элемента
  
  void clear();

  bool delSelectedItems();
  bool delItem(TItem *);
  void selectAll(); //выделить все элементы
  
  void moveBySelected(const int dx, const int dy);
  
  int getCountSelected(itemList * = NULL); //число выделенных элементов, 
                                           // выделенные элементы помещаются в список itemlist
  int getItems(itemList * = NULL); //число элементов, элементы помещаются в список itemlist
  
  QString getName() const;
  bool setName(const QString &name);
  //Работа со свойствами кадра:
  TListOfString getPropCategories(); //список названий категорий свойств для кадра;
  TListOfString getPropNames(const QString *category); //список названий свойств в категории category для кадра;
  TPropType getPropType(const QString *propertyName); //тип свойства propertyName;
  TListOfString getPropEnumTypeValues(const QString *propertyName); //список возможных значений 
                                                                    //для свойства перечислимого типа;
  QString getPropValue(const QString *propertyName); //значение свойства;
  bool setPropValue(const QString *propertyName, const QString *newValue); //изменение значения свойства;
     
  void startDynamic(); //запуск динамики (таймера для обновления (динамизации) элементов)
  void stopDynamic(); //останов динамики (таймера для обновления (динамизации) элементов)
signals:
  void addItem(int x, int y);
  void selectingItem();
  void movingItem();
  void resizingItem();
  
  void mouseMove(int x, int y);
  void mousePress(int x, int y);
  
protected:
  void contentsMousePressEvent(QMouseEvent*);
  void contentsMouseMoveEvent(QMouseEvent*);
  void contentsMouseReleaseEvent(QMouseEvent*);
  itemList collisions (const int x, const int y) const; //возвращает список элементов, 
                                                        // сталкивающихся с точкой (x, y). 
							// Список отсортирован по z-координате элемента
  void unselectAll(); //снять выделение всех элементов
  bool contains(itemList *, TItem *); //содержит ли список itemList элемент TItem?

private slots:
  void timerDynamic(); //обработка событий таймера обновления динамики

private:
  friend class TSaxHandler;
  QTimer timer;
  TVISIONMode mode;
  
  double z; //текущая максимальная координата z
  
  QString name;
  int updateCycle; //период обновления (в мс)
  double rotate;
  double scaleX;
  double scaleY;
   
  QCanvasItem* itemToMove;
  QPoint movingStart;
  itemList itemsList;
  
  //обработка событий мыши:
  bool moving; //в данный момент производится перемещение элемента (ов);
  bool resizing; //в данный момент производится изменение размера элемента;
  QPoint pointStart; //начальная точка указателя мыши при перетаскивании/изменении размера;
  char resizeItem; //направление изменения размеров элемента;
  bool insertingNewItem;
};

//------------------------Работа с файлом-----------------------------------
class TSaxHandler : public QXmlDefaultHandler 
{ 
public: 
  TSaxHandler(TFrameView *frame); 
  
  bool startElement(const QString &namespaceURI, 
                    const QString &localName, 
                    const QString &qName, 
                    const QXmlAttributes &attribs); 
  bool endElement(const QString &namespaceURI, 
                  const QString &localName, 
                  const QString &qName); 
  bool characters(const QString &str); 
  bool fatalError(const QXmlParseException &exception); 
  
private:
  QString currentProp;
  TItem *currentItem;
  TFrameView *frame;
  bool valueWas;
  bool dynamicWas;
  bool addingItems;
  bool addingFrame;
};

}
#endif

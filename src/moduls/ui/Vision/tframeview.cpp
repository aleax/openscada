// file "tframeview.cpp"

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

#include <qwmatrix.h>
#include <qmessagebox.h>
#include <qcursor.h>

#include "tframeview.h"
#include "tvisionerror.h"


using namespace VISION;

TFrameView::TFrameView(QCanvas *c, TVISIONMode m, QWidget* parent, const char* name, WFlags f)
    : QCanvasView(c,parent,name,f),
    mode(m),
    z(0.0),
    scaleX(1.0),
    scaleY(1.0),
    rotate(0.0),
    updateCycle(1000),
    timer()
{
    viewport()->setMouseTracking(true);
    moving = resizing =insertingNewItem = false;

    connect(&timer,  SIGNAL(timeout()), this, SLOT(timerDynamic()));
    
    //обновление информации сразу после загрузки кадра:
    timerDynamic();
}

TItem* TFrameView::addItem(int itemRtti, int x, int y, bool toSelect)
{
   /*QString s;
   s = "addItem from TFrameDev: " + QString::number(x);
   s += ", " + QString::number(y);
   s += ", rtti = " + QString::number(itemRtti);
   QMessageBox::information( NULL, "info", s); 
   */
   TItem *i = NULL;
   switch (itemRtti)
   {
      case 10001 : {
                        i = new TCRectangle(canvas(), mode, z, x, y);
			i->setSelectedItem(toSelect);
                        i->showItem();
			itemsList.push_back(i);
			z += 1.0;
                        break;
			 }
      case 10002 : {
                        i = new TCLine(canvas(), mode, z, x, y);
			i->setSelectedItem(toSelect);
			i->showItem();
			itemsList.push_back(i);
			z += 1.0;
                        break;
			 }
	case 10003 : {
                        i = new TCText(canvas(), mode, z, x, y, "text");
			i->setSelectedItem(toSelect);
			i->showItem();
			itemsList.push_back(i);
			z += 1.0;
                        break;
			 }
	case 10004 : {
                        i = new TCImage(canvas(), mode, z, x, y);
			i->setSelectedItem(toSelect);
			i->showItem();
			itemsList.push_back(i);
			z += 1.0;
                        break;
			 }
      
      default : break;
   }
   canvas()->update();
   return i;
}

//режим добавления нового элемента:
void TFrameView::setInsertingNewItem(bool inserting)
{//QMessageBox::information( NULL, "info", "режим добавления нового элемента" );
   insertingNewItem = inserting;
   if (inserting)
      viewport()->setCursor(Qt::CrossCursor); // "+"
      else viewport()->setCursor(Qt::ArrowCursor); 
}

bool TFrameView::delSelectedItems()
{
   itemList items;
   getCountSelected(&items);
   for (itemList::iterator i = items.begin();i != items.end(); i++)
       delItem(*i);
}

bool TFrameView::delItem(TItem *item)
{
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
      if (*i == item)
         {
          itemsList.erase(i);
	  delete (*i);
	  return true;
	 }
   return false;
}

void TFrameView::moveBySelected(const int dx, const int dy)
{
    itemList selectedItems;
    getCountSelected(&selectedItems); 
    for (itemList::iterator i = selectedItems.begin();i != selectedItems.end(); i++)
       (*i)->moveByItem(dx, dy);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
void TFrameView::clear()
{
   itemsList.clear();
}

 //число выделенных элементов, выделенные элементы помещаются в список itemlist:
int TFrameView::getCountSelected(itemList *selectedItems)
{
   unsigned int count = 0;
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
      if ((*i)->isSelectedItem())
         {
          ++count;
	  if (selectedItems != NULL)
	     selectedItems->push_back(*i);
	 }
   return count;
}

 //число элементов, элементы помещаются в список itemlist:
int TFrameView::getItems(itemList *items)
{
   unsigned int count = 0;
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
         {
          ++count;
	  if (items != NULL)
	     items->push_back(*i);
	 }
   return count;
}

//запуск динамики (таймера для обновления (динамизации) элементов)
void TFrameView::startDynamic()
{
   timer.start(updateCycle);
}

//останов динамики (таймера для обновления (динамизации) элементов)
void TFrameView::stopDynamic()
{
   timer.stop();
}

//обработка событий таймера обновления динамики
void TFrameView::timerDynamic()
{
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
      (*i)->doDynamic();
   canvas()->update();
}

//-----------------------------------работа со свойствами кадра---------------------------------------
QString TFrameView::getName() const
{
   return QString(name);
}

bool TFrameView::setName(const QString &name)
{
   bool result = false;
   if (setPropValue("FrameName", name))
      {
       this->name = name;
       result = true;
      }
   return result;
}

TListOfString TFrameView::getPropCategories()
{
   TListOfString result;
   result.push_back(("Frame"));
   result.push_back(("Geometry"));
   result.push_back(("Colors"));

   return result;
}

TListOfString TFrameView::getPropNames(const QString *category)
{
   TListOfString result;
   if (category == NULL)
       return result;
   
   if (*category == QString("Frame"))
      {
       result.push_back(("FrameName"));
       result.push_back(("updateCycle"));
      }
      
   if (!strcmp(category->ascii(), ("Geometry")))
      {
       result.push_back(("width"));
       result.push_back(("height"));
       result.push_back(("rotate"));
       result.push_back(("scaleX"));
       result.push_back(("scaleY"));
      }

   if (!strcmp(category->ascii(), ("Colors")))
      {
       result.push_back(("BackgroundColor"));
      }
   
   return result;
}

TPropType TFrameView::getPropType(const QString *propertyName)
{
   if (*propertyName == QString("FrameName"))
      return stringType;
   if (*propertyName == QString("updateCycle"))
      return intType;
   if (*propertyName == QString("width"))
      return intType;
   if (*propertyName == QString("height"))
      return intType;
   if (*propertyName == QString("rotate"))
      return floatType;
   if (*propertyName == QString("scaleX"))
      return floatType;
   if (*propertyName == QString("scaleY"))
      return floatType;
   if (*propertyName == QString("BackgroundColor"))
      return colorType;
   
   return unknownType;
}

TListOfString TFrameView::getPropEnumTypeValues(const QString *propertyName)
{
   TListOfString result;
   return result;
}

QString TFrameView::getPropValue(const QString &propertyName)
{
   QString result;
   if (*propertyName == QString("FrameName"))
      return name;
   if (*propertyName == QString("updateCycle"))
      return QString::number(updateCycle);
   if (*propertyName == QString("width"))
      return QString::number(canvas()->width());
   if (*propertyName == QString("height"))
      return QString::number(canvas()->height());
   if (*propertyName == QString("rotate"))
      return QString::number(rotate, 'f', 2);
   if (*propertyName == QString("scaleX"))
      return QString::number(scaleX, 'f', 2);
   if (*propertyName == QString("scaleY"))
      return QString::number(scaleY, 'f', 2);
   if (*propertyName == QString("BackgroundColor"))
      return QString::number(canvas()->backgroundColor().rgb(), 10);
   return result;
}

bool TFrameView::setPropValue(const QString &propertyName, const QString &newValue)
{
   bool result = false;
   
   if (propertyName == "FrameName")
      {
       name = *newValue;
       return true;
      }
   if (propertyName == "width")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result)
          {
           QCanvas *c = canvas();
	   c->resize(newVal, canvas()->height());
	   setCanvas(c);
	   //setMinimumSize(QSize(newVal+10, canvas.height()+10));
	   //setMaximumSize(QSize(newVal+10, canvas.height()+10));
	  }
       return result;
      }
   if (propertyName == "updateCycle")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result)
          updateCycle = newVal;
       return result;
      }
   if (propertyName == "height")
      {
       int newVal = newValue.toInt(&result, 10);
       if (result)
          {
           QCanvas *c = canvas();
	   c->resize(canvas()->width(), newVal);
	   setCanvas(c);
	   //setMinimumSize(QSize(canvas.width()+10, newVal+10));
	   //setMaximumSize(QSize(canvas.width()+10, newVal+10));
	  }
       return result;
      }
   if (propertyName == "rotate")
      {
       int newVal = newValue.toInt(&result);
       if (result)
          rotate = newVal;//setSize(width(), newVal);
       return result;
      }
   if (propertyName == "scaleX")
      {
       int newVal = newValue.toInt(&result);
       if (result)
          scaleX = newVal;//setSize(width(), newVal);
       return result;
      }
   if (propertyName == "scaleY")
      {
       int newVal = newValue.toInt(&result);
       if (result)
          scaleY = newVal;//setSize(width(), newVal);
       return result;
      }
   if (propertyName == "BackgroundColor")
      {
       int newVal = newValue.toULong(&result);
       if (result)
          {
	   QCanvas *c = canvas();
           c->setBackgroundColor(QColor(QRgb(newVal)));
	   setCanvas(c);
	  }
       return result;
      }
}

//-----------------------------------------------------------------------------------------------------------
//возвращает список элементов, сталкивающихся с точкой (x, y). Список отсортирован по z-координате элемента:
itemList TFrameView::collisions (const int x, const int y) const
{
   itemVector collideItems;
   int cursor;
   char resizeItem;
   for (itemList::const_iterator i = itemsList.begin(); i !=itemsList.end(); i++)
      {
       //точка (x, y) покрывает элемент?:
       cursor = Qt::ArrowCursor;
       bool yes = (*i)->getCursorShape(x, y, resizeItem, cursor, 1);
       if (yes || (cursor == Qt::SizeAllCursor)) //yes==true - курсор над границей элемента
	   collideItems.push_back(*i); //если да, то добавляем элемент в список
      }
   //Сортировка элементов списка по убыванию координаты z элемента:
   if (collideItems.size() > 1)      
   {
    bool changes;
    do
    {
     changes = false;
     for (int i = 0; i < collideItems.size() -1; i++)
       {
	if (collideItems[i]->zItem() < collideItems[i+1]->zItem())
	   {
	    TItem *item = collideItems[i];
	    collideItems[i] = collideItems[i+1];
	    collideItems[i+1] = item;
	    changes = true;
	   }
       }
    }
    while (changes);
   }//if
   
   itemList collideItemsList;
   for (int i = 0; i < collideItems.size(); i++)
      collideItemsList.push_back(collideItems[i]);
   
   return collideItemsList;
}

void TFrameView::contentsMousePressEvent(QMouseEvent* e)
{
    emit mousePress(e->x(), e->y());
 if (mode == Development)
  {
   if (insertingNewItem)
      {
       insertingNewItem = false;
       QPoint p = inverseWorldMatrix().map(e->pos());//!!!
       emit addItem(p.x(), p.y());//!!!
       //QMessageBox::information( NULL, "info", "press" );
       unselectAll();
       viewport()->setCursor(Qt::SizeAllCursor); //"x"
       emit selectingItem();
      }
    QCursor currCursor = viewport()->cursor();
    // если курсор Arrow (указатель находится над пустым местом):
    if (currCursor.shape() == Qt::ArrowCursor)
       {
        unselectAll();
	emit selectingItem();
	emit mousePress(e->x(), e->y());
	return;
       }
    // если курсор SizeAll (указатель находится над элементом):
    if (currCursor.shape() == SizeAllCursor)
       {
       //1) если указатель находится над невыбранным элементом,
       //   то отмена выделения всех элементов, выделение данного элемента:
        TItem *itemUnderCursor = NULL; //элемент под указателем
        QPoint p = inverseWorldMatrix().map(e->pos());
        itemList l=collisions(p.x(), p.y());
          itemUnderCursor = *(l.begin());
	if (l.size() >= 1)//itemUnderCursor != NULL)
	   {
	    itemList selectedItems;
	    getCountSelected(&selectedItems);
	    if (!contains(&selectedItems, itemUnderCursor))
	       if (e->state() != Qt::ShiftButton)
	          unselectAll();
	    if (e->state() == Qt::ShiftButton)
	       itemUnderCursor->setSelectedItem(!itemUnderCursor->isSelectedItem());
	       else itemUnderCursor->setSelectedItem(true);
	   }
	//2) 
	moving = true;
	pointStart.setX(e->x());
	pointStart.setY(e->y());
	emit selectingItem();
	emit mousePress(e->x(), e->y());
	return;
       }
       
       // иначе (указатель находится над границей элемента - изменение размеров элемента):
    if (resizeItem != (char)resizeNothing)
       {
	resizing = true;
	pointStart.setX(e->x());
	pointStart.setY(e->y());
       }
  }//mode == Development
}

void TFrameView::contentsMouseMoveEvent(QMouseEvent* e)
{
  emit mouseMove(e->x(), e->y());
  if (mode == Development)
   {   
    if (insertingNewItem)
       return;
    
    const int precision = 4; //число пикселей, определяющих ширину окна при изменении курсора;
     
    if ( (!moving) && (!resizing) )
      {
       itemList selectedItem;
       if (getCountSelected(&selectedItem) == 1)  
          //если выбран один элемент:
          {//QMessageBox::information( NULL, "info", "выбран 1 элемент" ); //
	   //если перемещаюсь в пределах границы этого элемента, то форма курсора изменяется на соответствующую
	   // самим элементом:
	   TItem *item = *(selectedItem.begin());
	   int cursor = QCursor(viewport()->cursor()).shape(); //возвращает true, если курсор над границей элемента
	   bool toReturn = item->getCursorShape(e->x(), e->y(), resizeItem, cursor);
	   viewport()->setCursor(cursor);
	   if (toReturn)
	      return;
	   } //если выбран один элемент
	   	
	//если перемещаюсь над элементом:
	QPoint p = inverseWorldMatrix().map(e->pos());
        itemList l=collisions(p.x(), p.y());
	if (l.size())
	   {
	    viewport()->setCursor(Qt::SizeAllCursor); //"x"
	    return;
	   }
	   
	//иначе - курсор поумолчанию:	   
       viewport()->setCursor(Qt::ArrowCursor);
      } //if (!moving && !resizing)
      
   if (moving) 
      {//QMessageBox::information( NULL, "info", "moving" ); //
       QPoint p = inverseWorldMatrix().map(e->pos());
       itemList selectedItems;
       getCountSelected(&selectedItems);
       //QMessageBox::information( NULL, "info", s ); //
       for (itemList::iterator i = selectedItems.begin(); i != selectedItems.end(); i++)
	   (*i)->moveOnMouse(p.x(), p.y(), pointStart.x(), pointStart.y());
       pointStart = p;
       canvas()->update();
       emit movingItem();
      } //moving
   
   if (resizing)
      {
       itemList selectedItems;
       getCountSelected(&selectedItems);
       QPoint p = inverseWorldMatrix().map(e->pos());
       //элемент изменяет свои размеры и положение в зависимости от дельта перемещения и курсора мыши;
       // если изменение невозможно (достигнуты минимальные границы), возвращает false:
       bool mouseRelease = !((*selectedItems.begin())->resizeOnMouse(p.x(), p.y(), 
                                                          pointStart.x(), pointStart.y(), resizeItem));
       pointStart = p;
       canvas()->update();
       
       emit resizingItem();
      } //resizing
   }//mode == Development   
}

void TFrameView::contentsMouseReleaseEvent(QMouseEvent* e)
{
 if (mode == Development)
  {
   moving = resizing = false;

   const int precision = 4; //число пикселей, определяющих ширину окна при изменении курсора;
    
   //если выбран один элемент, то отслеживаем положение указателя мыши относительно элемента 
   // и устанавливаем соотв-й курсор;
   // если выбрано несколько элементов, то курсор SizeAllCursor;
   // иначе обычный курсор мыши:
   itemList selectedItem;
   if (getCountSelected(&selectedItem) == 1)  
          //если выбран один элемент:
          {//QMessageBox::information( NULL, "info", "выбран 1 элемент" ); //
	   //если перемещаюсь в пределах границы этого элемента, то форма курсора изменяется на соответствующую
	   // самим элементом:
	   TItem *item = *(selectedItem.begin());
	   int cursor = QCursor(viewport()->cursor()).shape(); //возвращает true, если курсор над границей элемента
	   bool toReturn = item->getCursorShape(e->x(), e->y(), resizeItem, cursor);
	   viewport()->setCursor(cursor);
	   if (toReturn)
	      return;
	   } //если выбран один элемент
	
	//если перемещаюсь над элементом:
	QPoint p = inverseWorldMatrix().map(e->pos());
        itemList l=collisions(p.x(), p.y());
	if (l.size())
	   {
	    viewport()->setCursor(Qt::SizeAllCursor); //"x"
	    return;
	   }
	   
	//иначе - курсор поумолчанию:	   
       viewport()->setCursor(Qt::ArrowCursor);
   }//mode == Development
}
//---------------------------------------------------------------------------------------------------------------

//выделить все элементы
void TFrameView::selectAll()
{
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
      (*i)->setSelectedItem(true);
   
   emit selectingItem();
}

//снять выделение всех элементов
void TFrameView::unselectAll()
{
   for (itemList::iterator i = itemsList.begin(); i != itemsList.end(); i++)
      (*i)->setSelectedItem(false);
}

 //содержит ли список itemList элемент TItem?
bool TFrameView::contains(itemList *items, TItem *item)
{
   bool result = false;
   for (itemList::iterator i = items->begin(); i != items->end(); i++)
       if ((*i) == item)
          {
	   result = true;
           break;
	  }
    return result;
}


//--------------------------------------------TSaxHandler------------------------------------------------------

TSaxHandler::TSaxHandler(TFrameView *frame)
{
   this->frame = frame;
   currentItem = NULL;
   valueWas = false;
   dynamicWas = false;
   addingItems = false;
   addingFrame = false;
}

bool TSaxHandler::startElement(const QString &, const QString &, 
                                                 const QString &qName, const QXmlAttributes &attribs) 
{ 
  bool result= false;
  if (qName == "items")
     {
      addingItems = true;
      result = true;
     }
  if (qName == "frame")
     {
      addingFrame = true;
      result = true;
     }
  if ((qName == "item") && addingItems)
    {
     result = false;
     //QMessageBox::information( NULL, "sax", "adding tcrectangle"); result = true;currentItem = (TItem*)1;
     currentItem = frame->addItem(attribs.value("class").toInt(), 0, 0, false);
     if (frame->z < currentItem->zItem())
	frame->z = currentItem->zItem();
     if (currentItem == NULL)
	{
	 QMessageBox::critical( NULL, "Opening error", 
	                       "Can't find item with class = '" + attribs.value("class") + "'!");
	 return false;
	}
	else result = true;
    }
   
  if ((qName == "property") && (addingItems || addingFrame))
    {
     currentProp = attribs.value("name"); 
     //QMessageBox::information( NULL, "sax: startElement", "currentProp = " + currentProp);
     result = true;
    }
  
  if ((qName == "value") && (addingItems || addingFrame))
    {
     valueWas = true; //устанавливаем признак начала тега value
     result = true;
    }
  
  if ((qName == "dynamic") && (addingItems /*|| addingFrame*/))
    {
     dynamicWas = true; //устанавливаем признак начала тега value
     result = true;
    }
  
  return result; 
}

bool TSaxHandler::endElement(const QString &, const QString &, const QString &qName) 
{  
  if (qName == "property")
     currentProp = "";
  if (qName == "items")
     addingItems = false;
  if (qName == "frame")
     addingFrame = false;
  
  return true; 
}

bool TSaxHandler::characters(const QString &str) 
{ 
  bool result;
  if ( (currentItem !=NULL) && valueWas && addingItems)
     {
      result = currentItem->setPropValue(currentProp, str);
      if (!result)
         QMessageBox::critical( NULL, "Opening error", 
	                        "Can't change value for property '" + currentProp + "' to '" +  str + "'!");
      valueWas = false; //сбрасываем признак начала тега value
      //QMessageBox::information( NULL, "sax: charactrers. SetPropValue", 
      // "currentProp = " + currentProp + "; value = " + str);
     }
  
  if ( (currentItem !=NULL) && dynamicWas && addingItems)
     {
      try
      {
       result = currentItem->setDynamic(currentProp, str);
       if (!result)
          QMessageBox::critical( NULL, "Opening error", 
	                               "Can't set dynamic " + str + " for property '" + currentProp + "'!");
       dynamicWas = false; //сбрасываем признак начала тега value
       //QMessageBox::information( NULL, "sax: charactrers. SetPropValue", 
       // "currentProp = " + currentProp + "; value = " + str);
      }
      catch (TVisionError e)
      {QMessageBox::warning( NULL, (e.getClass()), (e.getMess()));
       dynamicWas = false;}
     }
  
     
  if (valueWas && addingFrame)
     {
      result = frame->setPropValue(currentProp, str);
      if (!result)
         QMessageBox::critical( NULL, "Opening error", 
	                              "Can't change value for property '" + currentProp + "' to '" +  str + "'!");
      valueWas = false; //сбрасываем признак начала тега value
      //QMessageBox::information( NULL, "sax: charactrers. SetPropValue", 
      // "currentProp = " + currentProp + "; value = " + str);
     }
        
  return true;
}

bool TSaxHandler::fatalError(const QXmlParseException &exception) 
{
   QMessageBox::critical( NULL, "Opening error", 
	                         "Error '" + QString(exception.message()) + "' in line #" +
				 QString::number(exception.lineNumber()) + ", coloumn #" + 
				 QString::number(exception.columnNumber()) + "'!");
   
   currentItem = NULL;
   return false; 
}

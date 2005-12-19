// file "titemsupervisor.cpp"

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

#include <qtable.h>
#include <qscrollbar.h> 
#include <qtabwidget.h> 
#include <qlineedit.h> 
#include <qhbox.h> 
#include <qlabel.h> 
#include <qframe.h> 
#include <qpixmap.h> 
#include <qpushbutton.h> 
#include <qlayout.h> 
#include <qmessagebox.h> 
#include <qcolordialog.h> 
#include <qstringlist.h> 

#include "titemsupervisor.h"
#include "tframeview.h" //itemList
#include "tdynamicpropdialog.h"

using namespace VISION;

TItemSupervisor::TItemSupervisor(QWidget *parent, const char *name, int wflags) 
    : QScrollView(parent, name, wflags) 
{   
   table = new QTable(1, 3, this, "table");
   table->setSelectionMode(QTable::NoSelection);
   table->verticalHeader()->hide();
   table->setLeftMargin(0);
   table->setColumnWidth(0, 80);
   table->setColumnWidth(1, 80);
   table->setColumnWidth(2, 50);
   QStringList l;
   l.push_back("Name");
   l.push_back("Value");
   l.push_back("Dynamic");
   table->setColumnLabels(l);
   //connect(table, SIGNAL(currentChanged(int, int)), this, SLOT(whenSelectionChanged()));
   connect(table, SIGNAL(valueChanged(int, int)), this, SLOT(valueChanged(int, int)));
   
   addChild(table);
   setHScrollBarMode(QScrollView::Auto);
   setVScrollBarMode(QScrollView::Auto);
   /*tw->addTab(table, tr("Properties"));
   tw->addTab(NULL, tr("Reactions"));
   tw->show();*/
}

void TItemSupervisor::setFrame(TFrameDev *fr)
{
   frame = fr;
}

void TItemSupervisor::showColorDialog()
{
   if (frame == 0)
      {
       clearSupervisor();
       return;
      }
      
   QColor currentColor;
   bool result = false;
   itemList items;
   frame->frameView()->getCountSelected(&items);
   
   //---Работа с кадром:---
   if (items.size() == 0)
      {
       //QMessageBox::information( NULL, "info", "propName = " + 
       // ((table->text(table->currentRow(), table->currentColumn()-1)).stripWhiteSpace()) ); //  
       currentColor = QRgb(frame->frameView()->getPropValue((table->text(table->currentRow(),table->currentColumn()-1)).stripWhiteSpace()).toULong());
       QColor newColor = QColorDialog::getColor(currentColor, this, "color dialog" );
       if (newColor.isValid())
         {
          result = frame->frameView()->setPropValue(table->text(table->currentRow(), table->currentColumn()-1).stripWhiteSpace(),
	                                            QString::number(newColor.rgb(), 10));
          if (!result)
          QMessageBox::critical( NULL, "Changing error", 
                         	       "Can't change value for property '" + table->text(table->currentRow(),
		                       table->currentColumn()-1).stripWhiteSpace() + "' to '" +
                                       QString::number(newColor.rgb(), 10) + "'!");
	 }
	updateAll();
	return;
      }
   //---
   
   for (itemList::iterator i = items.begin(); i != items.end(); i++)
      {//QMessageBox::information( NULL, "info", "propName = " + ((table->text(table->currentRow(),
       // table->currentColumn()-1)).stripWhiteSpace()) ); //  
       if (i == items.begin())
          currentColor = QRgb((*i)->getPropValue((table->text(table->currentRow(),
	                      table->currentColumn()-1)).stripWhiteSpace()).toULong());
	  else if (currentColor != QRgb((*i)->getPropValue((table->text(table->currentRow(),
	                            table->currentColumn()-1)).stripWhiteSpace()).toULong()))
	     {
	      currentColor = QColor();
	      break;
	     }
      }
   
   QColor newColor = QColorDialog::getColor(currentColor, this, "color dialog" );
   if (newColor.isValid())
       for (itemList::iterator i = items.begin(); i != items.end(); i++)
         {//QMessageBox::information( NULL, "info", QString::number(newColor.rgb(), 10) ); //  
	  result = (*i)->setPropValue(table->text(table->currentRow(), table->currentColumn()-1).stripWhiteSpace(),
	                                           QString::number(newColor.rgb(), 10));
          if (!result)
          QMessageBox::critical( NULL, "Changing error", 
	                               "Can't change value for property '" + table->text(table->currentRow(),
		                        table->currentColumn()-1).stripWhiteSpace() + "' to '" +
					QString::number(newColor.rgb(), 10) + "'!");
	 }
      
   updateAll();
}

void TItemSupervisor::showDynamicDialog()
{
   if (frame == 0)
      {
       clearSupervisor();
       return;
      }

   itemList items;
   frame->frameView()->getCountSelected(&items);
   
   QString currentProp, currentProp2;
   QString propName = (table->text(table->currentRow(), table->currentColumn()-2)).stripWhiteSpace();
   for (itemList::iterator i = items.begin(); i != items.end(); i++)
      if (i==items.begin())
         (*i)->getDynamic(propName, currentProp);
	 else 
	    {
	     (*i)->getDynamic(propName, currentProp2);
	     if (currentProp != currentProp2)
	        {
	         currentProp = "";//table->setText( n, 2, (table, QTableItem::Always, value));
		 break;
		}
            }
	   
   TDynamicPropDialog::getDynamic(this, *(items.begin()), propName, currentProp);
}

void TItemSupervisor::valueChanged (int row, int col)
{//QMessageBox::information( NULL, "info", "valueChanged");
   if (frame == 0)
      {
       clearSupervisor();
       return;
      }
      
   itemList items;
   frame->frameView()->getCountSelected(&items);
   bool result;
   for (itemList::iterator i = items.begin(); i != items.end(); i++)
      {
       result = (*i)->setPropValue(table->text(row, col-1).stripWhiteSpace(), 
                                     table->text(row, col).stripWhiteSpace());
       if (!result)
          QMessageBox::critical( NULL, "Changing error", 
	                               "Can't change value for property '" + 
				       table->text(row, col-1).stripWhiteSpace() + "' to '" +  
				       table->text(row, col).stripWhiteSpace() + "'!");
      }
   
   //---Работа с кадром:---
   if (items.size() == 0)
      {
       result = frame->frameView()->setPropValue(table->text(row, col-1).stripWhiteSpace(), 
                                                 table->text(row, col).stripWhiteSpace());
       if (!result)
          QMessageBox::critical( NULL, "Changing error", 
	                               "Can't change value for property '" + 
				       table->text(row, col-1).stripWhiteSpace() + "' to '" +  
				       table->text(row, col).stripWhiteSpace() + "'!");
      }
   //---
      
   updateAll();
   table->setCurrentCell(row, col);
}

void TItemSupervisor::clearSupervisor()
{
   table->setNumRows(0);
}

//обновить полностью:
void TItemSupervisor::updateAll(const QString *category, const QString *propName, const bool clear)
{
   if (frame == 0)
      {
       clearSupervisor();
       return;
      }
      
   if (clear)
      clearSupervisor();
   
   itemList items;
   int count = frame->frameView()->getCountSelected(&items);
   if (count == 0)
      {
       clearSupervisor();
       if (frame == NULL)
          return;
       //---Работа с кадром---
       //QMessageBox::information( NULL, "superVisor", "frame");
       int n = 0;
       TListOfString categories = frame->frameView()->getPropCategories();
       table->setNumRows(categories.size()+1); //число строк на 1 больше 
                                               //(для добавления строк со свойствами для последней категории)
       //проход по именам категорий:
       for (TListOfString::iterator i = categories.begin(); i != categories.end(); i++)
          {
	   table->setItem( n, 0, new QTableItem(table, QTableItem::Never, (*i)));
           n++;
           //проход по именам свойств по данной категории:
	   QString value;
           TListOfString names = frame->frameView()->getPropNames(&(*i));
	   for (TListOfString::iterator j = names.begin(); j != names.end(); j++)
	      {
	      //вставка названия свойства:
	      table->insertRows(n+1);
	      table->setItem( n, 0, new QTableItem(table, QTableItem::Never, "   " + (*j)));
	      //тип свойства и вставка значения:
	      if (frame->frameView()->getPropType(&(*j)) == enumType)
	         {
	         //перечислимый тип:
	          QStringList list; //содержимое comboBox;
	          TListOfString listEnumValues = frame->frameView()->getPropEnumTypeValues(&(*j));
	          for (TListOfString::iterator l = listEnumValues.begin(); l != listEnumValues.end(); l++)
	              list.push_back(*l);
	          QComboTableItem *tableItem = new QComboTableItem(table, list);
	          table->setItem( n, 1, tableItem);
                  tableItem->setCurrentItem(frame->frameView()->getPropValue(*j));
	         }
	         else
	            if (frame->frameView()->getPropType(&(*j)) == colorType)
	               {
		        //тип "цвет":
		  
		        QHBox *hbox = new QHBox(this);//QHBox
		        //QHBoxLayout *hBoxLayout  = new QHBoxLayout(hbox);
		        QFrame *fr = new QFrame(hbox);
		        fr->setFrameShape(QFrame::Box);
		        unsigned long color;
		        QString colorName;
		        color = frame->frameView()->getPropValue(*j).toULong();
		        colorName = QColor(QRgb(color)).name();
		        QLabel *lb = new QLabel(colorName, hbox);
		        QPushButton *bt = new QPushButton("...", hbox);
		        fr->setFixedWidth((int)1.5*fr->height());
		        bt->setFixedWidth(bt->height());
		        connect(bt, SIGNAL(clicked()), this, SLOT(showColorDialog()));
		  
		        fr->setPaletteBackgroundColor(QColor(QRgb(color)));
		        fr->setPaletteForegroundColor(QColor(QRgb(0x000000)));
		        table->setCellWidget(n, 1, hbox);
		       }
	               else
	               {
	                //неперечислимый тип и не цвет:
	                value = frame->frameView()->getPropValue(*j);
	                table->setText/*Item*/( n, 1, /*new QTableItem*/(table, QTableItem::Always, value));
	               }
	      n++;
	     }
	  }
       //---
       return;
      }
   if (count == 1)
      items.push_back(*(items.begin()));
   
   itemList::iterator itemIterator = items.begin();
   itemList::iterator itemIterator2 = items.begin();
   itemIterator2++;
   for (int k = 0; k < items.size() - 1; k++)
   {
   
   TListOfString categories = (*itemIterator)->getPropCategories();
   TListOfString categories2 = (*(itemIterator2))->getPropCategories();
   table->setNumRows(categories.size()+1); //число строк на 1 больше 
                                           //(для добавления строк со свойствами для последней категории)
   int n = 0;
   QLineEdit *edit = NULL;
   //проход по именам категорий:
   bool doCategory;
   for (TListOfString::iterator i = categories.begin(); i != categories.end(); i++)
    for (TListOfString::iterator i2 = categories2.begin(); i2 != categories2.end(); i2++)
      {
      doCategory = ( (*i) == (*i2) ) ? true : false ;
      if ( (category != NULL) && doCategory )
         if ( (*category != *i) || (*category != *i2) )
	    doCategory = false;
      if (doCategory)
      {
       table->setItem( n, 0, new QTableItem(table, QTableItem::Never, (*i)));
       n++;
       
       //проход по именам свойств по данной категории:
       TListOfString names = (*itemIterator)->getPropNames(&(*i));
       TListOfString names2 = (*itemIterator2)->getPropNames(&(*i2));
       QString value, value2;
       TPropType propType;
       bool doPropName;
       for (TListOfString::iterator j = names.begin(); j != names.end(); j++)
        for (TListOfString::iterator j2 = names2.begin(); j2 != names2.end(); j2++)
          {
	  doPropName = ( ((*j) == (*j2)) && ((*itemIterator)->getPropType(&(*j)) ==
	                 (*itemIterator2)->getPropType(&(*j2))) ) ? true : false;
	  if ( (propName != NULL) && doPropName )
             if (*propName != *j)
	        doPropName = false;
          if (doPropName)
          {
	   //вставка названия свойства:
	   table->insertRows(n+1);
	   table->setItem( n, 0, new QTableItem(table, QTableItem::Never, "   " + (*j)));
	   /////динамика:
	   value = value2 = "";
	   if ( (*itemIterator)->getDynamic(*j, value) && (*itemIterator2)->getDynamic(*j, value2) )
	      if (value != value2)
	         value = " ";//table->setText( n, 2, (table, QTableItem::Always, value));
	   QHBox *hbox = new QHBox(this);
	   QPushButton *bt = new QPushButton("...", hbox);
	   QLabel *label = new QLabel(value, hbox);
	   bt->setFixedWidth(bt->height());
           connect(bt, SIGNAL(clicked()), this, SLOT(showDynamicDialog()));
	   table->setCellWidget(n, 2, hbox);
	   /////
	   //тип свойства и вставка значения:
	   if ((*itemIterator)->getPropType(&(*j)) == enumType)
	      {
	       //перечислимый тип:
	       QStringList list; //содержимое comboBox;
	       TListOfString listEnumValues = (*itemIterator)->getPropEnumTypeValues(&(*j));
	       TListOfString listEnumValues2 = (*itemIterator2)->getPropEnumTypeValues(&(*j2));
	       if (listEnumValues == listEnumValues2)
	          for (TListOfString::iterator l = listEnumValues.begin(); l != listEnumValues.end(); l++)
	              list.push_back(*l);
	       QComboTableItem *tableItem = new QComboTableItem(table, list);
	       table->setItem( n, 1, tableItem);
	       if ( (*itemIterator)->getPropValue(*j) == (*itemIterator2)->getPropValue(*j2))
	          tableItem->setCurrentItem((*itemIterator)->getPropValue(*j));
		  else tableItem->setCurrentItem(0);
	      }
	      else
	      if ((*itemIterator)->getPropType(&(*j)) == colorType)
	         {
		  //тип "цвет":
		  QHBox *hbox = new QHBox(this);//QHBox
		  //QHBoxLayout *hBoxLayout  = new QHBoxLayout(hbox);
		  QFrame *fr = new QFrame(hbox);
		  fr->setFrameShape(QFrame::Box);
		  unsigned long color;
		  QString colorName;
		  if ((*itemIterator)->getPropValue(*j).toULong() ==
		       ((*itemIterator2)->getPropValue(*j).toULong()))
		     {
		      color = (*itemIterator)->getPropValue(*j).toULong();
		      colorName = QColor(QRgb(color)).name();
		     }
		     else
		        {
		         color = 0xffffff;
			 colorName = " ";
			}
		  QLabel *lb = new QLabel(colorName, hbox);
		  QPushButton *bt = new QPushButton("...", hbox);
		  fr->setFixedWidth((int)1.5*fr->height());
		  bt->setFixedWidth(bt->height());
		  connect(bt, SIGNAL(clicked()), this, SLOT(showColorDialog()));
		  
		  fr->setPaletteBackgroundColor(QColor(QRgb(color)));
		  
		  if ((*itemIterator)->getPropValue(*j).toULong() ==
		       ((*itemIterator2)->getPropValue(*j).toULong()))
		     fr->setPaletteForegroundColor(QColor(QRgb(0x000000)));
		     else 
			 fr->setPaletteForegroundColor(QColor(QRgb(0xffffff)));
		  /*hBoxLayout->addWidget(fr);
		  hBoxLayout->addWidget(lb);
		  hBoxLayout->addStretch(1);
		  hBoxLayout->addWidget(bt);
		  */
		  table->setCellWidget(n, 1, hbox);
		 }
	         else
	         {
	          //неперечислимый тип и не цвет:
	          value = ( (*itemIterator)->getPropValue(*j) == (*itemIterator2)->getPropValue(*j2) ) ?
		                                                     (*itemIterator)->getPropValue(*j) : "";
	          table->setText/*Item*/( n, 1, /*new QTableItem*/(table, QTableItem::Always, value));
	         }
	   n++;
	  }
	  }
      }
      } // for i, i2
      
      itemIterator++;
      itemIterator2++;
      } //for k
   table->setNumRows(table->numRows()-1); //убираем лишнюю последнюю строку
}

/*
void TItemSupervisor::updatePropValue(const QString& propName)
{
   for (int i = 0; i < table->numRows(); i++)
      {
       if (table->text(i, 0).stripWhiteSpace() == propName.stripWhiteSpace())
      }
}*/

/*void TItemSupervisor::whenSelectionChanged()
{
   //QMessageBox::information( NULL, "info", "press" );
}
*/

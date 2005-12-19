// file "tframedev.cpp"

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

#include <qmessagebox.h>
#include <string.h>

#include "tframedev.h"
#include "titem.h"


using namespace VISION;

TFrameDev::TFrameDev(QWidget *parent, TVISIONMode m, const QString &name) :
    QMainWindow(parent, 0, Qt::WDestructiveClose),
    canvas(800, 600),
    fView(&canvas, m, this)
{
   canvas.setDoubleBuffering(true);
   canvas.setAdvancePeriod(100);
   this->setCentralWidget(&fView);
   
   fView.setName(name);
   
   connect(&fView, SIGNAL(addItem(int, int)), this, SLOT(addItemSlot(int, int)));
   connect(&fView, SIGNAL(selectingItem()), this, SLOT(selectingItemSlot()));
   connect(&fView, SIGNAL(movingItem()), this, SLOT(movingItemSlot()));
   connect(&fView, SIGNAL(resizingItem()), this, SLOT(resizingItemSlot()));
   connect(&fView, SIGNAL(mouseMove(int, int)), this, SLOT(mouseMoveSlot(int, int)));
   connect(&fView, SIGNAL(mousePress(int, int)), this, SLOT(mousePressSlot(int, int)));
}

TFrameView* TFrameDev::frameView()
{
   return &fView;
}

bool TFrameDev::saveFrame(const QString &fileName)
{
   QFile file(fileName);
   if (file.open(IO_WriteOnly))
      {
       itemList items;
       fView.getItems(&items);
       QTextStream stream(&file);
       TListOfString listOfString;
       unsigned int numSpaces = 0;
       stream << "<frame name = \"" << fView.getName() << "\">\n";
       //проход по категориям:
       //numSpaces++;
       listOfString = fView.getPropCategories();
       for (TListOfString::iterator i = listOfString.begin(); i != listOfString.end(); i++)
          {
           //проход по именам свойств в данной категории:
           numSpaces++;
           TListOfString propertiesNames = fView.getPropNames(&(*i));
           for (TListOfString::iterator i2 = propertiesNames.begin(); i2 != propertiesNames.end(); i2++)
              {
	       for (int j = 0; j < numSpaces; j++)
                  stream << " ";
               stream << "<property name = \"" << (*i2) << "\">\n";
	       //проход по значению свойства:
	       QString value = fView.getPropValue(*i2);
	       for (int j = 0; j < numSpaces; j++)
                  stream << " ";
	       stream << " <value>" << value << "</value>\n";
	   
	       for (int j = 0; j < numSpaces; j++)
                  stream << " ";
               stream << "</property>\n";
	      }
           numSpaces--;
	  }
       /////////////
       
       stream << " <items>\n";
       for (itemList::iterator i = items.begin(); i != items.end(); i++)
          (*i)->saveToFile(file);
       stream << " </items>\n";
       stream << "</frame>\n";
       file.close();
      }
      else QMessageBox::critical( NULL, "Saving error", "Can't save current frame to the file '" + fileName + "'!");
}
  
void TFrameDev:: selectAll()
{
   fView.selectAll();
}

void TFrameDev::keyPressEvent ( QKeyEvent * e )
{
   int d = 1;
   switch (e->key())
      {
       //------------------------------------------
       //перемещение элементов:
       case Qt::Key_Left : 
               {
	        if (e->state() & Qt::ShiftButton)
                   d = 10;
                if (e->state() & Qt::ControlButton)
                   d = 25;
		fView.moveBySelected(-d, 0);
		emit movingItem();
		break;
	       }
       case Qt::Key_Right : 
               {
	        if (e->state() & Qt::ShiftButton)
                   d = 10;
                if (e->state() & Qt::ControlButton)
                   d = 25;
		fView.moveBySelected(d, 0);
		emit movingItem();
		break;
	       }
       case Qt::Key_Up :
               {
	        if (e->state() & Qt::ShiftButton)
                   d = 10;
                if (e->state() & Qt::ControlButton)
                   d = 25;
		fView.moveBySelected(0, -d);
		emit movingItem();
		break;
	       }
       case Qt::Key_Down :
               {
	        if (e->state() & Qt::ShiftButton)
                   d = 10;
                if (e->state() & Qt::ControlButton)
                   d = 25;
		fView.moveBySelected(0, d);
		emit movingItem();
		break;
	       }
	//------------------------------------------
	case Qt::Key_Delete :
               {
	        fView.delSelectedItems();
		//emit movingItem();
		break;
	       }
	
        default : //ignore();
	              break;
       }
   
}

void TFrameDev::addItemSlot(int x, int y)
{
   emit addItem(x, y);
}

void TFrameDev::selectingItemSlot()
{
   emit selectingItem();
}

void TFrameDev::movingItemSlot()
{
   emit movingItem();
}

void TFrameDev::resizingItemSlot()
{
   emit resizingItem();
}

void TFrameDev::mouseMoveSlot(int x, int y)
{
   emit mouseMove(x, y);
}

void TFrameDev::mousePressSlot(int x, int y)
{
   emit mousePress(x, y);
}

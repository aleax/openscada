// file "tframedev.h"

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

#ifndef TFRAMEDEV 
#define TFRAMEDEV 

#include <qmainwindow.h> 
#include <qstringlist.h> 
#include "tframeview.h"


class QCanvas; 


namespace VISION
{
//Класс "MDI-oкно кадра во время разработки"
class TFrameDev : public QMainWindow 
{ 
  Q_OBJECT 
public: 
  TFrameDev(QWidget *parent, TVISIONMode m, const QString &name);
  
  TFrameView* frameView();
  
  bool saveFrame(const QString &);
  void selectAll();

signals:
  void addItem(int x, int y);
  void selectingItem();
  void movingItem();
  void resizingItem();
  void deletingItem();
  void mouseMove(int x, int y);
  void mousePress(int x, int y);

protected: 
     void keyPressEvent ( QKeyEvent * e );
  
private:
   QCanvas canvas;
   TFrameView fView; 

private slots:
   void addItemSlot(int x, int y);
   void selectingItemSlot();
   void movingItemSlot();
   void resizingItemSlot();
   
   void mouseMoveSlot(int x, int y);
   void mousePressSlot(int x, int y);
};

}
#endif
